#pragma once

#include "3d/Texture.h"
#include "Definitions.h"
#include "MessageQueue.h"
#include <map>
#include <set>
#include "utils/Timer.h"
#include <unordered_map>
#include <Windows.h>

class QGLContext;

class GpuStreamer
{
public:
    GpuStreamer();
    GpuStreamer(const GpuStreamer&) = delete;
    GpuStreamer& operator=(const GpuStreamer&) = delete;
    ~GpuStreamer();

    enum messageType{ mtUndefined, mtTexture, mtCreateModel };
    
    class Message : public MessageQueue::Message
    {
    public:
        Message() = delete;
        Message(void *ipSender);
        Message(const Message&) = delete;
        Message& operator=(const Message&) = delete;
        ~Message() = default;
        
        messageType mMessageType;
        unsigned int mAffectedDefinitionId;
        realisim::treeD::Texture mTexture;
        void *mpData ;
        ModelNode *mpModelNode;
        std::unordered_map<unsigned int, realisim::treeD::Texture> *mpImageIdToTexture;
    };

    void postMessage(Message*);
    void registerDoneQueue(void *ipRequester, MessageQueue* ipDoneQueue);
    void setGLContext(HDC, HGLRC);

private:
    realisim::treeD::Texture createTexture(Image*);
    void processMessage(MessageQueue::Message*);
    std::string toString(messageType);

    bool mMakeCurrentNeeded;
    MessageQueue mRequestQueue;
    std::map<void*, MessageQueue*> mSenderToDoneQueue;
    std::set<unsigned int> mPendingRequests;
    HGLRC mGLContext;
    HDC mDC;
    realisim::utils::Timer mTimer;
};