/*Created by Pierre-Olivier Beaudoin on 10-02-20.*/

#include "network/utils.h"
#include "Widget.h"

using namespace realisim;
  using namespace reusables;
    using namespace network;
  using namespace prototypes;
using namespace std;

//*****************************************************************************
Widget::Widget(QWidget* ipParent /*=0*/) : QWidget(ipParent),
  mpPort(0),
  mpStartServer(0),
  mpStopServer(0),
  mpConnectedPeers(0),
  mpLog(0),
  mServer()
{
  initUi();
  
  connect(&mServer, SIGNAL( error()), this, SLOT( updateUi() ) );
  connect(&mServer, SIGNAL( socketConnected( int ) ),
    this, SLOT( peerConnected( int ) ) );
  connect(&mServer, SIGNAL( socketDisconnected( int ) ),
    this, SLOT( peerDisconnected( int ) ) );
  connect(&mServer, SIGNAL( gotPacket( int ) ),
    this, SLOT( gotPacket( int ) ) );
    connect(&mServer, SIGNAL( downloadStarted( int ) ),
    this, SLOT( updateUi() ) );
  connect(&mServer, SIGNAL( downloadEnded( int ) ),
    this, SLOT( updateUi() ) );
}

Widget::~Widget()
{}

//------------------------------------------------------------------------------
void Widget::gotPacket( int i )
{ updateUi(); }
  
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
    
    pServerInfo->addWidget(pServerAddress);
    pServerInfo->addWidget(mpPort);
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
  
  pMainLyt->addStretch( 1 );
}

//------------------------------------------------------------------------------
void Widget::peerConnected( int i )
{ updateUi(); }
//------------------------------------------------------------------------------
void Widget::peerDisconnected( int i )
{ 
	mpConnectedPeers->takeTopLevelItem( i );
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
  updateUi();
}
//------------------------------------------------------------------------------
void Widget::updateUi()
{
	mpNumberOfPeers->setText( QString::number( mServer.getNumberOfSockets() ) );

	/*on ajoute les peer manquant a la liste*/
  if( mServer.getNumberOfSockets() == 0 )
  	mpConnectedPeers->clear();
  else if( mServer.getNumberOfSockets() > mpConnectedPeers->topLevelItemCount() )
  {
  	for( int i = mpConnectedPeers->topLevelItemCount(); 
    	i < mServer.getNumberOfSockets(); ++i )
    {
      QTreeWidgetItem* item = new QTreeWidgetItem();
      mpConnectedPeers->insertTopLevelItem(i, item);
    }
    
  }
  //on ajuste le texte pour chaque peer
	for(int i = 0; i < mServer.getNumberOfSockets(); ++i)
  {
    QTreeWidgetItem* item = mpConnectedPeers->topLevelItem( i );
    item->setText( 0, QString::number(i) );
    item->setText( 1, mServer.getSocketPeerAddress( i ) );
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
    if( mServer.isDownloadCompleted( i ) )
    {
      item->setText( 3, "completed" );
    	QByteArray b = mServer.getDownload( i );
    }
    else if( mServer.hasDownload(i) )
    {
    	double a = mServer.getDownloadStatus( i );
      QString s = QString::number( (int)(a * 100) ) + "%";
    	item->setText( 3, s );
    }
  }

	if( mServer.hasError() )
  {
		mpLog->setText( mServer.getAndClearLastErrors() + "\n" + mpLog->toPlainText() );
  }
}
