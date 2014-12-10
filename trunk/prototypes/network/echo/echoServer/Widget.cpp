/*Created by Pierre-Olivier Beaudoin on 10-02-20.*/

#include "network/utils.h"
#include "Widget.h"

using namespace realisim;
  using namespace network;
  using namespace prototypes;
using namespace std;

//*****************************************************************************
Widget::Widget(QWidget* ipParent /*=0*/) : QWidget(ipParent),
  mpPort(0),
  mpStartServer(0),
  mpStopServer(0),
  mpLog(0),
  mServer()
{
  initUi();
  
  connect(&mServer, SIGNAL( errorRaised() ), this, SLOT( errorRaised() ) );
  connect(&mServer, SIGNAL( socketConnected( int ) ),
    this, SLOT( socketConnected( int ) ) );
  connect(&mServer, SIGNAL( socketDisconnected( int ) ),
    this, SLOT( socketDisconnected( int ) ) );
  connect(&mServer, SIGNAL( gotPacket( int, int ) ),
    this, SLOT( gotPacket(int, int) ) );
  connect(&mServer, SIGNAL( sentPacket( int, int ) ),
    this, SLOT( sentPacket(int, int) ) );
  connect(&mServer, SIGNAL( downloadEnded( int, int ) ),
    this, SLOT( downloadEnded( int, int ) ) );
  connect(&mServer, SIGNAL( uploadEnded( int, int ) ),
    this, SLOT( uploadEnded(int, int) ) );
    
  mServer.setMaximumUploadPayloadSize( 64 * 1024 );
}

Widget::~Widget()
{}

//------------------------------------------------------------------------------
void Widget::addToLog( QString iM ) const
{ mLog += mLog.isEmpty() ? iM : "\n" + iM; }
//------------------------------------------------------------------------------
void Widget::downloadEnded( int i, int iId )
{
  QByteArray d = mServer.getDownload( i, iId );
  addToLog( QString().sprintf( "download %d ended sur socket %d; %s", iId, i,
    QString(d).toStdString().c_str() ) );
    
  //echo
  mServer.send( i, d );
  updateUi();
}

//------------------------------------------------------------------------------
void Widget::errorRaised()
{ addToLog( mServer.getAndClearLastErrors() ); updateUi(); }
  
//------------------------------------------------------------------------------
void Widget::gotPacket(int iIndex, int iId)
{
	addToLog( QString().sprintf( "gotPacket %d sur socket %d", iId, iIndex ) );
  updateUi();
}
//------------------------------------------------------------------------------
void Widget::initUi()
{
	QVBoxLayout* pMainLyt = new QVBoxLayout(this);
  pMainLyt->setMargin(5);
  pMainLyt->setSpacing(5);
  
  QHBoxLayout* pServerInfo = new QHBoxLayout(this);
  {
    QComboBox* pServerAddress = new QComboBox(this);
    pServerAddress->addItems(getLocalIpAddresses());
    mpPort = new QLineEdit("12345",this);
    mpProtocols = new QComboBox( this );
    mpProtocols->insertItem( tpRaw, "raw" );
    mpProtocols->insertItem( tpRealisim, "realisim" );
    
    connect(mpProtocols, SIGNAL(activated(int)),
    	this, SLOT(protocolChanged(int)) );
    
    pServerInfo->addWidget(pServerAddress);
    pServerInfo->addWidget(mpPort);
    pServerInfo->addWidget(mpProtocols);
  }
  
  QHBoxLayout* pButtonsLyt = new QHBoxLayout(this);
  {
    mpStartServer = new QPushButton("Start",this);
    mpStopServer = new QPushButton("Stop",this);
    mpStopServer->setDisabled(true);
    pButtonsLyt->addStretch(1);
    pButtonsLyt->addWidget(mpStartServer);
    pButtonsLyt->addWidget(mpStopServer);
    
    connect(mpStartServer, SIGNAL(clicked()), this, SLOT(startServer()));
    connect(mpStopServer, SIGNAL(clicked()), this, SLOT(stopServer()));
  }
  
  QHBoxLayout* pLyt1 = new QHBoxLayout();
  {
	  QLabel* pConnectedPeers = new QLabel("Connected Peers:", this);
  	mpNumberOfPeers = new QLabel( "0", this );
    
    pLyt1->addWidget( pConnectedPeers );
    pLyt1->addWidget( mpNumberOfPeers );
    pLyt1->addStretch( 1 );
  }
  
  mpConnectedPeers = new QTreeWidget(this);
  mpConnectedPeers->setColumnCount(4);
  QTreeWidgetItem* item = new QTreeWidgetItem();
  item->setText(0, "Id");
  item->setText(1, "Ip Address");
  item->setText(2, "state");
  item->setText(3, "transmission");
  mpConnectedPeers->setHeaderItem(item);
  connect( mpConnectedPeers, SIGNAL( itemClicked ( QTreeWidgetItem*, int ) ),
  	this, SLOT( peerItemClicked( QTreeWidgetItem*, int ) ) );
  
  //---générale
  QLabel* pLog = new QLabel("Log:", this);
  mpLog = new QTextEdit(this);
  
  //---assemblage dans le layout
  pMainLyt->addLayout(pServerInfo);
  pMainLyt->addLayout(pButtonsLyt);
  pMainLyt->addLayout(pLyt1);
  pMainLyt->addWidget(mpConnectedPeers);
  
  pMainLyt->addWidget(pLog);
  pMainLyt->addWidget(mpLog);
}

//------------------------------------------------------------------------------
void Widget::protocolChanged(int iP)
{
	mServer.setProtocol( (transferProtocol)iP );
	updateUi();
}

//------------------------------------------------------------------------------
void Widget::sentPacket(int iIndex, int iId)
{
	addToLog( QString().sprintf( "sentPacket %d sur socket %d", iId, iIndex ) );
  updateUi();
}

//------------------------------------------------------------------------------
void Widget::socketConnected( int i )
{	 updateUi(); }
//------------------------------------------------------------------------------
void Widget::socketDisconnected( int i )
{ 
  mpConnectedPeers->clear();
	updateUi();
}

//------------------------------------------------------------------------------
void Widget::startServer()
{
  if(mServer.startServer(mpPort->text().toInt()))
  {
    mpStartServer->setDisabled(true);
    mpStopServer->setEnabled(true);
  }
  updateUi();
}

//------------------------------------------------------------------------------
void Widget::stopServer()
{
  mServer.stopServer();
  mpStartServer->setEnabled(true);
  mpStopServer->setDisabled(true);
  mpConnectedPeers->clear();
  mLog = QString();
  updateUi();
}
//------------------------------------------------------------------------------
void Widget::updateUi()
{
  mpProtocols->setCurrentIndex( mServer.getProtocol() );
	mpNumberOfPeers->setText( QString::number( mServer.getNumberOfSockets() ) );

  //on ajuste le texte pour chaque peer
	for(int i = 0; i < mServer.getNumberOfSockets(); ++i)
  {
    QTreeWidgetItem* item = mpConnectedPeers->topLevelItem( i );
    if( !item )
    { 
    	item = new QTreeWidgetItem();
      mpConnectedPeers->insertTopLevelItem( i, item );
    }
    item->setText( 0, QString::number(i) );
    item->setText( 1,	mServer.getSocketPeerAddress( i ));
    QString s;
    switch ( mServer.getSocketState( i ) )
    {
      case QAbstractSocket::UnconnectedState: s = "unconnected"; break;
      case QAbstractSocket::HostLookupState: s = "host look up"; break;
      case QAbstractSocket::ConnectingState: s = "connecting"; break;
      case QAbstractSocket::ConnectedState: s = "connected"; break;
      case QAbstractSocket::BoundState: s = "bound"; break;
      case QAbstractSocket::ListeningState: s = "listening"; break;
      case QAbstractSocket::ClosingState: s = "closing"; break;
      default: break;
    }	
    item->setText( 2, s );
    
    QString udStatus;
    if( mServer.hasDownloads( i ) )
    {
    	for( int j = 0; j < mServer.getNumberOfDownloads( i ); ++j )
      {
      	double a = mServer.getDownloadStatus( i, mServer.getDownloadId( i, j ) );
				udStatus += "D" + QString::number( (int)(a * 100) ) + "% ";
      }
    }
    if( mServer.hasUploads( i ) )
		{    
      for( int j = 0; j < mServer.getNumberOfUploads( i ); ++j )
      {
      	double a = mServer.getUploadStatus( i, mServer.getUploadId( i, j ) );
				udStatus += "U" + QString::number( (int)(a * 100) ) + "% ";
      }    	
    }
    item->setText( 3, udStatus );
  }

  mpLog->setText( mLog );
  QScrollBar *sb = mpLog->verticalScrollBar();
  sb->setValue(sb->maximum());
}

//------------------------------------------------------------------------------
void Widget::uploadEnded( int iIndex, int iId )
{
  addToLog( QString().sprintf( "uploadEnded %d sur socket %d", iId, iIndex ) );
  updateUi();
}
