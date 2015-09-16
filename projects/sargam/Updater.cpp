/* */

#include "Updater.h"

using namespace realisim;
  using namespace sargam;

Updater::Updater(QObject* ipParent) : QObject(ipParent)
{
  mpAccess = new QNetworkAccessManager(this);
  connect( mpAccess, SIGNAL(finished(QNetworkReply*)),
          this, SLOT(replyFinished(QNetworkReply*)) );
}

Updater::~Updater()
{}
//---------------------------------------------------------------------
void Updater::checkForUpdate()
{
  QNetworkRequest r;
  r.setSslConfiguration(QSslConfiguration::defaultConfiguration());
  //r.setUrl(QUrl("https://sargamdev.wordpress.com/Releases"));
  //r.setUrl(QUrl("http://stackoverflow.com/questions/26565666/cannot-retrieve-page-contents-through-https-with-qnetworkaccessmanager"));
  //r.setUrl(QUrl("https://www.google.ca/"));
  //r.setUrl(QUrl("https://www.reddit.com/"));
  r.setUrl(QUrl("https://github.com/realisim/realisim"));
  mpAccess->get(r);
}
//---------------------------------------------------------------------
void Updater::replyFinished(QNetworkReply* ipReply)
{
  QNetworkReply::NetworkError e = ipReply->error();
  if( e == QNetworkReply::NoError )
  {
    QString content( ipReply->readAll() );
    printf( "Url: %s\n%s\n", ipReply->url().toString().toStdString().c_str(),
           content.toStdString().c_str() );
  }
  ipReply->deleteLater();
}