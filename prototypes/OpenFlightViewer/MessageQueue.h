#pragma once

#include <condition_variable>
#include <deque>
#include <functional>
#include <mutex>
#include <string>
#include <thread>

class MessageQueue
{
public:
    MessageQueue();
    MessageQueue(const MessageQueue&) = delete;
    MessageQueue& operator=(const MessageQueue&) = delete;
    virtual ~MessageQueue();
    
    enum state{sIdle, sRunning, sStopping};
    
    class Message
    {
    public:
        Message() = delete;
        explicit Message(void* ipSender);
        Message(const Message&) = default;
        Message& operator=(const Message&) = default;
        virtual ~Message() = default;
        
        void* mpSender;
    };
    
    void clear();
    int getNumberOfMessageInQueue() const;
    state getState() const;
    void post( Message* );
    void processNextMessage();
    void processMessages();
    void setProcessingFunction(std::function<void(Message*)>);
    void startInThread();
    void stopThread();
    
    
protected:
    void dummyProcessingFunction(Message*);
    void threadLoop();
    void setState(state);
    
    std::thread mThread;
    std::deque<Message*> mQueue;
    mutable std::recursive_mutex mMutex;
    std::mutex mWaitConditionMutex;
    std::condition_variable mQueueNotEmptyCondition;
    state mState;
    std::function<void(Message*)> mProcessingFunction;
};