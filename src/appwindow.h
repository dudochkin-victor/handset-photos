/*
 * Meego-handset-photos is a photo viewer.
 * Copyright (C) 2010, Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */


#ifndef APPWINDOW_H
#define APPWINDOW_H

#include <MApplicationWindow>
#include <QGraphicsLinearLayout>
#include <MPannableViewport>
#include <MLabel>
#include <MImageWidget>
#include <MTextEdit>
#include <MFreestyleLayoutPolicy>
#include <QSettings>
#include <QParallelAnimationGroup>
#include <MAction>

#include "dynamicphotolist.h"
#include "dynamicalbumlist.h"
#include "mplabelwithclick.h"
#include "searchengine.h"
#include "abstractdeleteconfirm.h"
#include "mpproxylistmodel.h"
#include "sharingactionfactory.h"
#include "socialuploaderpage.h"
#include "photostrippage.h"

class MApplicationPage;
class MButton;
class MOverlay;
class Albums;
class Photos;
class PhotoStrip;


class MEBlockedMWidgetController : public MWidgetController
{
    Q_OBJECT
    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event);
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    virtual void cancelEvent(MCancelEvent *event);

private:

};


class SharingActionListener : public OnTriggeredListener {
public:
    SharingActionListener() : socialAccount(NULL) {};
    virtual void actionTriggered(const SocialAccount *account);
    void sendShare(const QStringList &photosPath, const QString &name)
    {
        if (socialAccount)
            socialAccount->send(photosPath, name);
    }

    const QString &getSocialMethodName() { return socialAccount->text;};
private:
    const SocialAccount *socialAccount;

};


class M_EXPORT AppWindow : public MApplicationWindow
{
    Q_OBJECT

public:
    AppWindow(bool isSinglePhotoMode, QWidget *parent = 0);
    virtual ~AppWindow();

    static AppWindow *instance();

    void showSinglePhoto(const QAbstractItemModel *model, const QModelIndex &curIndex);
    DynamicPhotoList *photoList;
    MWidgetController *photoListWrapper;
    DynamicAlbumList *albumList;
    DynamicPhotoList *photosInOneAlbumList;
    DynamicMList* currentPhotoList;
    QStringList photoRemoveBlacklist;   //don't handle if a urn is removed in this list
    MPSettings settings;
    

    void showOverlay(void);
    void toggleOverlay();
//    void hideSearchToolbar(void);
    void updateCurrentPhoto(const QString &photoPath, QString urn);
    void sendShare(const QStringList &photos, const QString &albumName=QString());
    bool isOverlayShown();
    void tryUpdatePhotoInOneAlbum(const QString albumUrn);

    static const int albumInfoButtonIndex = 0;  /* index of the albumInfoButton in the top overlay layout */
    static const int photoInfoButtonIndex = 2;  /* index of the photoInfoButton in the top overlay layout */
    static const int allInfoButtonIndex = 0;  /* index of the photoInfoButton in the top overlay layout */
    static const int vSepIndex = 1;  /* index of the photoInfoButton in the top overlay layout */
    static const int vSep2Index = 1;  /* index of the photoInfoButton in the top overlay layout */

    static const int albumInfoContainerIndex = 1;
    static bool deletePhoto(const QString &title, const QString &text, const AbstractDeleteConfirm *);



public slots:
    void hideOverlay(bool hideNavBar = false);
    void switchPageBack();
    void onBackButtonClicked();
    void showAllPhotos();
    void showAllAlbums();
    void showAlbum(QString &albumUrn);
    void showCurrentAlbum();
    void updateBackPage(MApplicationPage *page);
    void startSlideShow();
    void gotoCamera();
//    void showSearchToolbar();
    void doSearch();
    void share();
    void doRealSearch();
    void homeButtonClicked();
    void closeButtonClicked();
    void allAlbumButtonInInfoContainerClicked();
    void animationFinished();
    void sendShare();
    void cancelShare();
    void nextShare();
    void prepareViewMenuOnSharing();
    void restoreViewMenu();
    void viewSinglePhoto(const QString&);

private:
    MAction *createAction(MWidget *widget, MAction::Location location, const QString &text, QObject *parent = NULL);
    QStringList getPhotosToBeShared();
    QStringList getPhotoThumbnailToBeShared();

    MApplicationPage *albumPage;
    MApplicationPage *allAlbumsPage;
    MApplicationPage *allPhotosPage;
    PhotoStripPage *singlePhotoPage;
    MApplicationPage *backPage;
    MApplicationPage *searchResultsPage;
    SocialUploaderPage *socialUploaderPage;

    MPannableViewport *viewport;    //containing all photos or all albums
    QGraphicsLinearLayout *listLayout;

    MButton *slideShowButton;
    MButton *cameraButton;
    MButton *sendShareButton;
    MButton *nextShareButton;
    MButton *cancelShareButton;
    MButton *searchButton;
    MButton *searchButton2;
    MButton *allPhotosButton;
    MButton *allPhotosButton2;
    MButton *albumsButton;
    MButton *albumsButton2;
    MOverlay *topToolbar;
    QGraphicsLinearLayout *topToolbarLayout;
    MWidgetController *topToolbar2;
    MOverlay *bottomToolbar;
    MOverlay *shareBottomToolbar;
    QGraphicsLinearLayout *bottomToolbarLayout;
    QGraphicsLinearLayout *shareBottomToolbarLayout;

    Albums *allAlbums;
    MWidgetController *allPhotos;
    MWidgetController *allAlbumsContainer;
    QGraphicsLinearLayout *allPhotosLayout;
    Photos *album;
    PhotoStrip *photoStrip;
    MWidgetController *singlePhotoContainer;
    QGraphicsLinearLayout *singlePhotoLayout;

    MWidgetController *albumInfoContainer;
    //MWidgetController *albumInfoFreestyleContainer;
    MEBlockedMWidgetController *albumInfoFreestyleContainer;
    QGraphicsLinearLayout *albumInfoContainerLayout;
    QPropertyAnimation *positionAnimation0;
    QPropertyAnimation *positionAnimation1;
    QPropertyAnimation *positionAnimation2;
    QParallelAnimationGroup *animationGroup;
    MPLabelWithClick *allAlbumButtonInInfoContainer;
    MPLabelWithClick *albumInfoButtonInInfoContainer;
    MImageWidget *vsep2;
    MImageWidget *vsep;
    MPLabelWithClick *photoInfoButtonInInfoContainer;
    QString currentAlbumUrn; //ID of the album contained in photosInOneAlbumList
    QString currentAlbumName;

    MTextEdit *searchInput;
    MButton *startSearch;
    bool isSearchBarShown;
   
    static AppWindow *mainWindowInstance;
    QTimer autoHideTimer;
    friend class Viewport;

    MButton *homeButton;
    MButton *closeButton;

    QString currentPhotoUrn;
    bool isAllPhotoButtonActive;
    MFreestyleLayoutPolicy *albumInfoContainerFreestylePolicy;
    
    MOverlay *searchOverlay;
    MTextEdit *searchInputBox;
    MPProxyListModel *searchPhotoProxyModel;
    MPProxyListModel *searchAlbumProxyModel;

    MAction *allPhotosAction;
    MAction *albumsAction;
    MAction *searchAction;
    MAction *shareAction;
    MAction *sendShareAction;
    MAction *cancelShareAction;
    MAction *cameraAction;
    MAction *slideShowAction;
    SharingActionFactory *sharingActionFactory;
    SharingActionListener *sharingActionListener;

    bool overlayVisible;

    friend  class SharingActionListener;
};

#endif // APPWINDOW_H
