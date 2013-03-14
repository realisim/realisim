/*Created by Pierre-Olivier Beaudoin on 10-02-26.*/

#ifndef realisim_reusables_network_ServerBase_h
#define realisim_reusables_network_ServerBase_h

#include <cassert>
#include <QAbstractSocket>
#include <QObject>
#include <QStringList>
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
  virtual QString getAndClearLastErrors() const;
  virtual QString getLocalAddress() const;
  virtual quint16 getLocalPort() const {return mPort;}
virtual QByteArray getDownload( int ) const;
virtual double getDownloadStatus( int ) const;
  virtual int getNumberOfSockets() const;
  virtual QString getSocketPeerAddress( int ) const;
  virtual qint16 getSocketPeerPort( int ) const;
  virtual QAbstractSocket::SocketState getSocketState( int );
virtual bool hasDownload( int ) const;
  virtual bool hasError() const;
virtual bool isDownloadCompleted( int ) const;
  virtual void setLocalPort(const quint16 iP) {mPort = iP;}
  virtual void send( int, const QByteArray& );
  virtual bool startServer();
  virtual bool startServer(quint16);
  virtual void stopServer();  
  
signals:
void error();
	void gotPacket( int );
  void socketConnected( int );
  void socketDisconnected( int );
void downloadStarted( int );
void downloadEnded( int );

protected slots:
  virtual void handleNewConnection();
  virtual void handleSocketReadyRead();
  virtual void handleSocketDisconnected();
  virtual void handleSocketError(QAbstractSocket::SocketError);
  virtual void handleSocketStateChanged( QAbstractSocket::SocketState );
  
protected:
  ServerBase(const ServerBase&) {assert(0);}
  void operator=(const ServerBase&) {assert(0);}
  
  virtual void addError( QString ) const;
  virtual int findSocketFromSender( QObject* );
  virtual QTcpSocket* getSocket( int );
  virtual const QTcpSocket* getSocket( int ) const;
	virtual void readTcpSocket( int );
  virtual void socketStateChanged( int, QAbstractSocket::SocketState );

  mutable QString mErrors;
  quint16 mPort;
  QTcpServer* mpTcpServer;
  std::vector<QTcpSocket*> mSockets;
  mutable std::map< int, Transfer > mDownloads;

};

}
}
}

#endif
