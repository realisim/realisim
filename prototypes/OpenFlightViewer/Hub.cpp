
#include <cassert>
#include "FileStreamer.h"
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
void Hub::setFileStreamer(FileStreamer* ipStreamer)
{
    mpFileStreamer = ipStreamer;
}

