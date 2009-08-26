/*
 *  PlatonicSolid.cpp
 *  Created by Pierre-Olivier Beaudoin on 09-08-18.
 */

#include "MathUtils.h"
#include "PlatonicSolid.h"
#include "Vect.h"

using namespace realisim;
  using namespace math;
  
PlatonicSolid::PlatonicSolid(type t, int iLevel /*=0*/) : 
  mType(t),
  mVertex(),
  mFaces()
{
  switch (getType()) 
  {
    case tCube:
      makeCube();
      break;
    case tIsocahedron:
      makeIsocahedron(iLevel);
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
  mVertex(iP.getVertex()),
  mFaces(iP.getFaces())
{}

PlatonicSolid& PlatonicSolid::operator=(const PlatonicSolid& iP)
{
  mType = iP.getType();
  mVertex = iP.getVertex();
  mFaces = iP.getFaces();
  return *this;
}

PlatonicSolid::~PlatonicSolid()
{}

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

  mFaces.push_back(Face(0, 1, 5));
  mFaces.push_back(Face(0, 5, 4));
  mFaces.push_back(Face(1, 2, 6));
  mFaces.push_back(Face(1, 6, 5));
  mFaces.push_back(Face(2, 3, 7));
  mFaces.push_back(Face(2, 7, 6));
  mFaces.push_back(Face(3, 0, 4));
  mFaces.push_back(Face(3, 4, 7));
  mFaces.push_back(Face(0, 2, 1));
  mFaces.push_back(Face(0, 3, 2));
  mFaces.push_back(Face(4, 5, 6));
  mFaces.push_back(Face(4, 6, 7));
}

//------------------------------------------------------------------------------
void PlatonicSolid::makeIsocahedron(int iLevel)
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
  
  mFaces.push_back(Face(0, 1, 4));
  mFaces.push_back(Face(0, 4, 9));
  mFaces.push_back(Face(9, 4, 5));
  mFaces.push_back(Face(4, 8, 5));
  mFaces.push_back(Face(4, 1, 8));
  
  mFaces.push_back(Face(8, 1, 10));
  mFaces.push_back(Face(8, 10, 3));
  mFaces.push_back(Face(5, 8, 3));
  mFaces.push_back(Face(5, 3, 2));
  mFaces.push_back(Face(2, 3, 7));
  
  mFaces.push_back(Face(7, 3, 10));
  mFaces.push_back(Face(7, 10, 6));
  mFaces.push_back(Face(7, 6, 11));
  mFaces.push_back(Face(11, 6, 0));
  mFaces.push_back(Face(0, 6, 1));
  
  mFaces.push_back(Face(6, 10, 1));
  mFaces.push_back(Face(9, 11, 0));
  mFaces.push_back(Face(9, 2, 11));
  mFaces.push_back(Face(9, 5, 2));
  mFaces.push_back(Face(7, 11, 2));
  
  if(iLevel > 0)
    subdivide(iLevel);
}

//------------------------------------------------------------------------------
void PlatonicSolid::makeTetrahedron()
{
  mVertex.push_back(Point3d(1, 1, 1));
  mVertex.push_back(Point3d(-1, -1, 1));
  mVertex.push_back(Point3d(-1, 1, -1));
  mVertex.push_back(Point3d(1, -1, -1));

  mFaces.push_back(Face(0, 2, 1));
  mFaces.push_back(Face(0, 1, 3));
  mFaces.push_back(Face(1, 2, 3));
  mFaces.push_back(Face(2, 0, 3));
}

//------------------------------------------------------------------------------
void PlatonicSolid::subdivide(int iLevel)
{
  //On stocke la liste de face dans un vecteur temporaire puisque le 
  //but est de refaire la surface du solide.
  vector<Face> preSubdividedFaces = getFaces();
  mFaces.clear();
  
  for(unsigned int i = 0; i < preSubdividedFaces.size(); ++i)
  {
    Face f(preSubdividedFaces[i]);
    int subIndex1, subIndex2, subIndex3;
    
    Vector3d v1 = toVector(getVertex()[f.index1]),
      v2 = toVector(getVertex()[f.index2]),
      v3 = toVector(getVertex()[f.index3]);
    Vector3d v12, v23, v31;
    
    //si le côté de la face à déja été subdivisé, on prend l'index du point dans
    //la map, sinon, on crée le point et on l'insère dans la map.
    SubdivisionMap::const_iterator it = 
      mSubdivisionMap.find(make_pair(f.index1, f.index2));
    if(it == mSubdivisionMap.end())
    {
      v12 = v1 + v2;
      v12.normalise();
      mVertex.push_back(toPoint(v12));
      subIndex1 = mVertex.size() - 1;
      mSubdivisionMap.insert (make_pair(make_pair(f.index1, f.index2), subIndex1));
      mSubdivisionMap.insert (make_pair(make_pair(f.index2, f.index1), subIndex1));
    }
    else
      subIndex1 = it->second;
    
    it = 
      mSubdivisionMap.find(make_pair(f.index2, f.index3));
    if(it == mSubdivisionMap.end())
    {
      v23 = v2 + v3;
      v23.normalise();
      mVertex.push_back(toPoint(v23));
      subIndex2 = mVertex.size() - 1;
      mSubdivisionMap.insert (make_pair(make_pair(f.index2, f.index3), subIndex2));
      mSubdivisionMap.insert (make_pair(make_pair(f.index3, f.index2), subIndex2));
    }
    else
      subIndex2 = it->second;
      
    it = 
      mSubdivisionMap.find(make_pair(f.index3, f.index1));
    if(it == mSubdivisionMap.end())
    {
      v31 = v3 + v1;
      v31.normalise();
      mVertex.push_back(toPoint(v31));
      subIndex3 = mVertex.size() - 1;
      mSubdivisionMap.insert (make_pair(make_pair(f.index3, f.index1), subIndex3));
      mSubdivisionMap.insert (make_pair(make_pair(f.index1, f.index3), subIndex3));
    }
    else
      subIndex3 = it->second;
      
    //A partir des 3 nouveaux points, on refait la face.
    mFaces.push_back(Face(f.index1, subIndex1, subIndex3));
    mFaces.push_back(Face(f.index2, subIndex2, subIndex1));
    mFaces.push_back(Face(f.index3, subIndex3, subIndex2));
    mFaces.push_back(Face(subIndex1, subIndex2, subIndex3));
  }
  
  mSubdivisionMap.clear();
  if(--iLevel > 0)
    subdivide(iLevel);
}