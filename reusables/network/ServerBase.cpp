/*Created by Pierre-Olivier Beaudoin on 10-02-26.*/

#include <algorithm>
#include "ServerBase.h"
#include <QTcpServer>
#include <QTcpSocket>

using namespace realisim;
using namespace reusables;
using namespace network;
using namespace std;

ServerBase::ServerBase(QObject* ipParent /*=0*/) : QObject(ipParent),
mErrors(),
mPort(12345),
mpTcpServer(new QTcpServer(ipParent)),
mPeers()
{
  connect(mpTcpServer, SIGNAL(newConnection()),
   this, SLOT(handleNewConnection()));
}

ServerBase::~ServerBase()
{
  for(uint i = 0; i < mPeers.size(); ++i)
  {
    mPeers[i]->abort();
    delete mPeers[i];
  }
  mPeers.clear();
  
  stopServer();
  delete mpTcpServer;
}


//------------------------------------------------------------------------------
QStringList ServerBase::getPeersIps() const
{
  QStringList r;
  for(uint i = 0; i < mPeers.size(); ++i)
    r.push_back(mPeers[i]->peerAddress().toString());
  return r;
}

//------------------------------------------------------------------------------
QStringList ServerBase::getLastErrors(bool iClear /*= true*/)
{
  QStringList r = mErrors;
  if(iClear)
    mErrors.clear();
  return r;
}

//------------------------------------------------------------------------------
QString ServerBase::getAddress() const
{ return mpTcpServer->serverAddress().toString(); }

//------------------------------------------------------------------------------
void ServerBase::handleNewConnection()
{
  if(mpTcpServer->hasPendingConnections())
  {
    QTcpSocket* s = mpTcpServer->nextPendingConnection();
    connect(s, SIGNAL(error(QAbstractSocket::SocketError)),
      this, SLOT(tcpSocketError(QAbstractSocket::SocketError)));
    connect(s, SIGNAL(readyRead()), this, SLOT(readTcpSocket()));
    mPeers.push_back(s);
    emit newPeerConnected();
    
    //on envoit a tous les peers la liste des clients
    QByteArray a;
		a.append(getPeersIps().join(","));
    send(network::pPeersListChanged, a);
  }
}

//------------------------------------------------------------------------------
void ServerBase::readTcpSocket()
{}


#include <QFile>
//------------------------------------------------------------------------------
void ServerBase::send(Protocol iP, const QByteArray& iA)
{
  //Todo: faire un check sur la taille du message, il faudra
  //peut être le splitter en morceau...
  
  QByteArray block;
  QDataStream out(&block, QIODevice::WriteOnly);
  out.setVersion(QDataStream::Qt_4_0);
  //reserve a 16 bit integer that will contain the total 
  //size of the data block we are sending
  out << (quint16)0;
  //TODO out << (quint32)0xEE02FF04 //magic number header to determine format
  out << (qint32)network::kProtocolVersion;
  out << (quint32)iP;
  out << iA.data();
  out.device()->seek(0);
  out << (quint16)(block.size() - sizeof(quint16));

  //on envoit a tous les peers
  for(uint i = 0; i < mPeers.size(); ++i)
  {
    if(mPeers[i]->isValid())
      mPeers[i]->write(block);
  }   
}

//------------------------------------------------------------------------------
bool ServerBase::startServer()
{
	//early out. si le server écoute déja sur le port demandé
  if(mpTcpServer->isListening() && getPort() == mpTcpServer->serverPort())
    return true;

  bool r = true;
  if(mpTcpServer->isListening() && getPort() != mpTcpServer->serverPort())
    mpTcpServer->close();
  
  //Le server écoute pour toutes les adresses sur le port demandé
  if(!mpTcpServer->listen(QHostAddress::Any, getPort()))
  {
    r = false;
    mErrors.push_back(mpTcpServer->errorString());
    mpTcpServer->close();
  }
  
  return r;
}

//------------------------------------------------------------------------------
bool ServerBase::startServer(quint16 iPort)
{
  setPort(iPort);
  return startServer();
}

//------------------------------------------------------------------------------
void ServerBase::stopServer()
{
  if(mpTcpServer->isListening())
    mpTcpServer->close();
    
  //close all connectedPeers
  for(uint i = 0; i < mPeers.size(); ++i)
  {
    mPeers[i]->disconnectFromHost();
    delete mPeers[i];
  }
  mPeers.clear();
}

//------------------------------------------------------------------------------
void ServerBase::tcpSocketError(QAbstractSocket::SocketError iError)
{

  int socketId = -1;
  QTcpSocket* s = dynamic_cast<QTcpSocket*>(sender());
  if(s)
  {
    vector<QTcpSocket*>::iterator it = 
      find(mPeers.begin(), mPeers.end(), s);
    if(it != mPeers.end())
      socketId = std::distance(mPeers.begin(), it);
  }
  
  
  mErrors.push_back(network::asString(iError));
  emit error();
}