/*Created by Pierre-Olivier Beaudoin on 10-02-20.*/

#ifndef Realisim_Prototypes_Network_Widget_hh
#define Realisim_Prototypes_Network_Widget_hh

#include <QTextEdit>
#include <QWidget>
#include <QtGui>
#include "network/ServerBase.h"

namespace realisim 
{
namespace prototypes 
{

class Widget :public QWidget
{
  Q_OBJECT
public:
  Widget(QWidget* = 0);
  virtual ~Widget();
  
protected slots:
	virtual void gotPacket( int );
	virtual void peerConnected( int );
  virtual void peerDisconnected( int );
  virtual void startServer();
  virtual void stopServer();
  virtual void updateUi();
  
protected:
  
  QLineEdit* mpPort;
  QPushButton* mpStartServer;
  QPushButton* mpStopServer;
  QTreeWidget* mpConnectedPeers;
  QTextEdit* mpLog;
  QLabel* mpNumberOfPeers;

  reusables::network::ServerBase mServer;
  
private:
  void initUi();
};

}
}

#endif