

#include "3d/Utilities.h"
#include "math/Vect.h"
#include "math/Point.h"
#include <QColor>
#include "Representations.h"
#include <vector>

using namespace realisim;
using namespace treeD;
using namespace Representations;
using namespace std;

namespace  {
    void drawRectangleOgl2_1(const Point2d& iO, const Vector2d& iS)
    {
        glBegin(GL_QUADS);
        glTexCoord2d(0.0, 0.0);
        glVertex2d(iO.x(), iO.y());
        
        glTexCoord2d(1.0, 0.0);
        glVertex2d(iO.x() + iS.x(), iO.y());
        
        glTexCoord2d(1.0, 1.0);
        glVertex2d(iO.x() + iS.x(), iO.y() + iS.y());
        
        glTexCoord2d(0.0, 1.0);
        glVertex2d(iO.x(), iO.y() + iS.y());
        glEnd();
    }
}

//----------------
//--- Representation
//----------------
Representation::Representation() :
    mFenceSync(0)
{}

Representation::~Representation()
{}

void Representation::deleteFenceSync()
{
    if (hasFenceSync())
    {
        glDeleteSync(fenceSync());
        mFenceSync = 0;
    }
}

void Representation::draw()
{}

GLsync Representation::fenceSync() const
{ return mFenceSync; }

bool Representation::hasFenceSync() const
{ return mFenceSync != 0; }

bool Representation::isFenceSignaled() const
{
    bool r = true;
    if (hasFenceSync())
    {
        GLint result = GL_UNSIGNALED;
        glGetSynciv(fenceSync(), GL_SYNC_STATUS, sizeof(GLint), NULL, &result);
        r = result == GL_SIGNALED;
    }
    return r;
}

void Representation::setFenceSync(GLsync iFs)
{
    if (hasFenceSync())
    {
        deleteFenceSync();
    }
    mFenceSync = iFs;
}

//-------------------------------------------------------------------------------------
//--- BoundingBox
//-------------------------------------------------------------------------------------
void BoundingBox::create(IGraphicNode* ipNode)
{
    mpGraphicNode = ipNode;
}

//-------------------------------------------------------------------------------------
void BoundingBox::draw()
{
    //show bounding box
    if (mpGraphicNode && mpGraphicNode->isBoundingBoxVisible())
    {
        glDisable(GL_CULL_FACE);
        glColor3ub(255, 255, 255);

        /*GLint previousPolygonMode = GL_LINE;
        glGetIntegerv(GL_POLYGON_MODE, &previousPolygonMode);*/

        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        drawRectangularPrism( mpGraphicNode->getPositionnedAABB().getMin(),
            mpGraphicNode->getPositionnedAABB().getMax() );

        //glPolygonMode(GL_FRONT_AND_BACK, previousPolygonMode);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        glEnable(GL_CULL_FACE);
    }
}

//-------------------------------------------------------------------------------------
//--- Model
//-------------------------------------------------------------------------------------
Model::Model( ) :
    Representation(),
    mpModelNode(nullptr),
    mDisplayList(0)
{}

//--------------------------------------------------------------------------------------
void Model::create(ModelNode* ipModel,
             const std::unordered_map<unsigned int, realisim::treeD::Texture>& iTextureLibrary )
{
    // early out
    if(mDisplayList != 0) {return ;}

    //--- assign model
    mpModelNode = ipModel;


    //sort face per texture...
    map<int, vector<const Face*> > mTextureToFaces;
    
    for(size_t i = 0; i < mpModelNode->mFaces.size(); ++i)
    {
        int textureId = 0;
        const Face* f = mpModelNode->mFaces[i];

        if(f->mpMaterial && f->mpMaterial->mpImage)
        {
            auto it = iTextureLibrary.find(f->mpMaterial->mpImage->getId());
            if(it != iTextureLibrary.end())
            { 
                textureId = it->second.getId();
            }
        }

        auto insertIt = mTextureToFaces.insert( make_pair(textureId, vector<const Face*>() ) );
        vector<const Face*>& v = insertIt.first->second;
        v.push_back(f);
    }


    //--- Create display list.
    //--- opengl stuff...
    mDisplayList = glGenLists(1);    
    glNewList(mDisplayList, GL_COMPILE);   
    
    auto it = mTextureToFaces.begin();
    for (; it != mTextureToFaces.end(); ++it)
    {
        int texId = it->first;
        vector<const Face*>& v = it->second;

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texId);

        for (auto itFace : v)
        {
            const Face* f = itFace;
            glBegin(GL_POLYGON);
            for(int j = 0; j < f->mVertexIndices.size(); ++j)
            {
                int index = f->mVertexIndices[j];
                QColor col = f->mpVertexPool->mColors[index];
                glColor4ub( col.red(), col.green(), col.blue(), col.alpha() );
                glTexCoord2dv( f->mpVertexPool->mTextureCoordinates[index].getPtr() );
                glNormal3dv( f->mpVertexPool->mNormals[index].getPtr() );
                glVertex3dv( f->mpVertexPool->mVertices[index].getPtr() );
            }
            glEnd();
        }
        
    }
    glBindTexture(GL_TEXTURE_2D, 0);
    glEndList();
}

//--------------------------------------------------------------------------------------
void Model::createInstance(ModelNode* ipModelNode, Model* ipModel)
{
    mpModelNode = ipModelNode;
    mDisplayList = ipModel->mDisplayList;
}

//--------------------------------------------------------------------------------------
Model::~Model()
{
    glDeleteLists(mDisplayList, 1);
    mDisplayList = 0;
}

//--------------------------------------------------------------------------------------
void Model::draw()
{
    //drawRectangleOgl2_1( Point2d(-25.0, -50.0), Vector2d(50, 100) );
    
    glPushMatrix();
    glMultMatrixd( mpModelNode->mWorldTransform.getDataPointer() );
        
    glCallList(mDisplayList);
    glPopMatrix();
    
    //printf("drawing model: %d\n", mpModelNode->getId());
}


//----------------
//--- ModelInstance
//----------------
