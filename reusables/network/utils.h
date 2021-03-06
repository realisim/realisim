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
namespace network
{
  QString asString(QAbstractSocket::SocketError);
  QString getGuid();
  QStringList getLocalIpAddresses();
  QString getMacAddress();
  
  enum transferProtocol{ tpRaw, tpRealisim, tpNumberOfProtocols };
   
  class Transfer
  {
  	public:
    	Transfer();
      virtual ~Transfer();
      
      int getId() const { return mId; }
      bool isValid() const { return mIsValid; }
      const QByteArray& getPayload() const { return mPayload; }
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
  QByteArray readPacket( QByteArray&, int* );
  Transfer readUploadHeader( const QByteArray& );
  
}//network
}//realisim

#endif

 

