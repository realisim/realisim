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

class chatWindow : public QWidget
{
	Q_OBJECT
public:
	chatWindow( reusables::network::Client&, const chatPeer&, const chatPeer& );
  virtual ~chatWindow();
  
  void gotChat( const QString& );
  void gotFile( const QString&, const QByteArray& );
  virtual void updateUi();
  
protected slots:
  virtual void sendChat();
  virtual void sendFile();
  
protected:
  
  QTextEdit* mpChatLogView;
  QLineEdit* mpChat;
  QProgressBar* mpProgressUpload;
  QProgressBar* mpProgressDownload;

  QStringList mChatLog;
  reusables::network::Client& mClient;
  const chatPeer& mPeer;
  const chatPeer& mChatPeer;
};

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
  virtual void downloadStarted( int );
  virtual void gotError();
  virtual void gotPacket(int);
  virtual void peerItemDoubleClicked( QTreeWidgetItem*, int );
  virtual void sentPacket(int);
  virtual void socketConnected();
  virtual void socketDisconnected();
  virtual void updateUi();
  
protected:
	int findPeer( const chatPeer& ) const;
  chatWindow* getChatWindow( const chatPeer& );
	virtual void initUi();
  
  QLineEdit* mpAddress;
  QLineEdit* mpPort;
  QPushButton* mpConnect;
  QPushButton* mpDisconnect;
  QTreeWidget* mpPeerListView;
  QTextEdit* mpLog;

	reusables::network::Client mClient;
  std::vector< chatPeer > mPeers;
  std::map< int, chatWindow* > mPeerToChatWindow;
  chatPeer mPeer;
};

}
}

#endif