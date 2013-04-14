/*Created by Pierre-Olivier Beaudoin on 10-02-26.*/
 
#ifndef REALISIM_REUSABLE_NETWORK_Client_HH
#define REALISIM_REUSABLE_NETWORK_Client_HH

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

class Client : public QObject
{
	Q_OBJECT
public:
  Client(QObject* = 0);
  virtual ~Client();

  virtual void connectToTcpServer(QString, quint16);
  virtual void disconnect();
  virtual QString getAndClearLastErrors() const;
	virtual QByteArray getDownload( int ) const;
	virtual int getDownloadId( int ) const;
	virtual double getDownloadStatus( int ) const;
  virtual int getMaximumUploadPayloadSize() const;
	virtual int getNumberOfDownloads() const;
	virtual int getNumberOfUploads() const;
  virtual QString getLocalAddress() const { return mpTcpSocket->localAddress().toString() ; }
  virtual quint16 getLocalPort() const { return mpTcpSocket->localPort(); }
  virtual QString getHostAddress() const { return mTcpHostAddress.toString(); }
  virtual quint16 getHostPort() const { return mTcpHostPort; }
	virtual QByteArray getUpload(int) const;  
	virtual int getUploadId( int ) const;
	virtual double getUploadStatus( int ) const;
	virtual bool hasDownloads() const;
	virtual bool hasUploads() const;
  virtual bool hasError() const;
  virtual bool isConnected() const;
  virtual void setMaximumUploadPayloadSize( int );
  virtual void setTcpHostAddress(const QString iA) {mTcpHostAddress = iA;}
  virtual void setTcpHostPort(const quint16 iP) {mTcpHostPort = iP;}
  virtual void send( const QByteArray& );

signals:
	void downloadStarted( int );
	void downloadEnded( int );
	void gotError();
	void gotPacket( int );
	void sentPacket( int );
  void socketConnected();
  void socketDisconnected();
	void uploadEnded( int );
	void uploadStarted( int );

protected slots:
	virtual void handleSocketConnected();
	virtual void handleSocketDisconnected();
  virtual void handleSocketError(QAbstractSocket::SocketError);
  virtual void handleSocketReadyRead();
  virtual void handleSocketBytesWritten( qint64 );

protected:
  Client(const Client&){assert(0);}
  void operator=(const Client&){assert(0);}
	virtual void addError( const QString& ) const;
  virtual int findDownload( int ) const;
  virtual int findUpload( int ) const;

  mutable QString mErrors;
  quint16 mTcpHostPort;
  QHostAddress mTcpHostAddress;
  QTcpSocket* mpTcpSocket; //jamais null
  int mMaximumUploadPayloadSize;
  std::vector< Transfer > mUploads;
  mutable std::vector< Transfer > mDownloads;
  static int mUploadId;
  int mUploadIndex;
};

}//network
}//reusable
}//realisim

#endif

 

