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
mSockets()
{
  connect(mpTcpServer, SIGNAL( newConnection() ),
   this, SLOT( handleNewConnection() ) );
}

ServerBase::~ServerBase()
{
  if(mpTcpServer->isListening())
  	mpTcpServer->close();

  for(int i = 0; i < getNumberOfSockets() ; ++i)
  {
    getSocket(i)->abort();
    delete getSocket(i);
  }
  mSockets.clear();
  
  stopServer();
  delete mpTcpServer;
}
//------------------------------------------------------------------------------
void ServerBase::addError( QString iE ) const
{
	if( !mErrors.isEmpty() ) mErrors += " ";
	mErrors += iE;
}
//------------------------------------------------------------------------------
void ServerBase::broadcast( const QByteArray& iA )
{
  for(int i = 0; i < getNumberOfSockets(); ++i)
  { send( i, iA ); }
}

//------------------------------------------------------------------------------
int ServerBase::findSocketFromSender( QObject* ipObject )
{
	int r = -1;
  QTcpSocket* s =	dynamic_cast<QTcpSocket*>( ipObject );
  vector<QTcpSocket*>::iterator it = find( mSockets.begin(), mSockets.end(), s );
  if( it != mSockets.end() )
  	r = distance( mSockets.begin(), it );
  return r;
}
//------------------------------------------------------------------------------
QString ServerBase::getAndClearLastErrors() const
{
  QString r = mErrors;
  mErrors = QString();
  return r;
}
//------------------------------------------------------------------------------
QByteArray ServerBase::getDownload( int iIndex ) const
{
	QByteArray r;
	if( hasDownload( iIndex ) )
  {
  	r = mDownloads[ iIndex ].mPayload;
    if( isDownloadCompleted( iIndex ) )
	  	mDownloads.erase( iIndex );
  }
  return r;
}
//------------------------------------------------------------------------------
double ServerBase::getDownloadStatus( int iIndex ) const
{
	double r = 0.0;
  if( hasDownload( iIndex ) )
  	r = mDownloads[ iIndex ].mPayload.size() /
    	(double)mDownloads[ iIndex ].mTotalSize;
printf( "getDownloadStatus: %f\n", r );
  return r;
}
//------------------------------------------------------------------------------
QString ServerBase::getLocalAddress() const
{ return mpTcpServer->serverAddress().toString(); }
//------------------------------------------------------------------------------
int ServerBase::getNumberOfSockets() const
{ return mSockets.size(); }
  
//------------------------------------------------------------------------------
QTcpSocket* ServerBase::getSocket( int i )
{
  return const_cast< QTcpSocket* >(
  	const_cast< const ServerBase* >(this)->getSocket( i ) );
}
//------------------------------------------------------------------------------
const QTcpSocket* ServerBase::getSocket( int i ) const
{ return mSockets[ i ]; }
//------------------------------------------------------------------------------
QString ServerBase::getSocketPeerAddress( int i ) const
{ return getSocket( i )->peerAddress().toString(); }
//------------------------------------------------------------------------------
qint16 ServerBase::getSocketPeerPort( int i ) const
{ return getSocket( i )->peerPort(); }
//------------------------------------------------------------------------------
QAbstractSocket::SocketState ServerBase::getSocketState( int i )
{ return mSockets[i]->state(); }
//------------------------------------------------------------------------------
void ServerBase::handleNewConnection()
{
  if(mpTcpServer->hasPendingConnections())
  {
    QTcpSocket* s = mpTcpServer->nextPendingConnection();
    connect(s, SIGNAL( error( QAbstractSocket::SocketError) ),
      this, SLOT( handleSocketError(QAbstractSocket::SocketError) ) );
    connect(s, SIGNAL( stateChanged ( QAbstractSocket::SocketState ) ),
      this, SLOT( handleSocketStateChanged(QAbstractSocket::SocketState) ) );
    connect(s, SIGNAL( disconnected() ),
      this, SLOT( handleSocketDisconnected() ) );
    connect( s, SIGNAL( readyRead() ), this,
    	SLOT( handleSocketReadyRead() ) );
    mSockets.push_back(s);
    emit socketConnected( getNumberOfSockets() - 1 );
  }
}
//------------------------------------------------------------------------------
void ServerBase::handleSocketDisconnected()
{
	/*Quand le serveur n'est pas en train d'écouter, on ne
    veut pas nettoyer la liste de socket parce qu'il est sans aucun doute
    en train de fermer. voir ::stop() et le destructeur.*/
	if( mpTcpServer->isListening() )
  {
    int i = findSocketFromSender( sender() );
    if( i != -1 )
    {
      getSocket( i )->deleteLater();
      mSockets.erase( mSockets.begin() + i );
      emit socketDisconnected( i );
    }
    else
      addError( "handleSocketDisconnected is called for unknown peer..." );
  }
}
//------------------------------------------------------------------------------
void ServerBase::handleSocketError(QAbstractSocket::SocketError iError)
{
  int socketId = findSocketFromSender( sender() );
  addError( "Error on socket " +  QString::number( socketId ) + ": " +
  	network::asString(iError) );
  emit error();
}

//------------------------------------------------------------------------------
void ServerBase::handleSocketReadyRead()
{
	int iIndex = findSocketFromSender( sender() );
  if( iIndex != -1 )
  {
  	QTcpSocket* s = getSocket( iIndex );
    if( !hasDownload( iIndex ) )
    {
    	QByteArray h = readPacket( s );
      Transfer t = readUploadHeader( h );
      if( t.mIsValid )
      {
      	mDownloads[ iIndex ] = t;
      	emit downloadStarted( iIndex );
      }    	
    }
    
    while( s->bytesAvailable() )
    {
    	printf( "bytesAvailable avant readPacket: %d\n", s->bytesAvailable() );
      if( !isDownloadCompleted( iIndex ) )
      {
      	mDownloads[ iIndex ].mPayload += readPacket( s );
        gotPacket( iIndex );
      }
      else s->readAll(); //on jete dans le bitBucket!
      
      printf( "bytesAvailable apres readPacket: %d\n", s->bytesAvailable() );
    }
    
    if( isDownloadCompleted( iIndex ) )
    	emit downloadEnded( iIndex );
  }
  else 
  { addError( "handleSocketReadyRead is called for unknown peer..." ); }
}

//------------------------------------------------------------------------------
void ServerBase::handleSocketStateChanged(QAbstractSocket::SocketState iState)
{
	int i = findSocketFromSender( sender() );
  if( i != -1 )
  { socketStateChanged( i, iState ); }
  else 
  { addError( "handleSocketStateChanged is called for unknown peer..." ); }
}

//------------------------------------------------------------------------------
bool ServerBase::hasDownload( int i ) const
{ return mDownloads.find( i ) != mDownloads.end(); }

//------------------------------------------------------------------------------
bool ServerBase::hasError() const
{	return !mErrors.isEmpty(); }

//------------------------------------------------------------------------------
bool ServerBase::isDownloadCompleted( int iIndex ) const
{	return hasDownload( iIndex ) && getDownloadStatus( iIndex ) >= 1.0 ; }

//------------------------------------------------------------------------------
void ServerBase::readTcpSocket( int iSocket )
{}

//------------------------------------------------------------------------------
void ServerBase::send( int iSocket, const QByteArray& iA )
{
	if( getSocket( iSocket )->isValid() )
  	getSocket( iSocket )->write( iA ); 
}

//------------------------------------------------------------------------------
void ServerBase::socketStateChanged( int iSocket,
	QAbstractSocket::SocketState iState )
{
	switch ( iState ) 
  {
    case QAbstractSocket::UnconnectedState:break;
    case QAbstractSocket::HostLookupState: break;
    case QAbstractSocket::ConnectingState: break;
    case QAbstractSocket::ConnectedState: break;
    case QAbstractSocket::BoundState: break;
    case QAbstractSocket::ListeningState: break;
    case QAbstractSocket::ClosingState: break;
    default: break;
  }	
}

//------------------------------------------------------------------------------
bool ServerBase::startServer()
{
	//early out. si le server écoute déja sur le port demandé
  if(mpTcpServer->isListening() && getLocalPort() == mpTcpServer->serverPort())
    return true;

  bool r = true;
  if(mpTcpServer->isListening() && getLocalPort() != mpTcpServer->serverPort())
    mpTcpServer->close();
  
  //Le server écoute pour toutes les adresses sur le port demandé
  if(!mpTcpServer->listen(QHostAddress::Any, getLocalPort()))
  {
    r = false;
    addError( mpTcpServer->errorString() );
    mpTcpServer->close();
  }
  
  return r;
}

//------------------------------------------------------------------------------
bool ServerBase::startServer(quint16 iPort)
{
  setLocalPort(iPort);
  return startServer();
}

//------------------------------------------------------------------------------
void ServerBase::stopServer()
{
  if(mpTcpServer->isListening())
  	mpTcpServer->close();
    
  //close all connectedSockets
  for(int i = 0; i < getNumberOfSockets(); ++i)
  {
    getSocket(i)->disconnectFromHost();
    delete getSocket(i);
  }
  mSockets.clear();
}