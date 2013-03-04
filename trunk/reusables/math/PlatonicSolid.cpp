/*
 *  PlatonicSolid.cpp
 *  Created by Pierre-Olivier Beaudoin on 09-08-18.
 */

#include "MathUtils.h"
#include "PlatonicSolid.h"
#include "Vect.h"

using namespace realisim;
  using namespace math;
  
PlatonicSolid::PlatonicSolid( type t ) : 
  mType(t),
  mPolygons(),
  mVertex(),
  mVertexIndices()
{
  switch (getType()) 
  {
    case tCube:
      makeCube();
      break;
    case tIsocahedron:
      makeIsocahedron();
      break;
    case tTetrahedron:
      makeTetrahedron();
      break;
    default:
      break;
  }
}

PlatonicSolid::PlatonicSolid(const PlatonicSolid& iP) :
  mType(iP.getType()),
  mPolygons( iP.getPolygons() ),
  mVertex( iP.getVertices() ),
  mVertexIndices( iP.mVertexIndices )
{}

PlatonicSolid& PlatonicSolid::operator=(const PlatonicSolid& iP)
{
  mType = iP.getType();
  mPolygons = iP.getPolygons();
  mVertex = iP.getVertices();
  mVertexIndices = iP.mVertexIndices;
  return *this;
}

PlatonicSolid::~PlatonicSolid()
{}
//------------------------------------------------------------------------------
void PlatonicSolid::addPolygon( int a, int b, int c )
{
  vector<Point3d> ps;
	ps.push_back( mVertex[a] );
  ps.push_back( mVertex[b] );
  ps.push_back( mVertex[c] );
	mPolygons.push_back( Polygon( ps ) );	
  
  vector< int > indices;
  indices.push_back( a ); indices.push_back( b ); indices.push_back( c );
  mVertexIndices.push_back( indices );
}
//------------------------------------------------------------------------------
void PlatonicSolid::addPolygon( int a, int b, int c, int d )
{
  vector<Point3d> ps;
	ps.push_back( mVertex[a] );
  ps.push_back( mVertex[b] );
  ps.push_back( mVertex[c] );
  ps.push_back( mVertex[d] );
	mPolygons.push_back( Polygon( ps ) );
  
  vector< int > indices;
  indices.push_back( a ); indices.push_back( b ); indices.push_back( c );
  indices.push_back( d );
  mVertexIndices.push_back( indices );
}

//------------------------------------------------------------------------------
const Polygon& PlatonicSolid::getPolygon( int i ) const
{ return mPolygons[i]; }
//------------------------------------------------------------------------------
const Point3d& PlatonicSolid::getVertex( int i ) const
{ return mVertex[i]; }
//------------------------------------------------------------------------------
const vector<int>& PlatonicSolid::getVertexIndicesForPolygon( int i ) const
{ return mVertexIndices[i]; }
//------------------------------------------------------------------------------
void PlatonicSolid::makeCube()
{
  double c = 0.5;
  mVertex.push_back(Point3d(-c, -c, c));
  mVertex.push_back(Point3d(c, -c, c));
  mVertex.push_back(Point3d(c, -c, -c));
  mVertex.push_back(Point3d(-c, -c, -c));
  mVertex.push_back(Point3d(-c, c, c));
  mVertex.push_back(Point3d(c, c, c));
  mVertex.push_back(Point3d(c, c, -c));
  mVertex.push_back(Point3d(-c, c, -c));

	addPolygon( 3, 2, 1, 0 ); //bas
  addPolygon( 4, 5, 6, 7 ); //haut 
	addPolygon( 0, 1, 5, 4 ); //devant  
	addPolygon( 2, 3, 7, 6 ); //derriere
	addPolygon( 1, 2, 6, 5 ); //droite  
	addPolygon( 3, 0, 4, 7 ); //gauche
}

//------------------------------------------------------------------------------
void PlatonicSolid::makeIsocahedron()
{
  double X = .525731112119133606 ;
  double Z = .850650808352039932;
  
  mVertex.push_back(Point3d(-X, 0.0, Z));
  mVertex.push_back(Point3d(X, 0.0, Z));
  mVertex.push_back(Point3d(-X, 0.0, -Z));
  mVertex.push_back(Point3d(X, 0.0, -Z));
  mVertex.push_back(Point3d(0.0, Z, X));
  mVertex.push_back(Point3d(0.0, Z, -X));
  mVertex.push_back(Point3d(0.0, -Z, X));
  mVertex.push_back(Point3d(0.0, -Z, -X));
  mVertex.push_back(Point3d(Z, X, 0.0));
  mVertex.push_back(Point3d(-Z, X, 0.0));
  mVertex.push_back(Point3d(Z, -X, 0.0));
  mVertex.push_back(Point3d(-Z, -X, 0.0));

  addPolygon(0, 1, 4);
  addPolygon(0, 4, 9);
  addPolygon(9, 4, 5);
  addPolygon(4, 8, 5);
  addPolygon(4, 1, 8);
  
  addPolygon(8, 1, 10);
  addPolygon(8, 10, 3);
  addPolygon(5, 8, 3);
  addPolygon(5, 3, 2);
  addPolygon(2, 3, 7);
  
  addPolygon(7, 3, 10);
  addPolygon(7, 10, 6);
  addPolygon(7, 6, 11);
  addPolygon(11, 6, 0);
  addPolygon(0, 6, 1);
  
  addPolygon(6, 10, 1);
  addPolygon(9, 11, 0);
  addPolygon(9, 2, 11);
  addPolygon(9, 5, 2);
  addPolygon(7, 11, 2);
  
//  if(iLevel > 0)
//    subdivide(iLevel);
}

//------------------------------------------------------------------------------
void PlatonicSolid::makeTetrahedron()
{
  mVertex.push_back(Point3d(1, 1, 1));
  mVertex.push_back(Point3d(-1, -1, 1));
  mVertex.push_back(Point3d(-1, 1, -1));
  mVertex.push_back(Point3d(1, -1, -1));

	addPolygon( 0, 2, 1);
  addPolygon( 0, 1, 3 );
  addPolygon( 1, 2, 3 );  
	addPolygon( 2, 0, 3 );  
}

//------------------------------------------------------------------------------
//void PlatonicSolid::subdivide(int iLevel)
//{
//  //On stocke la liste de face dans un vecteur temporaire puisque le 
//  //but est de refaire la surface du solide.
//  vector<Face> preSubdividedFaces = getFaces();
//  mFaces.clear();
//  
//  for(unsigned int i = 0; i < preSubdividedFaces.size(); ++i)
//  {
//    Face f(preSubdividedFaces[i]);
//    int subIndex1, subIndex2, subIndex3;
//    
//    Vector3d v1 = toVector(getVertex()[f.index1]),
//      v2 = toVector(getVertex()[f.index2]),
//      v3 = toVector(getVertex()[f.index3]);
//    Vector3d v12, v23, v31;
//    
//    //si le côté de la face à déja été subdivisé, on prend l'index du point dans
//    //la map, sinon, on crée le point et on l'insère dans la map.
//    SubdivisionMap::const_iterator it = 
//      mSubdivisionMap.find(make_pair(f.index1, f.index2));
//    if(it == mSubdivisionMap.end())
//    {
//      v12 = v1 + v2;
//      v12.normalise();
//      mVertex.push_back(toPoint(v12));
//      subIndex1 = mVertex.size() - 1;
//      mSubdivisionMap.insert (make_pair(make_pair(f.index1, f.index2), subIndex1));
//      mSubdivisionMap.insert (make_pair(make_pair(f.index2, f.index1), subIndex1));
//    }
//    else
//      subIndex1 = it->second;
//    
//    it = 
//      mSubdivisionMap.find(make_pair(f.index2, f.index3));
//    if(it == mSubdivisionMap.end())
//    {
//      v23 = v2 + v3;
//      v23.normalise();
//      mVertex.push_back(toPoint(v23));
//      subIndex2 = mVertex.size() - 1;
//      mSubdivisionMap.insert (make_pair(make_pair(f.index2, f.index3), subIndex2));
//      mSubdivisionMap.insert (make_pair(make_pair(f.index3, f.index2), subIndex2));
//    }
//    else
//      subIndex2 = it->second;
//      
//    it = 
//      mSubdivisionMap.find(make_pair(f.index3, f.index1));
//    if(it == mSubdivisionMap.end())
//    {
//      v31 = v3 + v1;
//      v31.normalise();
//      mVertex.push_back(toPoint(v31));
//      subIndex3 = mVertex.size() - 1;
//      mSubdivisionMap.insert (make_pair(make_pair(f.index3, f.index1), subIndex3));
//      mSubdivisionMap.insert (make_pair(make_pair(f.index1, f.index3), subIndex3));
//    }
//    else
//      subIndex3 = it->second;
//      
//    //A partir des 3 nouveaux points, on refait la face.
//    mFaces.push_back(Face(f.index1, subIndex1, subIndex3));
//    mFaces.push_back(Face(f.index2, subIndex2, subIndex1));
//    mFaces.push_back(Face(f.index3, subIndex3, subIndex2));
//    mFaces.push_back(Face(subIndex1, subIndex2, subIndex3));
//  }
//  
//  mSubdivisionMap.clear();
//  if(--iLevel > 0)
//    subdivide(iLevel);
//}