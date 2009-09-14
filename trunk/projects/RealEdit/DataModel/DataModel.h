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
  //virtual DataModelBase& operator= (const DataModelBase& iD){assert(0);}
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
  
  virtual const Point3d& pos () const {return mpGuts->mPoint;}
  virtual void set(const Point3d&);
  virtual double x () const {return mpGuts->mPoint.getX ();}
  virtual double y () const {return mpGuts->mPoint.getY ();}
  virtual double z () const {return mpGuts->mPoint.getZ ();}
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

  virtual const RealEditPoint& getPoint(unsigned int iIndex) const;
  virtual const std::vector<RealEditPoint>& getPoints() const;
  virtual unsigned int getPointCount() const;
  virtual const std::vector<Vector3d>& getNormals() const;
  virtual void computeNormals();
  
private:
  struct Guts
  {
    Guts();
    Guts (const std::vector<RealEditPoint>& iP);
    ~Guts();
    
    void computeNormals();
    
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
	
  virtual void addPoint (RealEditPoint iP);
  virtual void addPolygon (RealEditPolygon ipPoly);
  virtual const BB3d& getBoundingBox () const;
  virtual unsigned int getPointCount () const;
/*c'est méthode const sont dangereuses parce quelle retourne une reference
sur un object qui est partagé implicitement. Donc si l'utilisateur fait une
copie et modifie la copie, il modifiera aussi l'object référencé qui se
veut const!!!*/
virtual const RealEditPoint& getPoint (unsigned int iIndex) const;
virtual const RealEditPoint& getPointFromId(unsigned int) const;
  virtual unsigned int getPolygonCount () const;
/*c'est méthode const sont dangereuses parce quelle retourne une reference
sur un object qui est partagé implicitement. Donc si l'utilisateur fait une
copie et modifie la copie, il modifiera aussi l'object référencé qui se
veut const!!!*/  
virtual const RealEditPolygon& getPolygon (unsigned int iIndex) const;
virtual const RealEditPolygon& getPolygonFromId(unsigned int) const;
  virtual bool hasPoint(unsigned int) const;
  virtual bool hasPolygon(unsigned int) const;
  virtual void updateBoundingBox();
  virtual void updateNormals();

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
