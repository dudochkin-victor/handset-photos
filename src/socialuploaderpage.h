#ifndef SOCIAL_UPLOADER_PAGE_H
#define SOCIAL_UPLOADER_PAGE_H
#include <MApplicationPage>
#include <QGraphicsGridLayout>
#include <MLabel>
#include <MTextEdit>

class SocialUploaderPage : public MApplicationPage
{
    Q_OBJECT
public:
    SocialUploaderPage(QGraphicsItem *parent = 0);
    virtual ~SocialUploaderPage();
    virtual void createContent();
    void setPhotos(const QStringList &thumbnails, const QStringList &photos, const QString &socialMethodName, bool isSinglePhoto = false);
public slots:
    void upload();
    void cancel();
private:
    QGraphicsGridLayout *gridLayout;
    MLabel *nameLabel;
    MLabel *title;
    QStringList photos;
    MTextEdit *nameText;
};

#endif
