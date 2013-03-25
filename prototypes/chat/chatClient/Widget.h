/*Created by Pierre-Olivier Beaudoin on 10-02-20.*/

#ifndef Realisim_Prototypes_Network_Widget_hh
#define Realisim_Prototypes_Network_Widget_hh

#include "protocol.h"
#include "network/ClientBase.h"
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
	chatWindow( reusables::network::ClientBase&, const chatPeer&, const chatPeer& );
  virtual ~chatWindow();
  
  void gotChat( const QString& );
  void gotFile( const QString&, const QByteArray& );
  
protected slots:
  virtual void sendChat();
  virtual void sendFile();
  
protected:
	virtual void updateUi();
  
  QTextEdit* mpChatLogView;
  QLineEdit* mpChat;
  QProgressBar* mpProgressUpload;
  QProgressBar* mpProgressDownload;

  QStringList mChatLog;
  reusables::network::ClientBase& mClient;
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
  virtual void downloadEnded();
  virtual void downloadStarted();
  virtual void gotError();
  virtual void peerItemDoubleClicked( QTreeWidgetItem*, int );
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

	reusables::network::ClientBase mClient;
  std::vector< chatPeer > mPeers;
  std::map< int, chatWindow* > mPeerToChatWindow;
  chatPeer mPeer;
};

}
}

#endif