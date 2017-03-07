

#include "3d/Utilities.h"
#include "math/Vect.h"
#include "math/Point.h"
#include <QColor>
#include "Representations.h"

using namespace realisim;
using namespace treeD;
using namespace Representations;

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
Representation::Representation()
{}

Representation::~Representation()
{}

void Representation::draw()
{}

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

    //--- opengl stuff...
    mDisplayList = glGenLists(1);    
    glNewList(mDisplayList, GL_COMPILE);
    
    // meshes...
    
    // faces...
    int textureId = 0;
    for(size_t i = 0; i < mpModelNode->mFaces.size(); ++i)
    {
        const Face* f = mpModelNode->mFaces[i];

        // textures can change on each faces, we should sort the faces per texture so we 
        //minimize the texture change...
        //
        if(f->mpMaterial && f->mpMaterial->mpImage)
        {
            auto it = iTextureLibrary.find(f->mpMaterial->mpImage->getId());
            if(it != iTextureLibrary.end() && textureId != it->second.getId())
            { 
                textureId = it->second.getId();
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, textureId);
            }
        }

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
