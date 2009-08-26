/*
 *  PlatonicSolid.h
 *  Created by Pierre-Olivier Beaudoin on 09-08-18.
 */
 
 #ifndef realisim_math_platonicSolid_h
 #define realisim_math_platonicSolid_h
 
 /*Cette classe réprésente les solides platoniques tels que le tétrahédron, cube
   octahedron, dodecahedron, isocahedron etc...   
   Il est possible de subdiviser l'isocahédron afin d'approximer une sphère.
   
   voir: http://en.wikipedia.org/wiki/Platonic_solid
 
   mVertex: représente la liste de points constituant le solide
   mFaces: représente le maillage des points qui forme la surface. Une Face est
     composée de 3 indexs de vertex.
  mSubdivisionMap: est utilisé lors de la subdivision de l'isocahedron. Un 
    nouveau point est créé dans le milieu de chacun des côtés d'une face. La map
    stocke une paire d'index (un côté) d'une face et associé l'index du nouveau
    point. Avant d'ajouter un nouveau point sur un côté d'une face, on regarde
    dans la map s'il n'existe pas déjà.*/
 #include <map>
 #include "Point.h"
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
    struct Face
    {
      Face(int a, int b, int c) : index1(a), index2(b), index3(c){;}
      Face(const Face& f) : index1(f.index1), index2(f.index2), index3(f.index3){;}
      int index1; int index2; int index3;
    };
        
    PlatonicSolid(type, int = 0);
    PlatonicSolid(const PlatonicSolid&);
    PlatonicSolid& operator=(const PlatonicSolid&);
    virtual ~PlatonicSolid();
    
    virtual const vector<Face>& getFaces() const {return mFaces;}
    virtual const type getType() const {return mType;}
    virtual const vector<Point3d>& getVertex() const {return mVertex;}
    
  protected:
    virtual void makeCube();
    virtual void makeIsocahedron(int);
    virtual void makeTetrahedron();
    virtual void subdivide(int);
    
    type mType;
    vector<Point3d> mVertex;
    vector<Face> mFaces;
    
    typedef map<pair<int, int>, int> SubdivisionMap;
    SubdivisionMap mSubdivisionMap;
  };
}
}

#endif