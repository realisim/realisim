/* Created by Pierre-Olivier Beaudoin on 10-02-13. */
 
#ifndef REALISIM_REUSABLE_NETWORK_UTILS_HH
#define REALISIM_REUSABLE_NETWORK_UTILS_HH

#include <QAbstractSocket>
#include <qlist.h>
#include <QStringList>
class QByteArray;
class QString;
class QTcpSocket;


/*
*/
namespace realisim 
{
namespace reusables 
{
namespace network
{
  QStringList getLocalIpAddresses();
  QString asString(QAbstractSocket::SocketError);
  
  class Transfer
  {
  	public:
    	Transfer();
      virtual ~Transfer();
      
      void setPayload( const QByteArray& );
      
      bool mIsValid;
			quint16 mVersion;
			quint32 mTotalSize;
      QByteArray mPayload;
  };
	
  QByteArray makePacket( const QByteArray& );
  QByteArray makeUploadHeader( const Transfer& );
void printAsHex( const QByteArray& );
  QByteArray readPacket( QTcpSocket* );
  Transfer readUploadHeader( const QByteArray& );
  
}//network
}//reusable
}//realisim

#endif

 

