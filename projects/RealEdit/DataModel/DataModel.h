/* 
  NOTE: Seul les objects non initialisés (dummy) ont un ID de 0
*/

#ifndef RealEdit_DataModel_hh
#define RealEdit_DataModel_hh

#include "BoundingBox.h"
#include "Point.h"
#include "Primitives.h"
#include "Vect.h"

#include <vector>
#include <cassert>

namespace realEdit
{
using namespace realisim::math;
using namespace realisim::treeD;

class DataModelBase
{
public:
	DataModelBase ();
  DataModelBase (const DataModelBase& iD);
  virtual DataModelBase& operator= (const DataModelBase& iD){assert(0);}
	virtual ~DataModelBase() = 0;
  
  void assign();
  unsigned int getId() const{ return mId; }
	
protected:
	unsigned int mId;  //identificateur unique a l'intérieur d'un projet
	
private:
  static unsigned int mIdCounter;
};

//-----------------------------------------------------------------------------
class RealEditPoint : public DataModelBase
{
public:
  RealEditPoint ();
  RealEditPoint (const Point3d& iPos);
  RealEditPoint (const RealEditPoint& iP);
  RealEditPoint& operator= (const RealEditPoint& iP);
  virtual ~RealEditPoint ();
  
  const Point3d& pos () const {return mpGuts->mPoint;}
  void set(const Point3d&);
  double x () const {return mpGuts->mPoint.getX ();}
  double y () const {return mpGuts->mPoint.getY ();}
  double z () const {return mpGuts->mPoint.getZ ();}
private:
  struct Guts 
  {
    explicit Guts (const Point3d& iP);
    ~Guts ();
    
    Point3d mPoint;
    unsigned int mRefCount;
  };
  
  Guts* mpGuts;
};

//-----------------------------------------------------------------------------
//class LineSegment : public DataModelBase 
//{
//public:
//  LineSegment();
//  ~LineSegment();
//
//  virtual void draw() const;
//private:
//};
//
//-----------------------------------------------------------------------------
class RealEditPolygon : public DataModelBase
{
public:
  RealEditPolygon ();
  RealEditPolygon (const std::vector<RealEditPoint>& iP);
  RealEditPolygon (const RealEditPolygon& iP);
  RealEditPolygon& operator= (const RealEditPolygon& iP);
  virtual ~RealEditPolygon ();

  const std::vector<RealEditPoint>& getPoints() const;
  const std::vector<Vector3d>& getNormals() const;
  
  //void calculateNormal();
  
private:
  struct Guts
  {
    Guts();
    Guts (const std::vector<RealEditPoint>& iP);
    ~Guts();
    
    unsigned int mRefCount;
    std::vector<RealEditPoint> mPoints;
    std::vector<Vector3d> mNormals;
  };
  
  Guts* mpGuts;
};

//-----------------------------------------------------------------------------
class RealEditModel : public DataModelBase
{
public:  
	RealEditModel ();
  RealEditModel (const RealEditModel& iModel);
  RealEditModel& operator= (const RealEditModel& iM);
	virtual ~RealEditModel ();
	
  void addPoint (RealEditPoint iP);
  void addPolygon (RealEditPolygon ipPoly);
  const BB3d& getBoundingBox () const;
  unsigned int getPointCount () const;
  const RealEditPoint& getPoint (unsigned int iIndex) const;
  unsigned int getPolygonCount () const;
  const RealEditPolygon& getPolygon (unsigned int iIndex) const;

private:
  struct Guts
  {
    Guts();
    ~Guts();
    
    BB3d mBoundingBox;
    std::vector<RealEditPoint> mPoints;
    //std::vector<LineSegment> mLineSegments;
    std::vector<RealEditPolygon> mPolygons;
    unsigned int mRefCount;
  };
  
  Guts* mpGuts;
};

}//realEdit

#endif
