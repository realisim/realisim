/*Created by Pierre-Olivier Beaudoin on 10-02-26.*/

#include "Client.h"
#include <cmath>
#include "network/utils.h"
#include <QHostInfo>
#include <QTcpSocket>

using namespace realisim;
using namespace reusables;
using namespace network;
using namespace std;

int Client::mUploadId = 0;

Client::Client(QObject* ipParent /*=0*/) : QObject(ipParent),
  mErrors(),
  mTcpHostPort(0),
  mTcpHostAddress("127.0.0.1"), //localhost
  mpTcpSocket(new QTcpSocket(ipParent)),
  mMaximumUploadPayloadSize( 64 * 1024 )
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

Client::~Client()
{
  //delete client tcp socket
  mpTcpSocket->abort();
  delete mpTcpSocket;
  mpTcpSocket = 0;
}

//------------------------------------------------------------------------------
void Client::addError( const QString& iE ) const
{
	if( !mErrors.isEmpty() ) mErrors += " ";
	mErrors += iE;
}

//------------------------------------------------------------------------------
/*Se connected au server situé à l'adresse iAdress qui est de la forme
  111.222.333.444 sur le port iPort.
  TODO: il faudrait ajouter la possibilité de se connecter a des URL
  www.google.com ou localhost (voir QHostInfo).*/
void Client::connectToTcpServer(QString iAddress, quint16 iPort)
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
void Client::disconnect()
{
  if( mpTcpSocket->isValid() )
    mpTcpSocket->disconnectFromHost();
}

//------------------------------------------------------------------------------
int Client::findDownload( int iId ) const
{
	int r = -1;
	for( size_t i = 0; i < mDownloads.size(); ++i )
  {
  	if( mDownloads[i].getId() == iId )
    { r = i; break; }
  }
  return r;
}


//------------------------------------------------------------------------------
int Client::findUpload( int iId ) const
{
	int r = -1;
	for( size_t i = 0; i < mUploads.size(); ++i )
  {
  	if( mUploads[i].getId() == iId )
    { r = i; break; }
  }
  return r;
}

//------------------------------------------------------------------------------
QString Client::getAndClearLastErrors() const
{
  QString r = mErrors;
  mErrors = QString();
  return r;
}

//------------------------------------------------------------------------------
QByteArray Client::getDownload( int iId ) const
{
	QByteArray r;
  int i = findDownload( iId );
	if( i != -1 )
  {
  	r = mDownloads[i].mPayload;
    if( getDownloadStatus( iId ) >= 1.0 )
	  	mDownloads.erase( mDownloads.begin() + i );
  }
  return r;
}

//------------------------------------------------------------------------------
int Client::getDownloadId( int iIndex ) const
{ return mDownloads[iIndex].getId(); }

//------------------------------------------------------------------------------
double Client::getDownloadStatus( int iId ) const
{
	double r = 0.0;
  int i = findDownload( iId );
  if( i != -1 )
  	r = mDownloads[i].mPayload.size() /
    	(double)mDownloads[i].mTotalSize;
  return r;
}

//------------------------------------------------------------------------------
int Client::getMaximumUploadPayloadSize() const
{ return mMaximumUploadPayloadSize; }

//------------------------------------------------------------------------------
int Client::getNumberOfDownloads() const
{ return mDownloads.size(); }

//------------------------------------------------------------------------------
int Client::getNumberOfUploads() const
{ return mUploads.size(); }

//------------------------------------------------------------------------------
QByteArray Client::getUpload( int iId ) const
{
  QByteArray r;
  int i = findUpload( iId );
	if( i != -1 )
  { r = mUploads[i].mPayload; }
  return r;
}

//------------------------------------------------------------------------------
int Client::getUploadId( int iIndex ) const
{ return mUploads[ iIndex ].getId(); }

//------------------------------------------------------------------------------
double Client::getUploadStatus( int iId ) const
{
	double r = 0.0;
  int i = findUpload( iId );
  if( i != -1 )
  { r = mUploads[i].mCursor / (double)mUploads[i].mTotalSize; }
  return r;
}

//------------------------------------------------------------------------------
void Client::handleSocketBytesWritten( qint64 iNumberOfBytesWritten )
{
	if( hasUploads() )
  {
    Transfer& t = mUploads[ mUploadIndex ];
    if( t.mPayload.size() > 0 && mpTcpSocket->bytesToWrite() <= 4 * 
      getMaximumUploadPayloadSize() )
    {
      /*Au lieu de .left() et .remove, un compteur de position et la fonction
        .mid() serait plus approprié.*/
      QByteArray ba = t.mPayload.mid( t.mCursor, getMaximumUploadPayloadSize() );
      t.mCursor += getMaximumUploadPayloadSize();
      mpTcpSocket->write( makePacket( ba, t.getId() ) );
      emit sentPacket( t.getId() );
    }
    
    if( getUploadStatus( t.getId() ) >= 1.0 )
    {      
      mUploads.erase( mUploads.begin() + mUploadIndex );
      emit uploadEnded( t.getId() );
    }
    
    if( getNumberOfUploads() > 0 )
      { mUploadIndex = (mUploadIndex + 1) % getNumberOfUploads(); }
  }
}

//------------------------------------------------------------------------------
void Client::handleSocketConnected()
{ emit socketConnected(); }

//------------------------------------------------------------------------------
void Client::handleSocketDisconnected()
{ emit socketDisconnected(); }

//------------------------------------------------------------------------------
void Client::handleSocketError(QAbstractSocket::SocketError iError)
{ 
  addError( "Error on socket: " +	network::asString(iError) );
	emit gotError();
}

//------------------------------------------------------------------------------
void Client::handleSocketReadyRead()
{
	int downloadId;
  while( mpTcpSocket->bytesAvailable() )
  {
  	QByteArray p = readPacket( mpTcpSocket, &downloadId );
    int downloadIndex = findDownload( downloadId );
    if( downloadIndex == -1 )
    {
      Transfer t = readUploadHeader( p );
      if( t.mIsValid )
      {
        mDownloads.push_back( t );
        emit downloadStarted( downloadId );
      }    	
    }
    else
    {      
      if( !p.isEmpty() )
      {
        mDownloads[ downloadIndex ].mPayload += p;
        emit gotPacket( downloadId );
      }
      else
      {
      	mpTcpSocket->readAll(); //on jete dans le bitBucket!
        mDownloads.erase( mDownloads.begin() + downloadIndex );
        addError( "A problem occured while reading packet... and the whole"
         " download was dropped..." );
      }
    }
    
    if( getDownloadStatus( downloadId ) >= 1.0 )
      emit downloadEnded( downloadId );
  }
}

//------------------------------------------------------------------------------
bool Client::hasDownloads() const
{ return !mDownloads.empty(); }

//------------------------------------------------------------------------------
bool Client::hasError() const
{ return !mErrors.isEmpty(); }

//------------------------------------------------------------------------------
bool Client::hasUploads() const
{ return !mUploads.empty(); }

//------------------------------------------------------------------------------
bool Client::isConnected() const
{ return mpTcpSocket->state() == QAbstractSocket::ConnectedState; }

//-----------------------------------------------------------------------------
void Client::send( const QByteArray& iA )
{
	if( mpTcpSocket->isValid() )
  {
  	Transfer t;
    t.setPayload( iA, mUploadId++ );
    mUploads.push_back( t );
    mUploadIndex = mUploads.size() - 1;
    QByteArray header = makeUploadHeader( t );
    mpTcpSocket->write( makePacket( header, t.getId() ) );
    emit uploadStarted( t.getId() );
  }
}

//------------------------------------------------------------------------------
void Client::setMaximumUploadPayloadSize( int iSize )
{ mMaximumUploadPayloadSize = iSize; }
