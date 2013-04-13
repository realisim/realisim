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
      
      int getId() const { return mId; }
      bool isValid() const { return mIsValid; }
      void setPayload( const QByteArray&, int );
      
      bool mIsValid;
			quint16 mVersion;
      qint32 mId;
			quint32 mTotalSize;
      QByteArray mPayload;
      quint32 mCursor;
  };
	
  QByteArray makePacket( const QByteArray&, int );
  QByteArray makeUploadHeader( const Transfer& );
void printAsHex( const QByteArray& );
  QByteArray readPacket( QTcpSocket*, int* );
  Transfer readUploadHeader( const QByteArray& );
  
}//network
}//reusable
}//realisim

#endif

 

