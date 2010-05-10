/*Created by Pierre-Olivier Beaudoin on 10-02-20.*/

#ifndef Realisim_Prototypes_Network_Widget_hh
#define Realisim_Prototypes_Network_Widget_hh

#include <QWidget>
#include <QtGui>
#include "network/ClientBase.h"

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
  void connectToServer();
  void disconnectFromServer();
  void error();
  void peersListChanged();
  
protected:
  QLineEdit* mpAddress;
  QLineEdit* mpPort;
  QPushButton* mpConnect;
  QPushButton* mpDisconnect;
  QListWidget* mpLog;

  reusables::network::ClientBase mClient;
  
private:
  void initUi();
};

}
}

#endif