
#include "VertexBufferObject.h"

using namespace realisim;
using namespace math;
using namespace treeD;

#define BUFFER_OFFSET(i) ((char*)NULL + (i))

namespace
{
    enum layoutPosition { lpVertex = 0, lpNormal, lpColor, lp2dTextureCoord };
}
//---------------------------------------------------------------------------
//--- Guts
//----------------------------------------------------------------------------
VertexBufferObject::Guts::Guts() :
    mRefCount(1),
    mVaoId(0),
    mBuffers(),
    mNumberOfIndices(0),
    mIsBaked(false)
{
    for(int i = 0; i < btCount; ++i){ mBuffers[i] = 0; }
}

//---------------------------------------------------------------------------
//--- Vertex Buffer
//----------------------------------------------------------------------------
VertexBufferObject::VertexBufferObject() : mpGuts(nullptr)
{
    makeGuts();
}

//----------------------------------------------------------------------------
VertexBufferObject::VertexBufferObject(const VertexBufferObject& iT) :
    mpGuts(nullptr)
{
    shareGuts(iT.mpGuts);
}

//----------------------------------------------------------------------------
VertexBufferObject::~VertexBufferObject()
{
    deleteGuts();
}

//----------------------------------------------------------------------------
VertexBufferObject& VertexBufferObject::operator=(const VertexBufferObject& iT)
{
    shareGuts(iT.mpGuts);
    return *this;
}

//----------------------------------------------------------------------------
void VertexBufferObject::deleteGuts()
{
    if (mpGuts && --mpGuts->mRefCount == 0)
    {
        /*On relache toutes les ressources openGL.*/
        clear();
        delete mpGuts;
        mpGuts = nullptr;
    }
}

//----------------------------------------------------------------------------
void VertexBufferObject::bake()
{
    clear();

    glGenVertexArrays(1, &mpGuts->mVaoId);
    glBindVertexArray(mpGuts->mVaoId);
    bakeArrays();    
    glBindVertexArray(0);

    mpGuts->mIsBaked = true;
}

//----------------------------------------------------------------------------
void VertexBufferObject::bakeArrays()
{
    //vertex
    std::vector<float>& v = mpGuts->mVertices;
    const int vSize = sizeof(float) * (int)v.size();
    if(vSize > 0)
    {
        glGenBuffersARB(1, &(mpGuts->mBuffers[btVertex]));
        glBindBufferARB(GL_ARRAY_BUFFER_ARB, mpGuts->mBuffers[btVertex]);
        glBufferDataARB(GL_ARRAY_BUFFER_ARB, vSize, &v[0], GL_STATIC_DRAW_ARB);
        glEnableVertexAttribArray(lpVertex);
        glVertexAttribPointer(lpVertex, 3, GL_FLOAT, GL_FALSE, 0, 0);
    }
    
    //normal
    std::vector<float>& n = mpGuts->mNormals;
    const int nSize = sizeof(float) * (int)n.size();
    if (nSize > 0)
    {
        glGenBuffersARB(1, &(mpGuts->mBuffers[btNormal]));
        glBindBufferARB(GL_ARRAY_BUFFER_ARB, mpGuts->mBuffers[btNormal]);
        glBufferDataARB(GL_ARRAY_BUFFER_ARB, nSize, &n[0], GL_STATIC_DRAW_ARB);
        glEnableVertexAttribArray(lpNormal);
        glVertexAttribPointer(lpNormal, 3, GL_FLOAT, GL_FALSE, 0, 0);
    }

    //color
    std::vector<float>& c = mpGuts->mColors;
    const int cSize = sizeof(float) * (int)c.size();
    if (cSize > 0)
    {
        glGenBuffersARB(1, &(mpGuts->mBuffers[btColor]));
        glBindBufferARB(GL_ARRAY_BUFFER_ARB, mpGuts->mBuffers[btColor]);
        glBufferDataARB(GL_ARRAY_BUFFER_ARB, cSize, &c[0], GL_STATIC_DRAW_ARB);
        glEnableVertexAttribArray(lpColor);
        glVertexAttribPointer(lpColor, 4, GL_FLOAT, GL_FALSE, 0, 0);
    }

    //2d Texture coords
    std::vector<float>& t2d = mpGuts->m2dTextureCoordinates;
    const int t2dSize = sizeof(float) * (int)t2d.size();
    if (t2dSize > 0)
    {
        glGenBuffersARB(1, &(mpGuts->mBuffers[bt2dTexture]));
        glBindBufferARB(GL_ARRAY_BUFFER_ARB, mpGuts->mBuffers[bt2dTexture]);
        glBufferDataARB(GL_ARRAY_BUFFER_ARB, t2dSize, &t2d[0], GL_STATIC_DRAW_ARB);
        glEnableVertexAttribArray(lp2dTextureCoord);
        glVertexAttribPointer(lp2dTextureCoord, 2, GL_FLOAT, GL_FALSE, 0, 0);
    }

    //elements (indices)
    std::vector<int>& i = mpGuts->mIndices;
    const int iSize = sizeof(int) * (int)i.size();
    if (iSize > 0)
    {
        glGenBuffersARB(1, &(mpGuts->mBuffers[btElement]));
        glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, mpGuts->mBuffers[btElement]);
        glBufferDataARB(GL_ELEMENT_ARRAY_BUFFER_ARB, iSize, &i[0], GL_STATIC_DRAW);
        //keep how many indices because we are going to clear the indices vector!
        mpGuts->mNumberOfIndices = (int)i.size();
    }

    // it is safe to delete after copying data to VBO
    mpGuts->mVertices.clear();
    mpGuts->mNormals.clear();
    mpGuts->mColors.clear();
    mpGuts->m2dTextureCoordinates.clear();
}

//----------------------------------------------------------------------------
void VertexBufferObject::clear()
{
    if (getVertexArrayObjectId() != 0)
    {
        glDeleteBuffersARB(btCount, mpGuts->mBuffers);
        glDeleteVertexArrays(1, &mpGuts->mVaoId);
        for(int i = 0; i < btCount; ++i){ mpGuts->mBuffers[i] = 0; }
        mpGuts->mVaoId = 0;
        mpGuts->mNumberOfIndices = 0;
        mpGuts->mIsBaked = false;
        mpGuts->mVertices.clear();
        mpGuts->mNormals.clear();
        mpGuts->mColors.clear();
        mpGuts->m2dTextureCoordinates.clear();
    }


}

//----------------------------------------------------------------------------
void VertexBufferObject::draw() const
{
    if (mpGuts->mIsBaked)
    {
        glBindVertexArray(getVertexArrayObjectId());
        glDrawElements(GL_TRIANGLES, mpGuts->mNumberOfIndices, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }
    else
    {
        printf("Vbo cannot be drawm because it is not yet baked!\n");
    }
}

//----------------------------------------------------------------------------
void VertexBufferObject::makeGuts()
{
    mpGuts = new Guts();
}

//----------------------------------------------------------------------------
void VertexBufferObject::set2dTextureCoordinates( int iNumberOfCoords, float *ipCoords)
{
    mpGuts->m2dTextureCoordinates.clear();
    mpGuts->m2dTextureCoordinates.resize(iNumberOfCoords);
    for (int i = 0; i < iNumberOfCoords; ++i)
    {
        mpGuts->m2dTextureCoordinates[i] = ipCoords[i];
    }
}

//----------------------------------------------------------------------------
void VertexBufferObject::setColors(int iNum, float *ipColors)
{
	mpGuts->mColors.clear();
	mpGuts->mColors.resize(iNum);
	for (int i = 0; i < iNum; ++i)
	{
		mpGuts->mColors[i] = ipColors[i];
	}
}

//----------------------------------------------------------------------------
void VertexBufferObject::setColors(const std::vector<float>& iColors)
{
    mpGuts->mColors = iColors;
}

//----------------------------------------------------------------------------
//pointer to an array of 3d vertices.
void VertexBufferObject::setIndices(int iNum, int *ipIndices)
{
    mpGuts->mIndices.clear();
    mpGuts->mIndices.resize(iNum);
    for (int i = 0; i < iNum; ++i)
    {
        mpGuts->mIndices[i] = ipIndices[i];
    }
}

//----------------------------------------------------------------------------
void VertexBufferObject::setIndices(const std::vector<int>& iIndices)
{
    mpGuts->mIndices = iIndices;
}

void VertexBufferObject::setNormals(int iNumberOfNormales, float *ipNormals)
{
    mpGuts->mNormals.clear();
    mpGuts->mNormals.resize(iNumberOfNormales);
    for (int i = 0; i < iNumberOfNormales; ++i)
    {
        mpGuts->mNormals[i] = ipNormals[i];
    }
}

//----------------------------------------------------------------------------
//pointer to an array of 3d vertices.
void VertexBufferObject::setVertices(int iNumVertives, float *ipVertices)
{
    mpGuts->mVertices.clear();
    mpGuts->mVertices.resize(iNumVertives);
    for (int i = 0; i < iNumVertives; ++i)
    {
        mpGuts->mVertices[i] = ipVertices[i];
    }
}

//----------------------------------------------------------------------------
void VertexBufferObject::setVertices(const std::vector<math::Point3d>& iVertices)
{
    mpGuts->mVertices.clear();
    mpGuts->mVertices.resize(3 * iVertices.size());

    for (int i = 0; i < iVertices.size(); ++i)
    {
        mpGuts->mVertices[i * 3 + 0] = (float)iVertices[i].x();
        mpGuts->mVertices[i * 3 + 1] = (float)iVertices[i].y();
        mpGuts->mVertices[i * 3 + 2] = (float)iVertices[i].z();
    }
}

//----------------------------------------------------------------------------
void VertexBufferObject::shareGuts(Guts* g)
{
    if (mpGuts != g)
    {
        deleteGuts();
        mpGuts = g;
        ++mpGuts->mRefCount;
    }
}
