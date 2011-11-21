
#include "utilities.h"
#include <QFile>
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