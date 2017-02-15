#pragma once

#include "MessageQueue.h"
#include <map>
#include <set>

class IGraphicNode;
class RgbImageLoader;

class FileStreamer
{
public:
    FileStreamer();
    FileStreamer(const FileStreamer&) = delete;
    FileStreamer& operator=(const FileStreamer&) = delete;
    ~FileStreamer() = default;

    enum requestType{ rtUndefined, rtLoadFlt, rtLoadRgbImage };
    
    // probably should be renamed to FileStreamer::Message...
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
        unsigned int mAffectedDefinitionId;
        void* mpData;
    };

    void postRequest(Request*);
    void registerDoneQueue(void *ipRequester, MessageQueue* ipDoneQueue);

private:
    IGraphicNode* loadFlt(const std::string& iFilenamePath);
    RgbImageLoader* loadRgbImage(const std::string& iFilenamePath);
    void processMessage(MessageQueue::Message*);
    std::string toString(requestType);

    MessageQueue mRequestQueue;
    std::map<void*, MessageQueue*> mSenderToDoneQueue;
    std::set<std::string> mPendingFileRequests;
};