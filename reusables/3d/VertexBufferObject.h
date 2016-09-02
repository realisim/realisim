
#ifndef Realisim_TreeD_VertexBufferObject_hh
#define Realisim_TreeD_VertexBufferObject_hh

#include "openGlHeaders.h"
#include <math/Point.h>
#include <math/Vect.h>
#include <map>
#include <vector>


namespace realisim
{
namespace treeD
{

/*This class encapsulates VBOs.
 It currently supports vertices, normals, colors, 1 texture coordinates and draw triangles.

 Vertices must be 3d coordinaates (x,y,z)
 Normals must be 3d coordinaates (x,y,z)
 colors must be 4 component floats (r, g, b, a) -> [0, 1]
 texture coordinates must be 2d (u,v) -> [0, 1]

ex: an XY rectangle with texture coordinates.
    float v[12] = {
    0, 0, 0,
    10, 0, 0,
    10, 10, 0,
    0, 10, 0 };

    int i[6] = {
    0, 1, 3,
    1, 2, 3 };

    float t[8] = {
    0.0, 0.0,
    1.0, 0.0,
    1.0, 1.0,
    0.0, 1.0
    };

    vbo.setVertices(12, v);
    vbo.setIndices(6, i);
    vbo.set2dTextureCoordinates(8, t);
    vbo.bake();
    vbo.draw();
*/
class VertexBufferObject
{
public:
    VertexBufferObject();
    VertexBufferObject(const VertexBufferObject&);
    virtual ~VertexBufferObject();
    virtual VertexBufferObject& operator=(const VertexBufferObject&);

    //virtual VertexBufferObject copy();

    virtual void bake();
    virtual void clear();
    virtual void draw() const;

    virtual GLuint getVertexArrayObjectId() const { return mpGuts->mVaoId; }
    virtual void set2dTextureCoordinates(int iNumberOfCoords, float *ipCoords);
    //virtual void set3dTextureCoordinates(GLenum target, int iNumberOfCoords, float *ipCoords);
    virtual void setColors(const std::vector<float>&);
    virtual void setIndices(int iNum, int *ipIndices);
    virtual void setIndices(const std::vector<int>&);
    virtual void setNormals(int iNumberOfNormales, float *ipNormals);
    //  virtual void setNormals(const std::vector<math::Vector3d>&);
    virtual void setVertices(int iNumVertices, float *ipVertices);
    virtual void setVertices(const std::vector<math::Point3d>&);

protected:

    enum bufferType { btVertex = 0, btNormal, btColor, bt2dTexture, btElement, btCount };
    struct Guts
    {
        Guts();

        unsigned int mRefCount;
        GLuint mVaoId;
        GLuint mBuffers[btCount];
        int mNumberOfIndices;
        bool mIsBaked;

        //temporary storage until bake() is called;
        std::vector<float> mVertices;
        std::vector<int> mIndices;
        std::vector<float> mColors;
        std::vector<float> mNormals;
        std::vector<float> m2dTextureCoordinates;
        //std::vector<math::Point3d> m3dTextureCoordinates;
    };

    virtual void bakeArrays();
    virtual void deleteGuts();
    virtual void makeGuts();
    virtual void shareGuts(Guts*);

    Guts* mpGuts;
};

}//treeD
}//realisim

#endif