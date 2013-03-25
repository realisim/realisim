/*Created by Pierre-Olivier Beaudoin on 10-02-26.*/
 
#ifndef REALISIM_REUSABLE_NETWORK_CLIENTBASE_HH
#define REALISIM_REUSABLE_NETWORK_CLIENTBASE_HH

#include <cassert>
#include "network/utils.h"
#include <QHostAddress>
#include <QObject>
#include <QTcpSocket>
#include <vector>

/*
membres:
*/
namespace realisim 
{
namespace reusables 
{
namespace network
{

class ClientBase : public QObject
{
	Q_OBJECT
public:
  ClientBase(QObject* = 0);
  virtual ~ClientBase();

  virtual void connectToTcpServer(QString, quint16);
  virtual void disconnect();
  virtual QString getAndClearLastErrors() const;
virtual QByteArray getDownload() const;
virtual double getDownloadStatus() const;
  virtual int getMaximumUploadPayloadSize() const;
  virtual QString getLocalAddress() const { return mpTcpSocket->localAddress().toString() ; }
  virtual quint16 getLocalPort() const { return mpTcpSocket->localPort(); }
  virtual QString getHostAddress() const { return mTcpHostAddress.toString(); }
  virtual quint16 getHostPort() const { return mTcpHostPort; }
virtual double getUploadStatus() const;
virtual bool hasActiveUpload() const;
virtual bool hasDownload() const;
  virtual bool hasError() const;
  virtual bool isConnected() const;
virtual bool isDownloadCompleted() const;
//virtual bool isUploadCompleted( int ) const;
  virtual void setMaximumUploadPayloadSize( int );
  virtual void setTcpHostAddress(const QString iA) {mTcpHostAddress = iA;}
  virtual void setTcpHostPort(const quint16 iP) {mTcpHostPort = iP;}
  virtual void send( const QByteArray& );

signals:
void downloadStarted();
void downloadEnded();
	void gotError();
void gotPacket();
void sentPacket();
  void socketConnected();
  void socketDisconnected();
void uploadEnded();
void uploadStarted();

protected slots:
	virtual void handleSocketConnected();
	virtual void handleSocketDisconnected();
  virtual void handleSocketError(QAbstractSocket::SocketError);
  virtual void handleSocketReadyRead();
  virtual void handleSocketBytesWritten( qint64 );

protected:
  ClientBase(const ClientBase&){assert(0);}
  void operator=(const ClientBase&){assert(0);}
	virtual void addError( const QString& ) const;

  mutable QString mErrors;
  quint16 mTcpHostPort;
  QHostAddress mTcpHostAddress;
  QTcpSocket* mpTcpSocket; //jamais null
  int mMaximumUploadPayloadSize;
  Transfer mUpload;
  mutable Transfer mDownload;
};

}//network
}//reusable
}//realisim

#endif

 

