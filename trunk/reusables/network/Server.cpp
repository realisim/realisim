/*Created by Pierre-Olivier Beaudoin on 10-02-26.*/

#include <algorithm>
#include "Server.h"
#include <QTcpServer>
#include <QTcpSocket>

using namespace realisim;
using namespace reusables;
using namespace network;
using namespace std;

int Server::mUploadId = 0;

Server::Server(QObject* ipParent /*=0*/) : QObject(ipParent),
mErrors(),
mPort(12345),
mpTcpServer(new QTcpServer(ipParent)),
mSockets(),
mReadBuffers(),
mMaximumUploadPayloadSize( 64 * 1024 ),
mUploadIndices()
{
  connect(mpTcpServer, SIGNAL( newConnection() ),
   this, SLOT( handleNewConnection() ) );
}

Server::~Server()
{
  if(mpTcpServer->isListening())
  	mpTcpServer->close();

  for(int i = 0; i < getNumberOfSockets() ; ++i)
  {
    getSocket(i)->abort();
    delete getSocket(i);
  }
  mSockets.clear();
  mReadBuffers.clear();
  mUploadIndices.clear();
  
  stopServer();
  delete mpTcpServer;
}
//------------------------------------------------------------------------------
void Server::addError( QString iE ) const
{
	if( !mErrors.isEmpty() ) mErrors += " ";
	mErrors += iE;
}
//------------------------------------------------------------------------------
void Server::broadcast( const QByteArray& iA )
{
  for(int i = 0; i < getNumberOfSockets(); ++i)
  { send( i, iA ); }
}
//------------------------------------------------------------------------------
/*On envoit a tout les sockets sauf le socket iIndex*/
void Server::broadcast( const QByteArray& iA, int iIndex )
{
  for(int i = 0; i < getNumberOfSockets(); ++i)
  { if( i != iIndex ) send( i, iA ); }
}
//------------------------------------------------------------------------------
int Server::findSocketFromSender( QObject* ipObject )
{
	int r = -1;
  QTcpSocket* s =	dynamic_cast<QTcpSocket*>( ipObject );
  vector<QTcpSocket*>::iterator it = find( mSockets.begin(), mSockets.end(), s );
  if( it != mSockets.end() )
  	r = distance( mSockets.begin(), it );
  return r;
}
//------------------------------------------------------------------------------
int Server::findDownload( int iSocketIndex, int iId ) const
{
	int r = -1;
	const vector< Transfer >& vt = mDownloads[ iSocketIndex ];
  for( size_t i = 0; i < vt.size(); ++i )
  {
  	if( vt[i].getId() == iId )
    { r = i; break; }
  }
  return r;
}
//------------------------------------------------------------------------------
int Server::findUpload( int iSocketIndex, int iUploadId ) const
{
	int r = -1;
	const vector< Transfer >& vt = mUploads[ iSocketIndex ];
  for( size_t i = 0; i < vt.size(); ++i )
  {
  	if( vt[i].getId() == iUploadId )
    { r = i; break; }
  }
  return r;
}
//------------------------------------------------------------------------------
QString Server::getAndClearLastErrors() const
{
  QString r = mErrors;
  mErrors = QString();
  return r;
}
//------------------------------------------------------------------------------
QByteArray Server::getDownload( int iSocketIndex, int iId ) const
{
	QByteArray r;
  int i = findDownload( iSocketIndex, iId );

	if( i != -1 )
  {
  	vector< Transfer >& vt = mDownloads[ iSocketIndex ];
  	r = vt[i].mPayload;
    if( getDownloadStatus( iSocketIndex, iId ) >= 1.0 )
    { vt.erase( vt.begin() + i ); }
  }

  return r;
}

//------------------------------------------------------------------------------
int Server::getDownloadId( int iSocketIndex, int iIndex ) const
{ return mDownloads[ iSocketIndex ][ iIndex ].getId(); }

//------------------------------------------------------------------------------
double Server::getDownloadStatus( int iSocketIndex, int iId ) const
{
	double r = 0.0;
  int i = findDownload( iSocketIndex, iId );
  if( i != -1 )
  	r = mDownloads[ iSocketIndex ][i].mPayload.size() /
    	(double)mDownloads[ iSocketIndex ][i].mTotalSize;
  return r;
}
//------------------------------------------------------------------------------
QString Server::getLocalAddress() const
{ return mpTcpServer->serverAddress().toString(); }

//------------------------------------------------------------------------------
int Server::getMaximumUploadPayloadSize() const
{ return mMaximumUploadPayloadSize; }

//------------------------------------------------------------------------------
int Server::getNumberOfDownloads( int iSocketIndex ) const
{ return mDownloads[ iSocketIndex ].size(); }

//------------------------------------------------------------------------------
int Server::getNumberOfSockets() const
{ return mSockets.size(); }

//------------------------------------------------------------------------------
int Server::getNumberOfUploads( int iSocketIndex ) const
{ return mUploads[ iSocketIndex ].size(); }

//------------------------------------------------------------------------------
QTcpSocket* Server::getSocket( int i )
{
  return const_cast< QTcpSocket* >(
  	const_cast< const Server* >(this)->getSocket( i ) );
}
//------------------------------------------------------------------------------
const QTcpSocket* Server::getSocket( int i ) const
{ return mSockets[ i ]; }
//------------------------------------------------------------------------------
QString Server::getSocketPeerAddress( int i ) const
{ return getSocket( i )->peerAddress().toString(); }
//------------------------------------------------------------------------------
qint16 Server::getSocketPeerPort( int i ) const
{ return getSocket( i )->peerPort(); }
//------------------------------------------------------------------------------
QAbstractSocket::SocketState Server::getSocketState( int i )
{ return mSockets[i]->state(); }

//------------------------------------------------------------------------------
QByteArray Server::getUpload( int iSocketIndex, int iId ) const
{
	QByteArray r;
  int i = findUpload( iSocketIndex, iId );
  
	if( i != -1 )
  {
  	vector< Transfer >& vt = mUploads[ iSocketIndex ];
  	r = vt[i].mPayload;
  }
  return r;
}

//------------------------------------------------------------------------------
int Server::getUploadId( int iSocketIndex, int iIndex ) const
{ return mUploads[ iSocketIndex ][ iIndex ].getId(); }

//------------------------------------------------------------------------------
double Server::getUploadStatus( int iIndex, int iId ) const
{
	double r = 0.0;
  int i = findUpload( iIndex, iId );
  if( i != -1 )
  { r = mUploads[iIndex][i].mCursor / (double)mUploads[iIndex][i].mTotalSize ; }
  return r;
}
//------------------------------------------------------------------------------
void Server::handleNewConnection()
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
    connect( s, SIGNAL( bytesWritten( qint64 ) ),
      this, SLOT( handleSocketBytesWritten( qint64 ) ) );
    mSockets.push_back(s);
    mReadBuffers.push_back( QByteArray() );
    mUploadIndices.push_back( 0 );
    emit socketConnected( getNumberOfSockets() - 1 );
  }
}
//------------------------------------------------------------------------------
void Server::handleReadBuffer( int iSocketIndex )
{
  int downloadId;
  QByteArray p = readPacket( mReadBuffers[ iSocketIndex ], &downloadId ) ;
  while( !p.isEmpty() )
  {
    vector< Transfer >& vt = mDownloads[ iSocketIndex ];
    int downloadIndex = findDownload( iSocketIndex, downloadId );
    if( downloadIndex == -1 )
    {
      Transfer t = readUploadHeader( p );
      if( t.mIsValid )
      {
        vt.push_back( t );
        emit downloadStarted( iSocketIndex, downloadId );
      }  
    }
    else
    {
      vt[downloadIndex].mPayload += p;
      emit gotPacket( iSocketIndex, downloadId );
    }
    
    if( getDownloadStatus( iSocketIndex, downloadId ) >= 1.0 )
      emit downloadEnded( iSocketIndex, downloadId );
      
    p = readPacket(mReadBuffers[ iSocketIndex ], &downloadId);
  }
}
//------------------------------------------------------------------------------
void Server::handleSocketBytesWritten( qint64 iNumberOfBytesWritten )
{
	int i = findSocketFromSender( sender() );
  if( i != -1 )
  {
  	QTcpSocket* s = getSocket( i );
    if( hasUploads( i ) )
    {
      vector< Transfer >& vt = mUploads[ i ];
      Transfer& t = vt[ mUploadIndices[i] ];
      if( t.mPayload.size() > 0 && s->bytesToWrite() <= 4 * 
        getMaximumUploadPayloadSize() )
      {
        QByteArray ba = t.mPayload.mid( t.mCursor, getMaximumUploadPayloadSize() );
        t.mCursor += getMaximumUploadPayloadSize();
        s->write( makePacket( ba, t.getId() ) );
        emit sentPacket( i, t.getId() );
      }
      
      if( getUploadStatus( i, t.getId() ) >= 1.0 )
      {
      	vt.erase( vt.begin() + mUploadIndices[i] );
        emit uploadEnded( i, t.getId() );        
      }
      
      if( getNumberOfUploads( i ) > 0 )
        { mUploadIndices[i] = (mUploadIndices[i] + 1) % getNumberOfUploads( i ); }
    }
  }
}
//------------------------------------------------------------------------------
void Server::handleSocketDisconnected()
{
	/*Quand le serveur n'est pas en train d'écouter, on ne
    veut pas nettoyer la liste de socket parce qu'il est sans aucun doute
    en train de fermer. voir ::stop() et le destructeur.*/
	if( mpTcpServer->isListening() )
  {
    int i = findSocketFromSender( sender() );
    if( i != -1 )
    {
    	mDownloads.erase( i ); //efface tout les downloads
      mUploads.erase( i ); //efface tout les uploads
      getSocket( i )->deleteLater();
      mSockets.erase( mSockets.begin() + i );
      mReadBuffers.erase( mReadBuffers.begin() + i );
      mUploadIndices.erase( mUploadIndices.begin() + i );
      emit socketDisconnected( i );
    }
    else
      addError( "handleSocketDisconnected is called for unknown peer..." );
  }
}
//------------------------------------------------------------------------------
void Server::handleSocketError(QAbstractSocket::SocketError iError)
{
  int socketId = findSocketFromSender( sender() );
  addError( "Error on socket " +  QString::number( socketId ) + ": " +
  	network::asString(iError) );
  emit error();
}

//------------------------------------------------------------------------------
void Server::handleSocketReadyRead()
{
  int i = findSocketFromSender( sender() );
  if( i != -1 )
  { 
  	mReadBuffers[ i ] += getSocket(i)->readAll();
    handleReadBuffer( i );
  }
	else 
  { addError( "handleSocketReadyRead is called for unknown peer..." ); }
}

//------------------------------------------------------------------------------
void Server::handleSocketStateChanged(QAbstractSocket::SocketState iState)
{
	int i = findSocketFromSender( sender() );
  if( i != -1 )
  { socketStateChanged( i, iState ); }
  else 
  { addError( "handleSocketStateChanged is called for unknown peer..." ); }
}

//------------------------------------------------------------------------------
bool Server::hasDownloads( int iSocketIndex ) const
{
	map< int, vector< Transfer > >::const_iterator it =
  	mDownloads.find( iSocketIndex );
	return it != mDownloads.end() && !it->second.empty();
}

//------------------------------------------------------------------------------
bool Server::hasError() const
{	return !mErrors.isEmpty(); }

//------------------------------------------------------------------------------
bool Server::hasUploads( int iSocketIndex ) const
{
	map< int, vector< Transfer > >::const_iterator it =
  	mUploads.find( iSocketIndex );
	return it != mUploads.end() && !it->second.empty();
}

//------------------------------------------------------------------------------
void Server::send( int iSocketIndex, const QByteArray& iA )
{
	QTcpSocket* s = getSocket( iSocketIndex );
	if( s && s->isValid() )
  {
  	Transfer t;
    t.setPayload( iA, mUploadId++ );
  	mUploads[iSocketIndex].push_back( t );
    QByteArray header = makeUploadHeader( t );
    s->write( makePacket( header, t.getId() ) );
    emit uploadStarted( iSocketIndex, t.getId() );
  }
}

//------------------------------------------------------------------------------
void Server::setMaximumUploadPayloadSize( int iSize )
{ mMaximumUploadPayloadSize = iSize; }

//------------------------------------------------------------------------------
void Server::socketStateChanged( int iSocket,
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
bool Server::startServer()
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
bool Server::startServer(quint16 iPort)
{
  setLocalPort(iPort);
  return startServer();
}

//------------------------------------------------------------------------------
void Server::stopServer()
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
  mReadBuffers.clear();
  mUploadIndices.clear();
}