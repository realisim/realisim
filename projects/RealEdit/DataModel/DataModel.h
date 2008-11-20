/*
 
 */

#include "Point.h"
#include "Vect.h"

#include <string>
#include <vector>

namespace RealEdit 
{
  class DataModelBase;
  
  class RealEditPoint;
  //class LineSegment;
  class RealEditPolygon;
  class RealEditModel;
}

class RealEdit::DataModelBase
{
public:
	DataModelBase();
	virtual ~DataModelBase() = 0;
	  
  int getId() const{ return mId; }
	
protected:
	int mId;  //identificateur unique a l'intérieur d'un projet
	
private:
  static unsigned int mIdCounter;

};

//-----------------------------------------------------------------------------
class RealEdit::RealEditPoint : public RealEdit::DataModelBase,
                                public Realisim::Point3f
{
public:
  RealEditPoint( const Realisim::Point3f& iPos );
  ~RealEditPoint();
    
private:
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
  RealEditPolygon( RealEditPoint* ipP1, RealEditPoint* ipP2, RealEditPoint* ipP3 );
  RealEditPolygon( const RealEditPolygon& iP );
  ~RealEditPolygon();

  const std::vector<RealEditPoint*>& getPoints() const;
  const std::vector<Realisim::Vector3f>& getNormals() const;
  
  //void calculateNormal();
  
private:
  std::vector<RealEditPoint*> mPoints;
  std::vector<Realisim::Vector3f> mNormals;
};

//-----------------------------------------------------------------------------
class RealEdit::RealEditModel : public RealEdit::DataModelBase
{
public:
	RealEditModel( const std::string& iString );
  RealEditModel( const RealEditModel& iModel );
	~RealEditModel();
	
  //virtual void draw( bool iPickingDraw = false );
  const std::vector<RealEdit::RealEditPoint*>& getPoints() const{ return mPoints; }
  const std::vector<RealEdit::RealEditPolygon*>& getPolygons() const{ return mPolygons; }

  
protected:
private:
  
  std::vector<RealEdit::RealEditPoint*> mPoints;
  //  std::vector<Realisim::DataModel::LineSegment*> mLineSegments;
  std::vector<RealEdit::RealEditPolygon*> mPolygons;
  
  std::string mName;
};
