/* */

#include "Updater.h"
#include <QRegularExpression>
#include <QRegularExpressionMatch>

using namespace realisim;
  using namespace sargam;

Updater::Updater(QObject* ipParent) : QObject(ipParent)
{
  mpUpdateAccess = new QNetworkAccessManager(this);
  connect( mpUpdateAccess, SIGNAL(finished(QNetworkReply*)),
          this, SLOT(handleVersionUpdates(QNetworkReply*)) );
  
  mpTickCounterAccess =new QNetworkAccessManager(this);
  connect( mpTickCounterAccess, SIGNAL(finished(QNetworkReply*)),
          this, SLOT(handleTickRemoteCounter(QNetworkReply*)) );
}

Updater::~Updater()
{}
//---------------------------------------------------------------------
void Updater::checkForUpdate()
{
  QNetworkRequest r;
  r.setSslConfiguration(QSslConfiguration::defaultConfiguration());
  
  r.setUrl(QUrl("https://raw.githubusercontent.com/realisim/realisim/master/projects/sargam/releaseNotes/sargamReleaseNotes.txt"));
  //r.setUrl(QUrl("https://raw.githubusercontent.com/realisim/realisim/master/projects/sargam/releaseNotes/sargamReleaseNotes-preprod.txt"));
  
  //r.setUrl(QUrl("https://raw.githubusercontent.com/realisim/realisim/sargamReleaseNotes/sargamReleaseNotes.txt"));
  //r.setUrl(QUrl("https://raw.githubusercontent.com/realisim/realisim/sargamReleaseNotes/sargamReleaseNotes-preprod.txt"));
  mpUpdateAccess->get(r);
}
//---------------------------------------------------------------------
QStringList Updater::fetchTagContent(QString iTag, QString iString) const
{
  QStringList r;
  //printf("iString: %s\n\n", iString.toStdString().c_str());
  
  QString openingTag = "<" + iTag + ">";
  QString closingTag = "</" + iTag + ">";
  QString pattern = "(" + openingTag + ".*?" + closingTag + ")";
  QRegularExpression re(pattern, QRegularExpression::DotMatchesEverythingOption);
  QRegularExpressionMatchIterator it = re.globalMatch(iString);
  while(it.hasNext())
  {
    QRegularExpressionMatch m = it.next();
    QString s = m.captured(1);
    s.remove(openingTag);
    s.remove(closingTag);
    r.push_back(s);
  }
  return r;
}
//---------------------------------------------------------------------
QString Updater::getDownloadPage() const
{ return mDownloadPage; }
//---------------------------------------------------------------------
int Updater::getNumberOfVersions() const
{ return mVersions.size(); }
//---------------------------------------------------------------------
QString Updater::getReleaseNotes(int i) const
{ return mVersions[i].mReleaseNotes; }
//---------------------------------------------------------------------
QString Updater::getVersionAsQString(int i) const
{ return mVersions[i].mVersion; }
//---------------------------------------------------------------------
void Updater::handleVersionUpdates(QNetworkReply* ipReply)
{
  mVersions.clear();
  
  QNetworkReply::NetworkError e = ipReply->error();
  if( e == QNetworkReply::NoError )
  {
    QString content( ipReply->readAll() );
    
    QStringList d = fetchTagContent("downloadPage", content);
    if(d.size()){ mDownloadPage = d[0]; }
    
    QStringList releaseTags = fetchTagContent("release", content);
    for( int i = 0; i < releaseTags.size(); ++i)
    {
      QStringList versions = fetchTagContent("version", releaseTags[i]);
      QStringList releaseNotes = fetchTagContent("releaseNotes", releaseTags[i]);
      
      //on suppose qu'il n'y a q'une seule version et releaseNotes par
      //release
      if( versions.size() && releaseNotes.size() )
      { mVersions.push_back( VersionInfos(versions[0], releaseNotes[0]) ); }
    }
    
    emit updateInformationAvailable();
  }
  else
  {
    //log some error
  }
  ipReply->deleteLater();
}
//---------------------------------------------------------------------
void Updater::handleTickRemoteCounter(QNetworkReply* ipReply)
{
  
  QNetworkReply::NetworkError e = ipReply->error();
  if( e == QNetworkReply::NoError )
  {
    printf("remote counter ticked!\n%s", ipReply->readAll().toStdString().c_str());
  }
  else
  {
    printf("remote counter ticked: an error occured, %d\n", ipReply->error());
  }
  ipReply->deleteLater();
}
//---------------------------------------------------------------------
void Updater::tickRemoteCounter()
{
//#ifndef NDEBUG
  QNetworkRequest r;
  r.setUrl(QUrl("http://sargam.com.s3-website-us-east-1.amazonaws.com/sargamUsageCounter.html"));
  
  mpTickCounterAccess->get(r);
//#endif
}