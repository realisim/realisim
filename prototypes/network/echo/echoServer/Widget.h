/*Created by Pierre-Olivier Beaudoin on 10-02-20.*/

#ifndef Realisim_Prototypes_Network_Widget_hh
#define Realisim_Prototypes_Network_Widget_hh

#include "protocol.h"
#include <QTextEdit>
#include <QWidget>
#include <QtGui>
#include <vector>
#include "network/Server.h"

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
	virtual void downloadEnded(int, int );
  virtual void errorRaised();
  virtual void gotPacket(int, int);
  virtual void protocolChanged(int);
  virtual void sentPacket(int, int);
	virtual void socketConnected( int );
  virtual void socketDisconnected( int );
  virtual void startServer();
  virtual void stopServer();
  virtual void updateUi();
  virtual void uploadEnded( int, int );
  
protected:
	void addToLog( QString ) const;
  QLineEdit* mpPort;
  QPushButton* mpStartServer;
  QPushButton* mpStopServer;
  QTreeWidget* mpConnectedPeers;
  QTextEdit* mpLog;
  QLabel* mpNumberOfPeers;
  QComboBox* mpProtocols;

  reusables::network::Server mServer;
  mutable QString mLog;
private:
  void initUi();
};

}
}

#endif