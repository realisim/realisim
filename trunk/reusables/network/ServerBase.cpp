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
  
  switch (iError) 
  {
    case QAbstractSocket::ConnectionRefusedError :
    {mErrors.push_back("The connection was refused by the peer (or timed out)."); break;}
    case QAbstractSocket::RemoteHostClosedError :
    {mErrors.push_back("The remote host closed the connection."); break;}
		case QAbstractSocket::HostNotFoundError :
    {mErrors.push_back("The host address was not found."); break;}
    case QAbstractSocket::SocketAccessError :
    {mErrors.push_back("The socket operation failed because the application lacked the required privileges."); break;}
    case QAbstractSocket::SocketResourceError :
    {mErrors.push_back("The local system ran out of resources (e.g., too many sockets)."); break;}
    case QAbstractSocket::DatagramTooLargeError :
    {mErrors.push_back("The datagram was larger than the operating system's limit (which can be as low as 8192 bytes)."); break;}
    case QAbstractSocket::NetworkError :
    {mErrors.push_back("An error occurred with the network (e.g., the network cable was accidentally plugged out)."); break;}
    case QAbstractSocket::AddressInUseError :
    {mErrors.push_back("The address specified to QUdpSocket::bind() is already in use and was set to be exclusive."); break;}
    case QAbstractSocket::SocketAddressNotAvailableError :
    {mErrors.push_back("The address specified to QUdpSocket::bind() does not belong to the host."); break;}
    case QAbstractSocket::UnsupportedSocketOperationError :
    {mErrors.push_back("The requested socket operation is not supported by the local operating system (e.g., lack of IPv6 support)."); break;}
    case QAbstractSocket::ProxyAuthenticationRequiredError :
    {mErrors.push_back("The socket is using a proxy, and the proxy requires authentication."); break;}
    case QAbstractSocket::SslHandshakeFailedError :
    {mErrors.push_back("The SSL/TLS handshake failed, so the connection was closed (only used in QSslSocket)"); break;}
    case QAbstractSocket::UnfinishedSocketOperationError :
    {mErrors.push_back("Used by QAbstractSocketEngine only, The last operation attempted has not finished yet (still in progress in the background)."); break;}
/*  Only in QT 4.5 and higher
    case QAbstractSocket::ProxyConnectionRefusedError :
    {mErrors.push_back("Could not contact the proxy server because the connection to that server was denied"); break;}
    case QAbstractSocket::ProxyConnectionClosedError :
    {mErrors.push_back("The connection to the proxy server was closed unexpectedly (before the connection to the final peer was established)"); break;}
    case QAbstractSocket::ProxyConnectionTimeoutError :
    {mErrors.push_back("The connection to the proxy server timed out or the proxy server stopped responding in the authentication phase."); break;}
    case QAbstractSocket::ProxyNotFoundError :
    {mErrors.push_back("The proxy address set with setProxy() (or the application proxy) was not found."); break;}
    case QAbstractSocket::ProxyProtocolError :
    {mErrors.push_back("The connection negotiation with the proxy server because the response from the proxy server could not be understood."); break;}*/
    case QAbstractSocket::UnknownSocketError :
    {mErrors.push_back("An unidentified error occurred."); break;}

    default: mErrors.push_back("Unknown unidentified out of no where error occured..."); break;
  }
  
  emit error();
}