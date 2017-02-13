
#include <cassert>
#include "MessageQueue.h"


using namespace std;

MessageQueue::MessageQueue() :
mThread(),
mQueue(),
mMutex(),
mState(sIdle)
{
    using std::placeholders::_1;
    mProcessingFunction = std::bind(&MessageQueue::dummyProcessingFunction, this, _1);
}

//------------------------------------------------------------------------------
MessageQueue::~MessageQueue()
{
    stopThread();
    assert(mQueue.empty());
}

//------------------------------------------------------------------------------
void MessageQueue::clear()
{
    mMutex.lock();
    mQueue.clear();
    mMutex.unlock();
}

//------------------------------------------------------------------------------
void MessageQueue::dummyProcessingFunction(Message* iM)
{
    printf("dummy processing function\n" );
}

//------------------------------------------------------------------------------
int MessageQueue::getNumberOfMessageInQueue() const
{
    int r = 0;
    mMutex.lock();
    r = (int)mQueue.size();
    mMutex.unlock();
    return r;
}

//------------------------------------------------------------------------------
MessageQueue::state MessageQueue::getState() const
{
    return mState;
}

//------------------------------------------------------------------------------
// This is the function executed in a thread when method start() is called
void MessageQueue::threadLoop()
{
    // TODO.
    // have a wait condition when the queue is empty so we don't waste cpu.
    // wait will stop when a message is posted...
    
    while( getNumberOfMessageInQueue() > 0 || getState() == sRunning )
    {
        processNextMessage();
    }
    
    setState(sIdle);
}

//------------------------------------------------------------------------------
void MessageQueue::post( Message* iM )
{
    // the queue will not accept messages when a request to stop
    // has been issued. Else it would never terminate...
    //
    if(getState() != sStopping)
    {
        mMutex.lock();
        mQueue.push_back(iM);
        mMutex.unlock();
    }
}

//------------------------------------------------------------------------------
void MessageQueue::processMessages()
{
    while( getNumberOfMessageInQueue() > 0 )
    {
        processNextMessage();
    }
}

//------------------------------------------------------------------------------
void MessageQueue::processNextMessage()
{
    // pop first message and process it!
    if( getNumberOfMessageInQueue() > 0 )
    {
        mMutex.lock();
        Message *m = mQueue.front();
        mQueue.pop_front();
        mMutex.unlock();
        
        mProcessingFunction(m);
        
        delete m;
    }
}

//------------------------------------------------------------------------------
void MessageQueue::setProcessingFunction(std::function<void(Message*)> iFunction)
{
    if(iFunction)
    {
        mProcessingFunction = iFunction;
    }
    else
    {
        using std::placeholders::_1;
        mProcessingFunction = std::bind(&MessageQueue::dummyProcessingFunction, this, _1);
    }
}

//------------------------------------------------------------------------------
void MessageQueue::setState(MessageQueue::state iState)
{
    mMutex.lock();
    if(mState != iState)
    {
        mState = iState;
    }
    mMutex.unlock();
}

//------------------------------------------------------------------------------
void MessageQueue::startInThread()
{
    //early out
    if(getState() != sIdle) return;
    
    mMutex.lock();
    mThread = std::thread(&MessageQueue::threadLoop, this);
    
    setState(sRunning);
    mMutex.unlock();
}

//------------------------------------------------------------------------------
void MessageQueue::stopThread()
{
    if(mThread.joinable())
    {
        assert(getState() == sRunning && "If we are not in running state, it means we have a state issue...");
        setState(sStopping);
        mThread.join(); //wait till end of execution
    }
}

//------------------------------------------------------------------------------
//--- MessageQueue::Message
//------------------------------------------------------------------------------
MessageQueue::Message::Message(void *ipSender):
mpSender(ipSender)
{}
