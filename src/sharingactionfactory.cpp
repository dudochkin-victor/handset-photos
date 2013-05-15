
#include <QGraphicsWidget>
#include <QDebug>
#include <QDBusInterface>
#include <QString>
#include "sharingactionfactory.h"
#include "socialwebservice.h"


SocialAccount::SocialAccount(const QString &_text, QGraphicsWidget *parent, bool _isSocial) : text(_text), isSocial(_isSocial)
{
    action = new MAction(parent);
    action->setLocation(MAction::ObjectMenuLocation);
    action->setText(text);
    parent->addAction(action);
    connect(action, SIGNAL(triggered()), this, SLOT(onTriggered()));
}

SocialAccount::~SocialAccount()
{
    disconnect(action, SIGNAL(triggered()), this, SLOT(onTriggered()));
    //don't delete action
    //it'll be deleted in the parent
}

SocialAccount::SocialAccount(const SocialAccount &a) : text(a.text), isSocial(a.isSocial)
{
    action = a.action;
    connect(action, SIGNAL(triggered()), this, SLOT(onTriggered()));
}

SocialAccount &SocialAccount::operator=(const SocialAccount &a)
{
    text = a.text;
    action = a.action;
    isSocial = a.isSocial;
    connect(action, SIGNAL(triggered()), this, SLOT(onTriggered()));
}

void SocialAccount::onTriggered()
{
    emit triggered();
}

void SocialAccount::send(const QStringList &photos, const QString &albumName) const
{
/*    qDebug() << "SocialAccount::send " << photos;
    SocialWebService *service = new SocialWebService;
    service->uploadPhotos(photos, albumName); */
}

class SocialAccountEmail : public SocialAccount {
public:
    SocialAccountEmail(const AccountFactory::AccountName, const QString &_text, QGraphicsWidget *parent, bool isSocial) : SocialAccount(_text, parent, isSocial){};
    virtual void send(const QStringList &photos, const QString &albumName) const 
    {
        if (photos.count() < 1)
            return;

        QStringList photoList;
        for (int i = 0; i < photos.count(); i++) {
            QString path = QUrl(photos[i]).toLocalFile();
            photoList << path;
        }
        QDBusInterface email("com.meego.email", "/", "com.meego.email");
        if (email.isValid()) {
            QDBusReply<void> reply = email.call(QDBus::BlockWithGui, "showComposePage", "", photoList);
        }
    }
};

class SocialAccountSMS : public SocialAccount {
public:
    SocialAccountSMS(const AccountFactory::AccountName, const QString &_text, QGraphicsWidget *parent, bool isSocial) : SocialAccount(_text, parent, isSocial) {};
    virtual void send(const QStringList &phtoos, const QString &albumName) const 
    {
    }
};

class SocialAccountBT: public SocialAccount {
public:
    SocialAccountBT(const AccountFactory::AccountName, const QString &_text, QGraphicsWidget *parent, bool isSocial) : SocialAccount(_text, parent, isSocial) {};
    virtual void send(const QStringList &phtoos, const QString &albumName) const 
    {
    }
};

class SocialAccountTwitter: public SocialAccount {
public:
    SocialAccountTwitter(const AccountFactory::AccountName, const QString &_text, QGraphicsWidget *parent, bool isSocial) : SocialAccount(_text, parent, isSocial) {};
    virtual void send(const QStringList &phtoos, const QString &albumName) const 
    {
    }
};

SocialAccount *AccountFactory::createAccount(const AccountName name, const QString &text, QGraphicsWidget *parent, bool isSocial)
{
    SocialAccount *result = NULL;
    switch (name) {
        case Email:
            result = new SocialAccountEmail(name, text, parent, isSocial);
            break;
        case SMS:
            result = new SocialAccountSMS(name, text, parent, isSocial);
            break;
        case BT:
            result = new SocialAccountBT(name, text, parent, isSocial);
            break;
        case Twitter:
            result = new SocialAccountTwitter(name, text, parent, isSocial);
            break;
    }
    return result;
}

SharingActionFactory::SharingActionFactory(QGraphicsWidget *parent, OnTriggeredListener *listener) : onTriggeredListener(listener)
{
    /*Todo: get social network account from system configuration */
    SocialAccount *account;
    
    //% "email"
    account = AccountFactory::createAccount(AccountFactory::Email, qtTrId("email"), parent);
    socialAccounts << account;
	
	//% "sms"
    account = AccountFactory::createAccount(AccountFactory::SMS, qtTrId("sms"), parent);
    socialAccounts << account;
	
	//% "Bluetooth"
    account = AccountFactory::createAccount(AccountFactory::BT, qtTrId("Bluetooth"), parent);
    socialAccounts << account;
	
	//% "Twitter Pic"
    account = AccountFactory::createAccount(AccountFactory::Twitter, qtTrId("Twitter Pic"), parent, true);
    socialAccounts << account;

    for (int i = 0; i < socialAccounts.length(); i++) {
        connect(socialAccounts[i], SIGNAL(triggered()), this, SLOT(triggeredSlot()));
    }
}

SharingActionFactory::~SharingActionFactory()
{
    if (onTriggeredListener)
        delete onTriggeredListener;

    for (int i = 0; i < socialAccounts.length(); i++) {
        if (socialAccounts[i])
            delete socialAccounts[i];
    }
}

void SharingActionFactory::triggeredSlot()
{
    if (onTriggeredListener)
        onTriggeredListener->actionTriggered(dynamic_cast<SocialAccount *>(sender()));
}

