
#include "utilities.h"
#include <QDateTime>
#include <QFile>
//#include <QNetworkInterface>
#include <QString>

namespace realisim
{
namespace utils
{
//-----------------------------------------------------------------------------	
QByteArray fromFile(const QString& iFile)
{
	QByteArray r;
  QFile f(iFile);
  if(f.open(QIODevice::ReadOnly))
  {
  	r = f.readAll();
    f.close();
  }
  return r;
}

//a deplacer dans netWorkutils...
//-----------------------------------------------------------------------------
//QString getGuid()
//{
//	QString r;
//  r = getMacAddress();
//  if( !r.isEmpty() )
//  { r += QDateTime::currentDateTime().toString("yyyy-MM-dd_hh:mm:ss:zzz"); }
//  return r;
//}
//-----------------------------------------------------------------------------
//QString getMacAddress()
//{
//  foreach(QNetworkInterface netInterface, QNetworkInterface::allInterfaces())
//  {
//			if( !(netInterface.flags() & QNetworkInterface::IsLoopBack) &&
//      	!netInterface.hardwareAddress().isEmpty() )
//      { return netInterface.hardwareAddress(); }
//  }
//  return QString();
//}
//-----------------------------------------------------------------------------
bool toFile(const QString& iFile , const QByteArray& iData)
{
	bool r = false;
	QFile f(iFile);
  if(f.open(QIODevice::WriteOnly))
  {
  	qint64 bytesWritten = f.write(iData.constData(), iData.size());
    if(bytesWritten == iData.size())
    	r = true;
    f.close();
  }
  return r;
}

  
}
}