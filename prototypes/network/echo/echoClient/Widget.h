/*Created by Pierre-Olivier Beaudoin on 10-02-20.*/

#ifndef Realisim_Prototypes_Network_Widget_hh
#define Realisim_Prototypes_Network_Widget_hh

#include "protocol.h"
#include "network/Client.h"
#include <QWidget>
#include <QtGui>
#include <vector>

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
  virtual void downloadEnded( int );
  virtual void errorRaised();
  virtual void gotPacket(int);
  virtual void protocolChanged(int);
  virtual void sendClicked();
  virtual void sentPacket(int);
  virtual void socketConnected();
  virtual void socketDisconnected();
  virtual void updateUi();
  
protected:
	virtual void addToLog( QString );
	virtual void initUi();
  
  QLineEdit* mpAddress;
  QLineEdit* mpPort;
  QPushButton* mpConnect;
  QPushButton* mpDisconnect;
  QTextEdit* mpLog;
  QLineEdit* mpInput;
  QPushButton* mpSend;
  QComboBox* mpProtocols;
  
	reusables::network::Client mClient;
  QString mLog;
};

}
}

#endif