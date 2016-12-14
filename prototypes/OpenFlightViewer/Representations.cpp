

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

//----------------
//--- Model
//----------------
Model::Model(ModelNode* ipModel) :
Representation(),
mpModel(ipModel)
{
    mDisplayList = glGenLists(1);
    
    glNewList(mDisplayList, GL_COMPILE);
    
    // meshes...
    
    // faces...
    for(size_t i = 0; i < mpModel->mFaces.size(); ++i)
    {
        const Face* f = mpModel->mFaces[i];
        
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
    
    glEndList();
}

Model::~Model()
{
    glDeleteLists(mDisplayList, 1);
}

void Model::draw()
{
    //drawRectangleOgl2_1( Point2d(-25.0, -50.0), Vector2d(50, 100) );
    
    glPushMatrix();
    glMultMatrixd( mpModel->mWorldTransform.getDataPointer() );
    glCallList(mDisplayList);
    glPopMatrix();
    
    //printf("drawing model: %d\n", mpModel->mId);
}