#ifndef SHARING_ACTION_FACTORY_H
#define SHARING_ACTION_FACTORY_H
#include <QObject>
#include <MAction>

class SocialAccount : public QObject {
    Q_OBJECT
public:
    QString text;
    bool isSocial;
    MAction *action;
    SocialAccount(const QString &_text, QGraphicsWidget *parent,bool isSocial = false);
    SocialAccount(const SocialAccount &a);
    SocialAccount(){};
    SocialAccount &operator=(const SocialAccount &a);
    ~SocialAccount();

    virtual void send(const QStringList &photos, const QString &albumName = QString()) const = 0;

signals:
    void triggered();
public slots:
    void onTriggered();

};

class OnTriggeredListener {
public:
    virtual void actionTriggered(const SocialAccount *socialAccount) = 0;
};


class SharingActionFactory : QObject {
    Q_OBJECT
public:
    SharingActionFactory(QGraphicsWidget *parent, OnTriggeredListener *listener = NULL);
    ~SharingActionFactory();

    void setOnTriggeredListener(OnTriggeredListener *listener) {
        onTriggeredListener = listener;
    }
     
private:
    QList<SocialAccount *> socialAccounts;

    OnTriggeredListener *onTriggeredListener;

public slots:
    void triggeredSlot(); 
};

class AccountFactory {
public:
    enum AccountName {
        Email,
        SMS,
        BT,
        Twitter
    };
    static SocialAccount *createAccount(const AccountName name, const QString &text, QGraphicsWidget *parent, bool isSocial = false);
};


#endif
