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
  mpAddress(0),
  mpPort(0),
  mpConnect(0),
  mpDisconnect(0),
  mpLog(0),
  mClient()
{
  initUi();
  connect(&mClient, SIGNAL( socketConnected() ), this, SLOT( updateUi() ) );
  connect(&mClient, SIGNAL( socketDisconnected() ), this, SLOT( updateUi() ) );
  connect(&mClient, SIGNAL( sentPacket() ), this, SLOT( updateUi() ) );
  connect(&mClient, SIGNAL( uploadStarted() ), this, SLOT( updateUi() ) );
  connect(&mClient, SIGNAL( uploadEnded() ), this, SLOT( updateUi() ) );
  connect(&mClient, SIGNAL( gotError() ), this, SLOT( gotError() ) );
  mClient.setMaximumPayloadSize( 64 * 1024 );
  updateUi();
}

Widget::~Widget()
{}

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
  mClient.disconnectFromTcpServer();
  updateUi();
}

//------------------------------------------------------------------------------
void Widget::gotError()
{ updateUi(); }

//------------------------------------------------------------------------------
void Widget::initUi()
{
	QVBoxLayout* pMainLyt = new QVBoxLayout(this);
  pMainLyt->setMargin(5);
  pMainLyt->setSpacing(5);
  
  //---client side
  QHBoxLayout* pHostServerInfo = new QHBoxLayout(this);
  {
    mpAddress = new QLineEdit("127.0.0.1",this);
    mpPort = new QLineEdit("12345",this);
    
    pHostServerInfo->addWidget(mpAddress);
    pHostServerInfo->addWidget(mpPort);
  }
  
  QHBoxLayout* pClientButtonsLyt = new QHBoxLayout(this);
  {
    mpConnect = new QPushButton("Connect",this);
    mpDisconnect = new QPushButton("Diconnect",this);
    QPushButton* pWriteTest = new QPushButton( "write test", this );
    //mpDisconnect->setDisabled(true);
    pClientButtonsLyt->addStretch(1);
    pClientButtonsLyt->addWidget(mpConnect);
    pClientButtonsLyt->addWidget(mpDisconnect);
    pClientButtonsLyt->addWidget(pWriteTest);
    
    connect(mpConnect, SIGNAL(clicked()), this, SLOT(connectToServer()));
    connect(mpDisconnect, SIGNAL(clicked()), this, SLOT(disconnectFromServer()));
    connect(pWriteTest, SIGNAL(clicked()), this, SLOT( writeTest() ) );
  }
  
  QHBoxLayout* pChatLyt = new QHBoxLayout();
  {
  	mpChat = new QLineEdit( this );
  	QPushButton* pSend = new QPushButton( "send", this );
    connect( pSend, SIGNAL( clicked() ), this, SLOT( sendChat() ) );
    
    QPushButton* pSendFile = new QPushButton( "send file...", this );
    connect( pSendFile, SIGNAL( clicked() ), this, SLOT( sendFile() ) );
    
    pChatLyt->addWidget( mpChat, 5 );
    pChatLyt->addWidget( pSend, 1 );
    pChatLyt->addWidget( pSendFile, 1 );
  }
  QHBoxLayout* pProgressLyt = new QHBoxLayout();
  {
  	QVBoxLayout* pVlyt = new QVBoxLayout();
    mpProgressUpload = new QProgressBar( this );
    mpProgressDownload = new QProgressBar( this );
    
    pVlyt->addWidget( mpProgressUpload );
    pVlyt->addWidget( mpProgressDownload );
    
    pProgressLyt->addStretch( 1 );
    pProgressLyt->addLayout( pVlyt );
  }
  
  //---générale
  QLabel* pLog = new QLabel("Log:", this);
  mpLog = new QTextEdit(this);
  
  //---assemblage dans le layout    
  pMainLyt->addLayout( pHostServerInfo );
  pMainLyt->addLayout( pClientButtonsLyt );
  pMainLyt->addLayout( pChatLyt );
  pMainLyt->addLayout( pProgressLyt );
  pMainLyt->addWidget( pLog );
  pMainLyt->addWidget( mpLog );
  
  pMainLyt->addStretch(1);
}

//------------------------------------------------------------------------------
void Widget::sendChat()
{ 
	mClient.send( mpChat->text().toUtf8() );
  mpChat->clear();
  updateUi();
}

//------------------------------------------------------------------------------
void Widget::sendFile()
{ 
 QString fileName = QFileDialog::getOpenFileName(this, "choose File",
		"/home" );
  if( !fileName.isEmpty() )
  {
  	QFile f( fileName );
    if( f.open( QIODevice::ReadOnly ) )
    {
    	mClient.send( f.readAll() );
	    updateUi();
    }
  }
	
}

//------------------------------------------------------------------------------
void Widget::updateUi()
{
	mpConnect->setEnabled( !mClient.isConnected() );
  mpDisconnect->setEnabled( mClient.isConnected() );
  
  //progressBar
printf( "u status: %f\n", mClient.getUploadStatus() );
	mpProgressUpload->setValue( mClient.getUploadStatus() * 100 );

  
	if( mClient.hasError() )
  {
		mpLog->setText( mClient.getAndClearLastErrors() + "\n" + mpLog->toPlainText() );
  }
}

//------------------------------------------------------------------------------
void Widget::writeTest()
{ mClient.writeTest(); }