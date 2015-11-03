#ifndef resonant_corelibrary_sceneGraph_h
#define resonant_corelibrary_sceneGraph_h

//--- glew.h doit être inclus avant qgl.h (a cause de gl.h)
#ifdef WIN32
   #ifndef __glew_h__
      #include <GL/glew.h> //nécessaire pour l'utilisation de GL_BGRA
   #endif   
#endif
#include "qgl.h"
#include "coreLibrary, glCamera.h"
#include "coreLibrary, primitives.h"
#include "coreLibrary, volume.h"
#include "coreLibrary, types.h"
#include "utilities, log.h"
#include <list>
#include <vector>
#include <QColor>
#include <QString>
#include "utilities, texture.h"


namespace resonant {
namespace coreLibrary 
{ 
  class cameraNode;
  class drawableNode;
  class meshNode;
  class node;
  class nodePath;
  class transformationNode;
  class volumeNode;
  class sceneGraph;

  class plane;
} }
namespace resonant { namespace coreLibrary { using namespace std; } }
//------------------------------------------------------------------------------
//--- nodePath
//------------------------------------------------------------------------------
class resonant::coreLibrary::nodePath
{
public:   
   explicit nodePath( const node* );
   virtual ~nodePath() {;}

   matrix4 getTransformation() const;
protected:
   nodePath() {}
   nodePath( const nodePath& ){}
   nodePath& operator=(const nodePath& ){}

   std::list<const node*> mPath;
};
//devrait mettre un namespace sceneGraph... et renommer la classe sceneGraph
//par scene
//------------------------------------------------------------------------------
//--- node
//------------------------------------------------------------------------------
class resonant::coreLibrary::node
{
public:
   friend class sceneGraph;

   explicit node( QString );
   virtual ~node();
   
   int getNumberOfChilds() const;
   int getNumberOfParents() const;
   node* getChild( int ) const;
   node* getParent() const;
   node* getParent( int ) const;
   QString getToken() const;
   bool hasParent() const;
   bool isVerbose() const;
//void reparent(node*);
   QString toString() const;
   void setAsVerbose(bool);

protected:
   void addChild( node* );
   virtual void begin();
   unsigned int getId() const {return mId;}
   virtual void end();
//void deleteChild( int )
   void log( QString );
   void removeAllChilds();
   void removeChild(node*);
   void setLog( utilities::log* );
   void setParent( node* );
   void toString( const node*, QString* ) const;

   static unsigned int mId;
   QString mToken;
   node* mpParent;
   vector< node* > mChilds;
   bool mIsVerbose;
   utilities::log* mpLog;
};
//------------------------------------------------------------------------------
//--- drawableNode
//------------------------------------------------------------------------------
class resonant::coreLibrary::drawableNode : public node
{
public:
   friend class sceneGraph;

   explicit drawableNode( QString );
   virtual ~drawableNode();

   virtual void begin();
   void drawBoundingBox(bool);
   virtual void end();
   double getAlpha() const;
   coreLibrary::box getBoundingBox(bool iGlobal = true) const;
   QColor getColor() const;
   bool isDrawingBoundingBox() const;
   void setAlpha( double );
   void setColor( QColor );
   
protected:
   virtual void slice(const plane&) {}

   QColor mColor;
   bool mIsDrawingBoundingBox;
   box mBoundingBox;
};
//------------------------------------------------------------------------------
//--- volumeNode
//------------------------------------------------------------------------------
class resonant::coreLibrary::volumeNode : public drawableNode
{
public:
   explicit volumeNode( QString );
   volumeNode( QString, volume );
   virtual ~volumeNode(){}

   virtual void begin();
   volume getVolume() const;
   void setVolume(volume);   

protected:
   void initialize();
   bool isInitialized() const {return mIsInitialized;}
   virtual void slice(const plane&);

   volume mVolume;
   utilities::texture mTexture;
   bool mIsInitialized;
   polygon mSlice;
};
//------------------------------------------------------------------------------
//--- meshNode
//------------------------------------------------------------------------------
class resonant::coreLibrary::meshNode : public drawableNode
{
public:
   explicit meshNode( QString );
   meshNode( QString, mesh2* );
   virtual ~meshNode(){}

   virtual void begin();
   mesh2* getMesh() const;
   void setMesh(mesh2*);

protected:
   void initialize();
   bool isInitialized() const {return mIsInitialized;}
   virtual void slice(const plane&);

   mesh2* mpMesh;
   GLuint mDisplayList;
   bool mIsInitialized;
   vector<lineSegment> mSlice;
};
//------------------------------------------------------------------------------
//--- TransformationNode
//------------------------------------------------------------------------------
class resonant::coreLibrary::transformationNode : public node
{
public:
   explicit transformationNode( QString );
   transformationNode( QString, matrix4 );
   virtual ~transformationNode();

   matrix4 getTransformation() const;
   void setTransformation( matrix4 );
protected:
   virtual void begin();
   virtual void end();

   matrix4 mTransform;
};
//shaderNode
//------------------------------------------------------------------------------
//--- cameraNode
//------------------------------------------------------------------------------
class resonant::coreLibrary::cameraNode : public node
{
public:
   explicit cameraNode( QString );
   cameraNode( QString, glCamera );
   virtual ~cameraNode() {}

   glCamera getCamera() const;
   void setCamera(glCamera);

protected:
   virtual void begin();
   virtual void end();

   glCamera mCamera;
};
//------------------------------------------------------------------------------
//--- sceneGraph
//------------------------------------------------------------------------------
class resonant::coreLibrary::sceneGraph
{
public:
   explicit sceneGraph();
   virtual ~sceneGraph();

   void addNode( node* ); //addToRootNote?
   void addNode( node*, QString );   
//deleteNode( QString );
   void clear();
   node* findNode( QString );
   utilities::log& getLog() const;
   bool hasNode( QString );
   bool isVerbose() const;
//removeNode( node* );
   void removeNode( QString );
   //vector<QString> pick()
   void render();   
   void setAsVerbose( bool );
   void slice(const plane&);
   QString toString() const;

protected:
   struct statistics
   {
      statistics();
      QString toQString() const;

      double mFps;
      int mFrameCount;
      QTime mFrameTimer;
   };
   
   //void deleteNode( node* );
   node* findNode( node*, QString );
   void render( node* );
   void slice(node*, const plane&);
   void setAsVerbose(node*, bool);

   mutable utilities::log mLog;
   node* mpRoot;
   mutable QString mErrors;
   bool mIsVerbose;
   bool mAccumulateStatistics;
   statistics mStats;
};


#endif