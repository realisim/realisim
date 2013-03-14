/*Created by Pierre-Olivier Beaudoin on 10-02-26.*/
 
#ifndef REALISIM_REUSABLE_NETWORK_CLIENTBASE_HH
#define REALISIM_REUSABLE_NETWORK_CLIENTBASE_HH

#include <cassert>
#include "network/utils.h"
#include <QHostAddress>
#include <QObject>
#include <QStringList>
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
  virtual void disconnectFromTcpServer();
  virtual QString getAndClearLastErrors() const;
  virtual int getMaximumPayloadSize() const;
  virtual const quint16 getTcpHostPort() const {return mTcpHostPort;}
  virtual const QString getTcpHostAddress() const {return mTcpHostAddress.toString();}
virtual double getUploadStatus() const;
virtual bool hasActiveUpload() const;
  virtual bool hasError() const;
  virtual bool isConnected() const;
  virtual void setMaximumPayloadSize( int );
  virtual void setTcpHostAddress(const QString iA) {mTcpHostAddress = iA;}
  virtual void setTcpHostPort(const quint16 iP) {mTcpHostPort = iP;}
  virtual void send( const QByteArray& );
  
void writeTest();

signals:
	void gotError();
void sentPacket();
  void socketConnected();
  void socketDisconnected();
void uploadStarted();
void uploadEnded();

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
  int mMaximumPayloadSize;
  Transfer mUpload;
};

}//network
}//reusable
}//realisim

#endif

 

