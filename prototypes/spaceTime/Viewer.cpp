
#include "3d/Utilities.h"
#include "math/MathUtils.h"
#include "math/PlatonicSolid.h"
#include <QFile>
#include <QKeyEvent>
#include <QMouseEvent>
#include "Viewer.h"

using namespace SpaceTime;
using namespace realisim;
  using namespace treeD;
  using namespace math;

namespace  
{
  double kFov = 60.0;
  double kNear(0.005);
  double kFar(500000);
}
//-----------------------------------------------------------------------------
//--- Viewer
//-----------------------------------------------------------------------------
Viewer::Viewer(QWidget* ipParent /*=0*/, Engine& iEngine) : Widget3d(ipParent),
  Engine::Client(iEngine),
  mShipCamera(),
  mSphere(0),
  mCube(0),
  mAreaToRenderRadii(),
  mCameraFollowsShip(true),
  mIsDebugging(false),
  mIsThirdPersonView(true),
  mCubeMapFbo(),
  mThresholdToRenderCubeMap(5.0),
  mLastCubeMapRenderPosition(std::numeric_limits<double>::max()),
  mSmoothSphere()
{
  setFocusPolicy(Qt::StrongFocus);
  setMouseTracking(true);
  
  /*on pousse des valeurs par defaut pour les rayons des
    cubeMaps*/
  mAreaToRenderRadii.push_back(0.0);
  mAreaToRenderRadii.push_back(50.0);
  mAreaToRenderRadii.push_back(500000.0);
}

Viewer::~Viewer()
{}

//-----------------------------------------------------------------------------
void Viewer::call(Client::message iM)
{
	switch (iM)
  {
    case Client::mFrameReady:
    	if(getEngine().getState() == Engine::sSimulating)
      	invalidateCubeMapRender();
      centerMouse();
      update();
      break;
    case Client::mPaused: setMouseTracking(false); break;
    case Client::mPlaying: centerMouse(); setMouseTracking(true); break;
    case Client::mStateChanged: update(); break;
    default: break;
  }
}

//-----------------------------------------------------------------------------
void Viewer::centerMouse()
{
  QPoint p(width() / 2, height() / 2);
  getEngine().ignoreMouseMove(p);
  QCursor::setPos(mapToGlobal(p));
}

//-----------------------------------------------------------------------------
void Viewer::draw()
{
	drawBodies(getAreaToRenderInnerRadius(0),
    getAreaToRenderOuterRadius(0), lodHigh);
	drawDistantBodies(getAreaToRenderInnerRadius(1),
    getAreaToRenderOuterRadius(1));
  //drawDistantBodies(kOuter2, kOuter3);
  
  /*On dessine le Ship*/
  if(mIsThirdPersonView)
  {
    glColor3ub(200, 125, 12);
    glPushMatrix();
    glEnable(GL_RESCALE_NORMAL);
    glMultMatrixd(getEngine().getShip().getTransformation().getPtr());
    glScaled(0.005, 0.005, 0.005);
    glCallList(mCube);
    glDisable(GL_RESCALE_NORMAL);
    glPopMatrix();
  }
  
  /*on dessine un cube de reference*/
  
  glPushMatrix();
  glTranslated(0, 0, -5);
  glCallList(mCube);
  glPopMatrix();

	if(isDebugging())
  {
  	Point3d shipPos = getEngine().getShip().getTransformation().getTranslation();
  	Matrix4d m;
    m.setTranslation(shipPos);
    m = rotate(m, PI_SUR_2, Vector3d(0.0, 1.0, 0.0), shipPos);
    //X
  	glColor3ub(255, 0, 0);
    glPushMatrix();
    glMultMatrixd(m.getPtr());
    drawArrow();
    glPopMatrix();
    
    //Y
    m = rotate(m, PI_SUR_2, Vector3d(0.0, 0.0, 1.0), shipPos);
  	glColor3ub(0, 255, 0);
    glPushMatrix();
    glMultMatrixd(m.getPtr());
    drawArrow();
    glPopMatrix();
    
    //Z
    m = rotate(m, PI_SUR_2, Vector3d(1.0, 0.0, 0.0), shipPos);
  	glColor3ub(0, 0, 255);
    glPushMatrix();
    glMultMatrixd(m.getPtr());
    drawArrow();
    glPopMatrix();
    
    showFps();
  }
};

//-----------------------------------------------------------------------------
//dessine une fleche sur l'axe Z
void Viewer::drawArrow()
{
  glBegin(GL_LINES);
  glVertex3d(0.0, 0.0, 0.0);
  glVertex3d(0.0, 0.0, 2.0);
  
  glVertex3d(0.0, 0.0, 2.0);
  glVertex3d(-0.5, 0.0, 1.5);
      
  glVertex3d(0.0, 0.0, 2.0);    
  glVertex3d(0.5, 0.0, 1.5);
  glEnd();
}

//-----------------------------------------------------------------------------
void Viewer::drawBodies(double iInnerRadius, double iOuterRadius,
  levelOfDetail iLod)
{
	unsigned int i, j;
  Point3d p = getEngine().getShip().getTransformation().getTranslation();
  const vector<AstronomicalBody*>& vab = 
  	getEngine().getAstronomicalBodies(p, iInnerRadius, iOuterRadius);
    
  switch(iLod)
  {
  case lodHigh: break;
  case lodMed: glDisable(GL_LIGHTING); break;
  case lodLow: glDisable(GL_LIGHTING); break;
  default:break;
  } 
    
  for(i = 0; i < vab.size(); ++i)
  {  
  glColor3ub(255, 255, 255);
  switch (vab[i]->getType()) 
    {
      case AstronomicalBody::tBlackHole: glColor3ub(0, 255, 5); break;
      case AstronomicalBody::tComet: glColor3ub(234, 139, 30); break;
      case AstronomicalBody::tMoon: glColor3ub(237, 237, 237); break;
      case AstronomicalBody::tPlanet: glColor3ub(111, 178, 230); break;
      case AstronomicalBody::tStar: glColor3ub(250, 255, 99); break;
      default: break;
    }
    
    if(isDebugging())
    {
      //on dessine le vecteur de force
    	Vector3d f = vab[i]->getForce();
      f.normalise();
      glBegin(GL_LINES);
      glVertex3dv(vab[i]->getTransformation().getTranslation().getPtr());
      glVertex3d(vab[i]->getTransformation().getTranslation().getX() + 2 * f.getX(),
        vab[i]->getTransformation().getTranslation().getY() + 2 * f.getY(),
        vab[i]->getTransformation().getTranslation().getZ() + 2 * f.getZ() );
      glEnd();
      
      //On dessine le path 
      const deque<Point3d>& path = vab[i]->getPath();
      glBegin(GL_LINES);
      for(j = 0; path.size() >= 2 && j < path.size() - 1; ++j)
      {
      	glVertex3dv(path[j].getPtr());
        glVertex3dv(path[j + 1].getPtr());
      }
      glEnd();
      
//      //on dessine les liens des corps engagés
//      set<const AstronomicalBody*>::iterator engagedIt = 
//      	vab[i]->getEngagedBodies().begin();
//      for(; engagedIt != vab[i]->getEngagedBodies().end(); ++engagedIt)
//      {
//      	glBegin(GL_LINES);
//        	glVertex3dv((*engagedIt)->getTransformation().getTranslation().getPtr());
//          glVertex3dv(vab[i]->getTransformation().getTranslation().getPtr());
//        glEnd();
//      }
    }
    
    switch(iLod)
    {
    case lodHigh:
    	pushShader(mSmoothSphere);
      
      glPushMatrix();
      glMultMatrixd(vab[i]->getTransformation().getPtr());
      glScaled(vab[i]->getRadius(), vab[i]->getRadius(), vab[i]->getRadius());
      glCallList(mSphere);
      glPopMatrix();
      
      popShader();
    break;
    case lodMed:
     // glPointSize(1);
//      glBegin(GL_POINTS);
//        glVertex3d(vab[i]->getTransformation().getTranslation().getX(), vab[i]->getTransformation().getTranslation().getY(),
//          vab[i]->getTransformation().getTranslation().getZ());
//      glEnd();
      glPushMatrix();
      glMultMatrixd(vab[i]->getTransformation().getPtr());
      glScaled(vab[i]->getRadius(), vab[i]->getRadius(), vab[i]->getRadius());
      glCallList(mSphere);
      glPopMatrix();
    break;
    case lodLow:
      glPointSize(1);
      glBegin(GL_POINTS);
        glVertex3d(vab[i]->getTransformation().getTranslation().getX(), vab[i]->getTransformation().getTranslation().getY(),
          vab[i]->getTransformation().getTranslation().getZ());
      glEnd();
    break;
    default:break;
    } 
  } 
  glEnable(GL_LIGHTING);
}

//-----------------------------------------------------------------------------
void Viewer::drawCubeMapFace(cubeMapSide iSide, const Camera& iC,
  double iInnerRadius, double iOuterRadius)
{
  mCubeMapFbo.resize(iC.getWindowInfo().getWidth(), iC.getWindowInfo().getHeight());
  glPushMatrix();    
    glLoadIdentity();
    iC.applyModelViewTransformation();
    setCamera(iC, false);
    
    pushFrameBuffer(mCubeMapFbo);
      mCubeMapFbo.drawTo(iSide);  
      glClearColor(0.0, 0.0, 0.0, 0.0);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      drawBodies(iInnerRadius, iOuterRadius, lodMed);
    popFrameBuffer();
  glPopMatrix();
}

//-----------------------------------------------------------------------------
void Viewer::drawDebuggingBox(const BB3d& iBb)
{
  Point3d minCorner = iBb.getMin();
	Point3d maxCorner = iBb.getMax();
  glDisable(GL_LIGHTING);
  glColor3ub(255, 255, 255);
  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  glBegin(GL_QUADS);
  
  //cote X
  glVertex3d(maxCorner.getX(), maxCorner.getY(), maxCorner.getZ());
  glVertex3d(maxCorner.getX(), minCorner.getY(), maxCorner.getZ());
  glVertex3d(maxCorner.getX(), minCorner.getY(), minCorner.getZ());
  glVertex3d(maxCorner.getX(), maxCorner.getY(), minCorner.getZ());    
  
  //cote -X
  glVertex3d(minCorner.getX(), minCorner.getY(), minCorner.getZ());
  glVertex3d(minCorner.getX(), minCorner.getY(), maxCorner.getZ());
  glVertex3d(minCorner.getX(), maxCorner.getY(), maxCorner.getZ());
  glVertex3d(minCorner.getX(), maxCorner.getY(), minCorner.getZ());
  
  //cote -Z
  glVertex3d(minCorner.getX(), minCorner.getY(), minCorner.getZ());
  glVertex3d(minCorner.getX(), maxCorner.getY(), minCorner.getZ());
  glVertex3d(maxCorner.getX(), maxCorner.getY(), minCorner.getZ());
  glVertex3d(maxCorner.getX(), minCorner.getY(), minCorner.getZ());    
  
  //cote Z
  glVertex3d(maxCorner.getX(), maxCorner.getY(), maxCorner.getZ());
  glVertex3d(minCorner.getX(), maxCorner.getY(), maxCorner.getZ());
  glVertex3d(minCorner.getX(), minCorner.getY(), maxCorner.getZ());
  glVertex3d(maxCorner.getX(), minCorner.getY(), maxCorner.getZ());
  
  //cote Y
  glVertex3d(maxCorner.getX(), maxCorner.getY(), maxCorner.getZ());
  glVertex3d(maxCorner.getX(), maxCorner.getY(), minCorner.getZ());
  glVertex3d(minCorner.getX(), maxCorner.getY(), minCorner.getZ());
  glVertex3d(minCorner.getX(), maxCorner.getY(), maxCorner.getZ());
  
  //cote -Y
  glVertex3d(minCorner.getX(), minCorner.getY(), minCorner.getZ());
  glVertex3d(maxCorner.getX(), minCorner.getY(), minCorner.getZ());
  glVertex3d(maxCorner.getX(), minCorner.getY(), maxCorner.getZ());
  glVertex3d(minCorner.getX(), minCorner.getY(), maxCorner.getZ());
  
  glEnd();
  glPolygonMode(GL_FRONT, GL_FILL);
  glEnable(GL_LIGHTING);
}

//-----------------------------------------------------------------------------
void Viewer::drawDistantBodies(double iInnerRadius, double iOuterRadius)
{
	//On fait un Bounding box autour de la camera afin de projeter sur
  //les faces et faire le cube map.
	BB3d bb;  
  Point3d cameraPos = mShipCamera.getPos() * mShipCamera.getTransformationToGlobal();
  bb.add(cameraPos - Point3d(iInnerRadius));
  bb.add(cameraPos + Point3d(iInnerRadius));
  Point3d minCorner = bb.getMin();
  Point3d maxCorner = bb.getMax();
  
	if(isDebugging())
  {
    //on dessine le cadre
  	drawDebuggingBox(bb);
    //draw debugging bodies
    //drawBodies(iInnerRadius, iOuterRadius, lodMed);    
  }

  if(mCubeMapFbo.isValid() &&
     cameraPos.fastDist(mLastCubeMapRenderPosition) > getThresholdToRenderCubeMap())
  {
    mLastCubeMapRenderPosition = cameraPos;
    //On sauvegarde la camera actuelle
    Camera savedCam = getCamera();
    Camera c = getCubeMapCamera(cmsNegZ, bb);
    drawCubeMapFace(cmsNegZ, c, iInnerRadius, iOuterRadius);
    c = getCubeMapCamera(cmsPosZ, bb);
    drawCubeMapFace(cmsPosZ, c, iInnerRadius, iOuterRadius);
    c = getCubeMapCamera(cmsPosX, bb);
    drawCubeMapFace(cmsPosX, c, iInnerRadius, iOuterRadius);
    c = getCubeMapCamera(cmsNegX, bb);
    drawCubeMapFace(cmsNegX, c, iInnerRadius, iOuterRadius);
    c = getCubeMapCamera(cmsPosY, bb);
    drawCubeMapFace(cmsPosY, c, iInnerRadius, iOuterRadius);
    c = getCubeMapCamera(cmsNegY, bb);
    drawCubeMapFace(cmsNegY, c, iInnerRadius, iOuterRadius);
    //on restaure la camera precedente
    setCamera(savedCam, false);    
  }
  
  //Ondessine le ciel étoilé
  glColor3ub(255, 255, 255);
  glDisable(GL_LIGHTING);
  glEnable(GL_TEXTURE_2D);    
  glPolygonMode(GL_BACK, GL_FILL);
  Texture t = mCubeMapFbo.getTexture(cmsNegZ);
  glBindTexture(GL_TEXTURE_2D, t.getTextureId());    
  //cote -Z
  glBegin(GL_QUADS);
  glTexCoord2d(0.0, 0.0);
  glVertex3d(minCorner.getX(), minCorner.getY(), minCorner.getZ());
  glTexCoord2d(0.0, 1.0);
  glVertex3d(minCorner.getX(), maxCorner.getY(), minCorner.getZ());
  glTexCoord2d(1.0, 1.0);
  glVertex3d(maxCorner.getX(), maxCorner.getY(), minCorner.getZ());
  glTexCoord2d(1.0, 0.0);
  glVertex3d(maxCorner.getX(), minCorner.getY(), minCorner.getZ());
  glEnd();
  
  t = mCubeMapFbo.getTexture(cmsPosZ);
  glBindTexture(GL_TEXTURE_2D, t.getTextureId());    
  //cote Z
  glBegin(GL_QUADS);
  glTexCoord2d(0.0, 1.0);
  glVertex3d(maxCorner.getX(), maxCorner.getY(), maxCorner.getZ());
  glTexCoord2d(1.0, 1.0);
  glVertex3d(minCorner.getX(), maxCorner.getY(), maxCorner.getZ());
  glTexCoord2d(1.0, 0.0);
  glVertex3d(minCorner.getX(), minCorner.getY(), maxCorner.getZ());
  glTexCoord2d(0.0, 0.0);
  glVertex3d(maxCorner.getX(), minCorner.getY(), maxCorner.getZ());
  glEnd();
  
  t = mCubeMapFbo.getTexture(cmsPosX);
  glBindTexture(GL_TEXTURE_2D, t.getTextureId());    
  //cote X
  glBegin(GL_QUADS);
  glTexCoord2d(1.0, 1.0);
  glVertex3d(maxCorner.getX(), maxCorner.getY(), maxCorner.getZ());
  glTexCoord2d(1.0, 0.0);
  glVertex3d(maxCorner.getX(), minCorner.getY(), maxCorner.getZ());
  glTexCoord2d(0.0, 0.0);
  glVertex3d(maxCorner.getX(), minCorner.getY(), minCorner.getZ());
  glTexCoord2d(0.0, 1.0);
  glVertex3d(maxCorner.getX(), maxCorner.getY(), minCorner.getZ());    
  glEnd();
  
  t = mCubeMapFbo.getTexture(cmsNegX);
  glBindTexture(GL_TEXTURE_2D, t.getTextureId());    
  //cote -X
  glBegin(GL_QUADS);
  glTexCoord2d(1.0, 0.0);
  glVertex3d(minCorner.getX(), minCorner.getY(), minCorner.getZ());
  glTexCoord2d(0.0, 0.0);
  glVertex3d(minCorner.getX(), minCorner.getY(), maxCorner.getZ());
  glTexCoord2d(0.0, 1.0);
  glVertex3d(minCorner.getX(), maxCorner.getY(), maxCorner.getZ());
  glTexCoord2d(1.0, 1.0);
  glVertex3d(minCorner.getX(), maxCorner.getY(), minCorner.getZ());
  glEnd();
  
  t = mCubeMapFbo.getTexture(cmsPosY);
  glBindTexture(GL_TEXTURE_2D, t.getTextureId());    
  //cote Y
  glBegin(GL_QUADS);
  glTexCoord2d(1.0, 1.0);
  glVertex3d(maxCorner.getX(), maxCorner.getY(), maxCorner.getZ());
  glTexCoord2d(1.0, 0.0);
  glVertex3d(maxCorner.getX(), maxCorner.getY(), minCorner.getZ());
  glTexCoord2d(0.0, 0.0);
  glVertex3d(minCorner.getX(), maxCorner.getY(), minCorner.getZ());
  glTexCoord2d(0.0, 1.0);
  glVertex3d(minCorner.getX(), maxCorner.getY(), maxCorner.getZ());
  glEnd();
  
  t = mCubeMapFbo.getTexture(cmsNegY);
  glBindTexture(GL_TEXTURE_2D, t.getTextureId());    
  //cote -Y
  glBegin(GL_QUADS);
  glTexCoord2d(0.0, 1.0);
  glVertex3d(minCorner.getX(), minCorner.getY(), minCorner.getZ());
  glTexCoord2d(1.0, 1.0);
  glVertex3d(maxCorner.getX(), minCorner.getY(), minCorner.getZ());
  glTexCoord2d(1.0, 0.0);
  glVertex3d(maxCorner.getX(), minCorner.getY(), maxCorner.getZ());
  glTexCoord2d(0.0, 0.0);
  glVertex3d(minCorner.getX(), minCorner.getY(), maxCorner.getZ());
  glEnd();
  
  glPolygonMode(GL_FRONT, GL_FILL);
  glBindTexture(GL_TEXTURE_2D, 0);
  glDisable(GL_TEXTURE_2D);
  glEnable(GL_LIGHTING);

//    Sprite s;
//    s.setTexture(t);
//    s.setAnchorPoint(Sprite::aTopLeft);
//    s.set2dPositioningOn(true);
//    s.set2dPosition(10, 10, 0);
//    s.draw(getCamera());
}

//-----------------------------------------------------------------------------
void Viewer::drawSceneForPicking() const
{
	glDisable(GL_MULTISAMPLE);
};

//-----------------------------------------------------------------------------
Camera Viewer::getCubeMapCamera(cubeMapSide iSide, const BB3d& iBb)
{
  //Point3d shipPos =  getCamera().getPos() * 
    //getCamera().getTransformationToGlobal();
    //getEngine().getShip().getTransformation().getTranslation();   
  Point3d minCorner = iBb.getMin();
  Point3d maxCorner = iBb.getMax();
  Vector3d bbSide = Vector3d(maxCorner.getY(), maxCorner.getZ(), 0.0) -
    Vector3d(minCorner.getY(), maxCorner.getZ(), 0.0);
  double halfBbSideLength = bbSide.norm() / 2.0;  
  Point3d middle = maxCorner - Point3d(halfBbSideLength);


	Camera c = mShipCamera;
  c.set(Point3d(0.0, 0.0, 0.0),
  	Point3d(0.0, 0.0, -1.0),
    Vector3d(0.0, 1.0, 0.0));
  c.setProjection(-halfBbSideLength, halfBbSideLength, -halfBbSideLength, halfBbSideLength,
    halfBbSideLength, kFar, Camera::PERSPECTIVE, false);
    
  Matrix4d rot;  
  switch (iSide) 
  {
    case cmsPosX: rot = rotate(rot, -PI_SUR_2, Vector3d(0.0, 1.0, 0.0), middle); break;
    case cmsNegX: rot = rotate(rot, PI_SUR_2, Vector3d(0.0, 1.0, 0.0), middle); break;
    case cmsPosY: rot = rotate(rot, PI_SUR_2, Vector3d(1.0, 0.0, 0.0), middle); break;
    case cmsNegY: rot = rotate(rot, -PI_SUR_2, Vector3d(1.0, 0.0, 0.0), middle); break;
    case cmsPosZ: rot = rotate(rot, -PI, Vector3d(0.0, 1.0, 0.0), middle); break;
    case cmsNegZ: rot = rotate(rot, 0.0, Vector3d(0.0, 1.0, 0.0), middle); break;
    default: break;
  }
  
  rot.setTranslation(middle);
  c.setTransformationToGlobal(rot);  
  
  double sideLengthInPixel = halfBbSideLength * 2.0 * c.getPixelPerGLUnit();
  c.setWindowSize(sideLengthInPixel, sideLengthInPixel);
   
  return c;
}

//-----------------------------------------------------------------------------
double Viewer::getAreaToRenderInnerRadius(int iAreaIndex) const
{
	double r = 0.0;
  if(iAreaIndex >= 0 && iAreaIndex < (int)mAreaToRenderRadii.size())
  {
  	r = mAreaToRenderRadii[iAreaIndex];
  }
  return r;
}

//-----------------------------------------------------------------------------
double Viewer::getAreaToRenderOuterRadius(int iAreaIndex) const
{
	double r = 0.0;
  int i = iAreaIndex + 1;
  if(i >= 0 && iAreaIndex < (int)mAreaToRenderRadii.size())
  {
  	r = mAreaToRenderRadii[i];
  }
  return r;	
}

//-----------------------------------------------------------------------------
double Viewer::getAreaToRenderRadius(int iAreaIndex) const
{ return getAreaToRenderOuterRadius(iAreaIndex); }

//-----------------------------------------------------------------------------
void Viewer::initializeDisplayLists()
{
	if(glIsList(mSphere))
  {
  	glDeleteLists(mSphere, 1);
    mSphere = 0;
  }
  
  if(glIsList(mCube))
  {
  	glDeleteLists(mCube, 1);
    mCube = 0;
  }
  
	mSphere = glGenLists(1);	
  glNewList(mSphere, GL_COMPILE);
	utilities::draw(PlatonicSolid(PlatonicSolid::tIsocahedron, 2), true);
  glEndList();
  
  mCube = glGenLists(1);	
  glNewList(mCube, GL_COMPILE);
	utilities::draw(PlatonicSolid(PlatonicSolid::tCube, 1));
  glEndList();
}

//-----------------------------------------------------------------------------
void Viewer::initialize()
{
  setCameraOrientation(Camera::FREE);
  mShipCamera = getCamera();
  mShipCamera.setProjection(kFov, 1.0, kNear, kFar, true);
  
  //inialise la vue de troisieme personne
  mShipCamera.set(Point3d(0, 0.025, 0.075),
  	Point3d(0, 0, -0.008),
    Vector3d(0, 0.001, 0));
  setCamera(mShipCamera, false);
}

//-----------------------------------------------------------------------------
void Viewer::initializeGL()
{
  Widget3d::initializeGL();
  glClearColor(0.0, 0.0, 0.0, 0.0);
  
	/*Multi sample à été activé globalement pour l'application.
    voir main.cpp*/
  glEnable(GL_MULTISAMPLE);
  initializeDisplayLists();
    
  //initialisation du cube map
  mCubeMapFbo.addColorAttachment(true);
  mCubeMapFbo.addColorAttachment(true);
  mCubeMapFbo.addColorAttachment(true);
  mCubeMapFbo.addColorAttachment(true);
  mCubeMapFbo.addColorAttachment(true);
  mCubeMapFbo.addColorAttachment(true);  
  mCubeMapFbo.addDepthAttachment(true);
  mCubeMapFbo.resize(400, 400);
  
  //init des shaders
  QString vertString;
  QString fragString;
  
  QFile vert(":/shaders/main for celestial bodies.vert");
  vert.open(QIODevice::ReadOnly);
  vertString = vert.readAll();
  vert.close();

  QFile frag(":/shaders/main for celestial bodies.frag");
  frag.open(QIODevice::ReadOnly);
  fragString = frag.readAll();
  frag.close();
  
  mSmoothSphere.addVertexShaderSource(vertString);
  mSmoothSphere.addFragmentShaderSource(fragString);
  
  vert.setFileName(":/shaders/directional lighting.vert");
  vert.open(QIODevice::ReadOnly);
  vertString = vert.readAll();
  vert.close();
  
  frag.setFileName(":/shaders/directional lighting.frag");
  frag.open(QIODevice::ReadOnly);
  fragString = frag.readAll();
  frag.close();
  
  mSmoothSphere.addVertexShaderSource(vertString);
  mSmoothSphere.addFragmentShaderSource(fragString);
  mSmoothSphere.link();
}

//-----------------------------------------------------------------------------
void Viewer::invalidateCubeMapRender()
{ mLastCubeMapRenderPosition.set(std::numeric_limits<double>::max()); }

//-----------------------------------------------------------------------------
void Viewer::keyPressEvent(QKeyEvent* ipE)
{
	bool handled = true;
	switch (ipE->key()) 
  {
  	if(isDebugging())
    {
      case Qt::Key_F:
      {
        mCameraFollowsShip = !mCameraFollowsShip;
        Camera c = getCamera();
        if(mCameraFollowsShip)
        {
        	c = mShipCamera;
//          c.setTransformationToGlobal(getEngine().getShip().getTransformation());
//          c.set(Point3d(0, 5, 15),
//            Point3d(0, 0, -4),
//            Vector3d(0, 1, 0));
        }
        else
        {
          Matrix4d m;
          m.setTranslation(getEngine().getShip().getTransformation().getTranslation());
  		    c.setTransformationToGlobal(m);
          c.set(Point3d(0, 60, 60),
            Point3d(0, 0, 0),
            Vector3d(0, 1, 0));          
        }
        setCamera(c, true);
      }
      break;
    }
    case Qt::Key_W: getEngine().keyPressed(Engine::kW); break;
    case Qt::Key_A: getEngine().keyPressed(Engine::kA); break;
    case Qt::Key_S: getEngine().keyPressed(Engine::kS); break;
    case Qt::Key_D: getEngine().keyPressed(Engine::kD); break;
    case Qt::Key_Q: getEngine().keyPressed(Engine::kQ); break;
    case Qt::Key_E: getEngine().keyPressed(Engine::kE); break;
    case Qt::Key_Shift: getEngine().keyPressed(Engine::kShift); break;
    
    case Qt::Key_C: 
    {
    	mIsThirdPersonView = !mIsThirdPersonView;
    	if(mIsThirdPersonView)
      	mShipCamera.set(Point3d(0, 0.025, 0.075),
			  	Point3d(0, 0, -0.008),
			    Vector3d(0, 0.001, 0));
      else
  	    mShipCamera.set(Point3d(0, 0, 0),
  	    	Point3d(0, 0, -1),
          Vector3d(0, 1, 0));
      setCamera(mShipCamera, true);
    }
    break;
    default: handled = false; break;
  }
  
  if(!handled)
  	ipE->setAccepted(false);
}

//-----------------------------------------------------------------------------
void Viewer::keyReleaseEvent(QKeyEvent* ipE)
{
	switch (ipE->key()) 
  {
  	case Qt::Key_W: getEngine().keyReleased(Engine::kW); break;
    case Qt::Key_A: getEngine().keyReleased(Engine::kA); break;
    case Qt::Key_S: getEngine().keyReleased(Engine::kS); break;
    case Qt::Key_D: getEngine().keyReleased(Engine::kD); break;
    case Qt::Key_Q: getEngine().keyReleased(Engine::kQ); break;
    case Qt::Key_E: getEngine().keyReleased(Engine::kE); break;
    case Qt::Key_Shift: getEngine().keyReleased(Engine::kShift); break;
    default: break;
  }
}

//-----------------------------------------------------------------------------
void Viewer::mouseMoveEvent(QMouseEvent* ipE)
{
  getEngine().mouseMoved(ipE->x(), ipE->y());
  if(!isCameraFollowingShip())
  	Widget3d::mouseMoveEvent(ipE);
}

////-----------------------------------------------------------------------------
//void Viewer::mousePressEvent(QMouseEvent* ipE)
//{
//}
//
////-----------------------------------------------------------------------------
//void Viewer::mouseReleaseEvent(QMouseEvent* ipE)
//{
//}

//-----------------------------------------------------------------------------
void Viewer::paintGL()
{
  Widget3d::paintGL();
  draw();
  
  if(isDebugging())
  {
    glColor3ub(255, 255, 255);
  	QString info("Engine state: ");
    switch (getEngine().getState()) 
    {
      case Engine::sSimulating: info += "Simulating. "; break;
      case Engine::sPlaying: info += "Playing. "; break;
      case Engine::sPaused: info += "Paused. ";break;
      default: break;
    }
    info += "number of cycles: " + QString::number(getEngine().getNumberOfCycles());
    renderText(5, 30, info);
    info = "number of bodies: " + QString::number(getEngine().getAstronomicalBodies().size());
    renderText(5, 45, info);
  }  
}

//-----------------------------------------------------------------------------
void Viewer::resizeGL(int iW, int iH)
{
	//QUand le viewport est redimensionné, on invalide la derniere position
  //de rendu du cube map
	invalidateCubeMapRender();
	Widget3d::resizeGL(iW, iH);
  /*On réassigne la caméra afin d'avoir la bonne caméra lors 
    de l'intialisation des fenetre Qt, étant donné que Widget3d::resizeGL
    refait les calculs de projections, puisque la taille de la fênetre change,
    il est important de reprendre la camera afin que la premiere image rendu
    soit correcte.*/
  mShipCamera = getCamera();
}

//-----------------------------------------------------------------------------
void Viewer::setAreaToRenderRadius(int iAreaIndex, double iRadius)
{
	if(iAreaIndex < 0)
    return;
    
  int index = iAreaIndex + 1;
  if(index > (int)mAreaToRenderRadii.size())
  {
  	double value = mAreaToRenderRadii.back();
  	for(int i = mAreaToRenderRadii.size(); i < index; ++i)
    {
    	mAreaToRenderRadii[i] = value;
    }
  }
  mAreaToRenderRadii[index] = iRadius;
}

//-----------------------------------------------------------------------------
void Viewer::setAsDebugging(bool iD)
{
	mIsDebugging = iD;
}

//-----------------------------------------------------------------------------
void Viewer::update()
{
	if(isCameraFollowingShip())
  {
    mShipCamera = getCamera();
    mShipCamera.setTransformationToGlobal(getEngine().getShip().getTransformation());
    setCamera(mShipCamera, false);
    Widget3d::update();
  }
}