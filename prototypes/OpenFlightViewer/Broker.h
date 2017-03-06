
#include "3d/Camera.h"

struct StatsPerFrame
{
    StatsPerFrame();

    void clear();
    void clearPerFrameStats();

    int mNumberOfVertices;
    int mNumberOfPolygons;
    int mNumberOfIGraphicNodeDisplayed;    
    int mTotalNumberOfIGraphicNode;

    double mTimeToPrepareFrame;
};


//-----------------------------------------------------------------------------
class Broker
{
public:
    Broker();
    Broker(const Broker&) = delete;
    Broker& operator=(const Broker&) = delete;
    ~Broker();

    StatsPerFrame& getStatsPerFrame();
    realisim::treeD::Camera getCamera() const;
    void setCamera( const realisim::treeD::Camera& );

protected:
    realisim::treeD::Camera mCamera;
    StatsPerFrame mStatsPerframe;
};