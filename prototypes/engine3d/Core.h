/**/

#pragma once

#include <Hub.h>
#include <set>
#include <QObject>
#include <QTimerEvent>
#include <utils/log.h>
#include <utils/Timer.h>

namespace engine3d { namespace core { class Broker; } }
namespace engine3d { namespace core { class SceneManagement; } }
namespace engine3d { namespace render { class Renderer; } }

namespace engine3d
{
namespace core
{
  
  class Core : public QObject
  {
    Q_OBJECT
  public:
    Core();
    ~Core();
 
    enum state{sNotInitialized, sInitializing, sRunning, sPaused,
      sQuitting};

    class Client
    {
    public:
      friend class Core;
      
      Client();
      virtual ~Client();
      
      core::Hub& getHub() {return *mpHub;};
      
    protected:
      virtual bool requestStateChange(state from, state to);
      void setHub(core::Hub*);
      void setLog(realisim::utils::Log*);
      virtual void stateChanged(state);
      virtual void update(double);
      
      core::Hub *mpHub; //not owned
      realisim::utils::Log *mpLog; //not owned
    };
    
    Hub& getHub() {return mHub;}
    state getState() const {return mState;}
//void loadFile(QString);
//    void pause(bool);
    void quit();
    void registerClient(Client*);
    void start();
    void unregisterClient(Client*);
    void update(double);
    
  private:
    virtual void timerEvent(QTimerEvent*) override;
    void goToState(state);
    bool requestStateChange() const;
    void stateChanged();
    
    Hub mHub;
    std::vector<Client*> mClients; //a vector because order is important
    realisim::utils::Log mLog;
    state mState;
    state mRequestedState;
    int mLoopTimerId;
    realisim::utils::Timer mLoopTimer;
    
    Broker *mpBroker;
    SceneManagement *mpSceneManagement;
    render::Renderer *mpRenderer;
    //InputHandler *mpInputHandler;
  };

  
}
}



