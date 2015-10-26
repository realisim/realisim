#include "coreLibrary, glSceneGraph.h"
#include "coreLibrary, intersection.h"
#include "coreLibrary, primitives.h"
#include "utilities3d.h"
#include "utilities, timer.h"
#include <vector>

using namespace resonant;
   using namespace coreLibrary;
   using namespace std;

//------------------------------------------------------------------------------
//--- nodePath
//------------------------------------------------------------------------------
nodePath::nodePath(const node* ipNode)
{
   mPath.clear();
   while(ipNode != 0)
   {
      mPath.push_front(ipNode);
      ipNode = ipNode->getParent(0);
   }
}
//------------------------------------------------------------------------------
matrix4 nodePath::getTransformation() const
{
   matrix4 r;
   auto it = mPath.begin();
   for(; it != mPath.end(); ++it)
   {
      const transformationNode *t = dynamic_cast<const transformationNode*>(*it);
      if( t ){ r *= t->getTransformation(); }
   }
   return r;
}
//------------------------------------------------------------------------------
//--- glNode
//------------------------------------------------------------------------------
unsigned int node::mId = 0;
node::node( QString iToken ) : 
mToken( iToken ), 
mpParent(0),
mIsVerbose(false),
mpLog(0)
{ mId++; }

node::~node()
{
   //destruction recursive des enfant
   for( int i = 0; i < getNumberOfChilds(); ++i )
   { delete getChild( i ); }
}
//------------------------------------------------------------------------------
void node::addChild( node* iC )
{ 
   iC->setParent( this );
   mChilds.push_back( iC );
}
//------------------------------------------------------------------------------
void node::begin()
{
   if( isVerbose() )
   { 
      QString m;
      m.sprintf( "node %s: begin()", getToken().toStdString().c_str() );
      log(m);
   }
}
//------------------------------------------------------------------------------
void node::end()
{
   if( isVerbose() )
   { 
      QString m;
      m.sprintf( "node %s: end()", getToken().toStdString().c_str() );
      log(m);
   }
}
//------------------------------------------------------------------------------
int node::getNumberOfChilds() const
{ return mChilds.size(); }
//------------------------------------------------------------------------------
int node::getNumberOfParents() const
{
   int r = 0;
   const node* n = this;
   while( n->getParent() != 0 ){ ++r; n = n->getParent(); }
   return r;
}
//------------------------------------------------------------------------------
node* node::getChild( int i ) const
{ return mChilds[i]; }
//------------------------------------------------------------------------------
node* node::getParent() const
{ return mpParent; }
//------------------------------------------------------------------------------
/*retourne le parent à l'index i en partant du père vers les génerations plus 
  anciennes*/
node* node::getParent( int i ) const
{
   node* r = 0;
   vector<node*> vp;
   const node* n = this;
   while( n->getParent() != 0 )
   { vp.push_back( n->getParent() ); n = n->getParent(); }

   if( i >= 0 && i < (int)vp.size() ){ r = vp[i]; }
   return r;
}
//------------------------------------------------------------------------------
QString node::getToken() const
{ return mToken; }
//------------------------------------------------------------------------------
bool node::hasParent() const
{ return mpParent != 0; }
//------------------------------------------------------------------------------
bool node::isVerbose() const
{ return mIsVerbose; }
//------------------------------------------------------------------------------
void node::log(QString iM)
{ if(mpLog){ (*mpLog)(iM); } }
//------------------------------------------------------------------------------
/*Enleve tous les enfants, les noeuds enfants ne seront pas deletés.
  Typiquement utilisé par sceneGraph pour reparenter les noeuds.*/
void node::removeAllChilds()
{ mChilds.clear(); }
//------------------------------------------------------------------------------
/*Enleve un enfant de la liste des enfants, le noeud enfant ne sera pas deleté.
  Typiquement utilisé par sceneGraph pour reparenter le noeud.*/
void node::removeChild(node* n)
{
   auto it = find( mChilds.begin(), mChilds.end(), n );
   if( it != mChilds.end() )
   { mChilds.erase( it ); }
}
//------------------------------------------------------------------------------
void node::setAsVerbose(bool iV)
{ mIsVerbose = iV; }
//------------------------------------------------------------------------------
void node::setLog(utilities::log* iL)
{ mpLog = iL; }
//------------------------------------------------------------------------------
void node::setParent( node* ipParent )
{ mpParent = ipParent; }
//------------------------------------------------------------------------------
QString node::toString() const
{ 
   QString r;
   toString( this, &r );
   return r;
}
//------------------------------------------------------------------------------
void node::toString( const node* ipNode, QString* s ) const
{ 
   for( int i = 0; i < ipNode->getNumberOfParents(); ++i )
   { *s += "--"; }

   *s += ipNode->getToken() + "\n";

   for( int i = 0; i < ipNode->getNumberOfChilds(); ++i )
   { toString( ipNode->getChild(i), s ); }
}

//------------------------------------------------------------------------------
//--- drawableNode
//------------------------------------------------------------------------------
drawableNode::drawableNode( QString iToken ) : node( iToken ),
   mColor( Qt::white ),
   mIsDrawingBoundingBox(false)
{}
drawableNode::~drawableNode()
{}
//------------------------------------------------------------------------------
void drawableNode::begin()
{
   node::begin();
   const QColor c = getColor();

   glEnable( GL_BLEND );
   glColor4ub( c.red(), c.green(), c.blue(), c.alpha() );

   //les boundingBox sont en coordonnées globales, on renverse donc la
   //transformation courante
   if( isDrawingBoundingBox() )
   {
      glPushMatrix();
      /*Si sceneGraph conservait la matrice courante au lieu d'utiliser, on
        pourrait remplacer le call a nodePath.*/
      nodePath np(this);
      glMultMatrixd( (GLdouble*) &(np.getTransformation().inverse()) );
      glDisable(GL_LIGHTING);
      glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
      const box bb = getBoundingBox();
      utilities::drawBox( bb.getMin(), bb.getMax() );
      glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
      glEnable(GL_LIGHTING);
      glPopMatrix();
   }
}
//------------------------------------------------------------------------------
void drawableNode::drawBoundingBox( bool iD )
{ mIsDrawingBoundingBox = iD; }
//------------------------------------------------------------------------------
void drawableNode::end()
{
   node::end();
   glDisable( GL_BLEND );
}
//------------------------------------------------------------------------------
double drawableNode::getAlpha() const
{ return mColor.alpha() / (double)255; }
//------------------------------------------------------------------------------
/*Retourne mBoundingBox, si iGlobal est vrai, alors on retourne un bounding box
  en coordonnées globales. Il s'agit d'une axisAligned bounding box qui englobe
  la boundingBox du noeud.*/
coreLibrary::box drawableNode::getBoundingBox(bool iGlobal /*=true*/) const
{
   box bb = mBoundingBox;
   if( iGlobal )
   {
      const nodePath np(this);
      const matrix4 m = np.getTransformation();

      //on applique la transforamtion aux point originaux afin de 
      //déterminer le boundingBox en coordonnées globales.
      const point3 p1 = mBoundingBox.getMin();
      const point3 p2 = mBoundingBox.getMax();
      const point3 a = m.transform(p1);
      const point3 b = m.transform(point3(p2.x(), p1.y(), p1.z()));
      const point3 c = m.transform(point3(p2.x(), p2.y(), p1.z()));
      const point3 d = m.transform(point3(p1.x(), p2.y(), p1.z()));
      const point3 e = m.transform(point3(p1.x(), p1.y(), p2.z()));
      const point3 f = m.transform(point3(p2.x(), p1.y(), p2.z()));
      const point3 g = m.transform(p2);
      const point3 h = m.transform(point3(p1.x(), p2.y(), p2.z()));

      coreLibrary::box bbGlobal;
      bbGlobal.add(a); bbGlobal.add(b); bbGlobal.add(c); bbGlobal.add(d);
      bbGlobal.add(e); bbGlobal.add(f); bbGlobal.add(g); bbGlobal.add(h);
      bb = bbGlobal;
   }
   return bb;
}
//------------------------------------------------------------------------------
QColor drawableNode::getColor() const
{ return mColor; }
//------------------------------------------------------------------------------
bool drawableNode::isDrawingBoundingBox() const
{ return mIsDrawingBoundingBox; }
//------------------------------------------------------------------------------
void drawableNode::setAlpha( double iA )
{ mColor.setAlpha( iA * 255 ); }
//------------------------------------------------------------------------------
void drawableNode::setColor( QColor iC )
{ mColor = iC; }

//------------------------------------------------------------------------------
//--- volumeNode
//------------------------------------------------------------------------------
volumeNode::volumeNode( QString iToken ) : drawableNode(iToken)
{}
//------------------------------------------------------------------------------
volumeNode::volumeNode( QString iToken, volume iV ) : 
  drawableNode(iToken),
  mIsInitialized(false)
{ setVolume(iV); }
//------------------------------------------------------------------------------
void volumeNode::begin()
{
   if(!isInitialized())
   { initialize(); }

   drawableNode::begin();

   vector3 numVox = mVolume.getSizeInVoxels();
   vector3 spacing = mVolume.getSpacing();

   const point3 p1 = point3::origin;
   const point3 p2 = point3( numVox.dx() * spacing.dx(),
      numVox.dy() * spacing.dy(),
      numVox.dz() * spacing.dz());
   const point3 a = p1;
   const point3 b(p2.x(), p1.y(), p1.z());
   const point3 c(p2.x(), p2.y(), p1.z());
   const point3 d(p1.x(), p2.y(), p1.z());
   const point3 e(p1.x(), p1.y(), p2.z());
   const point3 f(p2.x(), p1.y(), p2.z());
   const point3 g = p2;
   const point3 h(p1.x(), p2.y(), p2.z());

   if( !mSlice.isValid() )
   {
      glEnable(GL_TEXTURE_3D);
      glBindTexture(GL_TEXTURE_3D, mTexture.getId());
      glActiveTexture(GL_TEXTURE0);
      glBegin(GL_QUADS);
      //-z
      glNormal3d( 0.0, 0.0, -1 );
      glTexCoord3d(0.0, 0.0, 0.5);
      glVertex3dv((GLdouble*)&e);
      glTexCoord3d(1.0, 0.0, 0.5);
      glVertex3dv((GLdouble*)&h);
      glTexCoord3d(1.0, 1.0, 0.5);
      glVertex3dv((GLdouble*)&g);
      glTexCoord3d(0.0, 1.0, 0.5);
      glVertex3dv((GLdouble*)&f);

      //z
      glNormal3d( 0.0, 0.0, 1 );
      glTexCoord3d(0.0, 0.0, 0.5);
      glVertex3dv((GLdouble*)&a);
      glTexCoord3d(1.0, 0.0, 0.5);
      glVertex3dv((GLdouble*)&b);
      glTexCoord3d(1.0, 1.0, 0.5);
      glVertex3dv((GLdouble*)&c);
      glTexCoord3d(0.0, 1.0, 0.5);
      glVertex3dv((GLdouble*)&d);

      //-x
      glNormal3d( -1.0, 0.0, 0.0 );
      glTexCoord3d(0.5, 0.0, 0.0);
      glVertex3dv((GLdouble*)&a);
      glTexCoord3d(0.5, 0.0, 1.0);
      glVertex3dv((GLdouble*)&e);
      glTexCoord3d(0.5, 1.0, 1.0);
      glVertex3dv((GLdouble*)&h);
      glTexCoord3d(0.5, 1.0, 0.0);
      glVertex3dv((GLdouble*)&d);

      //x
      glNormal3d( 1.0, 0.0, 0.0 );
      glTexCoord3d(0.5, 0.0, 0.0);
      glVertex3dv((GLdouble*)&b);
      glTexCoord3d(0.5, 0.0, 1.0);
      glVertex3dv((GLdouble*)&f);
      glTexCoord3d(0.5, 1.0, 1.0);
      glVertex3dv((GLdouble*)&g);
      glTexCoord3d(0.5, 1.0, 0.0);
      glVertex3dv((GLdouble*)&c);

      //-y
      glNormal3d( 0.0, -1.0, 0.0 );
      glTexCoord3d(0.0, 0.5, 0.0);
      glVertex3dv((GLdouble*)&a);
      glTexCoord3d(0.0, 0.5, 1.0);
      glVertex3dv((GLdouble*)&e);
      glTexCoord3d(1.0, 0.5, 1.0);
      glVertex3dv((GLdouble*)&f);
      glTexCoord3d(1.0, 0.5, 0.0);
      glVertex3dv((GLdouble*)&b);

      //y
      glNormal3d( 0.0, 1.0, 0.0 );
      glTexCoord3d(0.0, 0.5, 0.0);
      glVertex3dv((GLdouble*)&d);
      glTexCoord3d(1.0, 0.5, 0.0);
      glVertex3dv((GLdouble*)&c);
      glTexCoord3d(1.0, 0.5, 1.0);
      glVertex3dv((GLdouble*)&g);
      glTexCoord3d(0.0, 0.5, 1.0);
      glVertex3dv((GLdouble*)&h);
      glEnd();
      glActiveTexture(GL_TEXTURE0);
      glDisable(GL_TEXTURE_3D);
   }
   else
   {
      //On normalize les points du polygon avec les coins du volume
      //afin d'obtenir les coordonnées de texture3d
      vector<point3> texCoords;
      for(int i = 0; i < mSlice.getNumberOfVertices(); ++i )
      {
         point3 p = mSlice.getVertex(i);
         p.set( (p.x() - p1.x()) / (p2.x() - p1.x()),
            (p.y() - p1.y()) / (p2.y() - p1.y()),
            (p.z() - p1.z()) / (p2.z() - p1.z()) );
         texCoords.push_back(p);
      }

      glDisable(GL_LIGHTING);
      glEnable(GL_TEXTURE_3D);
      glBindTexture(GL_TEXTURE_3D, mTexture.getId());
      glActiveTexture(GL_TEXTURE0);
      glBegin(GL_POLYGON);
         glNormal3dv( (GLdouble*)&(mSlice.getNormal()) );
         for(int i = 0; i < mSlice.getNumberOfVertices(); ++i)
         { 
            glTexCoord3dv((GLdouble*)&(texCoords[i]));
            glVertex3dv((GLdouble*)&(mSlice.getVertex(i)));
         }
      glEnd();
      glActiveTexture(GL_TEXTURE0);
      glDisable(GL_TEXTURE_3D);
      glEnable(GL_LIGHTING);
      /*glDisable(GL_LIGHTING);
      glColor3ub(255, 0, 0);
      utilities::drawPolygon( mSlice );
      glEnable(GL_LIGHTING);*/
   }
}
//------------------------------------------------------------------------------
volume volumeNode::getVolume() const
{ return mVolume; }
//------------------------------------------------------------------------------
void volumeNode::initialize()
{
   std::vector<int> s(3, 0);
   s[0]=mVolume.getSizeInVoxels().dx();
   s[1]=mVolume.getSizeInVoxels().dy();
   s[2]=mVolume.getSizeInVoxels().dz();
   switch ( mVolume.getBitDepth() )
   {
   case 8:
      mTexture.set( QGLContext::currentContext(), (void*)mVolume.getVoxels8(), s,
         GL_LUMINANCE, GL_UNSIGNED_BYTE, GL_LUMINANCE);
      break;
   case 16: 
      mTexture.set( QGLContext::currentContext(), (void*)mVolume.getVoxels16(), s,
         GL_LUMINANCE, GL_UNSIGNED_SHORT, GL_LUMINANCE);
      break;
   default: break;
   }

   //--- init du bounding box
   const vector3 numVox = mVolume.getSizeInVoxels();
   const vector3 spacing = mVolume.getSpacing();
   const point3 p1 = point3::origin;
   const point3 p2 = point3(numVox.dx() * spacing.dx(),
      numVox.dy() * spacing.dy(),
      numVox.dz() * spacing.dz() );
   const point3 a = p1;
   const point3 b = point3(p2.x(), p1.y(), p1.z());
   const point3 c = point3(p2.x(), p2.y(), p1.z());
   const point3 d = point3(p1.x(), p2.y(), p1.z());
   const point3 e = point3(p1.x(), p1.y(), p2.z());
   const point3 f = point3(p2.x(), p1.y(), p2.z());
   const point3 g = p2;
   const point3 h = point3(p1.x(), p2.y(), p2.z());

   mBoundingBox.add(a); mBoundingBox.add(b); mBoundingBox.add(c); mBoundingBox.add(d);
   mBoundingBox.add(e); mBoundingBox.add(f); mBoundingBox.add(g); mBoundingBox.add(h);

   mIsInitialized = true;
}
//------------------------------------------------------------------------------
void volumeNode::setVolume(volume iV)
{ 
   mVolume = iV;
   mIsInitialized = false;   
}
//------------------------------------------------------------------------------
void volumeNode::slice(const plane& p)
{ 
   mSlice = polygon();

   nodePath np(this);
   matrix4 m = np.getTransformation();
   matrix4 mInv = m.inverse();

   const vector3 numVox = mVolume.getSizeInVoxels();
   const vector3 spacing = mVolume.getSpacing();
   const box bb( point3::origin,
      point3(numVox.dx() * spacing.dx(),
      numVox.dy() * spacing.dy(),
      numVox.dz() * spacing.dz() ) );

   /*on fait l'intersection en coordonné global, donc on applique la transfo
     inverse au plan.*/
   plane planeInv( mInv.transform(p.getPoint()), mInv.rotateAndScale(p.getNormal()) );
   intersection x = intersect( planeInv, bb );
   if( x.hasIntersections() )
   { mSlice = x.getPolygon(0); }
}
//------------------------------------------------------------------------------
//--- meshNode
//------------------------------------------------------------------------------
meshNode::meshNode(QString iToken) : drawableNode(iToken),
   mDisplayList(0),
   mIsInitialized(false)
{}
//------------------------------------------------------------------------------
meshNode::meshNode(QString iToken, mesh2* ipMesh) : drawableNode(iToken),
   mDisplayList(0),
   mIsInitialized(false)
{ setMesh(ipMesh); }
//------------------------------------------------------------------------------
void meshNode::begin()
{ 
   drawableNode::begin();

   glEnable(GL_LIGHTING);
   glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
   glCallList(mDisplayList);
   
   //on dessine la slice
   glDisable(GL_LIGHTING);
   glEnable(GL_POLYGON_OFFSET_LINE);
   glLineWidth(2);
   glPolygonOffset(1.0, 4.0);
   glColor3ub(255, 0 , 0);
   glBegin(GL_LINES);
   for( size_t i = 0; i < mSlice.size(); ++i )
   {
      glVertex3dv( (GLdouble*)&(mSlice[i].getFirstPoint()) );
      glVertex3dv( (GLdouble*)&(mSlice[i].getSecondPoint()) );
   }
   glEnd();
   glLineWidth(1.0);
   glDisable(GL_POLYGON_OFFSET_LINE);
   glEnable(GL_LIGHTING);   
}
//------------------------------------------------------------------------------
mesh2* meshNode::getMesh() const
{ return mpMesh; }
//------------------------------------------------------------------------------
void meshNode::initialize()
{
   if(!mpMesh){return;}

   //create the list
   mDisplayList = glGenLists(1);
   
   //content of the list
   // compile the display list, store a triangle in it
   glNewList(mDisplayList, GL_COMPILE);

   glColor3ub(120, 120, 120);
   for(int i = 0; i < mpMesh->getNumberOfFaces(); ++i )
   {
      glBegin(GL_POLYGON);
         glNormal3dv( (GLdouble*)&(mpMesh->getPolygon(i).getNormal()) );
         for( int j = 0; j < mpMesh->getNumberOfVerticesOnFace(i); j++ )
         { glVertex3dv( (GLdouble*)&(mpMesh->getPolygon(i).getVertex(j)) ); }
      glEnd();           
   }

   ////draw normals
   //glColor3ub(255, 0, 0);
   //for(int i = 0; i < mpMesh->getNumberOfFaces(); ++i )
   //{
   //   glBegin(GL_LINES);
   //   glVertex3dv( (GLdouble*)&(mpMesh->getPolygon(i).getCenterOfMass()) );
   //   glVertex3dv( (GLdouble*)&(mpMesh->getPolygon(i).getCenterOfMass()+2*mpMesh->getPolygon(i).getNormal()) );
   //   glEnd();

   //   glPointSize( 5 );
   //   glBegin(GL_POINTS);
   //   glVertex3dv( (GLdouble*)&(mpMesh->getPolygon(i).getCenterOfMass()) );
   //   glEnd();
   //   glPointSize( 1 );
   //}

   glEndList();   

   //--- init du bounding box
   for(int i = 0; i < mpMesh->getNumberOfVertices(); ++i )
   { mBoundingBox.add( mpMesh->getVertex(i) ); }
}
//------------------------------------------------------------------------------
void meshNode::setMesh(mesh2* ipMesh)
{
   if( isInitialized() )
   {
      glDeleteLists( mDisplayList, 1);
      mDisplayList = 0;
   }
   mpMesh = ipMesh;
   initialize();
}
//------------------------------------------------------------------------------
void meshNode::slice(const plane& p)
{
   nodePath np(this);
   matrix4 m = np.getTransformation();
   const matrix4 mInv = m.inverse();
   mSlice.clear();
   /*on fait l'intersection en coordonné global, donc on applique la transfo
     inverse au plan.*/   
   if( intersects( getBoundingBox(), p ) )
   {
      plane planeInv( mInv.transform(p.getPoint()), mInv.rotateAndScale(p.getNormal()) );
      intersection x = intersect( *getMesh(), planeInv );
      if( x.hasIntersections() )
      {
         for( int i = 0; i < x.getNumberOfLineSegments(); ++i )
         { mSlice.push_back(x.getLineSegment(i)); }
      }
   }
}
//------------------------------------------------------------------------------
//--- transformationNode
//------------------------------------------------------------------------------
transformationNode::transformationNode( QString iToken ) : node( iToken )
{}
//------------------------------------------------------------------------------
transformationNode::transformationNode( QString iToken, matrix4 iM ) : 
   node( iToken ),
   mTransform( iM )
{}
//------------------------------------------------------------------------------
transformationNode::~transformationNode()
{
   //destructors are called automatically in the reverse order of construction. 
   //(Base classes last).
}
//------------------------------------------------------------------------------
void transformationNode::begin()
{  glPushMatrix(); glMultMatrixd( (GLdouble*)&mTransform ); }
//------------------------------------------------------------------------------
void transformationNode::end()
{ glPopMatrix(); }
//------------------------------------------------------------------------------
matrix4 transformationNode::getTransformation() const
{ return mTransform; }
//------------------------------------------------------------------------------
void transformationNode::setTransformation( matrix4 iT )
{ mTransform = iT; }
//------------------------------------------------------------------------------
//--- cameraNode
//------------------------------------------------------------------------------
cameraNode::cameraNode( QString iToken ) : node(iToken)
{}
//------------------------------------------------------------------------------
cameraNode::cameraNode(QString iToken, glCamera iCam) : 
   node(iToken),
   mCamera(iCam)
{}
//------------------------------------------------------------------------------
void cameraNode::begin()
{
   glMatrixMode( GL_PROJECTION );
   glPushMatrix();
   glMatrixMode( GL_MODELVIEW );
   glPushMatrix();
   mCamera.applyModelViewProjection();
}
//------------------------------------------------------------------------------
void cameraNode::end()
{
   glMatrixMode( GL_PROJECTION );
   glPopMatrix();
   glMatrixMode( GL_MODELVIEW );
   glPopMatrix();
}
//------------------------------------------------------------------------------
glCamera cameraNode::getCamera() const
{ return mCamera; }
//------------------------------------------------------------------------------
void cameraNode::setCamera(glCamera iCam)
{ mCamera = iCam; }
//------------------------------------------------------------------------------
//--- glSceneGraph
//------------------------------------------------------------------------------
const QString kRootToken = "root";
sceneGraph::sceneGraph() : 
   mpRoot( new node( kRootToken ) ),
   mIsVerbose( false ),
   mAccumulateStatistics(true)
{
   mLog.logToConsole(true);
   mStats.mFrameTimer.start();
}

sceneGraph::~sceneGraph()
{ delete mpRoot; }
//------------------------------------------------------------------------------
void sceneGraph::addNode( node* ipNode )
{ addNode( ipNode, kRootToken ); }
//------------------------------------------------------------------------------
/*Ajoute le noeud ipNode au noeud parent iParentToken. Si le parent n'existe
  pas, le noeud sera ajouté à la racine afin de ne pas fuire de mémoire.*/
void sceneGraph::addNode( node* ipNode, QString iParentToken )
{
   if( !findNode( ipNode->getToken() ) )
   {    
      ipNode->setLog( &mLog );
      //ipNode->setAsVerbose( isVerbose() );
      node* parent = findNode( iParentToken );
      if( parent )
      { parent->addChild( ipNode ); }
      else { addNode( ipNode ); }
   }
   else
   { 
      QString s = QString().sprintf( "The node %s cannot be added because it is "
         "already present in the sceneGraph.", ipNode->getToken().toStdString().
          c_str() );
      mLog( s );
   }

   if(isVerbose())
   { getLog()( toString() ); }
}
//------------------------------------------------------------------------------
void sceneGraph::clear()
{ 
   delete mpRoot;
   mpRoot = new node(kRootToken);
}
//------------------------------------------------------------------------------
node* sceneGraph::findNode( QString iToken )
{ return findNode( mpRoot, iToken ); }
//------------------------------------------------------------------------------
node* sceneGraph::findNode( node* iCurrentNode, QString iToken )
{
   node* r = 0;
   if( iCurrentNode->getToken() == iToken )
   { r = iCurrentNode; }
   else
   {
      for( int i = 0; i < iCurrentNode->getNumberOfChilds(); ++i )
      {
         r = findNode( iCurrentNode->getChild( i ), iToken );
         if( r ) break;
      }
   }
   return r;
}
//------------------------------------------------------------------------------
utilities::log& sceneGraph::getLog() const
{ return mLog; }
//------------------------------------------------------------------------------
bool sceneGraph::hasNode( QString iToken )
{ return findNode( iToken ) != 0; }
//------------------------------------------------------------------------------
bool sceneGraph::isVerbose() const
{ return mIsVerbose; }
//------------------------------------------------------------------------------
/*Enleve et delete le noeud iToken de l'arbre, les noeuds enfants sont
reparentés au parent du noeud iToken. Il n'est pas possible d'enlever le noeud
racine - kRootToken
*/
void sceneGraph::removeNode( QString iToken )
{
   if( iToken != kRootToken )
   {
      node* n = findNode( iToken );
      if(n)
      {
         node* p = n->getParent(); //garanti d'avoir un parent
         //on reparente les enfants de n
         for( int i = 0; i < n->getNumberOfChilds(); ++i )
         { p->addChild(n->getChild(i)); }

         n->removeAllChilds();
         p->removeChild(n);
         delete n;
      }
   }
}
//------------------------------------------------------------------------------
void sceneGraph::render()
{ 
   render( mpRoot );

   if( mAccumulateStatistics )
   {
      mStats.mFrameCount++;
      if(mStats.mFrameTimer.elapsed() > 5000 )
      {
         mStats.mFps = mStats.mFrameCount / (double)mStats.mFrameTimer.elapsed() *1000.0;
         mStats.mFrameTimer.start();
         mStats.mFrameCount = 0;

         mLog( mStats.toQString() );
      }
   }
}
//------------------------------------------------------------------------------
void sceneGraph::render( node* ipCurrentNode )
{
   ipCurrentNode->begin();
   for( int i = 0; i < ipCurrentNode->getNumberOfChilds(); ++i )
   { render( ipCurrentNode->getChild(i) ); }
   ipCurrentNode->end();
}
//------------------------------------------------------------------------------
void sceneGraph::setAsVerbose(bool iV)
{ 
   mIsVerbose = iV;
   setAsVerbose( mpRoot, iV );
}
//------------------------------------------------------------------------------
void sceneGraph::setAsVerbose(node* ipNode, bool iV)
{
   ipNode->setAsVerbose(iV);
   for( int i = 0; i < ipNode->getNumberOfChilds(); ++i )
   { setAsVerbose( ipNode->getChild(i), iV ); }
}
//------------------------------------------------------------------------------
void sceneGraph::slice(const plane& p)
{ slice(mpRoot, p); }
//------------------------------------------------------------------------------
void sceneGraph::slice(node* n, const plane& p)
{
   drawableNode* dn = dynamic_cast<drawableNode*>(n);
   if(dn){ dn->slice(p); }
   for(int i = 0; i < n->getNumberOfChilds(); ++i)
   { slice(n->getChild(i), p); }
}
//------------------------------------------------------------------------------
QString sceneGraph::toString() const
{ return mpRoot->toString(); }
//------------------------------------------------------------------------------
//--- scenegraph::statistics
//------------------------------------------------------------------------------
sceneGraph::statistics::statistics() : 
 mFps(0.0), mFrameCount(0), mFrameTimer()
{}
//------------------------------------------------------------------------------
QString sceneGraph::statistics::toQString() const
{
   QString s;
   s.sprintf("average fps (per 5s): %.2f", mFps);
   return s;
}