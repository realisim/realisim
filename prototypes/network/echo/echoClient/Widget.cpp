/*Created by Pierre-Olivier Beaudoin on 10-02-20.*/

#include "network/utils.h"
#include "Widget.h"

using namespace realisim;
  using namespace network;
  using namespace prototypes;
using namespace std;

//*****************************************************************************
Widget::Widget(QWidget* ipParent /*=0*/) : QWidget(ipParent),
  mpAddress(0),
  mpPort(0),
  mpConnect(0),
  mpDisconnect(0),
  mpLog(0),
  mClient()
{
  initUi();
  connect(&mClient, SIGNAL( errorRaised() ), this, SLOT( errorRaised() ) );
  connect(&mClient, SIGNAL( socketConnected() ), this, SLOT( socketConnected() ) );
  connect(&mClient, SIGNAL( socketDisconnected() ), this, SLOT( socketDisconnected() ) );
  connect(&mClient, SIGNAL( sentPacket( int ) ), this, SLOT( sentPacket( int ) ) );
  connect(&mClient, SIGNAL( gotPacket( int ) ), this, SLOT( gotPacket( int ) ) );
  connect(&mClient, SIGNAL( uploadEnded( int ) ), this, SLOT( updateUi() ) );
  connect(&mClient, SIGNAL( downloadEnded( int ) ), this, SLOT( downloadEnded( int ) ) );
  
  mClient.setMaximumUploadPayloadSize( 64 );
  updateUi();
}

Widget::~Widget()
{
	mClient.disconnect(); 
}

//------------------------------------------------------------------------------
void Widget::addToLog(QString l)
{
	mLog += mLog.isEmpty() ? l : "\n" + l;
	updateUi();
}

//------------------------------------------------------------------------------
void Widget::connectToServer()
{
  mClient.connectToTcpServer(mpAddress->text(),
     mpPort->text().toInt());
  updateUi();
}

//------------------------------------------------------------------------------
void Widget::disconnectFromServer()
{
  mClient.disconnect();
  updateUi();
}

//------------------------------------------------------------------------------
void Widget::downloadEnded( int iId )
{
  QByteArray d = mClient.getDownload( iId );
  addToLog( QString().sprintf( "download ended. size(bytes): %d, content: %s",
  	 d.size(), QString().fromUtf8( d.constData() ).toStdString().c_str() ) );
  updateUi();
}

//------------------------------------------------------------------------------
void Widget::errorRaised()
{ addToLog( mClient.getAndClearLastErrors() ); }

//------------------------------------------------------------------------------
void Widget::gotPacket( int iId )
{
	addToLog( QString().sprintf("got packet sur transfer %d, dowload status %f",
  	iId, mClient.getDownloadStatus(iId) * 100.0 ) );
}

//------------------------------------------------------------------------------
void Widget::initUi()
{
	QVBoxLayout* pMainLyt = new QVBoxLayout(this);
  pMainLyt->setMargin(5);
  pMainLyt->setSpacing(5);
  
  //---client side
  QHBoxLayout* pHostServerInfo = new QHBoxLayout();
  {
    mpAddress = new QLineEdit("localhost",this);
    mpPort = new QLineEdit("12345",this);
    mpProtocols = new QComboBox( this );
    mpProtocols->insertItem( tpRaw, "raw" );
    mpProtocols->insertItem( tpRealisim, "realisim" );
    
    connect(mpProtocols, SIGNAL(activated(int)),
    	this, SLOT(protocolChanged(int)) );
    
    pHostServerInfo->addWidget(mpAddress);
    pHostServerInfo->addWidget(mpPort);
    pHostServerInfo->addWidget(mpProtocols);
  }
  
  QHBoxLayout* pL1 = new QHBoxLayout();
  {
  	mpInput = new QLineEdit( this );
    mpSend = new QPushButton( "send", this );
    connect(mpSend, SIGNAL(clicked()), this, SLOT(sendClicked()));
    
    pL1->addWidget(mpInput);
    pL1->addWidget(mpSend);
    pL1->addStretch(1);
  }
  
  //---générale
  QVBoxLayout* pLogLyt = new QVBoxLayout();
  {
  	QLabel* pLog = new QLabel("Log:", this);
    mpLog = new QTextEdit(this);
    
    pLogLyt->addWidget( pLog );
    pLogLyt->addWidget( mpLog );
  }  
  
  QHBoxLayout* pClientButtonsLyt = new QHBoxLayout(this);
  {
    mpConnect = new QPushButton("Connect",this);
    mpDisconnect = new QPushButton("Diconnect",this);
    //mpDisconnect->setDisabled(true);
    pClientButtonsLyt->addStretch(1);
    pClientButtonsLyt->addWidget(mpConnect);
    pClientButtonsLyt->addWidget(mpDisconnect);
    
    connect(mpConnect, SIGNAL(clicked()), this, SLOT(connectToServer()));
    connect(mpDisconnect, SIGNAL(clicked()), this, SLOT(disconnectFromServer()));
  }
  
  //---assemblage dans le layout    
  pMainLyt->addLayout( pHostServerInfo );
  pMainLyt->addLayout( pL1 );
  pMainLyt->addLayout( pLogLyt );
  pMainLyt->addLayout( pClientButtonsLyt );
}

//------------------------------------------------------------------------------
void Widget::protocolChanged(int iP)
{
	mClient.setProtocol( (transferProtocol)iP );
	updateUi();
}

//------------------------------------------------------------------------------
void Widget::sendClicked()
{
	QByteArray ba;
  ba.append( mpInput->text() );
  if( mClient.getProtocol() == tpRaw ){ ba.append( '\r' ); ba.append( '\n' ); }
  addToLog( QString().sprintf("sending %d bytes", ba.size()) );
	mClient.send(ba);
  mpInput->clear();
}

//------------------------------------------------------------------------------
void Widget::sentPacket( int iId )
{
	addToLog( QString().sprintf("sentPacket packet %d, upload status %f",
  	iId, mClient.getUploadStatus(iId) * 100.0 ) );
}

//------------------------------------------------------------------------------
void Widget::socketConnected()
{ addToLog( "connected" ); }

//------------------------------------------------------------------------------
void Widget::socketDisconnected()
{ addToLog( "disconnected" ); }

//------------------------------------------------------------------------------
void Widget::updateUi()
{
	mpConnect->setEnabled( !mClient.isConnected() );
  mpDisconnect->setEnabled( mClient.isConnected() );
  mpProtocols->setCurrentIndex( mClient.getProtocol() );
  
  mpLog->setText( mLog );
  QScrollBar *sb = mpLog->verticalScrollBar();
  sb->setValue(sb->maximum());
}
