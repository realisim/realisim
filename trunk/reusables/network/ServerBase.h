/*Created by Pierre-Olivier Beaudoin on 10-02-26.*/

#ifndef realisim_reusables_network_ServerBase_h
#define realisim_reusables_network_ServerBase_h

#include <cassert>
#include <QAbstractSocket>
#include <QObject>
class QTcpServer;
class QTcpSocket;
#include "network/utils.h"
#include <vector>

namespace realisim
{
namespace reusables
{
namespace network
{

class ServerBase : public QObject
{
  Q_OBJECT;
public:
  ServerBase(QObject* = 0);
  virtual ~ServerBase();
  
  virtual void broadcast( const QByteArray& );
  virtual void broadcast( const QByteArray&, int );
  virtual QString getAndClearLastErrors() const;
	virtual QByteArray getDownload( int, int ) const;
	virtual int getDownloadId( int, int ) const;
	virtual double getDownloadStatus( int, int ) const;
  virtual QString getLocalAddress() const;
  virtual quint16 getLocalPort() const {return mPort;}
	virtual int getMaximumUploadPayloadSize() const;
	virtual int getNumberOfDownloads( int ) const;
  virtual int getNumberOfSockets() const;
	virtual int getNumberOfUploads( int ) const;
	virtual QTcpSocket* getSocket( int );
	virtual const QTcpSocket* getSocket( int ) const;
  virtual QString getSocketPeerAddress( int ) const;
  virtual qint16 getSocketPeerPort( int ) const;
  virtual QAbstractSocket::SocketState getSocketState( int );
	virtual QByteArray getUpload( int, int ) const;
	virtual int getUploadId( int, int ) const;
	virtual double getUploadStatus( int, int ) const;
	virtual bool hasDownloads( int ) const;
  virtual bool hasError() const;
	virtual bool hasUploads( int ) const;
  virtual void setLocalPort(const quint16 iP) {mPort = iP;}
	virtual void setMaximumUploadPayloadSize( int );
  virtual void send( int, const QByteArray& );
  virtual bool startServer();
  virtual bool startServer(quint16);
  virtual void stopServer();  
  
signals:
	void error();
	void downloadStarted( int, int );
	void downloadEnded( int, int );
	void gotPacket( int, int );
	void sentPacket( int, int );
  void socketConnected( int );
  void socketDisconnected( int );
	void uploadEnded( int, int );
	void uploadStarted( int, int );


protected slots:
  virtual void handleNewConnection();
  virtual void handleSocketBytesWritten( qint64 );
  virtual void handleSocketReadyRead();
  virtual void handleSocketDisconnected();
  virtual void handleSocketError(QAbstractSocket::SocketError);
  virtual void handleSocketStateChanged( QAbstractSocket::SocketState );
  
protected:
  ServerBase(const ServerBase&) {assert(0);}
  void operator=(const ServerBase&) {assert(0);}
  
  virtual void addError( QString ) const;
  virtual int findSocketFromSender( QObject* );
  virtual int findDownload( int, int ) const;
  virtual int findUpload( int, int ) const;
  virtual void socketStateChanged( int, QAbstractSocket::SocketState );

  mutable QString mErrors;
  quint16 mPort;
  QTcpServer* mpTcpServer;
  std::vector< QTcpSocket* > mSockets;
  mutable std::map< int, std::vector< Transfer > > mDownloads;
  mutable std::map< int, std::vector< Transfer > > mUploads;
  int mMaximumUploadPayloadSize;
  static int mUploadId;
  std::vector< int > mUploadIndices;

};

}
}
}

#endif
