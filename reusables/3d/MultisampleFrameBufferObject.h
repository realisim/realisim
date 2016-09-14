
#ifndef Realisim_TreeD_MultisampleFrameBufferObject_hh
#define Realisim_TreeD_MultisampleFrameBufferObject_hh

#include <3d/FrameBufferObject.h>
#include <cassert>
#include "openGlHeaders.h"
#include <QImage>
#include "Texture.h"
#include <vector>

/*This class is very similar to FrameBuffer object. The api is also similar. The major distinction
  is that the attachments (color, depth etc...) cannot be used as textures. The multisampleframebuffer
  must be resolved to a FrameBufferObject (via method resolveTo()) prior to being rendered on screen
  or used as input to other rendering pass.*/

namespace realisim
{
namespace treeD
{
    using namespace std;

    /**/
    class MultisampleFrameBufferObject
    {
    public:
        MultisampleFrameBufferObject();
		MultisampleFrameBufferObject(const MultisampleFrameBufferObject&);
        ~MultisampleFrameBufferObject();
        MultisampleFrameBufferObject& operator=(const MultisampleFrameBufferObject&);

        void addDepthAttachment();
		void addColorAttachment();
		void addColorAttachment(GLenum iInternalFormat);
        void begin();
        void clear();
        MultisampleFrameBufferObject copy(); //detach voir Shader::copy  
        void drawTo(int);
		void end();
		GLuint getColorAttachmentId(int) const;
		GLenum getColorAttachmentInternalFormat(int i) const {return mpGuts->mColorAttachmentInternalFormat[i];}
        GLuint getDepthAttachmentId() const { return mpGuts->mDepthTextureId; }
        GLuint getFrameBufferId() const { return mpGuts->mFrameBufferId; }
        int getHeight() const { return mpGuts->mHeight; }
        int getNumColorAttachment() const { return (int)mpGuts->mColorAttachments.size(); }
		int getNumberOfSamples() const {return mpGuts->mNumberOfSamples; }
        math::Vector2i getSize() const;
        int getWidth() const { return mpGuts->mWidth; }
		void glClear(GLenum);
		bool hasFixedSampleLocations() const {return mpGuts->mHasFixedSampleLocations;}      
        bool isValid() const;
        //virtual void removeColorAttachment(int);
        //virtual void removeDepthAttachment();
        void resize(int, int);
        void resize(math::Vector2i);
		void resolveToBackBuffer(int iColorAttachment);
		void resolveTo(int iFromColorAttachment, FrameBufferObject iFbo, int iToColorAttachment);
		void setHasFixedSampleLocations(bool);
		void setNumberOfSamples(int iN);

    private:
        void init();
        bool isColorAttachmentUsed() const { return !mpGuts->mColorAttachments.empty(); }
        bool isDepthAttachmentUsed() const;
        int getMaxColorAttachment() const { return mpGuts->mMaxColorAttachment; }
		void refreshColorAttachment();
		void refreshDepthAttachment();
        void validate();

        struct Guts
        {
            Guts();

            GLuint mFrameBufferId;
            bool mIsValid;
            int mWidth;
            int mHeight;
            vector<GLuint> mColorAttachments;
			vector<GLenum> mColorAttachmentInternalFormat;
			GLuint mDepthTextureId;
            int mMaxColorAttachment;
            vector<GLuint> mPreviousFrameBuffers;
			int mNumberOfSamples;
			bool mHasFixedSampleLocations;
            unsigned int mRefCount;
        };

        void deleteGuts();
        void makeGuts();
        void shareGuts(Guts*);

        Guts* mpGuts;
    };

}//treeD
}//realisim

#endif