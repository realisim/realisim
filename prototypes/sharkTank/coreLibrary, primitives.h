#ifndef resonant_coreLibrary_primitives_hh
#define resonant_coreLibrary_primitives_hh

#include "coreLibrary, types.h"
#include <QString>
#include <vector>

namespace resonant
{
namespace coreLibrary
{
//------------------------------------------------------------------------------
class line
{
public:
   line();
   line(point3, point3);
   line(point3, vector3);
   virtual ~line(){}

   bool contains(point3) const;
   point3 getPoint() const;
   vector3 getDirection() const;
   bool isValid() const;
   void set(point3, point3);
   void set(point3, vector3);

protected:
   void validate();

   bool mIsValid;
   point3 mPoint;
   vector3 mDirection;
};
//------------------------------------------------------------------------------
class lineSegment
{
public:
   lineSegment();
   lineSegment(point3, point3);
   virtual ~lineSegment(){}

   point3 getFirstPoint() const;
   point3 getSecondPoint() const;
   bool isValid() const;
   void set(point3, point3);

protected:
   void validate();

   bool mIsValid;
   point3 mPoint1;
   point3 mPoint2;
};
//------------------------------------------------------------------------------
class plane
{
public:
   plane();
   plane(point3, point3, point3);
   plane(point3, vector3);
   virtual ~plane(){}
   //plane(double a, double b, double c, double d); //forme ax+by+cz+d = 0 

   vector3 getNormal() const;
   point3 getPoint() const;   
   bool isValid() const;
   void parametricForm( double*, double*, double*, double* ) const; //cartesianForm
   void set(point3, point3, point3);
   void set(point3, vector3);

protected:
   void validate();

   bool mIsValid;
   point3 mPoint;
   vector3 mNormal;      
};
//------------------------------------------------------------------------------
/*Un polygone a n coté (3 minimum). Tous les points sont garantis coplanaire
  et le contour ordonné. La normale du polygone suit la règle de la main 
  gauche.*/
class polygon
{
public:
   polygon();
   polygon( point3, point3, point3 );
   polygon( const std::vector<point3>& );
   virtual ~polygon() {}

   point3 getCenterOfMass() const;
   vector3 getNormal() const;
   int getNumberOfVertices() const;
   point3 getVertex(int) const;
   bool isCoplanar() const;
   bool isValid() const;
   void orderizeContour();
   
protected:
   void validate();
   void polarOrderize( std::vector<point3>* );

   bool mIsValid;
   std::vector<point3> mVertices;
   vector3 mNormal;
};
//------------------------------------------------------------------------------
/*Il faudrait ajouter du refCounting sur cette classe...
  On suppose que toutes les faces sont des triangles.*/
class mesh2
{
public:
   mesh2();
   virtual ~mesh2();

   vector3 getNormal(int) const;
   int getNormalIndexOnFace(int, int) const;
   int getNumberOfFaces() const;
   int getNumberOfNormals() const;
   int getNumberOfPolygons() const;
   int getNumberOfVertices() const;
   int getNumberOfNormalsOnFace(int) const;
   int getNumberOfVerticesOnFace(int) const;
   polygon getPolygon(int) const;
   const int* getPointerToFaceNormalIndices() const;
   const int* getPointerToFaceVertexIndices() const;
   const double* getPointerToNormals() const;
   const double* getPointerToVertices() const;
   point3 getVertex(int) const;
   int getVertexIndexOnFace(int, int) const;
   static mesh2* makeFromObj( QString );

protected:
   struct face
   {
      std::vector<unsigned int> mVertexIndices;
      std::vector<unsigned int> mNormalIndices;
      //std::vector<int> mTextureCoordinatesIndices;
   };

   std::vector<point3> mVertices;
   std::vector<vector3> mNormals;
   std::vector<face> mFaces;
   /*std::vector<int> mFaceVertexIndices;
   std::vector<int> mFaceNormalIndices;
   int mNumberOfVerticesPerFace;
   int mNumberOfNormalsPerFace;*/
};
//------------------------------------------------------------------------------
class box
{
public:
   box();
   box(point3, point3);
   box(point3, vector3);
   virtual ~box(){}

   void add(point3);
   point3 getCenterOfMass() const;
   double getDepth() const {return mMax.z() - mMin.z() ;}
   double getHeight() const {return mMax.y() - mMin.y() ;}
   point3 getMax() const;
   point3 getMin() const;
   double getWidth() const {return mMax.x() - mMin.x() ;}
   vector3 getSize() const;
   bool isValid() const;
   void setMax(point3);
   void setMin(point3);

   bool contains( point3, bool = true ) const;
   bool contains( box, bool = true ) const;
protected:
   point3 mMin;
   point3 mMax;
};

}
}

#endif