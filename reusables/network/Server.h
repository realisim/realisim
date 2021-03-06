/*Created by Pierre-Olivier Beaudoin on 10-02-26.*/

#ifndef realisim_reusables_network_Server_h
#define realisim_reusables_network_Server_h

#include <cassert>
#include <QAbstractSocket>
#include <QObject>
class QTcpServer;
class QTcpSocket;
#include "network/utils.h"
#include "utils/log.h"
#include <vector>

namespace realisim
{
namespace network
{

/*
	Le signal socketAboutToDisconnect() est emit juste avant d'effacer les informations
  reliées au socket. La connexion tcp est terminée à ce moment. Par contre, Il
  est encore possible aux clients connectés à ce signal d'interroger les
  méthodes consernants le socket ( par exemple: getSocketPeerAddress, 
  getDownload, getDownloadStatus etc... ). La méthode getNumberOfSockets ne
  réflète que le socket est toujours présent.
  Le signal socketDisconnect() est emit lorsque le socket est completement
  enlevé du serveur.
*/
class Server : public QObject
{
  Q_OBJECT;
public:
  Server(QObject* = 0);
  virtual ~Server();
  
  virtual void broadcast( const QByteArray& );
  virtual void broadcast( const QByteArray&, int );
  virtual QString getAndClearLastErrors() const;
	virtual QByteArray getDownload( int, int ) const;
	virtual int getDownloadId( int, int ) const;
	virtual double getDownloadStatus( int, int ) const;
  virtual QString getLocalAddress() const;
  virtual quint16 getLocalPort() const {return mPort;}
  virtual const utils::Log& getLog() const {return *mpLog;}
	virtual int getMaximumUploadPayloadSize() const;
	virtual int getNumberOfDownloads( int ) const;
  virtual int getNumberOfSockets() const;
	virtual int getNumberOfUploads( int ) const;
  virtual transferProtocol getProtocol() const {return mProtocol;}
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
  virtual void setLog( utils::Log* );
	virtual void setMaximumUploadPayloadSize( int );
  virtual void setProtocol( transferProtocol p ) {mProtocol = p;}
  virtual void send( int, const QByteArray& );
  virtual bool startServer();
  virtual bool startServer(quint16);
  virtual void stopServer();  
  
signals:
	void errorRaised() const;
	void downloadEnded( int, int );
	void gotPacket( int, int );
	void sentPacket( int, int );
  void socketAboutToDisconnect( int );
  void socketConnected( int );
  void socketDisconnected( int );
	void uploadEnded( int, int );


protected slots:
  virtual void handleNewConnection();
  virtual void handleSocketBytesWritten( qint64 );
  virtual void handleSocketReadyRead();
  virtual void handleSocketDisconnected();
  virtual void handleSocketError(QAbstractSocket::SocketError);
  virtual void handleSocketStateChanged( QAbstractSocket::SocketState );
  
protected:
  Server(const Server&) {assert(0);}
  void operator=(const Server&) {assert(0);}
  
  virtual void addError( QString ) const;
  virtual int findSocketFromSender( QObject* );
  virtual int findDownload( int, int ) const;
  virtual int findUpload( int, int ) const;
  virtual void handleReadBuffer( int );
  virtual void socketStateChanged( int, QAbstractSocket::SocketState );

  mutable QString mErrors;
  quint16 mPort;
  QTcpServer* mpTcpServer;
  std::vector< QTcpSocket* > mSockets;
  std::vector< QByteArray > mReadBuffers;
  mutable std::map< int, std::vector< Transfer > > mDownloads;
  mutable std::map< int, std::vector< Transfer > > mUploads;
  int mMaximumUploadPayloadSize;
  static int mUploadId;
  std::vector< int > mUploadIndices;
	transferProtocol mProtocol;
  mutable utils::Log* mpLog;
  mutable utils::Log mDefaultLog;
};

}
}

#endif
