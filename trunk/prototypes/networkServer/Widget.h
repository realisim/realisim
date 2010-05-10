/*Created by Pierre-Olivier Beaudoin on 10-02-20.*/

#ifndef Realisim_Prototypes_Network_Widget_hh
#define Realisim_Prototypes_Network_Widget_hh

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
  void error();
  void newPeerConnected();
  void startServer();
  void stopServer();
  
protected:
  QLineEdit* mpPort;
  QPushButton* mpStartServer;
  QPushButton* mpStopServer;
  QTreeWidget* mpConnectedPeers;
      
  //générale
  QListWidget* mpLog;

  reusables::network::ServerBase mServer;
  
private:
  void initUi();
};

}
}

#endif