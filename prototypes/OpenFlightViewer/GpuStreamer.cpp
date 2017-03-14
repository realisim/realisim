
#include "Definitions.h"
#include "GpuStreamer.h"
#include <iostream>
#include <Representations.h>
#include <QGLContext>

using namespace std;

//------------------------------------------------------------------------------
GpuStreamer::GpuStreamer() :
mMakeCurrentNeeded(true),
mGLContext(0),
mDC(0)
{
}

GpuStreamer::~GpuStreamer()
{
    wglMakeCurrent(mDC, NULL);;
}

//------------------------------------------------------------------------------
realisim::treeD::Texture GpuStreamer::createTexture(Image* ipIm)
{
    realisim::treeD::Texture t;
    
    realisim::math::Vector2i size(ipIm->mWidth, ipIm->mHeight);
    GLenum internalFormat = GL_SRGB8_ALPHA8;
    GLenum format = GL_RGBA;
    GLenum datatype = GL_UNSIGNED_BYTE;
            
    switch(ipIm->mNumberOfChannels)
    {
        case 1:
            internalFormat = GL_R8;
            format = GL_RED;
            break;
        case 2:
            internalFormat = GL_RG8;
            format = GL_RG;
            break;
        case 3:
            //internalFormat = GL_SRGB8;
            internalFormat = GL_RGB8;
            format = GL_RGB;
            break;
        case 4:
            //internalFormat = GL_SRGB8_ALPHA8;
            internalFormat = GL_RGBA8;
            format = GL_RGBA;
            break;
        default: break;
    }
    
    t.set(ipIm->mpPayload, size, internalFormat, format, datatype);
    t.generateMipmap(true);
    t.setMagnificationFilter(GL_LINEAR);
    t.setMinificationFilter(GL_LINEAR_MIPMAP_LINEAR);

    return t;
}

//------------------------------------------------------------------------------
void GpuStreamer::postMessage(GpuStreamer::Message *iMessage)
{
    // do not insert the same request twice...
    unsigned int definitionId = 0;
    switch (iMessage->mMessageType)
    {
    case mtTexture:
        definitionId = ((Image*)iMessage->mpData)->getId();
        break;
    case mtCreateModel:
    {
        IDefinition* def = dynamic_cast<IDefinition*>(iMessage->mpModelNode);
        if (def)
        {
            definitionId = def->getId();
        }
    }break;
    default: break;
    }

    mPendingRequestsMutex.lock();
    auto it = mPendingRequests.insert(definitionId);
    mPendingRequestsMutex.unlock();
    
    if(it.second == true)
    { mRequestQueue.post(iMessage); }
    else
    {
        //printf("GpuStreamer::postMessage - message with defId: %d, was already present\n", definitionId);
        delete iMessage;
    }
}

//------------------------------------------------------------------------------
void GpuStreamer::processMessage(MessageQueue::Message* ipMessage)
{
    if (mMakeCurrentNeeded)
    {
        mMakeCurrentNeeded = !wglMakeCurrent(mDC, mGLContext);
    }

    if (!mMakeCurrentNeeded)
    {
        Message *message = (Message*)(ipMessage);

        //printf("GpuStreamer::processMessage - processing message...\n");

        Message *doneMessage = new Message(this);
        doneMessage->mMessageType = message->mMessageType;

        //--- deal with the message
        GLsync fenceSync = 0;
        messageType mt = message->mMessageType;
        unsigned int defId = message->mAffectedDefinitionId;        
        switch (mt)
        {
        case mtTexture:
        {
            Image *ipIm = (Image*)message->mpData;
            realisim::treeD::Texture tex;

            tex = createTexture(ipIm);
            //tex.setFenceSync(glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0));
            fenceSync = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);

            doneMessage->mAffectedDefinitionId = defId;
            doneMessage->mpData = ipIm;
            doneMessage->mTexture = tex;
        }break;
        case mtCreateModel:
        {
            ModelNode *modelNode = message->mpModelNode;
            std::unordered_map<unsigned int, realisim::treeD::Texture> *imageIdToTexture = message->mpImageIdToTexture;

            Representations::Model *model = new Representations::Model;
            model->create(modelNode, *imageIdToTexture);
            //model->setFenceSync(  glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0) );
            fenceSync = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);

            doneMessage->mAffectedDefinitionId = defId;
            doneMessage->mpData = model;
            
        }break;
        default : break;
        }

        //wait on the sync
        if (fenceSync)
        {
            /*GLint result = GL_UNSIGNALED;
            glGetSynciv(fenceSync, GL_SYNC_STATUS, sizeof(GLint), NULL, &result);
            while (result != GL_SIGNALED) 
            {
                glGetSynciv(fenceSync, GL_SYNC_STATUS, sizeof(GLint), NULL, &result);
            }*/
            glClientWaitSync(fenceSync, GL_SYNC_FLUSH_COMMANDS_BIT, long long(30000000000) );

            glDeleteSync(fenceSync);
        }
        //glFlush();
        //glFinish();

        //write into the requester done queue, if it was registered
        auto it = mSenderToDoneQueue.find( message->mpSender );
        if( it != mSenderToDoneQueue.end() )
        {
            it->second->post( doneMessage );
        }
        else
        {
            delete doneMessage;
        }

        ////remove request from unique file request

        //
        // MOVE THIS WHOLE PENDING REQUEST TO THE SENDER!!!!
        //
        //mPendingRequestsMutex.lock();
        //auto itToErase = mPendingRequests.find(defId);
        //if(itToErase != mPendingRequests.end())
        //{
        //    mPendingRequests.erase(itToErase);
        //}
        //mPendingRequestsMutex.unlock();

    } 
    else
    {
        printf("cannot make gpuStreamer context current...\n");
    }    

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
mMessageType(mtUndefined),
mAffectedDefinitionId(0),
mTexture(),
mpData(nullptr),
mpModelNode(nullptr),
mpImageIdToTexture(nullptr)
{
    
}