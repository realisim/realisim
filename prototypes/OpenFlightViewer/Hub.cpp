
#include <cassert>
#include "FileStreamer.h"
#include "GpuStreamer.h"
#include "Hub.h"


//-----------------------------------------------------------------------------
Hub::Hub() :
mpFileStreamer(nullptr)
{}

//-----------------------------------------------------------------------------
Hub::~Hub()
{}

//-----------------------------------------------------------------------------
Broker& Hub::getBroker()
{
    assert(mpBroker != nullptr);
    return *mpBroker;
}

//-----------------------------------------------------------------------------
FileStreamer& Hub::getFileStreamer()
{
    assert(mpFileStreamer != nullptr);
    return *mpFileStreamer;
}

//-----------------------------------------------------------------------------
GpuStreamer& Hub::getGpuStreamer()
{
    assert(mpGpuStreamer != nullptr);
    return *mpGpuStreamer;
}

//-----------------------------------------------------------------------------
void Hub::setBroker(Broker* ipBroker)
{
    mpBroker = ipBroker;
}

//-----------------------------------------------------------------------------
void Hub::setFileStreamer(FileStreamer* ipStreamer)
{
    mpFileStreamer = ipStreamer;
}

//-----------------------------------------------------------------------------
void Hub::setGpuStreamer(GpuStreamer* ipStreamer)
{
    mpGpuStreamer = ipStreamer;
}

