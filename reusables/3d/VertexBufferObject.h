
#ifndef Realisim_TreeD_VertexBufferObject_hh
#define Realisim_TreeD_VertexBufferObject_hh

#include "openGlHeaders.h"
#include <math/Point.h>
#include <math/Vect.h>

/*
  */

namespace realisim
{
namespace treeD
{
using namespace std;

/**/
class VertexBufferObject
{
public:  
  VertexBufferObject();
  VertexBufferObject(const VertexBufferObject&);
  virtual ~VertexBufferObject();
  virtual VertexBufferObject& operator=(const VertexBufferObject&);

  //virtual VertexBufferObject copy();

  virtual void bake();
  virtual void draw() const;
  
  virtual int getArrayId() const;
  virtual int getElementArrayId() const;
  virtual bool has2dTextureCoordinates() const {return mpGuts->mHas2dTextureCoordinates;}
  virtual bool has3dTextureCoordinates() const {return mpGuts->mHas3dTextureCoordinates;}
  virtual bool hasColors() const {return mpGuts->mHasColors;}
  virtual bool hasIndices() const {return mpGuts->mHasIndices;}
  virtual bool hasNormals() const {return mpGuts->mHasNormals;}
  virtual bool hasVertices() const {return mpGuts->mHasVertices;}
//  virtual void setTextureCoordinates(int, const std::vector<math::Point2d>&);
//  virtual void setTextureCoordinates(int, const std::vector<math::Point3d>&);
  virtual void setIndices(const std::vector<int>&);
  virtual void setColors(const std::vector<float>&);
//  virtual void setNormals(const std::vector<math::Vector3d>&);
  virtual void setVertices(const std::vector<math::Point3d>&);
  
protected:

  struct Guts
  {
		Guts();
    
    unsigned int mRefCount;
    GLuint mArrayId;
    GLuint mElementArrayId;
    int mNormalOffset;
    int mColorOffset;
    int mTextureOffset;
    int mNumberOfIndices;
    bool mIsBaked;
    bool mHasVertices;
    bool mHasIndices;
    bool mHasNormals;
    bool mHasColors;
    bool mHas2dTextureCoordinates;
    bool mHas3dTextureCoordinates;
    
    //temporary storage until bake() is called;
    std::vector<math::Point3d> mVertices;
    std::vector<int> mIndices;
    std::vector<float> mColors;
    std::vector<math::Vector3d> mNormals;
    //std::vector<math::Point2d> m2dTextureCoordinates;
    //std::vector<math::Point3d> m3dTextureCoordinates;
  };
  
  
  virtual void bakeArray();
  virtual void bakeElementArray();
  virtual void deleteGuts();
	virtual void makeGuts();
  virtual void shareGuts(Guts*);
  
  Guts* mpGuts;
};

}//treeD
}//realisim

#endif