#pragma once

#include "MessageQueue.h"
#include <map>

class IGraphicNode;

class FileStreamer
{
public:
    FileStreamer();
    FileStreamer(const FileStreamer&) = delete;
    FileStreamer& operator=(const FileStreamer&) = delete;
    ~FileStreamer() = default;

    enum requestType{ rtUndefined, rtLoadFlt, rtLoadRgbImage };
    class Request : public MessageQueue::Message
    {
    public:
        Request() = delete;
        Request(void *ipSender);
        Request(const Request&) = delete;
        Request& operator=(const Request&) = delete;
        ~Request() = default;
        
        requestType mRequestType;
        std::string mFilenamePath;
    };

    class DoneRequest : public MessageQueue::Message
    {
    public:
        DoneRequest() = delete;
        DoneRequest(void *ipSender);
        DoneRequest(const DoneRequest&) = delete;
        DoneRequest& operator=(const DoneRequest&) = delete;
        ~DoneRequest() = default;
        
        requestType mRequestType;
        std::string mFilenamePath;
        void* mpData;
    };

    void postRequest(Request*);
    void registerDoneQueue(void *ipRequester, MessageQueue* ipDoneQueue);

private:
    IGraphicNode* loadFlt(const std::string& iFilenamePath);
    void processMessage(MessageQueue::Message*);
    std::string toString(requestType);

    MessageQueue mRequestQueue;
    std::map<void*, MessageQueue*> mSenderToDoneQueue;
};