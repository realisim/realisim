#pragma once

#include "MessageQueue.h"
#include <map>
#include <set>
#include <Windows.h>

class QGLContext;

class GpuStreamer
{
public:
    GpuStreamer();
    GpuStreamer(const GpuStreamer&) = delete;
    GpuStreamer& operator=(const GpuStreamer&) = delete;
    ~GpuStreamer() = default;

    enum messageType{ mtUndefined, mtTexture };
    
    class Message : public MessageQueue::Message
    {
    public:
        Message() = delete;
        Message(void *ipSender);
        Message(const Message&) = delete;
        Message& operator=(const Message&) = delete;
        ~Message() = default;
        
        messageType mMessageType;
//        std::string mFilenamePath;
//        unsigned int mAffectedDefinitionId;
//        void* mpData;
    };

    void postMessage(Message*);
    void registerDoneQueue(void *ipRequester, MessageQueue* ipDoneQueue);
    void setGLContext(HDC, HGLRC);

private:
    void processMessage(MessageQueue::Message*);
    std::string toString(messageType);

    bool mMakeCurrentNeeded;
    MessageQueue mRequestQueue;
    std::map<void*, MessageQueue*> mSenderToDoneQueue;
    //std::set<std::string> mPendingFileRequests;
    HGLRC mGLContext;
    HDC mDC;
};