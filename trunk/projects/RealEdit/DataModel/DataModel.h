/*
 
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
	DataModelBase();
	virtual ~DataModelBase() = 0;
	  
  unsigned int getId() const{ return mId; }
	
protected:
	unsigned int mId;  //identificateur unique a l'intérieur d'un projet
	
private:
  static unsigned int mIdCounter;

};

//-----------------------------------------------------------------------------
class RealEdit::RealEditPoint : public RealEdit::DataModelBase,
                                public Realisim::Point3d
{
public:  
  RealEditPoint( const Realisim::Point3d& iPos );
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
  //Le vecteur de point ne doit JAMAIS contenir de pointeur NULL
  RealEditPolygon( const std::vector<RealEditPoint*>& iP );
  RealEditPolygon( const RealEditPolygon& iP );
  ~RealEditPolygon();

  const std::vector<RealEditPoint*>& getPoints() const;
  const std::vector<Realisim::Vector3d>& getNormals() const;
  
  //void calculateNormal();
  
private:
  std::vector<RealEditPoint*> mPoints;
  std::vector<Realisim::Vector3d> mNormals;
};

//-----------------------------------------------------------------------------
class RealEdit::RealEditModel : public RealEdit::DataModelBase
{
public:  
	RealEditModel();
  RealEditModel( const RealEditModel& iModel ){assert(0);}
	~RealEditModel();
	
  void addPoint( const RealEditPoint* ipPoint );
  void addPolygon( const RealEditPolygon* ipPoly );

  unsigned int getPointCount() const;
  const RealEditPoint* getPoint( int iIndex ) const;
  unsigned int getPolygonCount() const;
  const RealEditPolygon* getPolygon( int iIndex ) const;

  
protected:
private:
  
  std::vector<const RealEditPoint*> mPoints;
  //std::vector<Realisim::DataModel::LineSegment*> mLineSegments;
  std::vector<const RealEditPolygon*> mPolygons;
};

#endif
