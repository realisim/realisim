/* 
  NOTE: Seul les objects non initialisés (dummy) ont un ID de 0
*/

#ifndef RealEdit_DataModel_hh
#define RealEdit_DataModel_hh

#include "Point.h"
#include "Vect.h"

#include <vector>
#include <cassert>

namespace RealEdit { class DataModelBase; }
namespace RealEdit { class RealEditPoint; }
namespace RealEdit { class LineSegment; }
namespace RealEdit { class RealEditPolygon; }
namespace RealEdit { class RealEditModel; }

class RealEdit::DataModelBase
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
class RealEdit::RealEditPoint : public RealEdit::DataModelBase
{
public:
  RealEditPoint ();
  RealEditPoint (const Realisim::Point3d& iPos);
  RealEditPoint (const RealEditPoint& iP);
  RealEditPoint& operator= (const RealEditPoint& iP);
  virtual ~RealEditPoint ();
  
  const Realisim::Point3d& pos () const {return mpGuts->mPoint;}
  double x () const {return mpGuts->mPoint.getX ();}
  double y () const {return mpGuts->mPoint.getY ();}
  double z () const {return mpGuts->mPoint.getZ ();}
private:
  struct Guts 
  {
    explicit Guts (const Realisim::Point3d& iP);
    ~Guts ();
    
    Realisim::Point3d mPoint;
    unsigned int mRefCount;
  };
  
  Guts* mpGuts;
};

//-----------------------------------------------------------------------------
//class Realisim::DataModel::LineSegment : public Realisim::DataModel::DataModelBase 
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
class RealEdit::RealEditPolygon : public RealEdit::DataModelBase
{
public:
  RealEditPolygon ();
  RealEditPolygon (const std::vector<RealEditPoint>& iP);
  RealEditPolygon (const RealEditPolygon& iP);
  RealEditPolygon& operator= (const RealEditPolygon& iP);
  virtual ~RealEditPolygon ();

  const std::vector<RealEditPoint>& getPoints() const;
  const std::vector<Realisim::Vector3d>& getNormals() const;
  
  //void calculateNormal();
  
private:
  struct Guts
  {
    Guts();
    Guts (const std::vector<RealEditPoint>& iP);
    ~Guts();
    
    unsigned int mRefCount;
    std::vector<RealEditPoint> mPoints;
    std::vector<Realisim::Vector3d> mNormals;
  };
  
  Guts* mpGuts;
};

//-----------------------------------------------------------------------------
class RealEdit::RealEditModel : public RealEdit::DataModelBase
{
public:  
	RealEditModel ();
  RealEditModel (const RealEditModel& iModel);
  RealEditModel& operator= (const RealEditModel& iM);
	virtual ~RealEditModel ();
	
  void addPoint (RealEditPoint iP);
  void addPolygon (RealEditPolygon ipPoly);
  unsigned int getPointCount () const;
  const RealEditPoint& getPoint (unsigned int iIndex) const;
  unsigned int getPolygonCount () const;
  const RealEditPolygon& getPolygon (unsigned int iIndex) const;

private:
  struct Guts
  {
    Guts();
    ~Guts();
    
    unsigned int mRefCount;
    std::vector<RealEditPoint> mPoints;
    //std::vector<Realisim::DataModel::LineSegment> mLineSegments;
    std::vector<RealEditPolygon> mPolygons;
  };
  
  Guts* mpGuts;
};

#endif
