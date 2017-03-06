#include "Broker.h"

using namespace realisim;
using namespace treeD;

//-----------------------------------------------------------------------------
//--- StatsPerFrame
//-----------------------------------------------------------------------------
StatsPerFrame::StatsPerFrame()
{ clear(); }

void StatsPerFrame::clear()
{
    mNumberOfVertices = 0;
    mNumberOfPolygons = 0;
    mNumberOfIGraphicNodeDisplayed = 0;
    mTotalNumberOfIGraphicNode = 0;
    mTimeToPrepareFrame = 0;
}

void StatsPerFrame::clearPerFrameStats()
{
    mNumberOfVertices = 0;
    mNumberOfPolygons = 0;
    mNumberOfIGraphicNodeDisplayed = 0;    
}

//-----------------------------------------------------------------------------
//--- Broker
//-----------------------------------------------------------------------------
Broker::Broker() :
    mCamera()
{
}

//-----------------------------------------------------------------------------
Broker::~Broker()
{
}

//-----------------------------------------------------------------------------
Camera Broker::getCamera() const
{
    return mCamera;
}

//-----------------------------------------------------------------------------
StatsPerFrame& Broker::getStatsPerFrame()
{
    return mStatsPerframe;
}

//-----------------------------------------------------------------------------
void Broker::setCamera(const realisim::treeD::Camera& iC)
{
    mCamera = iC;
}