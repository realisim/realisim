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
  enum message{ mRequestDesktopInfo, mGiveDesktopInfo, mMouseMoved,
    mMousePressed, mMouseReleased, mNewFrame };
                    
  static QByteArray makeMessageGiveDesktopInfo( QByteArray );
  static QByteArray makeMessageMouseMoved( QPointF );
  static QByteArray makeMessageMousePressed( QPointF );
  static QByteArray makeMessageMouseReleased( QPointF );
  static QByteArray makeMessageNewFrame( QPixmap );
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
  void mouseMovedInView( QPoint );
  void mousePressedInView( QPoint );
  void mouseReleasedInView( QPoint );
  void serverDownloadEnded( int, int );
  void serverSocketConnected( int );
	void serverSocketDisconnected( int );
	void updateUi();
                
protected:
	enum clientMode { cmIdle, cmActiveMaster, cmActiveSlave };
	enum serverMode { smIdle, smActive };
	

	void createUi();
  clientMode getClientMode() const { return mClientMode; }
  serverMode getServerMode() const {return mServerMode; }
  void handleMessageFromClient( int, QByteArray );
  void handleMessageFromServer( QByteArray );
  void handleMouseInputFromClient( protocol::message, QPointF );
  void setClientMode( clientMode );
  void setServerMode( serverMode );
  void timerEvent( QTimerEvent* );
  
  clientMode mClientMode;
  serverMode mServerMode;
  reusables::network::Client mClient;
  reusables::network::Server mServer;
  int mServerTimerId;
  utils::Log mClientLog;
  utils::Log mServerLog;
  desktopInfo mDesktopInfo;
  desktopInfo mRemoteDesktopInfo;
  
  //--- pour le client
  QFrame* mpClientConnectionFrame;
  QFrame* mpClientActivityFrame;
  QLineEdit* mpHostName;
  QLineEdit* mpHostPort;
  graphicsView* mpView;
  QGraphicsScene* mpScene;
  QLabel* mpLabel;
  
  QPixmap mDesktopPixmap;
  
  //--- pour le server
  QFrame* mpServerFrame;
  QLabel* mpServerInfo;
};

}

#endif
