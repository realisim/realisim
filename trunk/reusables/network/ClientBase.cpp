/*Created by Pierre-Olivier Beaudoin on 10-02-26.*/

#include "ClientBase.h"
#include "network/utils.h"
#include <QHostInfo>
#include <QTcpSocket>

using namespace realisim;
using namespace reusables;
using namespace network;
using namespace std;

ClientBase::ClientBase(QObject* ipParent /*=0*/) : QObject(ipParent),
  mErrors(),
  mTcpHostPort(0),
  mTcpHostAddress("127.0.0.1"), //localhost
  mpTcpSocket(new QTcpSocket(ipParent)),
  mPeersList()
{
  connect(mpTcpSocket, SIGNAL(readyRead()),
    this, SLOT(readTcpSocket()));
  connect(mpTcpSocket, SIGNAL(error(QAbstractSocket::SocketError)),
    this, SLOT(tcpSocketError(QAbstractSocket::SocketError)));
}

ClientBase::~ClientBase()
{
  //delete client tcp socket
  if(mpTcpSocket)
  {
    mpTcpSocket->abort();
    delete mpTcpSocket;
  }
  mpTcpSocket = 0;
}

//------------------------------------------------------------------------------
/*Se connected au server situé à l'adresse iAdress qui est de la forme
  111.222.333.444 sur le port iPort.
  TODO: il faudrait ajouter la possibilité de se connecter a des URL
  www.google.com ou localhost (voir QHostInfo).*/
void ClientBase::connectToTcpServer(QString iAddress, quint16 iPort)
{
  //On prend pas de chance, on deconnecte le socket
  mpTcpSocket->abort();
	
  /*Si la connection échoue, le signal error(QAbstractSocket::SocketError)
    sera émit et capté par tcpSocketError. Ensuite un signal error() sera
    émis et le client peut aller consulter la liste des dernières erreurs
    par la méthode getLastErrors(bool) ou simplement la dernière erreur avec
    getLastError().*/
	mTcpHostPort = iPort;
  mTcpHostAddress = QHostAddress(iAddress);
  mpTcpSocket->connectToHost(mTcpHostAddress, mTcpHostPort);
}

//------------------------------------------------------------------------------
void ClientBase::disconnectFromTcpServer()
{
  if(mpTcpSocket)
    mpTcpSocket->disconnectFromHost();
}

//------------------------------------------------------------------------------
QStringList ClientBase::getLastErrors(bool iClearErrors /*= true*/)
{
  QStringList r = mErrors;
  if(iClearErrors)
    mErrors.clear();
  return r;
}

//------------------------------------------------------------------------------
void ClientBase::readTcpSocket()
{
  QDataStream in(mpTcpSocket);
  in.setVersion(QDataStream::Qt_4_0);
  //on va chercher la taille du message
  quint16 blockSize = 0;
  if (blockSize == 0)
  {
     if (mpTcpSocket->bytesAvailable() < (int)sizeof(quint16))
         return;

     in >> blockSize;
  }

  if (mpTcpSocket->bytesAvailable() < blockSize)
    return;

	//le numéro de version du Protocol
  qint32 protocolVersion = -1;
  in >> protocolVersion;
  if(protocolVersion != network::kProtocolVersion)
  {
    mErrors.push_back("Protocol version do not match. Message is skipped");
    emit error();
    return;
  }
  
  quint32 protocol;
  in >> protocol;
  char* pRawData;
  in >> pRawData;
  QByteArray a = QByteArray::fromRawData(pRawData, sizeof(pRawData));
  switch((Protocol)protocol)
  {
    case pPeersListChanged:
      {
        QString s(a.constData()); 
        mPeersList = s.split(",");
        emit peersListChanged();
      }
      break;
    default:
      //pass to client override's for their own protocol
      //readTcpSocket(a);
      break;
  }  
  delete[] pRawData;
}

//------------------------------------------------------------------------------
void ClientBase::tcpSocketError(QAbstractSocket::SocketError iError)
{
	mErrors.push_back(network::asString(iError));
  emit error();
}

//-----------------------------------------------------------------------------
void ClientBase::writeTest()
{
//  if(mpTcpSocket && mpTcpSocket->isValid())
//  {
//    QByteArray block;
//    QDataStream out(&block, QIODevice::WriteOnly);
//    out.setVersion(QDataStream::Qt_4_0);
//    out << (quint16)0;
//    out << QString("test string.");
//    out.device()->seek(0);
//    out << (quint16)(block.size() - sizeof(quint16));
//    mpTcpSocket->write(block);
//  }
}
