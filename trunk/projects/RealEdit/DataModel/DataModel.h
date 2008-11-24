/*
 
 */

#ifndef RealEdit_DataModel_hh
#define RealEdit_DataModel_hh

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
  //Le vecteur de point ne doit JAMAIS contenir de pointeur NULL
  RealEditPolygon( const std::vector<RealEditPoint*>& iP );
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
	
  void addPoint( const RealEditPoint* ipPoint );
  void addPolygon( const RealEditPolygon* ipPoly );

  unsigned int getNumPoint() const;
  const RealEditPoint* getPoint( int iIndex ) const;

  
protected:
private:
  
  std::vector<const RealEditPoint*> mPoints;
  //  std::vector<Realisim::DataModel::LineSegment*> mLineSegments;
  std::vector<const RealEditPolygon*> mPolygons;
  
  std::string mName;
};

//typedef unsigned int RealEditDataId;
//typedef unsigned int RealEditPointId;
//typedef unsigned int RealEditPolygonId;
//typedef unsigned int RealEditModelId;

#endif
