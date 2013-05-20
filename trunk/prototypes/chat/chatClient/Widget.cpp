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
  connect(&mClient, SIGNAL( socketConnected() ), this, SLOT( socketConnected() ) );
  connect(&mClient, SIGNAL( socketDisconnected() ), this, SLOT( socketDisconnected() ) );
  connect(&mClient, SIGNAL( sentPacket( int ) ), this, SLOT( sentPacket( int ) ) );
  connect(&mClient, SIGNAL( gotPacket( int ) ), this, SLOT( gotPacket( int ) ) );
  connect(&mClient, SIGNAL( uploadStarted( int ) ), this, SLOT( updateUi() ) );
  connect(&mClient, SIGNAL( uploadEnded( int ) ), this, SLOT( updateUi() ) );
  connect(&mClient, SIGNAL( downloadStarted( int ) ), this, SLOT( downloadStarted( int ) ) );
  connect(&mClient, SIGNAL( downloadEnded( int ) ), this, SLOT( downloadEnded( int ) ) );
  connect(&mClient, SIGNAL( gotError() ), this, SLOT( gotError() ) );
  
  mClient.setMaximumUploadPayloadSize( 64 * 1024 );
  
  connect( mpPeerListView, SIGNAL( itemDoubleClicked ( QTreeWidgetItem*, int ) ),
  	this, SLOT( peerItemDoubleClicked( QTreeWidgetItem*, int ) ) );
  updateUi();
}

Widget::~Widget()
{
	mClient.disconnect(); 
	mPeers.clear();
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
printf("download %d ended\n", iId);
	if( mClient.hasDownloads() )
  {
  	QByteArray d = mClient.getDownload( iId );
    chatProtocol cp = readProtocol( d );
    switch (cp)
    {
      case cpPeerList: 
        {
          mPeers = readPeerListPacket( d );
          /*on enleve le peer local de la list*/
          int i = findPeer( mPeer );
          if( i != -1 )
            mPeers.erase( mPeers.begin() + i );
        }
        break;
      case cpText:
      	{
        	chatPeer from = prototypes::from( d );
          QString m = readTextPacket( d );
          chatWindow* w = getChatWindow( from );
          if( w )
          {
          	w->show(); w->raise();
            w->gotChat( m );
          }
        }
        break;
      case cpFile:
      	{
        	chatPeer from = prototypes::from( d );
          QString f;
          QByteArray ba = readFilePacket( d, f );
          chatWindow* w = getChatWindow( from );
          if( w )
          {
          	w->show(); w->raise();
            w->gotFile( f, ba );
          }
        }
        break;
      case cpRequestToSendFile: break;
      default: break;
    }
  }
  updateUi();
}

//------------------------------------------------------------------------------
void Widget::downloadStarted( int iId )
{
	printf("download %d started\n", iId);
}
//------------------------------------------------------------------------------
int Widget::findPeer( const chatPeer& iPeer ) const
{
	int r = -1;
  for( int i = 0; i < (int)mPeers.size(); ++i )
  	if( mPeers.at(i) == iPeer ) { r = i; break; }
  return r;
}
//------------------------------------------------------------------------------
chatWindow* Widget::getChatWindow( const chatPeer& iPeer )
{
  chatWindow* w = 0;
  int i = findPeer( iPeer );
  if( i != -1 )
  {
    map< int, chatWindow* >::const_iterator it = mPeerToChatWindow.find( i );
    if( it != mPeerToChatWindow.end() )
    {
      w = it->second;
    }
    else
    {
      w = new chatWindow( mClient, mPeer, mPeers[i] );
      mPeerToChatWindow[i] = w;
    }
  }
	return w;
}
//------------------------------------------------------------------------------
void Widget::gotError()
{ updateUi(); }

//------------------------------------------------------------------------------
void Widget::gotPacket( int iId )
{
	if( mClient.getDownloadStatus( iId ) < 1.0 )
  {
  	chatPeer cp = prototypes::from( mClient.getDownload( iId ) );
    chatWindow* w = getChatWindow( cp );
    if( w ) w->updateUi();
  }
}

//------------------------------------------------------------------------------
void Widget::initUi()
{
	QVBoxLayout* pMainLyt = new QVBoxLayout(this);
  pMainLyt->setMargin(5);
  pMainLyt->setSpacing(5);
  
  //---client side
  QHBoxLayout* pHostServerInfo = new QHBoxLayout(this);
  {
    mpAddress = new QLineEdit("realisim.dyndns.org",this);
    mpPort = new QLineEdit("12345",this);
    
    pHostServerInfo->addWidget(mpAddress);
    pHostServerInfo->addWidget(mpPort);
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
  
  QVBoxLayout* pPeerViewLyt = new QVBoxLayout();
  {
  	mpPeerListView = new QTreeWidget( this );
    pPeerViewLyt->addWidget( mpPeerListView );
  }
    
  //---générale
  QVBoxLayout* pLogLyt = new QVBoxLayout();
  {
  	QLabel* pLog = new QLabel("Log:", this);
    mpLog = new QTextEdit(this);
    
    pLogLyt->addWidget( pLog );
    pLogLyt->addWidget( mpLog );
  }
  
  
  //---assemblage dans le layout    
  pMainLyt->addLayout( pHostServerInfo );
  pMainLyt->addLayout( pClientButtonsLyt );
  pMainLyt->addLayout( pPeerViewLyt, 6 );
  pMainLyt->addLayout( pLogLyt, 1 );
  pMainLyt->addStretch(1);
}

//------------------------------------------------------------------------------
void Widget::peerItemDoubleClicked( QTreeWidgetItem* iItem, int iCol )
{
	int i = mpPeerListView->indexOfTopLevelItem( iItem );
  const chatPeer& peer = mPeers[i];
  chatWindow* w = getChatWindow( peer );
  w->show();
  w->raise();
}

//------------------------------------------------------------------------------
void Widget::sentPacket( int iId )
{
	if( mClient.getUploadStatus( iId ) < 1.0 )
  {
  	chatPeer cp = prototypes::to( mClient.getUpload( iId ) );
    chatWindow* w = getChatWindow( cp );
    if( w ) w->updateUi();
  }

}

//------------------------------------------------------------------------------
void Widget::socketConnected()
{
	srand( time(0) );
  int a = rand();
  QString name;
  name.sprintf( "%08X", a );
  mPeer.setName( name );
  mPeer.setAddress( mClient.getLocalAddress() );
  mClient.send( makePeerNamePacket( mPeer.getName() ) );
  updateUi();
}

//------------------------------------------------------------------------------
void Widget::socketDisconnected()
{
	mPeers.clear();
	updateUi();
}

//------------------------------------------------------------------------------
void Widget::updateUi()
{
	mpConnect->setEnabled( !mClient.isConnected() );
  mpDisconnect->setEnabled( mClient.isConnected() );
  
  //la liste des peers
  mpPeerListView->clear();
  for( uint i = 0; i < mPeers.size(); ++i )
  {  
    QTreeWidgetItem* item = new QTreeWidgetItem();
    item->setText( 0, mPeers[i].getName() );
    //item->setText( 1, mPeers[i].getAddress() );
    mpPeerListView->addTopLevelItem( item );
  }
  
	if( mClient.hasError() )
  {
		mpLog->setText( mClient.getAndClearLastErrors() + "\n" + mpLog->toPlainText() );
  }
  
  if( mClient.hasUploads() )
  {
  	
  }
}

//------------------------------------------------------------------------------
//--- chatWindow
//------------------------------------------------------------------------------
chatWindow::chatWindow( Client& iClient,
	const chatPeer& iPeer,
	const chatPeer& iChatPeer ) :
  QWidget( 0 ),
  mClient( iClient ),
  mPeer( iPeer ),
  mChatPeer( iChatPeer )
{
  QVBoxLayout* pMainLyt = new QVBoxLayout( this );
  pMainLyt->setSpacing( 5 );
  pMainLyt->setMargin( 5 );

  QVBoxLayout* pChatLyt = new QVBoxLayout();
  {
  	mpChatLogView = new QTextEdit( this );
    mpChatLogView->setReadOnly( true ); 
    QHBoxLayout* pChatLineLyt = new QHBoxLayout();
    {
      mpChat = new QLineEdit( this );
      QPushButton* pSend = new QPushButton( "send", this );
      connect( pSend, SIGNAL( clicked() ), this, SLOT( sendChat() ) );
      
      QPushButton* pSendFile = new QPushButton( "send file...", this );
      connect( pSendFile, SIGNAL( clicked() ), this, SLOT( sendFile() ) );
      
      pChatLineLyt->addWidget( mpChat, 5 );
      pChatLineLyt->addWidget( pSend, 1 );
      pChatLineLyt->addWidget( pSendFile, 1 );
    }
    pChatLyt->addWidget( mpChatLogView, 5 );
    pChatLyt->addLayout( pChatLineLyt, 1);
  }
  
  QHBoxLayout* pProgressLyt = new QHBoxLayout();
  {
  	QVBoxLayout* pVlyt = new QVBoxLayout();
    mpProgressUpload = new QProgressBar( this );
    mpProgressDownload = new QProgressBar( this );
    mpProgressUpload->setRange( 1, 100 );
    mpProgressDownload->setRange( 1, 100 );
    
    pVlyt->addWidget( mpProgressUpload );
    pVlyt->addWidget( mpProgressDownload );
    
    pProgressLyt->addStretch( 1 );
    pProgressLyt->addLayout( pVlyt );
  }

	pMainLyt->addLayout( pChatLyt, 5 );
  pMainLyt->addLayout( pProgressLyt, 1 );
}

chatWindow::~chatWindow()
{}

//------------------------------------------------------------------------------
void chatWindow::gotChat( const QString& iChat )
{
	mChatLog.push_back( mChatPeer.getName() + " (" + QDateTime::currentDateTime().
  	toString( "dd-MMM-yy hh:mm" ) + "): " + iChat );
  updateUi();
}

//------------------------------------------------------------------------------
void chatWindow::gotFile( const QString& iFilename, const QByteArray& iBa )
{
	QString fileName = QFileDialog::getSaveFileName(this, tr("Save File"),
                            iFilename );
  if( !fileName.isEmpty() )
  {
  	QFile f( fileName );
    if( f.open( QIODevice::WriteOnly ) )
    	f.write( iBa );
  
  	mChatLog.push_back( mChatPeer.getName() + " (" + QDateTime::currentDateTime().
  	toString( "dd-MMM-yy hh:mm" ) + "): saved " + fileName );
  }
  else 
  {
    mChatLog.push_back( mChatPeer.getName() + " (" + QDateTime::currentDateTime().
    	toString( "dd-MMM-yy hh:mm" ) + "): discarded " + iFilename );
  }

	mpProgressDownload->reset();

  updateUi();
}

//------------------------------------------------------------------------------
void chatWindow::sendChat()
{ 
	mClient.send( makeTextPacket( mpChat->text(), mPeer, mChatPeer ) );
  mChatLog.push_back( mPeer.getName() + " (" + QDateTime::currentDateTime().
  	toString( "dd-MMM-yy hh:mm" ) + "): " + mpChat->text() );
  mpChat->clear();
  updateUi();
}

//------------------------------------------------------------------------------
void chatWindow::sendFile()
{ 
 	QString fileName = QFileDialog::getOpenFileName( this, "choose File",
		"/home" );
  if( !fileName.isEmpty() )
  {
  	QFile f( fileName );
    if( f.open( QIODevice::ReadOnly ) )
    {
    	mClient.send( makeFilePacket( f, mPeer, mChatPeer ) );
	    updateUi();
    }
  }
}
//------------------------------------------------------------------------------
void chatWindow::updateUi()
{
	QString m;
  for( int i = 0; i < mChatLog.size(); ++i )
  	m += mChatLog[i] + "\n";
	mpChatLogView->setText( m );
  
  double totalDownload = 0.0;
  int downloadCount = 0;
  for ( int i = 0; i < mClient.getNumberOfDownloads(); ++i)
  {
  	int id = mClient.getDownloadId( i );
  	double progress = 
    	mClient.getDownloadStatus( id );
    if( progress < 1.0 )
    {
    	if( prototypes::from( mClient.getDownload( id ) ) == mChatPeer )
      {
      	totalDownload += progress;
        downloadCount++;
      }
    }
  }
  totalDownload /= (double)downloadCount;
  mpProgressDownload->setValue( totalDownload * 100 );
  
  double totalUpload = 0.0;
  int uploadCount = 0;
  for ( int i = 0; i < mClient.getNumberOfUploads(); ++i)
  {
  	int id = mClient.getUploadId( i );
  	double progress = 
    	mClient.getUploadStatus( id );
    
    if( prototypes::to( mClient.getUpload( id ) ) == mChatPeer )
    {
    	totalUpload += progress;
      uploadCount++;    	
    }
  }
  totalUpload /= (double)uploadCount;
  if( totalUpload < 1.0 )
  { mpProgressUpload->setValue( totalUpload * 100 ); }
  else mpProgressUpload->reset();
}

