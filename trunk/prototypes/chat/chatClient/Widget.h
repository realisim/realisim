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
  virtual void connectToServer();
  virtual void disconnectFromServer();
  virtual void gotError();
  virtual void sendChat();
  virtual void sendFile();
  virtual void updateUi();
  virtual void writeTest();
  
protected:
  QLineEdit* mpAddress;
  QLineEdit* mpPort;
  QPushButton* mpConnect;
  QPushButton* mpDisconnect;
  QLineEdit* mpChat;
  QTextEdit* mpLog;
  QProgressBar* mpProgressUpload;
  QProgressBar* mpProgressDownload;

  reusables::network::ClientBase mClient;
  
private:
  void initUi();
};

}
}

#endif