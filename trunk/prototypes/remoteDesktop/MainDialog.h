/*
 */

#ifndef realisim_remoteDesktop_MainDialog_hh
#define realisim_remoteDesktop_MainDialog_hh

#include <network/Client.h>
#include <network/Server.h>
#include <QtGui>

namespace realisim 
{

class MainDialog : public QMainWindow
{
	Q_OBJECT
public:
	MainDialog();
	~MainDialog(){};
  
protected slots:
	void clientDownloadEnded(int);
  void clientSocketConnected();
  void clientSocketDisconnected();
	void connectClicked();
  void serverSocketConnected( int );
	void serverSocketDisconnected( int );
	void updateUi();
                
protected:
	enum clientMode { cmIdle, cmActiveMaster, cmActiveSlave };
	enum serverMode { smIdle, smActive };
  class protocol
  {
  	public:
    	protocol();
    	enum message{ mRequestMaster, mGiveUpMaster, mGiveMaster, mTakeBackMaster,
      	mNewFrame };
        
      static QByteArray makeMessageGiveMaster();
      static QByteArray makeMessageNewFrame( QPixmap );
    	static int mVersion;
  };

	void createUi();
  clientMode getClientMode() const { return mClientMode; }
  serverMode getServerMode() const {return mServerMode; }
  void handleMessageFromServer( QByteArray );
  void setClientMode( clientMode );
  void setServerMode( serverMode );
  void timerEvent( QTimerEvent* );
  
  clientMode mClientMode;
  serverMode mServerMode;
  reusables::network::Client mClient;
  reusables::network::Server mServer;
  
  //--- pour le client
  QFrame* mpClientConnectionFrame;
  QFrame* mpClientActivityFrame;
  QLineEdit* mpHostName;
  QLineEdit* mpHostPort;
  QLabel* mpLabel;
  
  QPixmap mDesktopPixmap;
  
  //--- pour le server
  QFrame* mpServerFrame;
  QLabel* mpServerInfo;
  
  int mServerTimerId;
};

}

#endif
