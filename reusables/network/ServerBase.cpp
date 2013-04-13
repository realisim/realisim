/*Created by Pierre-Olivier Beaudoin on 10-02-26.*/

#include <algorithm>
#include "ServerBase.h"
#include <QTcpServer>
#include <QTcpSocket>

using namespace realisim;
using namespace reusables;
using namespace network;
using namespace std;

int ServerBase::mUploadId = 0;

ServerBase::ServerBase(QObject* ipParent /*=0*/) : QObject(ipParent),
mErrors(),
mPort(12345),
mpTcpServer(new QTcpServer(ipParent)),
mSockets(),
mMaximumUploadPayloadSize( 64 * 1024 )
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
/*On envoit a tout les sockets sauf le socket iIndex*/
void ServerBase::broadcast( const QByteArray& iA, int iIndex )
{
  for(int i = 0; i < getNumberOfSockets(); ++i)
  { if( i != iIndex ) send( i, iA ); }
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
int ServerBase::findDownload( int iSocketIndex, int iId ) const
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
int ServerBase::findUpload( int iSocketIndex, int iUploadId ) const
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
QString ServerBase::getAndClearLastErrors() const
{
  QString r = mErrors;
  mErrors = QString();
  return r;
}
//------------------------------------------------------------------------------
QByteArray ServerBase::getDownload( int iSocketIndex, int iId ) const
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
int ServerBase::getDownloadId( int iSocketIndex, int iIndex ) const
{ return mDownloads[ iSocketIndex ][ iIndex ].getId(); }

//------------------------------------------------------------------------------
double ServerBase::getDownloadStatus( int iSocketIndex, int iId ) const
{
	double r = 0.0;
  int i = findDownload( iSocketIndex, iId );
  if( i != -1 )
  	r = mDownloads[ iSocketIndex ][i].mPayload.size() /
    	(double)mDownloads[ iSocketIndex ][i].mTotalSize;
  return r;
}
//------------------------------------------------------------------------------
QString ServerBase::getLocalAddress() const
{ return mpTcpServer->serverAddress().toString(); }

//------------------------------------------------------------------------------
int ServerBase::getMaximumUploadPayloadSize() const
{ return mMaximumUploadPayloadSize; }

//------------------------------------------------------------------------------
int ServerBase::getNumberOfDownloads( int iSocketIndex ) const
{ return mDownloads[ iSocketIndex ].size(); }

//------------------------------------------------------------------------------
int ServerBase::getNumberOfSockets() const
{ return mSockets.size(); }

//------------------------------------------------------------------------------
int ServerBase::getNumberOfUploads( int iSocketIndex ) const
{ return mUploads[ iSocketIndex ].size(); }

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
QByteArray ServerBase::getUpload( int iSocketIndex, int iId ) const
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
int ServerBase::getUploadId( int iSocketIndex, int iIndex ) const
{ return mUploads[ iSocketIndex ][ iIndex ].getId(); }

//------------------------------------------------------------------------------
double ServerBase::getUploadStatus( int iIndex, int iId ) const
{
	double r = 0.0;
  int i = findUpload( iIndex, iId );
  if( i != -1 )
  { r = mUploads[iIndex][i].mCursor / (double)mUploads[iIndex][i].mTotalSize ; }
  return r;
}
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
    connect( s, SIGNAL( bytesWritten( qint64 ) ),
      this, SLOT( handleSocketBytesWritten( qint64 ) ) );
    mSockets.push_back(s);
    emit socketConnected( getNumberOfSockets() - 1 );
  }
}
//------------------------------------------------------------------------------
void ServerBase::handleSocketBytesWritten( qint64 iNumberOfBytesWritten )
{
	int i = findSocketFromSender( sender() );
  if( i != -1 )
  {
  	QTcpSocket* s = getSocket( i );
    if( hasUploads( i ) )
    {
      vector< Transfer >& vt = mUploads[ i ];
      vector< Transfer >::iterator it = vt.begin();
        
      Transfer& t = *it;
      if( t.mPayload.size() > 0 && s->bytesToWrite() <= 4 * 
        getMaximumUploadPayloadSize() )
      {
        /*Au lieu de .left() et .remove, un compteur de position et la fonction
          .mid() serait plus approprié.*/
        QByteArray ba = t.mPayload.mid( t.mCursor, getMaximumUploadPayloadSize() );
        t.mCursor += getMaximumUploadPayloadSize();
        s->write( makePacket( ba, t.getId() ) );
        emit sentPacket( i, t.getId() );
      }
      
      if( getUploadStatus( i, t.getId() ) >= 1.0 )
      {
      	vt.erase( it );
        emit uploadEnded( i, t.getId() );        
      }
    }
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
    	mDownloads.erase( i ); //efface tout les downloads
      mUploads.erase( i ); //efface tout les uploads
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
	int socketIndex = findSocketFromSender( sender() );
  if( socketIndex != -1 )
  {
  	QTcpSocket* s = getSocket( socketIndex );
    while( s->bytesAvailable() )
    {
    	int downloadId;
    	vector< Transfer >& vt = mDownloads[ socketIndex ];
    	QByteArray p = readPacket( s, &downloadId );
      int downloadIndex = findDownload( socketIndex, downloadId );
      if( downloadIndex == -1 )
      {
        Transfer t = readUploadHeader( p );
        if( t.mIsValid )
        {
          vt.push_back( t );
          emit downloadStarted( socketIndex, downloadId );
        }  
      }
      else
      {
        if( !p.isEmpty() )
        {
        	vt[downloadIndex].mPayload += p;
          emit gotPacket( socketIndex, downloadId );
        }
        else
        {
        	s->readAll(); //on jete dans le bitBucket!
        	vt.erase( vt.begin() + downloadIndex );
        	addError( "A problem occured while reading packet... and the whole"
           " download was dropped..." );
        }
      }
      
      if( getDownloadStatus( socketIndex, downloadId ) >= 1.0 )
	    	emit downloadEnded( socketIndex, downloadId );
    }
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
bool ServerBase::hasDownloads( int iSocketIndex ) const
{
	map< int, vector< Transfer > >::const_iterator it =
  	mDownloads.find( iSocketIndex );
	return it != mDownloads.end() && !it->second.empty();
}

//------------------------------------------------------------------------------
bool ServerBase::hasError() const
{	return !mErrors.isEmpty(); }

//------------------------------------------------------------------------------
bool ServerBase::hasUploads( int iSocketIndex ) const
{
	map< int, vector< Transfer > >::const_iterator it =
  	mUploads.find( iSocketIndex );
	return it != mUploads.end() && !it->second.empty();
}

//------------------------------------------------------------------------------
void ServerBase::send( int iSocketIndex, const QByteArray& iA )
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
void ServerBase::setMaximumUploadPayloadSize( int iSize )
{ mMaximumUploadPayloadSize = iSize; }

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