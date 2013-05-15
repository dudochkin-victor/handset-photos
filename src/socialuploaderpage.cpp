#include <MApplicationPage>
#include <MButton>
#include <QGraphicsLinearLayout>
#include <QGraphicsGridLayout>
#include <MLabel>
#include <MTextEdit>
#include <MImageWidget>
#include <QDebug>
#include <MLayout>
#include <MLinearLayoutPolicy>
#include <MGridLayoutPolicy>

#include "socialuploaderpage.h"
#include "photostasklet.h"
#include "appwindow.h"

void SocialUploaderPage::createContent()
{
    MApplicationPage::createContent();
    
    QGraphicsWidget *container = centralWidget();

    QGraphicsLinearLayout *layout = new QGraphicsLinearLayout(Qt::Vertical, container);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    title = new MLabel();
    title->setObjectName("titleLabelInSocialUploaderPage");

    layout->addItem(title);
    
    MWidgetController *container2 = new MWidgetController();
    QGraphicsGridLayout *layout2 = new QGraphicsGridLayout(container2);
    layout2->setContentsMargins(0, 0, 0, 0);
    layout2->setSpacing(0);
    
    
    MWidgetController *container3 = new MWidgetController();
    gridLayout = new QGraphicsGridLayout(container3);
    gridLayout->setContentsMargins(0, 0, 0, 0);
//    gridLayout->setSpacing(0);
//    container3->setObjectName("imgGridContainer");

    MWidgetController *container4 = new MWidgetController();
    QGraphicsLinearLayout *layout4 = new QGraphicsLinearLayout(Qt::Vertical, container4);
    layout4->setContentsMargins(0, 0, 0, 0);
    layout4->setSpacing(0);



    nameLabel = new MLabel();
    nameLabel->setObjectName("nameLabelInSocialUploaderPage");
    nameText = new MTextEdit(MTextEditModel::SingleLine, "",container);
    nameText->setObjectName("nameTextEdit");
    
    //% "Upload"
    MButton *uploadButton = new MButton(qtTrId("Upload"));
    connect(uploadButton, SIGNAL(clicked()), this, SLOT(upload()));
    
    //% "Cancel"
    MButton *cancelButton = new MButton(qtTrId("Cancel"));
    connect(cancelButton, SIGNAL(clicked()), this, SLOT(cancel()));

    layout4->addItem(nameLabel);
    layout4->setItemSpacing(0, 5);
    layout4->addItem(nameText);
    layout4->setItemSpacing(1, 30);
    layout4->addItem(uploadButton);
    layout4->setItemSpacing(2, 60);
    layout4->addItem(cancelButton);
//    layout4->setItemSpacing(3, 10);

    layout2->setColumnFixedWidth(0, 400);
    layout2->setColumnFixedWidth(1, 400);
    layout2->addItem(container3, 0, 0);
    layout2->addItem(container4, 0, 1);
/*    layout2->addItem(container3);
    layout2->addItem(container4);
    layout2->setStretchFactor(container3, 1);
    layout2->setStretchFactor(container4, 1); */
//    qDebug() << "12345: row spacing" << layout2->rowSpacing(0);

    layout->addItem(layout2);
}

SocialUploaderPage::SocialUploaderPage(QGraphicsItem *parent) : MApplicationPage(parent)
{
}

SocialUploaderPage::~SocialUploaderPage()
{
}

void SocialUploaderPage::setPhotos(const QStringList &thumbnails, const QStringList &photos, const QString &socialMethodName, bool isSinglePhoto)
{
    this->photos = photos;
    if (!isContentCreated()) {
        createContent();
    }

    for (int i = 0; i < gridLayout->count(); i++) {
        QGraphicsLayoutItem *item = gridLayout->itemAt(gridLayout->count() - 1);
        gridLayout->removeAt(gridLayout->count() - 1);
        delete item;
    }

    int numPhotos = photos.count();
    MImageWidget tmp;
    if (isSinglePhoto)
        tmp.setObjectName("imgWidgetSocialUploaderSingle");
    else
        tmp.setObjectName("imgWidgetSocialUploaderPage");
    QSizeF pSize = tmp.preferredSize();
    int numImages = 8;
    if (numImages > numPhotos)
        numImages = numPhotos;
    for (int i = 0; i < numImages; i++) {
        MImageWidget *imgWidget;
        if (isSinglePhoto) {
            QString uri = QUrl(photos[i]).toLocalFile();
            QPixmap pix = PhotosTasklet::squareQPixmapFromPath(uri, pSize.width(), pSize.height());
            imgWidget = new MImageWidget(&pix);
        } else {
            QPixmap pix = PhotosTasklet::squareQPixmapFromPath(thumbnails[i], pSize.width(), pSize.height());
            imgWidget = new MImageWidget(&pix);
        }
        if (isSinglePhoto)
            imgWidget->setObjectName("imgWidgetSocialUploaderSingle");
        else
            imgWidget->setObjectName("imgWidgetSocialUploaderPage");

        gridLayout->addItem(imgWidget, i/3, i%3);
       
    } 

    if (numPhotos > numImages) {
        int remain = numPhotos - numImages;
        
        //% "and %1 more photos"
        QString t = QString(qtTrId("and %1 more photos")).arg(remain);
        MLabel *remainIndicator = new MLabel();
        remainIndicator->setObjectName("remainIndicatorInSocialUploaderPage");
        remainIndicator->setWordWrap(true);
        remainIndicator->setText(t);
        gridLayout->addItem(remainIndicator, numImages/3, numImages % 3);
    }

    if (isSinglePhoto) {
        //% "Photo name"
        nameLabel->setText(qtTrId("Photo name")+":");
        //% "Upload to "
        title->setText(qtTrId("Upload to") + " " + socialMethodName);
    } else {
        //% "Album name:"
        nameLabel->setText(qtTrId("Album name")+":");
        title->setText(qtTrId("Upload to") + " " + socialMethodName + ": " + QString("%1").arg(numPhotos) + " " + qtTrId("Photos"));
    }


}

void SocialUploaderPage::upload()
{
    qDebug() << "12345: upload albumName:" << nameText->text() << "photos" << photos;
    AppWindow::instance()->sendShare(photos, nameText->text());
    disappear();
}

void SocialUploaderPage::cancel()
{
    AppWindow::instance()->sendShare(QStringList());
    disappear();
}
