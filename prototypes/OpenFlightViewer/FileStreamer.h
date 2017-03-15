#pragma once

#include "MessageQueue.h"
#include <map>
#include <set>
#include "Utils/Timer.h"

class IGraphicNode;
class RgbImage;

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
#ifdef MESSAGE_QUEUE_NO_THREADING
    void processNextMessage();
#endif // MESSAGE_QUEUE_NO_THREADING
    void registerDoneQueue(void *ipRequester, MessageQueue* ipDoneQueue);

private:
    IGraphicNode* loadFlt(const std::string& iFilenamePath);
    RgbImage* loadRgbImage(const std::string& iFilenamePath);
    void processMessage(MessageQueue::Message*);
    std::string toString(requestType);

    MessageQueue mRequestQueue;
    std::map<void*, MessageQueue*> mSenderToDoneQueue;
    double mMegaBytesLoadedPerSecond;
    realisim::utils::Timer mTimer;
};