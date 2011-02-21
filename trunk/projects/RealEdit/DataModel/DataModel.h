/* 
  NOTE: Seul les objects non initialisés (dummy) ont un ID de 0
*/

#ifndef RealEdit_DataModel_hh
#define RealEdit_DataModel_hh

#include "math/BoundingBox.h"
#include "math/Point.h"
#include "math/Vect.h"

#include <cassert>
#include <vector>
#include <map>
#include <set>

namespace realEdit
{
using namespace realisim::math;

class DataModelBase
{
public:
	DataModelBase ();
  DataModelBase (const DataModelBase& iD);
  //virtual DataModelBase& operator= (const DataModelBase& iD){assert(0);}
	virtual ~DataModelBase() = 0;
  
  void assign();
  unsigned int getId() const{ return mId; }
  void setId(unsigned int iId) {mId = iId;}
	
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
/*Les deux point qui constituent le segment sont toujours dans l'ordre suivant
  mPoint1 est le point qui a le id le plus petit.*/
class RealEditSegment : public DataModelBase 
{
public:
  RealEditSegment();
  RealEditSegment(RealEditPoint, RealEditPoint);
  RealEditSegment(const RealEditSegment&);
  RealEditSegment& operator=(const RealEditSegment&);
  virtual ~RealEditSegment();
  
  virtual const RealEditPoint& getPoint1() const;
  virtual const RealEditPoint& getPoint2() const;
  
  struct Comparator
	{
  	bool operator()(const RealEditSegment& iS1, const RealEditSegment& iS2) const;
	};

private:
	struct Guts
  {
    Guts();
    Guts(RealEditPoint, RealEditPoint);
    ~Guts();
    
    unsigned int mRefCount;
    RealEditPoint mPoint1;
    RealEditPoint mPoint2;
  };
  
  Guts* mpGuts;

};

//-----------------------------------------------------------------------------
/* Un polygon peut être créé a partir de 3 (ou plus point).
   Il contient, la liste des points, une normale en chaque point
   (pour des raison de stockage lorsqu'on utilise une moyenne des
   normale pour faire un smooth shade) et la liste des segments. */
class RealEditPolygon : public DataModelBase
{
public:
  RealEditPolygon ();
  RealEditPolygon (const std::vector<RealEditPoint>& iP);
  RealEditPolygon (const RealEditPolygon& iP);
  RealEditPolygon& operator= (const RealEditPolygon& iP);
  virtual ~RealEditPolygon ();

  virtual void computeNormals();
  virtual const RealEditPoint& getPoint(unsigned int iIndex) const;
  virtual const std::vector<RealEditPoint>& getPoints() const;
  virtual unsigned int getPointCount() const;
  virtual const std::vector<Vector3d>& getNormals() const;
  virtual const std::vector<RealEditSegment>& getSegments() const;
  virtual void setSegment(unsigned int, RealEditSegment);
  
private:
  struct Guts
  {
    Guts();
    Guts (const std::vector<RealEditPoint>& iP);
    ~Guts();
    
    void computeNormals();
    void makeSegments();
    
    unsigned int mRefCount;
    std::vector<RealEditPoint> mPoints;
    std::vector<RealEditSegment> mSegments;
    std::vector<Vector3d> mNormals;
  };
  
  Guts* mpGuts;
};

//-----------------------------------------------------------------------------
class RealEditModel : public DataModelBase
{
public:  
	RealEditModel ();
  RealEditModel (const RealEditModel&);
  RealEditModel& operator= (const RealEditModel&);
	virtual ~RealEditModel();
	
  virtual void addPoint (RealEditPoint);
  virtual void addPolygon (RealEditPolygon);
  virtual void addSegment(RealEditSegment);
  virtual const BB3d& getBoundingBox () const;
  virtual const Point3d& getCentroid() const;
  virtual const RealEditPoint& getPoint(unsigned int) const;
  virtual const std::map<unsigned int, RealEditPoint>& getPoints() const;
  virtual const RealEditPolygon& getPolygon(unsigned int) const;
  virtual const std::map<unsigned int, RealEditPolygon>& getPolygons() const;
  virtual const RealEditSegment& getSegment(unsigned int) const;
  virtual const std::map<unsigned int, RealEditSegment>& getSegments() const;
  virtual bool hasPoint(unsigned int) const;
  virtual bool hasPolygon(unsigned int) const;
  virtual bool hasSegment(unsigned int) const;
  virtual void updateBoundingBox();
  virtual void updateNormals();

private:
  virtual unsigned int getPointCount () const;
  virtual unsigned int getPolygonCount () const;
  //virtual unsigned int getSegmentCount() const;
  
  struct Guts
  {
    Guts();
    ~Guts();
    
    BB3d mBoundingBox;
    std::map<unsigned int, RealEditPoint> mPoints;
    std::map<unsigned int, RealEditPolygon> mPolygons;
    std::map<unsigned int, RealEditSegment> mSegments;
    std::set<RealEditSegment, RealEditSegment::Comparator> mSegmentPool;
    unsigned int mRefCount;
    mutable Point3d mCentroid;
  };
  
  Guts* mpGuts;
};

}//realEdit

#endif
