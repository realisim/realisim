/*
 *  PlatonicSolid.h
 *  Created by Pierre-Olivier Beaudoin on 09-08-18.
 */
 
 #ifndef realisim_math_platonicSolid_h
 #define realisim_math_platonicSolid_h
 
 /*Cette classe réprésente les solides platoniques tels que le tétrahédron, cube
   octahedron, dodecahedron, isocahedron etc... Ils sont représentés comme une
   liste de faces triangulaires. Chaque face possède 3 indices. Chacun de ces
   indices représentent un point3d dans la liste de vertexes.
   Il est possible de subdiviser l'isocahédron afin d'approximer une sphère.
   Note: 
   
   voir: http://en.wikipedia.org/wiki/Platonic_solid
 
   mVertex: représente la liste de points constituant le solide
   mFaces: représente le maillage des points qui forme la surface. Une Face est
     composée de 3 indexes de vertex.
   mSubdivisionMap: est utilisé lors de la subdivision de l'isocahedron. Un 
    nouveau point est créé dans le milieu de chacun des côtés d'une face. La map
    stocke une paire d'index (un côté) d'une face et associé l'index du nouveau
    point. Avant d'ajouter un nouveau point sur un côté d'une face, on regarde
    dans la map s'il n'existe pas déjà.
    
    Examples:
    Dessiner un solide platonique avec OpenGL
    
      PlatonicSolid::Face f;
      for(unsigned int i = 0; i < iPs.getFaces().size(); ++i)
      {
        f = mPs.getFaces()[i];
        Vector3d n = Vector3d(mPs.getVertex()[f.index2], mPs.getVertex()[f.index1]) ^
          Vector3d(mPs.getVertex()[f.index2], mPs.getVertex()[f.index3]);
        n.normalise();
        glBegin(GL_TRIANGLES);
          glNormal3dv(n.getPtr());
          glVertex3d(mPs.getVertex()[f.index1].getX(),
            mPs.getVertex()[f.index1].getY(),
            mPs.getVertex()[f.index1].getZ());
            
          glVertex3d(mPs.getVertex()[f.index2].getX(),
            mPs.getVertex()[f.index2].getY(),
            mPs.getVertex()[f.index2].getZ());
            
          glVertex3d(mPs.getVertex()[f.index3].getX(),
            mPs.getVertex()[f.index3].getY(),
            mPs.getVertex()[f.index3].getZ());
        glEnd();
      }
    
    */
#include <map>
#include "Point.h"
#include "Polygon.h"
#include <vector>

namespace realisim
{
namespace math
{
  using namespace std;

  class PlatonicSolid
  {
  public:
    enum type{tCube, tIsocahedron, tTetrahedron};

    PlatonicSolid( type );
    PlatonicSolid(const PlatonicSolid&);
    PlatonicSolid& operator=(const PlatonicSolid&);
    virtual ~PlatonicSolid();
  
    virtual int getNumberOfPolygons() const { return mPolygons.size(); }
    virtual int getNumberOfVertices() const { return mVertex.size(); } 
    virtual const Polygon& getPolygon( int ) const;
    virtual const vector<Polygon>& getPolygons() const { return mPolygons; } 
    virtual const type getType() const {return mType;}
    virtual const Point3d& getVertex( int ) const;
    virtual const vector<int>& getVertexIndicesForPolygon( int ) const;
    virtual const vector<Point3d>& getVertices() const {return mVertex;}
    
  protected:
  	virtual void addPolygon( int, int, int );
    virtual void addPolygon( int, int, int, int );
    virtual void makeCube();
    virtual void makeIsocahedron();
    //virtual void makeIsocahedron(int);
    virtual void makeTetrahedron();
    //virtual void subdivide(int);
    
    type mType;
    vector<Polygon> mPolygons;
    vector<Point3d> mVertex;
    vector< vector< int > > mVertexIndices;
    
//    typedef map<pair<int, int>, int> SubdivisionMap;
//    SubdivisionMap mSubdivisionMap;
  };
}
}

#endif