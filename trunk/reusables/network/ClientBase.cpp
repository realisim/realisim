/*Created by Pierre-Olivier Beaudoin on 10-02-26.*/

#include "ClientBase.h"
#include <cmath>
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
  mMaximumPayloadSize( 64 * 1024 )
{
  connect(mpTcpSocket, SIGNAL( connected() ),
    this, SLOT( handleSocketConnected() ) );
  connect(mpTcpSocket, SIGNAL( disconnected() ),
    this, SLOT( handleSocketDisconnected() ) );
  connect(mpTcpSocket, SIGNAL(error(QAbstractSocket::SocketError)),
    this, SLOT( handleSocketError(QAbstractSocket::SocketError) ) );
  connect(mpTcpSocket, SIGNAL(readyRead()),
    this, SLOT( handleSocketReadyRead() ) );
  connect(mpTcpSocket, SIGNAL( bytesWritten( qint64 ) ),
    this, SLOT( handleSocketBytesWritten( qint64 ) ) );
}

ClientBase::~ClientBase()
{
  //delete client tcp socket
  mpTcpSocket->abort();
  delete mpTcpSocket;
  mpTcpSocket = 0;
}

//------------------------------------------------------------------------------
void ClientBase::addError( const QString& iE ) const
{
	if( !mErrors.isEmpty() ) mErrors += " ";
	mErrors += iE;
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
  if( mpTcpSocket->isValid() )
    mpTcpSocket->disconnectFromHost();
}

//------------------------------------------------------------------------------
QString ClientBase::getAndClearLastErrors() const
{
  QString r = mErrors;
  mErrors = QString();
  return r;
}

//------------------------------------------------------------------------------
int ClientBase::getMaximumPayloadSize() const
{ return mMaximumPayloadSize; }

//------------------------------------------------------------------------------
double ClientBase::getUploadStatus() const
{
	double r = 0.0;
  if( hasActiveUpload() )
  { r = 1.0 - ( mUpload.mPayload.size() / (double)mUpload.mTotalSize ); }
  return r;
}

//------------------------------------------------------------------------------
void ClientBase::handleSocketBytesWritten( qint64 iNumberOfBytesWritten )
{
printf( "handleSocketBytesWritten\n" );
	if( mUpload.mPayload.size() > 0 && 
  	mpTcpSocket->bytesToWrite() <= 4 * getMaximumPayloadSize() )
  {
  	QByteArray a = mUpload.mPayload.left( getMaximumPayloadSize() );
    mUpload.mPayload.remove( 0,  getMaximumPayloadSize() );
    int _a = mpTcpSocket->write( makePacket( a ) );
printf("byte written: %d\n", _a );
    emit sentPacket();
  }
  
  if( mUpload.mIsValid && !hasActiveUpload() )
  {
  	mUpload = Transfer();
printf( "uploadEnded:\n" );
  	emit uploadEnded();
  }
}

//------------------------------------------------------------------------------
void ClientBase::handleSocketConnected()
{ emit socketConnected(); }

//------------------------------------------------------------------------------
void ClientBase::handleSocketDisconnected()
{ emit socketDisconnected(); }

//------------------------------------------------------------------------------
void ClientBase::handleSocketError(QAbstractSocket::SocketError iError)
{ 
  addError( "Error on socket: " +	network::asString(iError) );
	emit gotError();
}

//------------------------------------------------------------------------------
void ClientBase::handleSocketReadyRead()
{
}

//------------------------------------------------------------------------------
bool ClientBase::hasActiveUpload() const
{ return !mUpload.mPayload.isEmpty(); }

//------------------------------------------------------------------------------
bool ClientBase::hasError() const
{ return !mErrors.isEmpty(); }

//------------------------------------------------------------------------------
bool ClientBase::isConnected() const
{ return mpTcpSocket->state() == QAbstractSocket::ConnectedState; }

//-----------------------------------------------------------------------------
void ClientBase::send( const QByteArray& iA )
{
	if( mpTcpSocket->isValid() )
  {
  	mUpload.mIsValid = true;
    mUpload.mTotalSize = iA.size();
  	mUpload.mPayload = iA;
    QByteArray header = makeUploadHeader( mUpload.mPayload );
    mpTcpSocket->write( makePacket( header ) );
    emit uploadStarted();
  }
}

//------------------------------------------------------------------------------
void ClientBase::setMaximumPayloadSize( int iSize )
{ mMaximumPayloadSize = iSize; }

//-----------------------------------------------------------------------------
void ClientBase::writeTest()
{
  if( mpTcpSocket->isValid() )
  {
  	QByteArray a = makePacket( QString("ウィキペディアへようこそééaaê").toUtf8() );
    mpTcpSocket->write( a );
  }
}
