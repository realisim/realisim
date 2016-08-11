
#include "VertexBufferObject.h"

using namespace realisim;
	using namespace math;
  using namespace treeD;

#define BUFFER_OFFSET(i) ((char*)NULL + (i))

//---------------------------------------------------------------------------
//--- Guts
//----------------------------------------------------------------------------
VertexBufferObject::Guts::Guts() :
  mRefCount(1),
  mArrayId(0),
  mElementArrayId(0),
  mNormalOffset(0),
  mColorOffset(0),
  mTextureOffset(0),
  mNumberOfIndices(0),
  mIsBaked(false),
  mHasVertices(false),
  mHasIndices(false),
  mHasNormals(false),
  mHasColors(false),
  mHas2dTextureCoordinates(false),
  mHas3dTextureCoordinates(false)
{}

//---------------------------------------------------------------------------
//--- Vertex Buffer
//----------------------------------------------------------------------------
VertexBufferObject::VertexBufferObject() : mpGuts(nullptr)
{ makeGuts(); }

//----------------------------------------------------------------------------
VertexBufferObject::VertexBufferObject(const VertexBufferObject& iT) :
  mpGuts(nullptr)
{ shareGuts(iT.mpGuts); }

//----------------------------------------------------------------------------
VertexBufferObject::~VertexBufferObject()
{ deleteGuts(); }

//----------------------------------------------------------------------------
VertexBufferObject& VertexBufferObject::operator=(const VertexBufferObject& iT)
{
  shareGuts(iT.mpGuts);
  return *this;
}

//----------------------------------------------------------------------------
void VertexBufferObject::deleteGuts()
{
  if(mpGuts && --mpGuts->mRefCount == 0)
  {
    /*On relache toutes les ressources openGL.*/
    delete mpGuts;
    mpGuts = nullptr;
  }
}

//----------------------------------------------------------------------------
void VertexBufferObject::bake()
{
  bakeArray();
  bakeElementArray();
  mpGuts->mIsBaked = true;
}

//----------------------------------------------------------------------------
//(VVVVNNNNCCCCT1T1T1T1T2T2T2T2T...)
void VertexBufferObject::bakeArray()
{
  if( getArrayId() == 0 )
  {
    glGenBuffersARB(1, &(mpGuts->mArrayId));
  }

  //allocate buffer to accomodate all values
  std::vector<math::Point3d>& v = mpGuts->mVertices;
  std::vector<math::Vector3d>& n = mpGuts->mNormals;
  std::vector<float>& c = mpGuts->mColors;
  
  const int vSize = sizeof(float) * 3 * v.size();
  const int nSize = sizeof(float) * 3 * n.size();
  const int cSize = sizeof(float) * c.size();
  const int totalBufferSize = vSize + cSize + nSize;
  
  mpGuts->mHasVertices = vSize;
  mpGuts->mHasNormals = nSize;
  mpGuts->mHasColors = cSize;
  mpGuts->mHas2dTextureCoordinates = false;
  mpGuts->mHas3dTextureCoordinates = false;
  
  mpGuts->mNormalOffset = vSize;
  mpGuts->mColorOffset = mpGuts->mNormalOffset + nSize;
  mpGuts->mTextureOffset = mpGuts->mColorOffset + cSize;
  
  //convert double vertices to float;
  GLfloat *vf = new GLfloat[3*v.size()];
  for(size_t i = 0; i < v.size(); ++i)
  {
    vf[i*3 + 0] = (float)v[i].x();
    vf[i*3 + 1] = (float)v[i].y();
    vf[i*3 + 2] = (float)v[i].z();
  }

  // bind VBO in order to use
  glBindBufferARB(GL_ARRAY_BUFFER_ARB, getArrayId());
  
  //allocate buffer but bo data transfered yet. We will use sub buffer
  glBufferDataARB(GL_ARRAY_BUFFER_ARB, totalBufferSize,
                  nullptr, GL_STATIC_DRAW_ARB);
  
  //vertex
  if(hasVertices())
  { glBufferSubDataARB(GL_ARRAY_BUFFER, 0, mpGuts->mNormalOffset, vf); }
  
  //normal
  if(hasNormals())
  { glBufferSubDataARB(GL_ARRAY_BUFFER, mpGuts->mNormalOffset, nSize, &n[0]); }
  
  //color
  if(hasColors())
  {glBufferSubDataARB(GL_ARRAY_BUFFER, mpGuts->mNormalOffset, cSize, &c[0]);}
  
  //Texture
  if(has2dTextureCoordinates())
  //glBufferSubDataARB(GL_ARRAY_BUFFER, mpGuts->mTextureOffset, totalBufferSize, t[0]);
  
  glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
  
  // it is safe to delete after copying data to VBO
  delete [] vf;
  mpGuts->mVertices.clear();
  mpGuts->mColors.clear();
}

//----------------------------------------------------------------------------
void VertexBufferObject::bakeElementArray()
{
  mpGuts->mHasIndices = mpGuts->mIndices.size();
 
  if(hasIndices())
  {
    if( getElementArrayId() == 0 )
    {
      glGenBuffersARB(1, &(mpGuts->mElementArrayId));
    }
    
    glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, getElementArrayId());
    glBufferDataARB(GL_ELEMENT_ARRAY_BUFFER_ARB,
                    sizeof(int) * mpGuts->mIndices.size(),
                    &mpGuts->mIndices[0], GL_STATIC_DRAW);
    
    mpGuts->mNumberOfIndices = mpGuts->mIndices.size();
    
    glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
    mpGuts->mIndices.clear();
  }
}
  
//----------------------------------------------------------------------------
void VertexBufferObject::draw() const
{
  if(mpGuts->mIsBaked)
  {
    glBindBufferARB(GL_ARRAY_BUFFER_ARB, getArrayId());

    if(hasVertices())
    {
      glEnableClientState(GL_VERTEX_ARRAY);
      glVertexPointer(3, GL_FLOAT, 0, BUFFER_OFFSET(0));
    }
    if(hasColors())
    {
      glEnableClientState(GL_COLOR_ARRAY);
      glColorPointer(4, GL_FLOAT, 0, BUFFER_OFFSET(mpGuts->mColorOffset));
    }
    
    glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, getElementArrayId());
    glDrawElements(GL_TRIANGLES, mpGuts->mNumberOfIndices, GL_UNSIGNED_INT, 0);
    
    //cleanup
    glDisableClientState(GL_COLOR_ARRAY);
    glDisableClientState(GL_VERTEX_ARRAY);
    
    glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
    glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, 0);
  }
  else
  {
    printf("Vbo cannot be drawm because it is not yet baked!\n");
  }
}

//----------------------------------------------------------------------------
int VertexBufferObject::getElementArrayId() const
{ return mpGuts->mElementArrayId; }

//----------------------------------------------------------------------------
int VertexBufferObject::getArrayId() const
{ return mpGuts->mArrayId; }

//----------------------------------------------------------------------------
void VertexBufferObject::makeGuts()
{ mpGuts = new Guts(); }

//----------------------------------------------------------------------------
void VertexBufferObject::setColors(const std::vector<float>& iColors)
{ mpGuts->mColors = iColors; }

//----------------------------------------------------------------------------
void VertexBufferObject::setIndices(const std::vector<int>& iIndices)
{ mpGuts->mIndices = iIndices; }

//----------------------------------------------------------------------------
void VertexBufferObject::setVertices(const std::vector<math::Point3d>& iVertices)
{ mpGuts->mVertices = iVertices; }

//----------------------------------------------------------------------------
void VertexBufferObject::shareGuts(Guts* g)
{
  if(mpGuts != g)
  {
    deleteGuts();
    mpGuts = g;
    ++mpGuts->mRefCount;
  }
}
