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
  
  connect(&mServer, SIGNAL(error()), this, SLOT(error()));
  connect(&mServer, SIGNAL(newPeerConnected()),
    this, SLOT(newPeerConnected()));
}

Widget::~Widget()
{}

//------------------------------------------------------------------------------
void Widget::error()
{
	new QListWidgetItem(mServer.getLastError(), mpLog);  
}

//------------------------------------------------------------------------------
void Widget::initUi()
{
	QGridLayout* pMainLyt = new QGridLayout(this);
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
  
  QLabel* pConnectedPeers = new QLabel("Connected Peers:", this);
  mpConnectedPeers = new QTreeWidget(this);
  mpConnectedPeers->setColumnCount(2);
  QTreeWidgetItem* item = new QTreeWidgetItem();
  item->setText(0, "Id");
  item->setText(0, "Ip Address");
  mpConnectedPeers->setHeaderItem(item);
  
  //---générale
  QLabel* pLog = new QLabel("Log:", this);
  mpLog = new QListWidget(this);
  
  //---assemblage dans le layout
  int row = 0;
  pMainLyt->addLayout(pServerInfo, row++, 0);
  pMainLyt->addLayout(pButtonsLyt, row++, 0);
  pMainLyt->addWidget(pConnectedPeers, row++, 0);
  pMainLyt->addWidget(mpConnectedPeers, row++, 0);
  
  pMainLyt->addWidget(pLog, row++, 0);
  pMainLyt->addWidget(mpLog, row++, 0);
  
  pMainLyt->setRowStretch(4, 1);
}

//------------------------------------------------------------------------------
void Widget::newPeerConnected()
{
  QStringList c = mServer.getPeersIps();
  mpConnectedPeers->clear();
  for(int i = 0; i < c.size(); ++i)
  {
    QTreeWidgetItem* item = new QTreeWidgetItem();
    item->setText(0, QString::number(i));
    item->setText(0, c[i]);
    mpConnectedPeers->insertTopLevelItem(0, item);
  }
    
}

//------------------------------------------------------------------------------
void Widget::startServer()
{
  if(mServer.startServer(mpPort->text().toInt()))
  {
    mpStartServer->setDisabled(true);
    mpStopServer->setEnabled(true);
  }
}

//------------------------------------------------------------------------------
void Widget::stopServer()
{
  mServer.stopServer();
  mpStartServer->setEnabled(true);
  mpStopServer->setDisabled(true);
}
