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
	virtual void downloadEnded( int, int );
	virtual void peerConnected( int );
  virtual void peerDisconnected( int );
  virtual void peerItemClicked( QTreeWidgetItem*, int );
  virtual void startServer();
  virtual void stopServer();
  virtual void updateUi();
  
protected:
	int findPeer( const chatPeer& ) const;
  virtual void printTransferInfo( const QByteArray& ) const;
  QLineEdit* mpPort;
  QPushButton* mpStartServer;
  QPushButton* mpStopServer;
  QTreeWidget* mpConnectedPeers;
  QTextEdit* mpLog;
  QLabel* mpNumberOfPeers;

  reusables::network::Server mServer;
  std::vector< chatPeer > mChatPeers;
  
private:
  void initUi();
};

}
}

#endif