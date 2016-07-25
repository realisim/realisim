
#include <Broker.h>
#include <cassert>
#include <Core.h>
#include <QCoreApplication>
#include <render/Renderer.h>
#include <SceneManagement.h>

using namespace engine3d;
using namespace core;

//----------------------------------------------
//--- Core
//----------------------------------------------
Core::Core() : QObject(),
mState(sNotInitialized),
mRequestedState(sNotInitialized),
mLoopTimerId(0),
mpBroker(nullptr),
mpSceneManagement(nullptr),
mpRenderer(nullptr)
{
  mLog.logToConsole(true);
  mLog.logToFile(true, "log.txt");
  
  mpBroker = new Broker();
  mpSceneManagement = new SceneManagement();
  mpRenderer = new render::Renderer(nullptr);
  
  registerClient(mpSceneManagement);
  registerClient(mpRenderer);
  
  mHub.initialize( mpBroker, mpSceneManagement, mpRenderer );
}

Core::~Core()
{
  mClients.clear();
}
  
//----------------------------------------------
void Core::goToState(state iState)
{
  if( getState() != iState )
  {
    mRequestedState = iState;
    if (requestStateChange())
    {
      switch( getState() )
      {
        case sNotInitialized:
        {
          switch(iState)
          {
            case sInitializing:
              mState = sInitializing;
              stateChanged();
              mLoopTimerId = startTimer(16);
              mLoopTimer.start();
              goToState(sRunning); //from intializing, we go directly to running
              break;
            case sQuitting:
              mState = sQuitting;
              stateChanged();
              QCoreApplication::quit(); //quit the app
              break;
            default: break;
          }
        } break;
        case sInitializing:
        {
          switch(iState)
          {
            case sRunning:
              mState = sRunning;
              stateChanged();
              mLoopTimerId = startTimer(16);
              mLoopTimer.start();
              break;
            case sQuitting:
              mState = sQuitting;
              stateChanged();
              QCoreApplication::quit(); //quit the app
              break;
            default: break;
          }
        } break;
        case sRunning:
        {
          switch(iState)
          {
            case sPaused:
              mState = sPaused;
              stateChanged();
              killTimer(mLoopTimerId);
              mLoopTimerId = 0;
              mLoopTimer.stop();
              break;
            case sQuitting:
              mState = sQuitting;
              stateChanged();
              QCoreApplication::quit(); //quit the app
              break;
            default: break;
          }
        } break;
        case sPaused:
        {
          switch(iState)
          {
            case sRunning:
              mState = sRunning;
              stateChanged();
              mLoopTimerId = startTimer(16);
              mLoopTimer.start();
              break;
            case sQuitting:
              mState = sQuitting;
              stateChanged();
              QCoreApplication::quit(); //quit the app
              break;
            default: break;
          }
        } break;
        case sQuitting:
          break;
        default: break;
      }
    }
  }
}

//----------------------------------------------
void Core::quit()
{
  goToState(sQuitting);
}


//----------------------------------------------
void Core::registerClient(Client* c)
{
  if(getState() == sNotInitialized)
  {
    c->setHub(&mHub);
    c->setLog(&mLog);
    mClients.push_back(c);
  }
  else
  { mLog.log("%s: Registering client is only available when core is not yet initialized.", __FILE__); }
}

//----------------------------------------------
bool Core::requestStateChange() const
{
  mLog.log("%s: Requesting a state change (%d to %d)", __FILE__, getState(), mRequestedState );
  bool r = true;
  for(auto it = mClients.begin(); it != mClients.end(); ++it)
  {
    r &= (*it)->requestStateChange( getState(), mRequestedState);
  }
  return r;
}

//----------------------------------------------
void Core::start()
{
  goToState(sInitializing);
}

//----------------------------------------------
void Core::stateChanged()
{
  mLog.log("%s: state changed to (%d)", __FILE__, getState());
  for(auto it = mClients.begin(); it != mClients.end(); ++it)
  { (*it)->stateChanged(getState()); }
}

//----------------------------------------------
void Core::timerEvent(QTimerEvent *ipE)
{
  if(ipE->timerId() == mLoopTimerId)
  {
    if( getState() != mRequestedState )
    { goToState(mRequestedState); }
    
    update(mLoopTimer.getElapsed());
    mLoopTimer.start(); //restart the loop timer
  }
}

//----------------------------------------------
void Core::unregisterClient(Client* c)
{
  auto it = find(mClients.begin(), mClients.end(), c);
  if( it != mClients.end() )
  { mClients.erase(it); }
}

//----------------------------------------------
void Core::update(double iSecondsElapsed)
{
  for(auto it = mClients.begin(); it != mClients.end(); ++it)
  { (*it)->update(iSecondsElapsed); }
}

//----------------------------------------------
//--- Core::Client
//----------------------------------------------
Core::Client::Client() :
mpHub(nullptr)
{}

//----------------------------------------------
Core::Client::~Client()
{}

//----------------------------------------------
bool Core::Client::requestStateChange(Core::state from,
                                      Core::state to)
{ return true;}

//----------------------------------------------
void Core::Client::setHub(core::Hub *ipHub)
{
  assert(mpHub == nullptr && "core::Client setHub should never be called twice...");
  mpHub = ipHub;
}

//----------------------------------------------
void Core::Client::setLog(realisim::utils::Log *ipLog)
{
  mpLog = ipLog;
}

//----------------------------------------------
void Core::Client::stateChanged(Core::state)
{}

//----------------------------------------------
void Core::Client::update(double)
{}

