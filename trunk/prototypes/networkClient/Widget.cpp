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
  connect(&mClient, SIGNAL(error()), this, SLOT(error()));
  connect(&mClient, SIGNAL(peersListChanged()), this, SLOT(peersListChanged()));
}

Widget::~Widget()
{}

//------------------------------------------------------------------------------
void Widget::connectToServer()
{
  mClient.connectToTcpServer(mpAddress->text(),
     mpPort->text().toInt());
}

//------------------------------------------------------------------------------
void Widget::disconnectFromServer()
{
  mClient.disconnectFromTcpServer();
  //mpConnect->setEnabled(true);
  //mpDisconnect->setDisabled(true);
}

//------------------------------------------------------------------------------
void Widget::error()
{
	new QListWidgetItem(mClient.getLastError(), mpLog);  
}

//------------------------------------------------------------------------------
void Widget::initUi()
{
	QGridLayout* pMainLyt = new QGridLayout(this);
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
    //mpDisconnect->setDisabled(true);
    pClientButtonsLyt->addStretch(1);
    pClientButtonsLyt->addWidget(mpConnect);
    pClientButtonsLyt->addWidget(mpDisconnect);
    
    connect(mpConnect, SIGNAL(clicked()), this, SLOT(connectToServer()));
    connect(mpDisconnect, SIGNAL(clicked()), this, SLOT(disconnectFromServer()));
  }
  
  //---générale
  QLabel* pLog = new QLabel("Log:", this);
  mpLog = new QListWidget(this);
  
  //---assemblage dans le layout
  int row = 0;
    
  pMainLyt->addLayout(pHostServerInfo, row++, 0);
  pMainLyt->addLayout(pClientButtonsLyt, row++, 0);
  
  pMainLyt->addWidget(pLog, row++, 0);
  pMainLyt->addWidget(mpLog, row++, 0);
  
  pMainLyt->setRowStretch(4, 1);
}

//------------------------------------------------------------------------------
void Widget::peersListChanged()
{
	for(int i = 0; i < mClient.getPeersList().size(); ++i)
 		new QListWidgetItem(mClient.getPeersList()[i], mpLog); 
}