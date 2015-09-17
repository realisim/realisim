/* */

#include "Updater.h"
#include <QRegularExpression>
#include <QRegularExpressionMatch>

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
  r.setUrl(QUrl("https://raw.githubusercontent.com/realisim/realisim/sargamReleaseNotes/sargamReleaseNotes.txt"));
  mpAccess->get(r);
}
//---------------------------------------------------------------------
void Updater::replyFinished(QNetworkReply* ipReply)
{
  QNetworkReply::NetworkError e = ipReply->error();
  if( e == QNetworkReply::NoError )
  {
    QString content( ipReply->readAll() );
    
    QStringList releaseTags;
    
    QRegularExpression re("(<release>.*?</release>)", QRegularExpression::DotMatchesEverythingOption);
    QRegularExpressionMatchIterator it = re.globalMatch(content);
    while(it.hasNext())
    {
      QRegularExpressionMatch m = it.next();
      printf("%s\n", m.captured(1).toStdString().c_str() );
    }
  }
  else
  {
    //log some error
  }
  ipReply->deleteLater();
}