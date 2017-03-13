
#include "Definitions.h"
#include "GpuStreamer.h"
#include <iostream>
#include <QGLContext>

using namespace std;

//------------------------------------------------------------------------------
GpuStreamer::GpuStreamer() :
mMakeCurrentNeeded(true),
mGLContext(0),
mDC(0)
{
}

//------------------------------------------------------------------------------
void GpuStreamer::postMessage(GpuStreamer::Message *iMessage)
{
    // do not insert the same file twice...
    //auto it = mPendingFileRequests.insert(iMessage->mFilenamePath);
    
    //if(it.second == true)
    { mRequestQueue.post(iMessage); }
}

//------------------------------------------------------------------------------
void GpuStreamer::processMessage(MessageQueue::Message* ipMessage)
{
    wglMakeCurrent(mDC, mGLContext);

    Message *r = (Message*)(ipMessage);

    printf("GpuStreamer::processMessage - processing message...\n");

    //write into the requester done queue, if it was registered
    auto it = mSenderToDoneQueue.find( r->mpSender );
    if( it != mSenderToDoneQueue.end() )
    {
        Message *d = new Message(this);
        it->second->post( d );
    }

    //    //remove request from unique file request
    //    auto itToErase = mPendingFileRequests.find(r->mFilenamePath);
    //    if(itToErase != mPendingFileRequests.end())
    //    {
    //        mPendingFileRequests.erase(itToErase);
    //    }

    wglMakeCurrent(NULL, NULL);
}

//------------------------------------------------------------------------------
void GpuStreamer::registerDoneQueue(void *ipRequester, MessageQueue *ipDoneQueue)
{
    mSenderToDoneQueue.insert( make_pair(ipRequester, ipDoneQueue) );
}

//------------------------------------------------------------------------------
void GpuStreamer::setGLContext(HDC iDc, HGLRC iContext)
{
    mMakeCurrentNeeded = true;
    mGLContext = iContext;
    mDC = iDc;

    using placeholders::_1;
    function<void(MessageQueue::Message*)> f =
        bind(&GpuStreamer::processMessage, this, _1);
    mRequestQueue.setProcessingFunction(f);
    mRequestQueue.startInThread();
}

//------------------------------------------------------------------------------
string GpuStreamer::toString(messageType iMt)
{
    string r("n/a");
    switch (iMt) {
        case mtTexture: r = "texture object"; break;
        default: break;
    }
    return r;
}

//------------------------------------------------------------------------------
//--- GpuStreamer::Message
//------------------------------------------------------------------------------
GpuStreamer::Message::Message(void* ipSender) :
MessageQueue::Message(ipSender),
mMessageType(mtUndefined)
{
    
}