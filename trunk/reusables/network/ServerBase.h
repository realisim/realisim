/*Created by Pierre-Olivier Beaudoin on 10-02-26.*/

#ifndef realisim_reusables_network_ServerBase_h
#define realisim_reusables_network_ServerBase_h

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
  
  virtual quint16 getPort() const {return mPort;}
  virtual QStringList getPeersIps() const;
  virtual QString getLastError() const {return mErrors[mErrors.size() - 1];}
  virtual QStringList getLastErrors(bool = true);
  virtual QString getAddress() const;
  virtual void setPort(const quint16 iP) {mPort = iP;}
  virtual void send(Protocol, const QByteArray&);
  virtual bool startServer();
  virtual bool startServer(quint16);
  virtual void stopServer();  
  
signals:
  void error();
  void newPeerConnected();
//void readyRead(int, const QDataStream&);

protected slots:
  virtual void handleNewConnection();
  virtual void readTcpSocket();
  virtual void tcpSocketError(QAbstractSocket::SocketError);
  
protected:
  QStringList mErrors;
  quint16 mPort;
  QTcpServer* mpTcpServer;
  std::vector<QTcpSocket*> mPeers;

private:
  ServerBase(const ServerBase&) {assert(0);}
  void operator=(const ServerBase&) {assert(0);}
};

}
}
}

#endif
