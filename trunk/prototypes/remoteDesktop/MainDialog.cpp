
#include "MainDialog.h"

using namespace realisim;

//-----------------------------------------------------------------------------
//--- protocol
//-----------------------------------------------------------------------------  
int MainDialog::protocol::mVersion = 1;
MainDialog::protocol::protocol()
{}
//-----------------------------------------------------------------------------
QByteArray MainDialog::protocol::makeMessageNewFrame( QPixmap iP )
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
QByteArray MainDialog::protocol::makeMessageGiveMaster()
{
	QByteArray r;
  QDataStream ds( &r, QIODevice::WriteOnly );
  ds.setVersion(QDataStream::Qt_4_7);
  ds << mVersion;
  ds << (qint32)mGiveMaster;
  return r;
}

//-----------------------------------------------------------------------------
//--- MainDialog
//-----------------------------------------------------------------------------  
MainDialog::MainDialog() : QMainWindow(),
  mClientMode( cmIdle ),
  mServerMode( smIdle ),
  mServerTimerId(0)
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
  
  mServer.startServer( 12345 );
  
  updateUi();
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
	setClientMode( cmActiveSlave );
  updateUi();
}
//-----------------------------------------------------------------------------
void MainDialog::clientSocketDisconnected()
{
	setClientMode( cmIdle );
  updateUi();
}
//-----------------------------------------------------------------------------
void MainDialog::createUi()
{
  resize(800, 600);
  
  QFrame* pMainFrame = new QFrame(this);
  setCentralWidget(pMainFrame);
  
  QVBoxLayout* pLyt = new QVBoxLayout(pMainFrame);
  pLyt->setMargin(2);
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
  pClientActivityLyt->setMargin(2);
  pClientActivityLyt->setSpacing(2);
  {
  	mpLabel = new QLabel( mpClientActivityFrame );
    pClientActivityLyt->addWidget( mpLabel );
  }
  
  //--- Server - frame d'activité 
  mpServerFrame = new QFrame( pMainFrame );
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
      case protocol::mGiveMaster : break;
      case protocol::mNewFrame : 
      {
      	ds >> mDesktopPixmap;
        mDesktopPixmap.save( "a.png", "PNG" );
        updateUi();
      }
      break;
      default: break;
    }
  }
  else {
    //pas la bonne version du protocole...
  }

}
//-----------------------------------------------------------------------------
void MainDialog::serverSocketConnected( int iIndex )
{
	setServerMode( smActive );
  if( mServer.getNumberOfSockets() == 1 )
  {
    QByteArray ba = protocol::makeMessageGiveMaster();
    mServer.send( iIndex, ba );
  }
	updateUi();
}
//-----------------------------------------------------------------------------
void MainDialog::serverSocketDisconnected( int iIndex )
{
	if( mServer.getNumberOfSockets() == 0 )
  { setServerMode( smIdle ); }
	updateUi();
}
//-----------------------------------------------------------------------------
void MainDialog::setClientMode( clientMode iMode )
{
	if( iMode != getClientMode() )
  {
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
      mpLabel->clear();
      mpLabel->setPixmap( mDesktopPixmap );
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
