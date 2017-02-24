
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
void Hub::setFileStreamer(FileStreamer* ipStreamer)
{
    mpFileStreamer = ipStreamer;
}

//-----------------------------------------------------------------------------
void Hub::setGpuStreamer(GpuStreamer* ipStreamer)
{
    mpGpuStreamer = ipStreamer;
}

