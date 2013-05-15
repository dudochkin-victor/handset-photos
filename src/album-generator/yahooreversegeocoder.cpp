#include <QProcessEnvironment>
#include <QNetworkProxy>
#include <QXmlStreamReader>
#include <QFile>
#include <QDir>

#include "yahooreversegeocoder.h"

YahooReverseGeocoder::YahooReverseGeocoder(ReverseGeocoderAction *_action) : action(_action)
{
    manager = new QNetworkAccessManager(this);
    apiKey = getAPIKeyFromXmlFile(QDir::homePath().append("/.apikey.xml"));
    QString httpProxyEnv = QProcessEnvironment::systemEnvironment().value("http_proxy");
    if(!httpProxyEnv.isEmpty()) {
        QString hostName;
        quint16 port;
        parseProxyEnv(httpProxyEnv, hostName, port);
        QNetworkProxy proxy(QNetworkProxy::HttpProxy, hostName, port);
        manager->setProxy(proxy);
    }

    QObject::connect(manager, SIGNAL(finished(QNetworkReply*)),this, SLOT(onFinished(QNetworkReply*)));

    // QUrl url("http://query.yahooapis.com/v1/public/yql?q=select place from flickr.places where lat=" + lat +" and lon=" + lon);
    //           reply = manager->get(QNetworkRequest(url));
    //
}

YahooReverseGeocoder::~YahooReverseGeocoder() {
    delete manager;
}
 
void YahooReverseGeocoder::reverseGeocoding(const QVariant &userdata, float lat, float lon)
{
    QUrl url = createUrl(lat, lon);
    qDebug() << "paul debug lat" << lat << "long " << lon << "url" << url;
    QNetworkReply *reply = manager->get(QNetworkRequest(url));
    qDebug() << "paul debug reverseGeocoding";
    replyToUserDataHash[reply] = userdata;
}

void YahooReverseGeocoder::onFinished(QNetworkReply *reply) 
{
    qDebug() << "paul debug geocoder on finished";
    if (!reply)
        return;
    QVariant userdata = replyToUserDataHash[reply];
    QString photoUrn = userdata.toString();
    QByteArray content = reply->readAll();
    qDebug() << "paul debug geocoder onfinsihed content " << QString(content) << "bytes:" << reply->bytesAvailable();
    QXmlStreamReader xmlReader(content);
    QString city;
    while (!xmlReader.atEnd()) {
        xmlReader.readNext();
        if (xmlReader.name() == "city") {
            city = xmlReader.readElementText();
            break;
        }
    }

    if (!city.isEmpty()) {
        qDebug() << "paul debug city:" << city;
        action->onTranslated(userdata, city);
    }
    replyToUserDataHash.remove(reply);
    reply->deleteLater();
}

void YahooReverseGeocoder::parseProxyEnv(const QString &env, QString &hostName, quint16 &port)
{
    QUrl x(env);
    hostName = x.host();
    port = x.port();
}

QUrl YahooReverseGeocoder::createUrl(const float lat, const float lon)
{
    QString urlStringTemplate = QString("http://where.yahooapis.com/geocode?q=%1,+%2&gflags=R&appid=[") + apiKey + "]";
    QString urlString = urlStringTemplate.arg(lat).arg(lon);
    return QUrl(urlString);
}

QString YahooReverseGeocoder::getAPIKeyFromXmlFile(const QString &filePath)
{
    QFile file(filePath);
    file.open(QIODevice::ReadOnly);
    QXmlStreamReader reader(&file);
    QString key;
    bool inElement = false;
    while (!reader.atEnd()) {
        reader.readNext();
        if (reader.isStartElement() && reader.name() == "YahooPlaceFinderAPIKey") {
            inElement = true;
        } else if (reader.isEndElement() && reader.name() == "YahooPlaceFinderAPIKey") {
            inElement = false;
        } else {
            if (inElement) {
                key = reader.text().toString();
                break;
            }
        }
    }
    file.close();
    return key;
}
