#ifndef YAHOO_REVERSE_GEO_CODER_H
#define YAHOO_REVERSE_GEO_CODER_H

#include <QNetworkReply>
#include <QNetworkAccessManager>

#include "reversegeocoder.h"

class YahooReverseGeocoder : public QObject, public ReverseGeocoder
{
    Q_OBJECT
public:
    YahooReverseGeocoder(ReverseGeocoderAction *action);
    virtual ~YahooReverseGeocoder();
    
    virtual void reverseGeocoding(const QVariant &userdata, float lat, float lon);

private slots:
    void onFinished(QNetworkReply *reply);

private:
    void parseProxyEnv(const QString &env, QString &hostName, quint16 &port);
    QString getAPIKeyFromXmlFile(const QString &filePath);

    QUrl createUrl(const float lat, const float lon);

    QNetworkAccessManager *manager;
    ReverseGeocoderAction *action;

    QHash<QNetworkReply *, QVariant> replyToUserDataHash;

    QString apiKey;
};

#endif
