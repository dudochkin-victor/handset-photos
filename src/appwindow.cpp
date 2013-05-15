/*
 * Meego-handset-photos is a photo viewer.
 * Copyright (C) 2010, Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */


#include <QDebug>
#include <QGraphicsGridLayout>
#include <QGraphicsLinearLayout>

#include <MApplicationPage>
#include <MAction>
#include <MButton>
#include <MContentItem>
#include <MLabel>
#include <MLayout>
#include <MLinearLayoutPolicy>
#include <MGridLayoutPolicy>
#include <MFlowLayoutPolicy>
#include <MImageWidget>
#include <MOverlay>
#include <MPannableWidget>
#include <MTextEdit>
#include <MWidgetController>
#include <MBasicLayoutAnimation>
#include <MPositionIndicator>
#include <MDialog>
#include <MObjectMenu>
#include <MSceneManager>

#include "appwindow.h"
#include "photostrip.h"
#include "mplabelwithclick.h"
#include "mpsettings.h"
#include "abstractdeleteconfirm.h"
#include "searchresults.h"
#include "socialuploaderpage.h"
#include "photos_adaptor.h"
#include "mpsinglephotomodel.h"

#define TOP_MARGIN 0
#define TOP_MARGIN_ALBUM 55 
#define ANI_DURATION    400
AppWindow *AppWindow::mainWindowInstance = NULL;


void MEBlockedMWidgetController::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    Q_UNUSED(event);
}

void MEBlockedMWidgetController::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
   Q_UNUSED(event);
}

void MEBlockedMWidgetController::cancelEvent(MCancelEvent *event)
{
    Q_UNUSED(event);
}


MAction *AppWindow::createAction(MWidget *widget, MAction::Location location, const QString &text, QObject *parent)
{
    MAction *action = new MAction(parent);
    action->setLocation(location);
    action->setText(text);
    widget->addAction(action);
    return action;
}

AppWindow::AppWindow(bool isSinglePhoto, QWidget *parent) : 
    MApplicationWindow(parent), photosInOneAlbumList(NULL), currentPhotoList(NULL), socialUploaderPage(NULL),
    bottomToolbarLayout(NULL), albumInfoContainer(NULL), searchInput(NULL), startSearch(NULL), isSearchBarShown(false),
    homeButton(NULL), closeButton(NULL), searchOverlay(NULL), searchResultsPage(NULL), searchPhotoProxyModel(NULL), searchAlbumProxyModel(NULL),
    overlayVisible(false), allPhotosPage(NULL)
{
    mainWindowInstance = this;
    new PhotosAdaptor(this);

    connect(this, SIGNAL(pageChanged(MApplicationPage *)), this, SLOT(updateBackPage(MApplicationPage *)));
    //hideOverlay();
    autoHideTimer.setSingleShot(true);
    autoHideTimer.stop();
    connect(&autoHideTimer, SIGNAL(timeout()), this, SLOT(hideOverlay()));

    ///////////////////////////
    // Create All Photos Page
    //
    // This a top level view (meaning that the navigation bar always has an 'X' for closing the app
    // instead of a back button) that will display all photos found on the system.  There is also
    // a MAction style of button inside the navigation bar that allows the user to switch to the
    // All Albums view.
    //
    // If the user selects a thumbnail then the Single Photo Page is opened for that specific photo,
    // with left and right flicking on the photo causing the single photo page to cycle through all
    // photos on the device.  Pressing the 'Play' button while in this mode will trigger a slide show
    // for all photos on the device.
    //
    // If the user then selects the back button in the single photo page then the user is returned
    // to the All Photos Page.
    //
    // While inside this view a toolbar is visible on the bottom of the page that contains a button 
    // for 'Play' which kicks off a slide show for all photos on the device, a button for 'Camera'
    // which kicks off the camera application, and a button for 'Search' for filtering the photos
    // presented in the view.
    
    if (!isSinglePhoto) {
        allPhotos = new MWidgetController;
        allPhotos->setViewType("background");
        allPhotos->setObjectName("AllPhotos");
        allPhotosPage = new MApplicationPage;
        allPhotosPage->setObjectName("AllPhotosPage");
        allPhotosPage->setCentralWidget(allPhotos);
        allPhotosPage->setPannable(false);
        allPhotosPage->setComponentsDisplayMode(MApplicationPage::AllComponents,
                MApplicationPageModel::Show);
        //% "Photo Viewer"
        allPhotosPage->setTitle(qtTrId("photo-viewer-title"));
        connect(allPhotosPage, SIGNAL(backButtonClicked()), this, SLOT(onBackButtonClicked()));

        //% "Search"
        searchAction = createAction(allPhotosPage, MAction::ApplicationMenuLocation, qtTrId("Search"), allPhotos);
        connect(searchAction, SIGNAL(triggered()), this, SLOT(doSearch()));
        
        //% "Slide show"
        slideShowAction = createAction(allPhotosPage, MAction::ApplicationMenuLocation, qtTrId("Slide show"), allPhotos);
        connect(slideShowAction, SIGNAL(triggered()), this, SLOT(startSlideShow()));
        //cameraAction = createAction(allPhotosPage, MAction::ApplicationMenuLocation, qtTrId("Go to Camera"), allPhotos);
        //connect(cameraAction, SIGNAL(triggered()), this, SLOT(gotoCamera()));
        
        //% "Share"
        shareAction = createAction(allPhotosPage, MAction::ApplicationMenuLocation, qtTrId("Share"), allPhotos);
        connect(shareAction, SIGNAL(triggered()), this, SLOT(share()));
        
        //% "All Photos"
        allPhotosAction = createAction(allPhotosPage, MAction::ApplicationMenuLocation, qtTrId("All Photos"), allPhotos);
        connect(allPhotosAction, SIGNAL(triggered()), this, SLOT(showAllPhotos()));
        
        //% "Albums"
        albumsAction = createAction(allPhotosPage, MAction::ApplicationMenuLocation, qtTrId("Albums"), allPhotos);
        connect(albumsAction, SIGNAL(triggered()), this, SLOT(showAllAlbums()));
		
		//% "Send"
        sendShareAction = createAction(allPhotosPage, MAction::ApplicationMenuLocation, qtTrId("Send"), allPhotos);
        connect(sendShareAction, SIGNAL(triggered()), this, SLOT(sendShare()));
        sendShareAction->setVisible(false);
        // allPhotosPage->removeAction(sendShareAction);
        
        //% "Cancel"
        cancelShareAction = createAction(allPhotosPage, MAction::ApplicationMenuLocation, qtTrId("Cancel"), allPhotos);
        connect(cancelShareAction, SIGNAL(triggered()), this, SLOT(cancelShare()));
        cancelShareAction->setVisible(false);
        // allPhotosPage->removeAction(cancelShareAction);

        //    connect(actionItalic, SIGNAL(triggered()), this, SLOT(makeTextItalic()));

        allPhotosLayout = new QGraphicsLinearLayout(Qt::Vertical, allPhotos);
        allPhotosLayout->setContentsMargins(0, 0, 0, 0);
        allPhotosLayout->setSpacing(0);
    }
    /////////////////////////
	// Create Header Overlay
	/////////////////////////
/*     MWidgetController *topToolbarHolder = new MWidgetController;
    topToolbarHolder->setViewType("background");
    topToolbarHolder->setObjectName("TopOverlayHolder");
    topToolbarHolder->setContentsMargins(0, 0, 0, 0);

    QGraphicsLinearLayout *topToolbarHolderLayout = new QGraphicsLinearLayout(Qt::Horizontal, topToolbarHolder);
    topToolbarHolderLayout->setSpacing(0);
    topToolbarHolderLayout->setContentsMargins(0, 0, 0, 0);
    topToolbarHolderLayout->addStretch();  */

    topToolbar = new MOverlay;
    //topToolbar->setViewType("background");
    topToolbar->setObjectName("TopOverlay");
    topToolbar->setContentsMargins(0, 0, 0, 0);

    MWidget *tToolbar = topToolbar->widget();
    topToolbarLayout = new QGraphicsLinearLayout(Qt::Vertical, tToolbar);
    MWidgetController *firstRow = new MWidgetController;
    firstRow->setObjectName("firstRowInTopOverlay");
    firstRow->setViewType("background");
    MLayout *topToolbar1stRowLayout = new MLayout(firstRow);
    MLinearLayoutPolicy *topToolbar1stRowLayoutLandscapePolicy = new MLinearLayoutPolicy(topToolbar1stRowLayout, Qt::Horizontal);
    MLinearLayoutPolicy *topToolbar1stRowLayoutPortraitPolicy = new MLinearLayoutPolicy(topToolbar1stRowLayout, Qt::Horizontal);
    topToolbar1stRowLayout->setLandscapePolicy(topToolbar1stRowLayoutLandscapePolicy);
    topToolbar1stRowLayout->setPortraitPolicy(topToolbar1stRowLayoutPortraitPolicy);
    topToolbar1stRowLayoutLandscapePolicy->setSpacing(0);
    topToolbar1stRowLayoutLandscapePolicy->setContentsMargins(0, 0, 0, 0);
    topToolbar1stRowLayoutPortraitPolicy->setSpacing(0);
    topToolbar1stRowLayoutPortraitPolicy->setContentsMargins(0, 0, 0, 0);

    topToolbarLayout->setContentsMargins(0, 0, 0, 0);
    topToolbarLayout->setSpacing(0);
   // topToolbarLayout->addItem(firstRow);

 /*   homeButton = new MButton();
    connect(homeButton, SIGNAL(clicked()), this, SLOT(homeButtonClicked()));
    homeButton->setObjectName("homeButton");
    topToolbar1stRowLayoutLandscapePolicy->addItem(homeButton);
    topToolbar1stRowLayoutPortraitPolicy->addItem(homeButton);

    allPhotosButton = new MButton(tr("All Photos"));
    connect(allPhotosButton, SIGNAL(clicked()), this, SLOT(showAllPhotos()));
    topToolbar1stRowLayoutLandscapePolicy->addItem(allPhotosButton);
    topToolbar1stRowLayoutPortraitPolicy->addItem(allPhotosButton);
    allPhotosButton->setObjectName("allPhotosButtonActive");
    isAllPhotoButtonActive = true;

    albumsButton = new MButton("Albums");
    albumsButton->setObjectName("albumsButtonInactive");
    connect(albumsButton, SIGNAL(clicked()), this, SLOT(showAllAlbums()));
    topToolbar1stRowLayoutLandscapePolicy->addItem(albumsButton);
    topToolbar1stRowLayoutPortraitPolicy->addItem(albumsButton);

    topToolbar1stRowLayoutLandscapePolicy->addStretch();

    searchButton = new MButton;
    searchButton->setObjectName("searchButtonInactive");
    topToolbar1stRowLayoutLandscapePolicy->addItem(searchButton);
    topToolbar1stRowLayoutPortraitPolicy->addItem(searchButton);
    connect(searchButton, SIGNAL(clicked()), this, SLOT(doSearch()));

    closeButton = new MButton;
    closeButton->setObjectName("closeButtonInactive");
    topToolbar1stRowLayoutLandscapePolicy->addItem(closeButton);
    topToolbar1stRowLayoutPortraitPolicy->addItem(closeButton);
    connect(closeButton, SIGNAL(clicked()), this, SLOT(closeButtonClicked()));
*/
    //albumInfoFreestyleContainer = new MWidgetController;
    albumInfoFreestyleContainer = new MEBlockedMWidgetController;
    albumInfoFreestyleContainer->setObjectName("albumInfoContainer"); 
    albumInfoFreestyleContainer->setViewType("background");
    
    MLayout *albumInfoFreestyleContainerLayout = new MLayout(albumInfoFreestyleContainer);
    albumInfoContainerFreestylePolicy = new MFreestyleLayoutPolicy(albumInfoFreestyleContainerLayout);
    albumInfoFreestyleContainerLayout->setPolicy(albumInfoContainerFreestylePolicy);
    albumInfoFreestyleContainerLayout->setContentsMargins(0, 0, 0, 0);
    albumInfoContainerFreestylePolicy->setSpacing(0);



    albumInfoContainer = new MWidgetController;
    albumInfoContainer->setObjectName("albumInfoContainer"); 
    albumInfoContainer->setViewType("background");

   
    albumInfoContainerFreestylePolicy->addItem(albumInfoContainer);
    /*
    QParallelAnimationGroup *hideAnimation = new QParallelAnimationGroup();
    QParallelAnimationGroup *showAnimation = new QParallelAnimationGroup();

    QPropertyAnimation* animation = new QPropertyAnimation(albumInfoContainer, "opacity");
    animation->setDuration(5000);
    animation->setEndValue(0.0);
    hideAnimation->addAnimation(animation);
    hideAnimation->start();
    */

    albumInfoContainerLayout = new QGraphicsLinearLayout(Qt::Horizontal, albumInfoContainer);
    //MLayout *albumInfoContainerMLayout = new MLayout(albumInfoContainer);
    //MLinearLayoutPolicy *albumInfoContainerLayout;
    //albumInfoContainerLayout = new MLinearLayoutPolicy(albumInfoContainerMLayout, Qt::Horizontal);
    albumInfoContainerLayout->setSpacing(0);
   // new MBasicLayoutAnimation(albumInfoContainerMLayout);

    albumInfoContainerLayout->setContentsMargins(0, 0, 0, 0);

    //allAlbumButtonInInfoContainer = new MPLabelWithClick(tr("All"));
    //allAlbumButtonInInfoContainer->setObjectName("allAlbumButtonInInfoContainer");
    //connect(allAlbumButtonInInfoContainer, SIGNAL(clicked()), this, SLOT(allAlbumButtonInInfoContainerClicked()));
    //albumInfoContainerLayout->addItem(allAlbumButtonInInfoContainer);
    
    positionAnimation0 = new QPropertyAnimation;
    positionAnimation0->setPropertyName("pos");
    positionAnimation0->setTargetObject(albumInfoContainer);
    positionAnimation0->setDuration(ANI_DURATION);
    positionAnimation0->setStartValue(QPointF(864, 0));
    positionAnimation0->setEndValue(QPointF(0,0));
    connect(positionAnimation0, SIGNAL(finished()), this, SLOT(animationFinished()));


    //vsep = new MImageWidget("vseparator");
    //vsep->setObjectName("vSeparator");
    //albumInfoContainerLayout->addItem(vsep);

    albumInfoButtonInInfoContainer = new MPLabelWithClick;
    albumInfoButtonInInfoContainer->setObjectName("albumInfoButtonInInfoContainer");
    connect(albumInfoButtonInInfoContainer, SIGNAL(clicked()), this, SLOT(showCurrentAlbum()));
    albumInfoContainerLayout->addItem(albumInfoButtonInInfoContainer);
   
    animationGroup = new QParallelAnimationGroup;
    animationGroup->addAnimation(positionAnimation0);

    vsep2 = new MImageWidget("vseparator");
    vsep2->setObjectName("vSeparator");
    vsep2->hide();
    albumInfoContainerLayout->addItem(vsep2);

    photoInfoButtonInInfoContainer = new MPLabelWithClick;
    photoInfoButtonInInfoContainer->setObjectName("photoInfoButtonInInfoContainer");
    albumInfoContainerLayout->addItem(photoInfoButtonInInfoContainer);
    photoInfoButtonInInfoContainer->hide();


    /*MLabel *spaceInInfoContainer = new MLabel;
    spaceInInfoContainer->setObjectName("spaceInInfoContainer");
    connect(spaceInInfoContainer, SIGNAL(clicked()), this, SLOT(showCurrentAlbum()));
    spaceInInfoContainer->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Fixed);
    albumInfoContainerLayout->addItem(spaceInInfoContainer); */
    albumInfoContainerLayout->addStretch();


    topToolbarLayout->addItem(albumInfoFreestyleContainer);

//    allPhotosLayout->addItem(topToolbarHolder);

    if (!isSinglePhoto) {
        MWidgetController *photosListArea = new MWidgetController;
        photosListArea->setObjectName("photosListContainer");
        //////////////////////////////////////
        listLayout = new QGraphicsLinearLayout(Qt::Vertical, photosListArea);
        listLayout->setSpacing(0);
        listLayout->setContentsMargins(0, TOP_MARGIN, 0, 0); 

        viewport = new MPannableViewport(photosListArea);
        viewport->setObjectName("viewportPhotos");
        viewport->positionIndicator()->hide();

        photoList = new DynamicPhotoList(allPhotos);
        photoListWrapper = new MWidgetController;
        photoListWrapper->setObjectName("photoListWrapper");
        //////////////////////////////////////
        QGraphicsLinearLayout *photoListWrapperLayout = new QGraphicsLinearLayout(Qt::Horizontal, photoListWrapper);
        photoListWrapperLayout->setSpacing(0);
        photoListWrapperLayout->setContentsMargins(0, 0, 0, 0);
        photoListWrapperLayout->addItem(photoList);


        viewport->setWidget(photoListWrapper);
        viewport->setPosition(QPointF(0, 0));
        listLayout->addItem(viewport);

        allPhotosLayout->addItem(photosListArea);

        //////////////////////////
        // Create toolbar overlay
        /*    MWidgetController *bottomToolbarHolder = new MWidgetController;
              bottomToolbarHolder->setViewType("background");
              bottomToolbarHolder->setObjectName("BottomHolder"); 
              bottomToolbarHolder->setContentsMargins(0, 0, 0, 0);

              QGraphicsLinearLayout *bottomToolbarHolderLayout = new QGraphicsLinearLayout(Qt::Horizontal, bottomToolbarHolder);
              bottomToolbarHolderLayout->addStretch();
              allPhotosLayout->addItem(bottomToolbarHolder); */
    
/*
        bottomToolbar = new MOverlay;
        //bottomToolbar->setViewType("background");
        bottomToolbar->setObjectName("BottomOverlay");

        MWidget *bToolbar = bottomToolbar->widget();
        bottomToolbarLayout = new QGraphicsLinearLayout(Qt::Horizontal, bToolbar);
        bottomToolbarLayout->setContentsMargins(0, 0, 0, 0);
        bottomToolbarLayout->setSpacing(0);

        slideShowButton = new MButton;
        //slideShowButton->setIconID("play_button_norm");
        //slideShowButton->setToggledIconID("button-slide-show-press");
        slideShowButton->setObjectName("slideShowButton");
        connect(slideShowButton, SIGNAL(clicked()), this, SLOT(startSlideShow()));
        bottomToolbarLayout->addItem(slideShowButton);
*/
        /*MButton *bottomPannel1 = new MButton();
          bottomPannel1->setObjectName("bottomPanel1");
          bottomToolbarLayout->addItem(bottomPannel1); */


        shareBottomToolbar = new MOverlay();
        shareBottomToolbar->setObjectName("shareBottomToolbar");

        MWidget *bToolbar2 = shareBottomToolbar->widget();
        shareBottomToolbarLayout = new QGraphicsLinearLayout(Qt::Horizontal, bToolbar2);
        shareBottomToolbarLayout->setContentsMargins(0, 0, 0, 0);
        shareBottomToolbarLayout->setSpacing(0);
		
		//% "Cancel"
        cancelShareButton = new MButton(qtTrId("Cancel"));
        cancelShareButton->setObjectName("cancelShareButton");
        connect(cancelShareButton, SIGNAL(clicked()), this, SLOT(cancelShare()));
        shareBottomToolbarLayout->addItem(cancelShareButton);
		
		//% "Send"
        sendShareButton = new MButton(qtTrId("Send"));
        sendShareButton->setObjectName("sendShareButton");
        connect(sendShareButton, SIGNAL(clicked()), this, SLOT(sendShare()));
        shareBottomToolbarLayout->addItem(sendShareButton);
		
		//% "Next"
        nextShareButton = new MButton(qtTrId("Next"));
        nextShareButton->setObjectName("nextShareButton");
        connect(nextShareButton, SIGNAL(clicked()), this, SLOT(nextShare()));

        //connect(cameraButton, SIGNAL(clicked()), this, SLOT(startCameraApp()));
        //shareBottomToolbar->appear();

        /*
           MAction *allAlbumsAction = new MAction("All Albums", allPhotosPage);
           allAlbumsAction->setLocation(MAction::ToolBarLocation);
           connect(allAlbumsAction, SIGNAL(triggered()), this, SLOT(showAllAlbums()));
           allPhotosPage->addAction(allAlbumsAction);
           */
        //////////////////////
        // Create Albums Page
        //
        // This is also a top level view (i.e. we always show an 'X' button instead of a back button)
        // which presents a view of all the albums found on the device.  There is a MAction style
        // button inside the navigation bar that allows the user to switch to the All Photos view.
        //
        // If the user selects an album then the Single Album Page is opened for the specific album.
        //j
        // While inside this view a toolbar is visible on the bottom of the page that contains a button 
        // for 'Play' which kicks off a slide show for all photos on the device, a button for 'Camera'
        // which kicks off the camera application, and a button for 'Search' for filtering the photos
        // presented in the view.

        //allAlbums = new Albums;

        allAlbumsContainer = new MWidgetController();
        //allAlbumsContainer->setViewType("background");
        QGraphicsLinearLayout *allAlbumsLayout = new QGraphicsLinearLayout(Qt::Vertical, allAlbumsContainer);
        allAlbumsLayout->setContentsMargins(0, 0, 0, 0);
        allAlbumsLayout->setSpacing(0);

        allAlbumsPage = new MApplicationPage;
        allAlbumsPage->setObjectName("allAlbumsPage");
        allAlbumsPage->setCentralWidget(allAlbumsContainer); 
        allAlbumsPage->setPannable(false);
        allAlbumsPage->setComponentsDisplayMode(MApplicationPage::AllComponents,
                MApplicationPageModel::Show);
        allAlbumsPage->addAction(searchAction);
        allAlbumsPage->addAction(slideShowAction);
        //allAlbumsPage->addAction(cameraAction);
        //    allAlbumsPage->addAction(shareAction);
        allAlbumsPage->addAction(allPhotosAction);
        allAlbumsPage->addAction(albumsAction);
        
        //% "Photo viewer"
        allAlbumsPage->setTitle(qtTrId("Photo viewer"));


        //allAlbums->setQuery("TODO: change this to a query for showing all albums");


        MWidgetController *allAlbumsViewportContainer = new MWidgetController();
        //allAlbumsViewportContainer->setObjectName("allAlbumsViewportContainer");
        QGraphicsLinearLayout *allAlbumsViewportLayout = new QGraphicsLinearLayout(Qt::Vertical, allAlbumsViewportContainer);
        allAlbumsViewportLayout->setSpacing(0);
        allAlbumsViewportLayout->setContentsMargins(0, TOP_MARGIN, 0, 0);

        MPannableViewport *allAlbumsViewport = new MPannableViewport(allAlbumsViewportContainer);
        allAlbumsViewport->setObjectName("allAlbumsViewport");

        allAlbumsViewportLayout->addItem(allAlbumsViewport);

        albumList = new DynamicAlbumList(allAlbumsContainer);

        allAlbumsViewport->setWidget(albumList);
        allAlbumsViewport->positionIndicator()->hide();

        allAlbumsLayout->addItem(allAlbumsViewportContainer);

        /*MAction *allPhotosAction = new MAction("All Photos", allAlbumsPage);
          allPhotosAction->setLocation(MAction::ToolBarLocation);
          connect(allPhotosAction, SIGNAL(triggered()), this, SLOT(showAllPhotos()));
          allAlbumsPage->addAction(allPhotosAction); */

        //////////////////////////////////
        // Create Single Photo Album Page
        //
        // This is a second level view with the backbutton always causing the a transition to the
        // All Albums view.  This view is shown for a given photo album, allowing the user to 
        // browse through thumbnails of the specific album.
        //
        // If the user selects a thumbnail then the Single Photo Page is opened for that specific
        // photo, with left and right flicking on the photo causing the single photo page to cycle 
        // through all photos from the album.  Pressing the 'Play' button while in this mode will 
        // trigger a slide show for all photos in the specific album
        //
        // While inside this view a toolbar is visible on the bottom of the page that contains a button 
        // for 'Play' which kicks off a slide show for the photos in the specific album, a button for 
        // 'Camera' which kicks off the camera application, and a button for 'Search' for filtering the 
        // photos presented in the view.

        /*  album = new Photos;

            albumPage = new MApplicationPage;
            albumPage->setTitle("Album");
            albumPage->setObjectName("AlbumPage");
            albumPage->setEscapeButtonMode(MEscapeButtonPanelModel::BackMode);
            connect(albumPage, SIGNAL(backButtonClicked()), this, SLOT(switchPage()));
            albumPage->setCentralWidget(album);
            */
    }
    /////////////////////
    // Single Photo Page
    //
    // This is page used for displaying a single photo with a photostrip/flickable view of configurable
    // list of photos.  The back button will return the user to the previous view, which in this 
    // case can be any of the other pages.
    //
    // While this view the navigation bar will autohide and return when the user touches the device.

    singlePhotoPage = new PhotoStripPage;
    singlePhotoPage->setObjectName("SinglePhotoPage");
    singlePhotoPage->setPannable(false);
    singlePhotoPage->setAcceptTouchEvents(true);
    singlePhotoPage->grabGesture(Qt::PinchGesture);
    //singlePhotoPage->setEscapeButtonMode(MEscapeButtonPanelModel::BackMode);
    //singlePhotoPage->setPannableAreaInteractive(false);
    singlePhotoPage->setComponentsDisplayMode(MApplicationPage::AllComponents,
					      MApplicationPageModel::AutoHide);
	
	//% "Photo viewer"
    singlePhotoPage->setTitle(qtTrId("Photo viewer"));
    if (!isSinglePhoto)
        singlePhotoPage->setEscapeMode(MApplicationPageModel::EscapeManualBack);

    connect(singlePhotoPage, SIGNAL(backButtonClicked()), this, SLOT(switchPageBack()));

    photoStrip = new PhotoStrip;
    photoStrip->setObjectName("PhotoStrip");
    photoStrip->setZValue(1.0);

    //singlePhotoLayout->addItem(photoStrip);
    //singlePhotoLayout->addItem(bottomToolbar);

    photoStrip->show();

    singlePhotoPage->setCentralWidget(photoStrip);
    if (!isSinglePhoto)
        singlePhotoPage->addAction(slideShowAction);
    //createAction(singlePhotoPage, MAction::ApplicationMenuLocation, "Edit", photoStrip);

    if (!isSinglePhoto) {
        sharingActionListener = new  SharingActionListener();
        sharingActionFactory = new SharingActionFactory(allPhotos, sharingActionListener);

        // Start off showing all photos
        showAllPhotos();
    }
}

AppWindow::~AppWindow()
{
}

AppWindow *AppWindow::instance()
{
    return mainWindowInstance;
}

void AppWindow::hideOverlay(bool hideNaviBar)
{  
 //   currentPage()->setComponentsDisplayMode(MApplicationPage::AllComponents, MApplicationPageModel::Hide);
 //   currentPage()->setComponentsDisplayMode(MApplicationPage::NavigationBar, MApplicationPageModel::Show);
    if (hideNaviBar) {
        currentPage()->setComponentsDisplayMode(MApplicationPage::AllComponents, MApplicationPageModel::Hide);
        currentPage()->setComponentsDisplayMode(MApplicationPage::AllComponents, MApplicationPageModel::AutoHide);
    }
    //bottomToolbar->disappear();
    topToolbar->disappear();
    overlayVisible = false;
    
    if (currentPage() == singlePhotoPage) {
        photoStrip->setStripSmall(false);
    }

}

void AppWindow::toggleOverlay()
{
    if (overlayVisible)
        hideOverlay();
    else 
        showOverlay();
}

bool AppWindow::isOverlayShown()
{
    return overlayVisible;
}
void AppWindow::showOverlay()
{
//    currentPage()->setComponentsDisplayMode(MApplicationPage::HomeButton, MApplicationPageModel::Show);
//    currentPage()->setComponentsDisplayMode(MApplicationPage::NavigationBar, MApplicationPageModel::Show);
//    currentPage()->setComponentsDisplayMode(MApplicationPage::NavigationBar, MApplicationPageModel::Hide);
    if (currentPage() == singlePhotoPage) {
        if (isAllPhotoButtonActive) {
            albumInfoContainerLayout->removeItem(albumInfoButtonInInfoContainer);
            albumInfoButtonInInfoContainer->hide();
            albumInfoContainerLayout->removeItem(vsep2);
            vsep2->hide();
        } else {
            albumInfoContainerLayout->insertItem(albumInfoButtonIndex, albumInfoButtonInInfoContainer);
            albumInfoButtonInInfoContainer->show();
            albumInfoContainerLayout->insertItem(vSep2Index, vsep2);
        }
        topToolbarLayout->insertItem(1, albumInfoFreestyleContainer);
        albumInfoFreestyleContainer->show();

        currentAlbumName = settings.getPhotoAlbumName(currentPhotoUrn);
        currentAlbumUrn = settings.getPhotoAlbumUrn(currentPhotoUrn);

        albumInfoButtonInInfoContainer->setText(currentAlbumName);
        if (!isAllPhotoButtonActive)
            vsep2->show();
        photoInfoButtonInInfoContainer->show();
        photoStrip->setStripSmall(true);
    }
//    bottomToolbar->appear();
    overlayVisible = true;
    topToolbar->appear();
    topToolbar->setZValue(500 - 1); //500 is zValue for ApplicationMenu, I know  I should not change zValue of a scene window when it has been
    //added into the scene manager. But MTF does not provide any interface allowing me to do so.

}

void AppWindow::updateCurrentPhoto(const QString &photoPath, QString urn)
{
    QFileInfo info(photoPath);
    photoInfoButtonInInfoContainer->setText(info.fileName());
    currentPhotoUrn = urn;
}

/* Popup a confirmation dialog.
 * return true if the user confirms */
bool AppWindow::deletePhoto(const QString &title, const QString &text, const AbstractDeleteConfirm *callback)
{
    MDialog *dialog = new MDialog(title,
            M::YesButton | M::NoButton);
    dialog->setCentralWidget(new MLabel(text));
    dialog->setObjectName("deleteConfirmDialog");

    dialog->appear(MSceneWindow::DestroyWhenDone);
    connect(dialog, SIGNAL(accepted()), callback, SLOT(onAccepted())); 
    connect(dialog, SIGNAL(rejected()), callback, SLOT(onRejected())); 
    return true;
}

void AppWindow::onBackButtonClicked()
{
    autoHideTimer.stop();
    showAllAlbums();
}

void AppWindow::switchPageBack()
{
    autoHideTimer.stop();
    if (backPage == allAlbumsPage) {
        // The user started a slide show from the allAlbumsPage 
        // and is now trying to return
        showAllAlbums();
    } else if (backPage == allPhotosPage) {
        // The user either clicked on a photo or started a slide show 
        // from the allPhotosPage and is now trying to return
        if (isAllPhotoButtonActive)
            showAllPhotos();
        else 
            showCurrentAlbum();
    }
    //showOverlay();
}

void AppWindow::showAllPhotos()
{
    if (searchOverlay)
        searchOverlay->disappear();
    QString boldText = "<b>" + allPhotosAction->text().remove("<b>").remove("</b>") + "</b>";
    allPhotosAction->setText(boldText);
    QString albumsActionText = albumsAction->text();
    albumsAction->setText(albumsActionText.remove("<b>").remove("</b>"));

    singlePhotoPage->disappear();
    allPhotosPage->disappear();
    allPhotosPage->setEscapeMode(MApplicationPageModel::EscapeCloseWindow);
    if (albumInfoFreestyleContainer) {
        albumInfoFreestyleContainer->hide();
        topToolbarLayout->removeItem(albumInfoFreestyleContainer);
    }
    listLayout->setContentsMargins(0, TOP_MARGIN, 0, 0); 

    //albumsButton->setObjectName("albumsButtonInactive");
    //allPhotosButton->setObjectName("allPhotosButtonActive");
    isAllPhotoButtonActive = true;
    //albumList->hide();
    if (photosInOneAlbumList)
        photosInOneAlbumList->hide();

    photoList->show();
    viewport->setWidget(photoListWrapper);
    viewport->setPosition(QPointF(0, 0));
    allPhotosPage->appear();
    updateBackPage(currentPage());
    autoHideTimer.stop();
    showOverlay();
    viewport->setObjectName("viewportPhotos");
    currentPhotoList = photoList;
}

void AppWindow::showAllAlbums()
{
    if (searchOverlay)
        searchOverlay->disappear();
    QString boldText = "<b>" + albumsAction->text().remove("<b>").remove("</b>") + "</b>";
    albumsAction->setText(boldText);
    allPhotosAction->setText(allPhotosAction->text().remove("<b>").remove("</b>"));
#if 0
    qDebug() << "AppWindow::showAllAlbums()";
    allPhotosPage->disappear();
    if (albumInfoContainer) {
        albumInfoContainer->hide();
        topToolbarLayout->removeItem(albumInfoContainer);
    }
    listLayout->setContentsMargins(5, 67, 5, 0); 
//    allPhotos->showAllAlbums();
    albumsButton->setObjectName("albumsButtonActive");
    allPhotosButton->setObjectName("allPhotosButtonInactive");

    viewport->setWidget(albumList);
    viewport->setPosition(QPointF(0, 0));
    photoList->hide();
    albumList->show();
    if (photosInOneAlbumList)
        photosInOneAlbumList->hide();

//    listLayout->removeItem(viewport);
//    viewport->hide();
//    listLayout->addItem(viewport);
//    allPhotosLayout->removeItem(listLayout);
//    allPhotosLayout->removeAt(0);
//    allPhotosLayout->invalidate();
//    allPhotosPage->disappear();
    allPhotosPage->appear(); 
    viewport->setObjectName("viewportPhotos");
    autoHideTimer.stop();
    showOverlay();
    currentPhotoList = photoList;
#else
    currentPage()->disappear();

    //albumsButton->setObjectName("albumsButtonActive");
    //allPhotosButton->setObjectName("allPhotosButtonInactive");
    isAllPhotoButtonActive = false;
    albumList->show();

    if (albumInfoFreestyleContainer) {
        albumInfoFreestyleContainer->hide();
        topToolbarLayout->removeItem(albumInfoFreestyleContainer);
    }
    autoHideTimer.stop();
    allAlbumsPage->appear();
    updateBackPage(currentPage());
    showOverlay();

#endif
}

void AppWindow::showCurrentAlbum() 
{
    showAlbum(currentAlbumUrn);
}

void AppWindow::showAlbum(QString &urn)
{
    QString displayString = settings.getAlbumName(urn);
    int photoNum = settings.getAlbumPhotoNum(urn);
    //% "number of photos"
    displayString.append(qtTrId("number of photos", photoNum));
    albumInfoButtonInInfoContainer->setText(displayString); 
    photoInfoButtonInInfoContainer->hide();
    vsep2->hide();
    currentAlbumName = displayString;

    albumInfoContainerLayout->insertItem(albumInfoButtonIndex, albumInfoButtonInInfoContainer);
    albumInfoButtonInInfoContainer->show();
 
//    qDebug() << "albumxxxx pos2" << albumInfoButtonInInfoContainer->pos();
//    qDebug() << "albumxxxx2 pos0" << allAlbumButtonInInfoContainer->pos();
//    qDebug() << "albumxxxx2 pos1" << vsep->pos();
    if (topToolbarLayout->count() < 2) {
        /* only scenario that needs animation */
//        (topToolbarLayout->itemAt(0)->graphicsItem())
       albumInfoContainerFreestylePolicy->moveItemTo(albumInfoContainer, QRectF(864, 0, 1, 59));
       topToolbarLayout->addItem(albumInfoFreestyleContainer);
       animationGroup->start();
    }

    albumInfoContainerLayout->insertItem(albumInfoButtonIndex, albumInfoButtonInInfoContainer);
    albumInfoContainerLayout->insertItem(vSep2Index, vsep2);
 
    listLayout->setContentsMargins(0, TOP_MARGIN_ALBUM, 0, 0); 
    if (photosInOneAlbumList == NULL) {
        photosInOneAlbumList = new DynamicPhotoList(allPhotos, urn); //Todo: generate sparql with the id
        photosInOneAlbumList->setObjectName("photosInOneAlbumList");
        photosInOneAlbumList->hide();
        currentAlbumUrn = urn;
    }

    if (urn != currentAlbumUrn) {
        photosInOneAlbumList->setAlbumUrn(urn);
        currentAlbumUrn = urn;
    }

    viewport->setWidget(photosInOneAlbumList);
    viewport->setPosition(QPointF(0, 0));
    photoList->hide();
    //albumList->hide();

    photosInOneAlbumList->show();
    albumInfoFreestyleContainer->show();
    //viewport->setObjectName("viewportAlbum");
    currentPhotoList = photosInOneAlbumList;

    if (currentPage())
        currentPage()->disappear();
    allPhotosPage->appear();
    allPhotosPage->setEscapeMode(MApplicationPageModel::EscapeManualBack);
    autoHideTimer.stop();
    showOverlay();
 
}
void AppWindow::tryUpdatePhotoInOneAlbum(const QString albumUrn)
{
    if (photosInOneAlbumList) {
        photosInOneAlbumList->setAlbumUrn(albumUrn);
    }
}

void AppWindow::showSinglePhoto(const QAbstractItemModel *model, const QModelIndex &curIndex)
{
    Q_UNUSED(model);
    photoStrip->setStripIndex(curIndex);
    if (allPhotosPage)
        allPhotosPage->disappear();
    photoStrip->setStripSmall(false);
    singlePhotoPage->appear();
    hideOverlay();
}

/*
void AppWindow::showSinglePhoto(QString res)
{
    // NOTE: Once this is hooked up to real data then we will
    //       need to query the database for a presentation title
    //       associated with the low level resoure name.
    singlePhotoPage->setTitle(res);

    allPhotosPage->disappear();
    singlePhotoPage->appear();
    hideOverlay();
}
*/
void AppWindow::updateBackPage(MApplicationPage *page)
{
    /*
     * The backPage is only used for deciding where the backbutton
     * in the singlePhotoPage should return too.
     */
    if (page == NULL)
        return;
    if (page != singlePhotoPage) {
        backPage = page;
    }
}

void AppWindow::homeButtonClicked()
{
    showMinimized();
}

void AppWindow::startSlideShow()
{
    if (searchOverlay)
        searchOverlay->disappear();
    if (currentPage() != singlePhotoPage) {
        /* start with current photo in singlePhotoPage */
        photoStrip->setStripIndex(currentPhotoList->itemModel()->index(0, 0));
    }
    photoStrip->setStripSmall(false);
    photoStrip->setSlideShow(true);
    if (currentPage())
        currentPage()->disappear();
    singlePhotoPage->appear();
    hideOverlay();
}

void AppWindow::gotoCamera()
{
    qDebug() << "AppWindow::startCameraApp";
    if (searchOverlay)
        searchOverlay->disappear();
}

void AppWindow::restoreViewMenu()
{
/*    allPhotosPage->addAction(searchAction);
    allPhotosPage->addAction(slideShowAction);
    allPhotosPage->addAction(cameraAction);
    allPhotosPage->addAction(shareAction);
    allPhotosPage->addAction(allPhotosAction);
    allPhotosPage->addAction(albumsAction);
    allPhotosPage->removeAction(sendShareAction);
    allPhotosPage->removeAction(cancelShareAction); */
    sendShareAction->setVisible(false);
    cancelShareAction->setVisible(false);

    searchAction->setVisible(true);
    /* allPhotosPage->removeAction(searchAction);
    allPhotosPage->addAction(searchAction); */
    slideShowAction->setVisible(true);
    //cameraAction->setVisible(true);
    shareAction->setVisible(true);
    allPhotosAction->setVisible(true);
    albumsAction->setVisible(true);

}

void AppWindow::prepareViewMenuOnSharing()
{
 /*   allPhotosPage->removeAction(searchAction);
    allPhotosPage->removeAction(slideShowAction);
    allPhotosPage->removeAction(cameraAction);
    allPhotosPage->removeAction(shareAction);
    allPhotosPage->removeAction(allPhotosAction);
    allPhotosPage->removeAction(albumsAction);
    allPhotosPage->addAction(sendShareAction);
    allPhotosPage->addAction(cancelShareAction); */
    searchAction->setVisible(false);
    slideShowAction->setVisible(false);
    //cameraAction->setVisible(false);
    shareAction->setVisible(false);
    allPhotosAction->setVisible(false);
    albumsAction->setVisible(false);
    sendShareAction->setVisible(true);
    cancelShareAction->setVisible(true);
}

void AppWindow::cancelShare()
{
    photoList->setSelectionMode(MList::NoSelection);
    photoList->enableItemClick();
    QTimer::singleShot(1500, this, SLOT(restoreViewMenu()));
    shareBottomToolbar->disappear();
}

void AppWindow::sendShare(const QStringList &photos, const QString &albumName)
{
    sharingActionListener->sendShare(photos, albumName);
    shareBottomToolbar->disappear();
    cancelShare();
}

void AppWindow::sendShare()
{
    QStringList photos = getPhotosToBeShared();
    sendShare(photos);
}

QStringList AppWindow::getPhotosToBeShared()
{
    QStringList photos;
    if (isAllPhotoButtonActive) {
        QModelIndexList selectedList = photoList->selectionModel()->selectedIndexes();
        QAbstractItemModel *model = photoList->itemModel();
        for (int i = 0; i < selectedList.count(); i++) {
            QModelIndex index = selectedList[i];
            QVariant data = model->data(index);
            MPListItem *item = static_cast<MPListItem *>(data.value<void *>());
            QString uri = item->photoURI;
            photos << uri;
        }
    } else {
        albumList->getPhotosToBeShared(photos);
    }

    return photos;
}

QStringList AppWindow::getPhotoThumbnailToBeShared()
{
    QStringList photos;
    if (isAllPhotoButtonActive) {
        QModelIndexList selectedList = photoList->selectionModel()->selectedIndexes();
        QAbstractItemModel *model = photoList->itemModel();
        for (int i = 0; i < selectedList.count(); i++) {
            QModelIndex index = selectedList[i];
            QVariant data = model->data(index);
            MPListItem *item = static_cast<MPListItem *>(data.value<void *>());
            QString uri = item->thumbnailURI;
            photos << uri;
        }
    } else {
        albumList->getPhotoThumbnailToBeShared(photos);
    }

    return photos;
}

void AppWindow::nextShare() 
{
    AppWindow::instance()->shareBottomToolbar->disappear();
    if (!AppWindow::instance()->socialUploaderPage) {
        SocialUploaderPage *page = new SocialUploaderPage();
        page->setObjectName("socialUploaderPage");
        page->setPannable(false);
        
        //% "Photo viewer"
        page->setTitle(qtTrId("Photo viewer"));
        AppWindow::instance()->socialUploaderPage = page;
    }

    QStringList photos = getPhotosToBeShared();
    QStringList thumbnails = getPhotoThumbnailToBeShared();
    AppWindow::instance()->socialUploaderPage->setPhotos(thumbnails, photos, sharingActionListener->getSocialMethodName());
    AppWindow::instance()->socialUploaderPage->appear();
}

void AppWindow::share()
{
    if (searchOverlay)
        searchOverlay->disappear();
    MObjectMenu *menu = new MObjectMenu(allPhotos);
    //menu->setCursorPosition(QPointF(100.0, 100.0));
    sceneManager()->appearSceneWindow(menu, MSceneWindow::DestroyWhenDone);
    photoList->setSelectionMode(MList::MultiSelection);
    if (isAllPhotoButtonActive) {
        photoList->disableItemClick();
        QTimer::singleShot(1500, this, SLOT(prepareViewMenuOnSharing()));
    } else {
        /* share whole album */
        prepareViewMenuOnSharing();
    }
}

void AppWindow::doSearch()
{
    if (searchOverlay == NULL) {
        searchOverlay = new MOverlay;
        //searchOverlay->setViewType("background");
        searchOverlay->setObjectName("searchOverlay");

        MWidget *searchWidget = searchOverlay->widget();
        QGraphicsLinearLayout *searchLayout = new QGraphicsLinearLayout(Qt::Horizontal, searchWidget);

        searchLayout->setContentsMargins(0, 0, 0, 0);

        searchInputBox = new MTextEdit(MTextEditModel::SingleLine, "");
        searchLayout->addItem(searchInputBox);
    
        //% "Search Button"
        MButton *doButton = new MButton(qtTrId("Search"));
        doButton->setObjectName("searchDoButton");
        searchLayout->addItem(doButton);
        connect(doButton, SIGNAL(clicked()), this, SLOT(doRealSearch()));
    }

    searchOverlay->appear();
}

void AppWindow::doRealSearch()
{
    if (!searchResultsPage) {
        searchResultsPage = new MApplicationPage();
        searchResultsPage->setPannable(false);
        MPListModel *orgModel = dynamic_cast<MPListModel *>(photoList->itemModel());
        searchPhotoProxyModel = new MPProxyListModel(MPAbstractListModelOps::Photo);
        searchPhotoProxyModel->setSourceModel(orgModel);
        searchPhotoProxyModel->setDynamicSortFilter(true);

        MPListModel *orgAlbumModel = dynamic_cast<MPListModel *>(albumList->itemModel());
        searchAlbumProxyModel = new MPProxyListModel(MPAbstractListModelOps::Album);
        searchAlbumProxyModel->setSourceModel(orgAlbumModel);
        searchAlbumProxyModel->setDynamicSortFilter(true);

        SearchResults *searchResults = new SearchResults(searchPhotoProxyModel, searchAlbumProxyModel);
        searchResultsPage->setCentralWidget(searchResults);
        
        //% "Photo viewer"
        searchResultsPage->setTitle(qtTrId("Photo viewer"));
        searchResultsPage->addAction(searchAction);
        searchResultsPage->addAction(allPhotosAction);
        searchResultsPage->addAction(albumsAction);
    }

    searchPhotoProxyModel->setSearchString(searchInputBox->text());
    searchAlbumProxyModel->setSearchString(searchInputBox->text());
    
    searchOverlay->disappear();    
    if(currentPage())
        currentPage()->disappear();
    searchResultsPage->appear();
    hideOverlay();
}

void AppWindow::closeButtonClicked()
{
    closeButton->setObjectName("closeButtonActive");
    close();
}

void AppWindow::allAlbumButtonInInfoContainerClicked()
{
    isAllPhotoButtonActive ? showAllPhotos() : showAllAlbums();
}

void AppWindow::animationFinished()
{
    albumInfoContainerFreestylePolicy->moveItemTo(albumInfoContainer, QRectF(0, 0, 864, 59));
}

void SharingActionListener::actionTriggered(const SocialAccount *account)
{
    socialAccount = account;

    if (!AppWindow::instance()->isAllPhotoButtonActive && account->isSocial) {
        AppWindow::instance()->nextShare();
        return ;
    }
        
//    AppWindow::instance()->shareBottomToolbarLayout->removeItem(AppWindow::instance()->cancelShareButton);
//    AppWindow::instance()->cancelShareButton->hide();
    AppWindow::instance()->shareBottomToolbarLayout->removeItem(AppWindow::instance()->sendShareButton);
    AppWindow::instance()->sendShareButton->hide();
    AppWindow::instance()->shareBottomToolbarLayout->removeItem(AppWindow::instance()->nextShareButton);
    AppWindow::instance()->nextShareButton->hide();
    
    if (account->isSocial) {
 //       AppWindow::instance()->shareBottomToolbarLayout->addItem(AppWindow::instance()->cancelShareButton);
 //       AppWindow::instance()->cancelShareButton->show();
        AppWindow::instance()->shareBottomToolbarLayout->addItem(AppWindow::instance()->nextShareButton);
        AppWindow::instance()->nextShareButton->show();
    } else {
        AppWindow::instance()->shareBottomToolbarLayout->addItem(AppWindow::instance()->sendShareButton);
        AppWindow::instance()->sendShareButton->show();
  //      AppWindow::instance()->shareBottomToolbarLayout->addItem(AppWindow::instance()->cancelShareButton);
  //      AppWindow::instance()->cancelShareButton->show();
    }

    AppWindow::instance()->shareBottomToolbar->appear();
}

void AppWindow::viewSinglePhoto(const QString &photoPath)
{
    qDebug() << "2222:" << photoPath;
    MPSinglePhotoModel *model = new MPSinglePhotoModel(photoPath);
    showSinglePhoto(model, model->index(0, 0));
}

