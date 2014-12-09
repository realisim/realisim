/*
 */

#ifndef realisim_remoteDesktop_MainDialog_hh
#define realisim_remoteDesktop_MainDialog_hh

#include <network/Client.h>
#include <network/Server.h>
#include <QtGui>
#include "utils/log.h"
#include <vector>

namespace realisim 
{

class protocol
{
public:
  protocol();
  enum message{ mRequestDesktopInfo, mGiveDesktopInfo, mKeyPressed,
  	mKeyReleased, mKeyRepeated, mMouseMoved, mMousePressed, mMouseReleased,
    mNewFrame };
                    
  static QByteArray makeMessageGiveDesktopInfo( QByteArray );
  static QByteArray makeMessageKeyPressed( QString, quint32 );
  static QByteArray makeMessageKeyReleased( QString, quint32 );
  static QByteArray makeMessageKeyRepeated( QString, quint32 );
  static QByteArray makeMessageMouseMoved( QPointF );
  static QByteArray makeMessageMousePressed( QPointF );
  static QByteArray makeMessageMouseReleased( QPointF );
  static QByteArray makeMessageNewFrame( QPixmap, QRect );
  static QByteArray makeMessageRequestDesktopInfo();
  static QString toString( message );
  static int mVersion;
};

class desktopInfo
{
public:
	desktopInfo();
  ~desktopInfo();

	void clear();
	int getNumberOfScreens() const;
  QSize getScreenSize( int ) const;
	void fromBinary( QByteArray );
  QByteArray toBinary() const;
  void setNumberOfScreens( int );
  void setScreenSize( int, QSize );
      
protected:
	static int mVersion;
  std::vector<QSize> mScreenSizes;
};

class graphicsView : public QGraphicsView
{
	Q_OBJECT
public:	
	graphicsView( QGraphicsScene*, QWidget* = 0 );
  virtual ~graphicsView();
  
signals:
	void keyPressed( QKeyEvent* );
  void keyReleased( QKeyEvent* );
  void keyRepeated( QKeyEvent* );
	void mouseMoved( QPoint );
	void mousePressed( QPoint );
  void mouseReleased( QPoint );
  
protected:
	virtual void keyPressEvent( QKeyEvent* );
	virtual void keyReleaseEvent( QKeyEvent* );
	virtual void mouseDoubleClickEvent( QMouseEvent* );
	virtual void mouseMoveEvent( QMouseEvent* );
	virtual void mousePressEvent( QMouseEvent* );
	virtual void mouseReleaseEvent( QMouseEvent* );
};
  
//-----------------------------------------------------------------------------
class MainDialog : public QMainWindow
{
	Q_OBJECT
public:
	MainDialog();
	~MainDialog();
  
protected slots:
	void clientDownloadEnded(int);
  void clientSocketConnected();
  void clientSocketDisconnected();
	void connectClicked();
  void hubClientDownloadEnded(int);
  void hubClientSocketConnected();
  void hubClientSocketDisconnected();
  void hubPeerDoubleClicked( QListWidgetItem* );
  void keyPressedInView( QKeyEvent* );
  void keyReleasedInView( QKeyEvent* );
  void keyRepeatedInView( QKeyEvent* );
  void mouseMovedInView( QPoint );
  void mousePressedInView( QPoint );
  void mouseReleasedInView( QPoint );
  void serverDownloadEnded( int, int );
  void serverSocketAboutToDisconnect( int );
  void serverSocketConnected( int );	
  void serverUploadEnded( int, int );
	void updateUi();
                
protected:
	enum clientState { csIdle, csBrowsingHub, csConnecting, csActive };
	enum serverMode { smIdle, smActive };
	

	void createUi();
  clientState getClientState() const { return mClientState; }
  QRect getModifiedRegion( QPixmap, QPixmap ) const;
  serverMode getServerMode() const {return mServerMode; }
  void handleKeyboardInputFromClient( protocol::message, QString, quint32 );
  void handleMessageFromClient( int, QByteArray );
  void handleMessageFromServer( QByteArray );
  void handleMouseInputFromClient( protocol::message, QPointF );
  void setClientState( clientState );
  void setServerMode( serverMode );
  
  clientState mClientState;
  serverMode mServerMode;
  network::Client mHubClient;
  network::Client mHolePunchClient;
  network::Client mClient;
  network::Server mServer;
  utils::Log mClientLog;
  utils::Log mServerLog;
  desktopInfo mDesktopInfo;
  desktopInfo mRemoteDesktopInfo;
  int mCurrentFrameId;
  std::vector<QString> mPeerAddresses;
  
  //--- pour le client
  //------ hub
  QFrame* mpClientHubFrame;
  QListWidget* mpPeerListWidget;
  
  //------ connection
  QFrame* mpClientConnectionFrame;
  QLineEdit* mpHostName;
  QLineEdit* mpHostPort;
  
  //------ activity
  QFrame* mpClientActivityFrame;
  graphicsView* mpView;
  QGraphicsScene* mpScene;
  QGraphicsPixmapItem* mpRemoteDesktopItem;
  QPixmap mRemoteDesktopPixmap;
  QRect mRemoteDesktopPixmapRect;
  
  //--- pour le server
  QFrame* mpServerFrame;
  QLabel* mpServerInfo;
  QPixmap mDesktopPixmap;
};

}

#endif
