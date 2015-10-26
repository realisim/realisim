#ifndef resonant_corelibrary_intersection_hh
#define resonant_corelibrary_intersection_hh

#include "coreLibrary, primitives.h"
#include "coreLibrary, types.h"
#include <vector>

namespace resonant
{
namespace coreLibrary
{

   class intersection
   {
   public:
      //transformer en masque binaire afin de combiner les types?
      enum type{ tNone = 0, tPoint = 1, tLine = 1<<1, tLineSegment = 1<<2,
        tPlane = 1<<3, tPolygon = 1<<4 };
      intersection();

      void addLine(const line&);
      void addLineSegment(const lineSegment&);
      void addPlan(const plane&);       
      void addPoint(const point3&);
      void addPolygon(const polygon&);
      void addType(type);
      line getLine(int) const;
      lineSegment getLineSegment(int) const;
      int getNumberOfLines() const;
      int getNumberOfLineSegments() const;
      int getNumberOfPlanes() const;
      int getNumberOfPoints() const;
      int getNumberOfPolygons() const;
      plane getPlane(int) const;
      point3 getPoint(int) const;
      polygon getPolygon(int) const;
      int getType() const;
      bool hasIntersections() const;
      void setType(type);
   protected:
      int mType;
      std::vector<point3> mPoints;
      std::vector<line> mLines;
      std::vector<lineSegment> mLineSegments;
      std::vector<plane> mPlanes;
      std::vector<polygon> mPolygons;
   };

   bool areAllVerticesOnSameSide(const std::vector<point3>&, const plane&);
   bool intersects(plane, line);
   bool intersects(plane, plane);
   bool intersects(const plane&, const box&);
   bool intersects(const plane&, const polygon&);
   bool intersects(const plane&, const mesh2&);
   //...

   bool intersects(line, plane);
   //...

   bool intersects(const box&, const plane&);
   bool intersects(const polygon&, const plane&);
   bool intersects(const mesh2&, const plane&);
   
   //---
   intersection intersect(plane, line);
   intersection intersect(plane, plane);
   intersection intersect(plane, lineSegment);
   intersection intersect(const plane&, const box&);
intersection intersect(const plane&, const polygon&);
intersection intersect(const plane&, const mesh2&);

   intersection intersect(line, line);
   intersection intersect(line, plane);
//intersection intersect(line, lineSegment)
//intersection intersect(line, box)
//intersection intersect(line, const polygon&)
//intersection intersect(line, const mesh&);

//intersection intersect(lineSegment, line);
  intersection intersect(lineSegment, plane);
   intersection intersect(lineSegment, lineSegment);
//intersection intersect(lineSegment, box)
//intersection intersect(lineSegment, const polygon&)
//intersection intersect(lineSegment, const mesh&)

//intersection intersect(box, line);
   intersection intersect(const box&, const plane&);
//intersection intersect(box, lineSegment);
//intersection intersect(box, box);
//intersection intersect(box, const polygon&);
//intersection intersect(box, const mesh&);

//intersection intersect( const polygon&, const line& );
   intersection intersect( const polygon&, const plane& );
//intersection intersect( const polygon&, const lineSegment& );
//intersection intersect( const polygon&, const box& );
//intersection intersect( const polygon&, const polygon& );
//intersection intersect( const polygon&, const mesh& );

//intersection intersect( const mesh&, const line& );
   intersection intersect( const mesh2&, const plane& );
//intersection intersect( const mesh&, const lineSegment& );
//intersection intersect( const mesh&, const box& );
//intersection intersect( const mesh&, const polygon& );
//intersection intersect( const mesh&, const mesh& );
}
}

#endif