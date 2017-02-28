

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
Model::Model(ModelNode* ipModel,
             const std::unordered_map<unsigned int, realisim::treeD::Texture>& iTextureLibrary ) :
Representation(),
mpModel(ipModel),
mTextureLibrary(iTextureLibrary)
{
    mDisplayList = glGenLists(1);
    
    glNewList(mDisplayList, GL_COMPILE);
    
    
//    const Face* f = mpModel->mFaces[0];
//    if(f->mpMaterial && f->mpMaterial->mpImage )
//    {
//        Image* im = f->mpMaterial->mpImage;
//        realisim::math::Vector2i size(im->mWidth, im->mHeight);
//        GLenum internalFormat = GL_SRGB8_ALPHA8;
//        GLenum format = GL_RGBA;
//        GLenum datatype = GL_UNSIGNED_BYTE;
//        
//        if(im->mNumberOfChannels == 3)
//        {
//            internalFormat = GL_SRGB8;
//            format = GL_RGB;;
//        }
//        mTexture.set(im->mpPayload, size, internalFormat, format, datatype);

//test
//{
//    QImage::Format f;
//    if(im->mNumberOfChannels == 3)
//        f = QImage::Format_RGB888;
//    if(im->mNumberOfChannels == 4)
//        f = QImage::Format_RGBA8888;
//    QImage qim(im->mpPayload,
//              im->mWidth,
//              im->mHeight,
//              f);
//    
//    QImage qim2 = qim.mirrored(true, true);
//    mTexture.set(qim2.bits(), size, internalFormat, format, datatype);
//}
//    }
    
    
    
    // meshes...
    
    // faces...
    int textureId = 0;
    for(size_t i = 0; i < mpModel->mFaces.size(); ++i)
    {
        const Face* f = mpModel->mFaces[i];

        // textures can change on each faces, we should sort the faces per texture so we 
        //minimize the texture change...
        //
        if(f->mpMaterial && f->mpMaterial->mpImage)
        {
            auto it = mTextureLibrary.find(f->mpMaterial->mpImage->mId);
            if(it != mTextureLibrary.end() && textureId != it->second.getId())
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