
#include "MainDialog.h"
#ifdef _WIN32
	#include <windows.h>
#endif

using namespace std;
using namespace realisim;

//-----------------------------------------------------------------------------
//--- protocol
//-----------------------------------------------------------------------------  
int protocol::mVersion = 1;
protocol::protocol()
{}
//-----------------------------------------------------------------------------
QByteArray protocol::makeMessageGiveDesktopInfo( QByteArray iBa )
{
	QByteArray r;
  QDataStream ds( &r, QIODevice::WriteOnly );
  ds.setVersion(QDataStream::Qt_4_7);
  ds << mVersion;
  ds << (qint32)mGiveDesktopInfo;
  ds << iBa;
  return r;
}
//-----------------------------------------------------------------------------
QByteArray protocol::makeMessageMouseMoved( QPointF iP )
{
  QByteArray r;
  QDataStream ds( &r, QIODevice::WriteOnly );
  ds.setVersion(QDataStream::Qt_4_7);
  ds << mVersion;
  ds << (qint32)mMouseMoved;
  ds << iP;
  return r;
}
//-----------------------------------------------------------------------------
QByteArray protocol::makeMessageMousePressed( QPointF iP )
{
  QByteArray r;
  QDataStream ds( &r, QIODevice::WriteOnly );
  ds.setVersion(QDataStream::Qt_4_7);
  ds << mVersion;
  ds << (qint32)mMousePressed;
  ds << iP;
  return r;
}
//-----------------------------------------------------------------------------
QByteArray protocol::makeMessageMouseReleased( QPointF iP )
{
  QByteArray r;
  QDataStream ds( &r, QIODevice::WriteOnly );
  ds.setVersion(QDataStream::Qt_4_7);
  ds << mVersion;
  ds << (qint32)mMouseReleased;
  ds << iP;
  return r;
}
//-----------------------------------------------------------------------------
QByteArray protocol::makeMessageNewFrame( QPixmap iP )
{
	QByteArray r;
  QDataStream ds( &r, QIODevice::WriteOnly );
  ds.setVersion(QDataStream::Qt_4_7);
  ds << mVersion;
  ds << (qint32)mNewFrame;
  ds << iP;
  return r;
}
//-----------------------------------------------------------------------------
QByteArray protocol::makeMessageRequestDesktopInfo()
{
	QByteArray r;
  QDataStream ds( &r, QIODevice::WriteOnly );
  ds.setVersion(QDataStream::Qt_4_7);
  ds << mVersion;
  ds << (qint32)mRequestDesktopInfo;
  return r;
}
//-----------------------------------------------------------------------------
QString protocol::toString( message m )
{
	QString r( "unknown message" );
  switch (m) 
  {
    case mRequestDesktopInfo: r = "mRequestDesktopInfo"; break;
    case mGiveDesktopInfo: r = "mGiveDesktopInfo"; break;
    case mNewFrame: r = "mNewFrame"; break;
    case mMouseMoved: r = "mMouseMoved"; break;
    case mMousePressed: r = "mMousePressed"; break;
    case mMouseReleased: r = "mMouseReleased"; break;
    default: break;
  }
  return r;
}
//-----------------------------------------------------------------------------
//--- desktopInfo
//-----------------------------------------------------------------------------  
int desktopInfo::mVersion = 1;

desktopInfo::desktopInfo() :
  mScreenSizes()
{}
desktopInfo::~desktopInfo()
{ clear(); }
//-----------------------------------------------------------------------------
void desktopInfo::clear()
{
	mScreenSizes.clear();
}
//-----------------------------------------------------------------------------
int desktopInfo::getNumberOfScreens() const
{ return mScreenSizes.size(); }
//-----------------------------------------------------------------------------
QSize desktopInfo::getScreenSize( int i ) const
{
	QSize r;
  if( i >= 0 && i <= getNumberOfScreens() - 1 ) 
  { r = mScreenSizes[i]; }
  return r;
}
//-----------------------------------------------------------------------------
void desktopInfo::fromBinary( QByteArray iBa )
{
	qint32 version;
  qint32 numberOfScreens;
  QSize s;
  QDataStream ds( &iBa, QIODevice::ReadOnly );
  ds.setVersion(QDataStream::Qt_4_7);
  ds >> version;
  ds >> numberOfScreens;
  setNumberOfScreens( numberOfScreens );
  for( int i = 0; i < getNumberOfScreens(); ++i )
  {
  	ds >> s;
    setScreenSize( i, s );
  }
}
//-----------------------------------------------------------------------------
QByteArray desktopInfo::toBinary() const
{
	QByteArray r;
  QDataStream ds( &r, QIODevice::WriteOnly );
  ds.setVersion(QDataStream::Qt_4_7);
  ds << (qint32)mVersion;
  ds << (qint32)getNumberOfScreens();
  for( int i = 0; i < getNumberOfScreens(); ++i )
  { ds << getScreenSize( i ); }
  return r;
}
//-----------------------------------------------------------------------------
void desktopInfo::setNumberOfScreens( int i )
{
	mScreenSizes.clear();
  mScreenSizes.resize( i );
}
//-----------------------------------------------------------------------------
void desktopInfo::setScreenSize( int i, QSize iS )
{ mScreenSizes[i] = iS; }

//-----------------------------------------------------------------------------
//--- graphicsView
//-----------------------------------------------------------------------------  
graphicsView::graphicsView( QGraphicsScene* ipScene, QWidget* ipParent/*= 0*/ ) :
	QGraphicsView( ipScene, ipParent )
{}

graphicsView::~graphicsView()
{}
//-----------------------------------------------------------------------------
void graphicsView::keyPressEvent( QKeyEvent* ipE )
{}
//-----------------------------------------------------------------------------
void graphicsView::keyReleaseEvent( QKeyEvent* ipE )
{}
//-----------------------------------------------------------------------------
void graphicsView::mouseDoubleClickEvent( QMouseEvent* ipE )
{}
//-----------------------------------------------------------------------------
void graphicsView::mouseMoveEvent( QMouseEvent* ipE )
{ emit mouseMoved( ipE->pos() ); }
//-----------------------------------------------------------------------------
void graphicsView::mousePressEvent( QMouseEvent* ipE )
{ emit mousePressed( ipE->pos() ); }
//-----------------------------------------------------------------------------
void graphicsView::mouseReleaseEvent( QMouseEvent* ipE )
{ emit mouseReleased( ipE->pos() ); }
//-----------------------------------------------------------------------------
//--- MainDialog
//-----------------------------------------------------------------------------  
MainDialog::MainDialog() : QMainWindow(),
  mClientMode( cmIdle ),
  mServerMode( smIdle ),
  mServerTimerId(0),
  mClientLog(),
  mServerLog()
{
	createUi();
  
  //--- connect client
  connect( &mClient, SIGNAL( socketConnected() ),
  	this, SLOT( clientSocketConnected() ) );
  connect( &mClient, SIGNAL( socketDisconnected() ),
  	this, SLOT( clientSocketDisconnected() ) );
  connect( &mClient, SIGNAL( downloadEnded(int) ),
  	this, SLOT( clientDownloadEnded(int) ) );
  
  
  //--- connect server
  connect( &mServer, SIGNAL( socketConnected( int ) ),
  	this, SLOT( serverSocketConnected( int ) ) );
  connect( &mServer, SIGNAL( socketDisconnected( int ) ),
  	this, SLOT( serverSocketDisconnected( int ) ) );
  connect( &mServer, SIGNAL( downloadEnded( int, int ) ),
  	this, SLOT( serverDownloadEnded( int, int ) ) );

  
  mServer.startServer( 12345 );
  
  mClientLog.logTimestamp(true);
  mClientLog.logToFile( true, "clientLog.txt" );
  mServerLog.logTimestamp(true);
  mServerLog.logToFile( true, "serverLog.txt" );
  
  
  //fill desktop info
  QDesktopWidget* dw = QApplication::desktop();
  mDesktopInfo.setNumberOfScreens( dw->screenCount() );
  for( int i = 0; i < mDesktopInfo.getNumberOfScreens(); ++i )
  { mDesktopInfo.setScreenSize( i, dw->screenGeometry( i ).size() ); }
  updateUi();
}

MainDialog::~MainDialog()
{
	mClient.disconnect();
	mServer.stopServer();
}

//-----------------------------------------------------------------------------
void MainDialog::clientDownloadEnded( int iId )
{
	QByteArray ba = mClient.getDownload( iId );
  handleMessageFromServer( ba );
}
//-----------------------------------------------------------------------------
void MainDialog::clientSocketConnected()
{
//showFullScreen();
//mpView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
//mpView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

	mClient.send( protocol::makeMessageRequestDesktopInfo() );
	
	setClientMode( cmActiveSlave );
  updateUi();
  
  mClientLog.log( "client connected." );
}
//-----------------------------------------------------------------------------
void MainDialog::clientSocketDisconnected()
{	
	setClientMode( cmIdle );
  updateUi();
  
  mClientLog.log( "client disconnected." );
}
//-----------------------------------------------------------------------------
void MainDialog::createUi()
{
  //resize(800, 600);
  
  QFrame* pMainFrame = new QFrame(this);
  setCentralWidget(pMainFrame);
  
  QVBoxLayout* pLyt = new QVBoxLayout(pMainFrame);
  pLyt->setMargin(0);
  pLyt->setSpacing(5);
  
  //--- Client - frame de connection
  mpClientConnectionFrame = new QFrame( pMainFrame );
  QVBoxLayout* pConnectionLyt = new QVBoxLayout( mpClientConnectionFrame );  
  pConnectionLyt->setMargin( 2 );
  pConnectionLyt->setSpacing( 5 );
  {
  	QHBoxLayout* pLine1 = new QHBoxLayout();
    {
	    QLabel* pL = new QLabel( "nom de l'hôte:", mpClientConnectionFrame );
  	  mpHostName = new QLineEdit( "localhost", mpClientConnectionFrame );
      pLine1->addWidget(pL);
      pLine1->addWidget(mpHostName);
      pLine1->addStretch(1);
    }
    
    QHBoxLayout* pLine2 = new QHBoxLayout();
    {
	    QLabel* pL = new QLabel( "port", mpClientConnectionFrame );
  	  mpHostPort = new QLineEdit( "12345", mpClientConnectionFrame );
      pLine2->addWidget(pL);
      pLine2->addWidget(mpHostPort);
      pLine2->addStretch(1);
    }
    
    QHBoxLayout* pLine3 = new QHBoxLayout();
    {	
    	QPushButton* pConnect = new QPushButton( "connecter", 
      	mpClientConnectionFrame );
      connect( pConnect, SIGNAL( clicked() ), this, SLOT( connectClicked() ) );
      pLine3->addStretch(1);
      pLine3->addWidget(pConnect);
    }
  	
    pConnectionLyt->addLayout(pLine1);
    pConnectionLyt->addLayout(pLine2);
    pConnectionLyt->addLayout(pLine3);
    pConnectionLyt->addStretch(1);
  }
  
  //--- Client - frame d'activité
  mpClientActivityFrame = new QFrame( pMainFrame );
  QVBoxLayout* pClientActivityLyt = new QVBoxLayout( mpClientActivityFrame );
  pClientActivityLyt->setMargin(0);
  pClientActivityLyt->setSpacing(0);
  {
  	//mpLabel = new QLabel( mpClientActivityFrame );
    mpScene = new QGraphicsScene( mpClientActivityFrame );
    mpView = new graphicsView( mpScene, mpClientActivityFrame );
    
    connect( mpView, SIGNAL( mouseMoved( QPoint ) ),
    	this, SLOT( mouseMovedInView(QPoint) ) );
    connect( mpView, SIGNAL( mousePressed( QPoint ) ),
    	this, SLOT( mousePressedInView( QPoint ) ) );
    connect( mpView, SIGNAL( mouseReleased( QPoint ) ),
    	this, SLOT( mouseReleasedInView( QPoint ) ) );
    
    pClientActivityLyt->addWidget( mpView );
  }
  
  //--- Server - frame d'activité 
  mpServerFrame = new QFrame( pMainFrame );
  mpServerFrame->setMaximumHeight( 30 );
  QVBoxLayout* pServerLyt = new QVBoxLayout( mpServerFrame );
  pServerLyt->setMargin( 2 );
  pServerLyt->setSpacing( 2 );
  {
  	mpServerInfo = new QLabel( "aucune connection", mpServerFrame );
    pServerLyt->addStretch( 1 );
    pServerLyt->addWidget( mpServerInfo );
  }
  
  //ajoute le left panel au layout principale
  pLyt->addWidget( mpClientConnectionFrame );
  pLyt->addWidget( mpClientActivityFrame );
  pLyt->addWidget( mpServerFrame );
}
//-----------------------------------------------------------------------------
void MainDialog::connectClicked()
{
	mClient.disconnect();
  mClient.connectToTcpServer(mpHostName->text(), mpHostPort->text().toInt());
}
//-----------------------------------------------------------------------------
void MainDialog::handleMessageFromClient( int iSocket, QByteArray iBa )
{
	qint32 version;
  qint32 m;
	QDataStream ds( &iBa, QIODevice::ReadOnly );
  ds.setVersion( QDataStream::Qt_4_7 );
  ds >> version;
  if( version == protocol::mVersion )
  {
  	ds >> m;
    switch ( (protocol::message)m ) 
    {
    	case protocol::mMouseMoved:
    	case protocol::mMousePressed:
      case protocol::mMouseReleased:
      {
      	QPointF p;
        ds >> p;
        handleMouseInputFromClient( (protocol::message)m, p );
      }
      break;
    	case protocol::mRequestDesktopInfo:
      {
      	QByteArray ba = protocol::makeMessageGiveDesktopInfo( 
        	mDesktopInfo.toBinary() );
        mServer.send( iSocket, ba );
      }
      break;
      default: break;
    }
    mServerLog.log( "handleMessageFromClient: %s", 
    	protocol::toString((protocol::message)m).toStdString().c_str() );
  }
  else {
    //pas la bonne version du protocole...
    mServerLog.log( "handleMessageFromClient: wrong protocol version.\
 expected %d, received %d", protocol::mVersion, version );
  }
}
//-----------------------------------------------------------------------------
void MainDialog::handleMessageFromServer( QByteArray iBa )
{
	qint32 version;
  qint32 m;
	QDataStream ds( &iBa, QIODevice::ReadOnly );
  ds.setVersion( QDataStream::Qt_4_7 );
  ds >> version;
  if( version == protocol::mVersion )
  {
  	ds >> m;
    switch ( (protocol::message)m ) 
    {
    	case protocol::mGiveDesktopInfo:
      {
      	QByteArray ba;
        ds >> ba;
        mRemoteDesktopInfo.fromBinary( ba );
mClientLog.log( "Remote desktop size: %dx%d", 
	mRemoteDesktopInfo.getScreenSize(0).width(), 
  mRemoteDesktopInfo.getScreenSize(0).height() );
      }
      break;
      case protocol::mNewFrame : 
      {
      	ds >> mDesktopPixmap;
        updateUi();
      }
      break;
      default: break;
    }
    mClientLog.log( "handleMessageFromServer: %s", 
    	protocol::toString((protocol::message)m).toStdString().c_str() );
  }
  else {
  	mClientLog.log( "handleMessageFromServer: wrong protocol version.\
 expected %d, received %d", protocol::mVersion, version );
    //pas la bonne version du protocole...
  }
}
//-----------------------------------------------------------------------------
void MainDialog::handleMouseInputFromClient( protocol::message iM, QPointF iP )
{
	mServerLog.log( "handleMouseInputFromClient %d -> %.2f, %.2f", iM, 
  	iP.x(), iP.y() );
    
#ifdef _WIN32
  INPUT input;
  input.mi.mouseData = 0; 
  input.mi.dx =  iP.x()*( 65535/(double)mDesktopInfo.getScreenSize(0).width() );//x being coord in pixels
  input.mi.dy =  iP.y()*( 65535/(double)mDesktopInfo.getScreenSize(0).height() );//y being coord in pixels
  input.type = INPUT_MOUSE;
  input.mi.time = 0;
  input.mi.dwExtraInfo = 0;

  switch (iM) 
  {
  	case protocol::mMouseMoved:
    	input.mi.dwFlags = (MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE );
    break;
    case protocol::mMousePressed:
      input.mi.dwFlags = (MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE | MOUSEEVENTF_LEFTDOWN);
    break;
    case protocol::mMouseReleased:
      input.mi.dwFlags = (MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE | MOUSEEVENTF_LEFTUP);
    break;
    default: break;
  }
  SendInput( 1, &input, sizeof(input) );
  //mServerLog.log( " mi.dx: %d, mi.dy: %d SendInput returns: %d, GetLastError: %d", 
  //   input.mi.dx, input.mi.dy , a, GetLastError() );
#endif
}
//-----------------------------------------------------------------------------
void MainDialog::mouseMovedInView( QPoint iP )
{
	mClientLog.log( "mouse moved at pos %d, %d -> %.2f, %.2f",
  	iP.x(), iP.y(),
    mpView->mapToScene( iP ).x(),
    mpView->mapToScene( iP ).y() );
  mClient.send( protocol::makeMessageMouseMoved( mpView->mapToScene( iP ) ) );
}
//-----------------------------------------------------------------------------
void MainDialog::mousePressedInView( QPoint iP )
{
	mClientLog.log( "mouse pressed at pos %d, %d -> %.2f, %.2f",
  	iP.x(), iP.y(),
    mpView->mapToScene( iP ).x(),
    mpView->mapToScene( iP ).y() );
  mClient.send( protocol::makeMessageMousePressed( mpView->mapToScene( iP ) ) );
}
//-----------------------------------------------------------------------------
void MainDialog::mouseReleasedInView( QPoint iP )
{
	mClientLog.log( "mouse released at pos %d, %d -> %.2f, %.2f",
  	iP.x(), iP.y(),
    mpView->mapToScene( iP ).x(),
    mpView->mapToScene( iP ).y() );
  mClient.send( protocol::makeMessageMouseReleased( mpView->mapToScene( iP ) ) );
}
//-----------------------------------------------------------------------------
void MainDialog::serverDownloadEnded( int iSocket, int iId )
{
	QByteArray ba = mServer.getDownload( iSocket, iId );
  handleMessageFromClient( iSocket, ba );
}
//-----------------------------------------------------------------------------
void MainDialog::serverSocketConnected( int iIndex )
{
	mServerLog.log( "Client %d with ip %s has connected.", iIndex, 
		mServer.getSocketPeerAddress( iIndex ).toStdString().c_str() );
    
	setServerMode( smActive );
	updateUi();
}
//-----------------------------------------------------------------------------
void MainDialog::serverSocketDisconnected( int iIndex )
{
	mServerLog.log( "Client %d with ip %s has disconnected.", iIndex, 
		mServer.getSocketPeerAddress( iIndex ).toStdString().c_str() );
    
	if( mServer.getNumberOfSockets() == 0 )
  { setServerMode( smIdle ); }
	updateUi();
}
//-----------------------------------------------------------------------------
void MainDialog::setClientMode( clientMode iMode )
{
	if( iMode != getClientMode() )
  {
  	mClientLog.log( "Client about to change mode from %d to %d", 
    	getClientMode(), iMode );
  	switch (iMode)
    {
      case cmIdle: mClientMode = iMode; break;
      case cmActiveMaster: mClientMode = iMode; break;
      case cmActiveSlave: mClientMode = iMode; break;
      default: break;
    }
  }
}
//-----------------------------------------------------------------------------
void MainDialog::setServerMode( serverMode iMode )
{
	if( iMode != getServerMode() )
  {
  	mServerLog.log( "Server about to change mode from %d to %d", 
    	getServerMode(), iMode );
  	switch (iMode)
    {
      case smIdle: 
      	mServerMode = smIdle;
        killTimer( mServerTimerId );
        mServerTimerId = 0;
        break;
      case smActive:
      	mServerMode = smActive;
        mServerTimerId = startTimer( 1 / 5.0 * 1000.0 ); //5fps
        break;
      default: break;
    }
  }
}
//-----------------------------------------------------------------------------
void MainDialog::timerEvent( QTimerEvent* ipE )
{
	if( ipE->timerId() == mServerTimerId )
  {
  	QPixmap p = QPixmap::grabWindow(QApplication::desktop()->winId());
  	QByteArray ba = protocol::makeMessageNewFrame( p );
    mServer.broadcast( ba );
  }
}
//-----------------------------------------------------------------------------
void MainDialog::updateUi()
{
	switch (getClientMode()) 
  {
    case cmIdle: 
    	mpClientConnectionFrame->show();
      mpClientActivityFrame->hide();
      break;
    case cmActiveMaster:
    case cmActiveSlave:
    	mpClientConnectionFrame->hide();
      mpClientActivityFrame->show();

      mpScene->clear();
      mpScene->addPixmap( mDesktopPixmap );
    	break;
    default: break;
  }
  
  switch (getServerMode())
  {
    case smIdle:
	    mpServerFrame->hide();
    	break;
    case smActive:
    {
    	mpServerFrame->show();
      QString m;
      m += "Peers: ";
      for( int i = 0; i < mServer.getNumberOfSockets(); ++i )
      {
      	m += mServer.getSocketPeerAddress(i) + " ";
      }
      
      mpServerInfo->setText( m );
      
    } break;
    default: break;
  }
}
