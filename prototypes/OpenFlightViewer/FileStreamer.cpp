
#include "Definitions.h"
#include "FileStreamer.h"
#include "FltImporter.h"
#include <iostream>
#include "openFlight/DotExporter.h"
#include "openFlight/OpenFlightReader.h"
#include "ImageLoader.h"
#include "utils/Timer.h"

using namespace std;

//------------------------------------------------------------------------------
FileStreamer::FileStreamer()
{
    using placeholders::_1;
    function<void(MessageQueue::Message*)> f =
        bind(&FileStreamer::processMessage, this, _1);
    mRequestQueue.setProcessingFunction(f);
    mRequestQueue.startInThread();
}

//------------------------------------------------------------------------------
IGraphicNode* FileStreamer::loadFlt(const std::string &iFilenamePath)
{
    realisim::utils::Timer __t;
    IGraphicNode* graphicNode = nullptr;
    
    
    OpenFlight::OpenFlightReader ofr;
    //    //ofr.enableDebug(true);
    //    //ofr.enableExternalReferenceLoading(false);
    OpenFlight::HeaderRecord *header = ofr.open( iFilenamePath );
    printf("Temps pour lire %s: %.4f (sec)\n", iFilenamePath.c_str(), __t.getElapsed() );

    if(!ofr.hasErrors())
    {
        __t.start();
        if(ofr.hasWarnings())
        { cout << "Warning while opening flt file: " << endl << ofr.getAndClearLastWarnings(); }

        FltImporter fltImporter(header);
        graphicNode = fltImporter.getGraphicNodeRoot();
        printf("Temps pour importer %s: %.4f (sec)\n", iFilenamePath.c_str(), __t.getElapsed() );

        //cout << OpenFlight::toDotFormat( fltImporter.getOpenFlightRoot() );
    }
    else
    {
        delete header;
        cout << "Error while opening flt file: " << endl << ofr.getAndClearLastErrors();
    }
    
    return graphicNode;
}

//------------------------------------------------------------------------------
RgbImage* FileStreamer::loadRgbImage(const std::string& iFilenamePath)
{
    RgbImage *im = new RgbImage();
    im->setFilenamePath(iFilenamePath);
    im->load();
    return im;
}

//------------------------------------------------------------------------------
void FileStreamer::postRequest(FileStreamer::Request *iRequest)
{
    // do not insert the same file twice...
    auto it = mPendingFileRequests.insert(iRequest->mFilenamePath);
    
    if(it.second == true)
    { mRequestQueue.post(iRequest); }
}

//------------------------------------------------------------------------------
void FileStreamer::registerDoneQueue(void *ipRequester, MessageQueue *ipDoneQueue)
{
    mSenderToDoneQueue.insert( make_pair(ipRequester, ipDoneQueue) );
}

//------------------------------------------------------------------------------
void FileStreamer::processMessage(MessageQueue::Message* ipRequest)
{
    Request *r = (Request*)(ipRequest);
    
    printf("FileStreamer processing message to load %s with filenamepath %s.\r",
           toString(r->mRequestType).c_str(),
           r->mFilenamePath.c_str() );
    
    void* dataPtr = nullptr;
    switch (r->mRequestType)
    {
        case rtLoadFlt: dataPtr = loadFlt(r->mFilenamePath); break;
        case rtLoadRgbImage: dataPtr = loadRgbImage(r->mFilenamePath); break;
        default: break;
    }
        
    //write into the requester done queue, if it was registered
    auto it = mSenderToDoneQueue.find( r->mpSender );
    if( it != mSenderToDoneQueue.end() )
    {
        Request *d = new Request(this);
        d->mRequestType = r->mRequestType;
        d->mFilenamePath = r->mFilenamePath;
        d->mAffectedDefinitionId = r->mAffectedDefinitionId;
        d->mpData = dataPtr;
        it->second->post( d );
    }
    
    //remove request from unique file request
    auto itToErase = mPendingFileRequests.find(r->mFilenamePath);
    if(itToErase != mPendingFileRequests.end())
    {
        mPendingFileRequests.erase(itToErase);
    }
}

//------------------------------------------------------------------------------
string FileStreamer::toString(requestType iRt)
{
    string r("n/a");
    switch (iRt) {
        case rtLoadFlt: r = "loadFlt"; break;
        case rtLoadRgbImage: r = "loadRgbImage"; break;
        default: break;
    }
    return r;
}

//------------------------------------------------------------------------------
//--- FileStreamer::Request
//------------------------------------------------------------------------------
FileStreamer::Request::Request(void* ipSender) :
MessageQueue::Message(ipSender),
mRequestType(rtUndefined),
mFilenamePath(""),
mAffectedDefinitionId(0),
mpData(nullptr)
{
    
}