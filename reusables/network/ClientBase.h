/*Created by Pierre-Olivier Beaudoin on 10-02-26.*/
 
#ifndef REALISIM_REUSABLE_NETWORK_CLIENTBASE_HH
#define REALISIM_REUSABLE_NETWORK_CLIENTBASE_HH

#include <cassert>
#include <QHostAddress>
#include <QObject>
#include <QStringList>
class QTcpSocket;
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
  virtual QString getLastError() const {return mErrors[mErrors.size() - 1];}
  virtual QStringList getLastErrors(bool = true);
virtual QStringList getPeersList() const {return mPeersList;}
  virtual const quint16 getTcpHostPort() const {return mTcpHostPort;}
  virtual const QString getTcpHostAddress() const {return mTcpHostAddress.toString();}

  virtual void setTcpHostAddress(const QString iA) {mTcpHostAddress = iA;}
  virtual void setTcpHostPort(const quint16 iP) {mTcpHostPort = iP;}
  
void writeTest();

signals:
  void error();
  void peersListChanged();

protected slots:
  virtual void readTcpSocket();
  virtual void tcpSocketError(QAbstractSocket::SocketError);

protected:
  QStringList mErrors;
  quint16 mTcpHostPort;
  QHostAddress mTcpHostAddress;
  QTcpSocket* mpTcpSocket;
  QStringList mPeersList;
  
private:
  ClientBase(const ClientBase&){assert(0);}
  void operator=(const ClientBase&){assert(0);}
};

}//network
}//reusable
}//realisim

#endif

 

