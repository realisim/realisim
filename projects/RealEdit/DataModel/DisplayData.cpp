/*
 */

#include "DisplayData.h"
#include <qgl.h>

using namespace realisim;
using namespace realEdit;

DisplayData::DisplayData() : mAxisDisplayList(0), mCubeDisplayList(0)
{}

DisplayData::~DisplayData()
{}

void DisplayData::drawAxis() const
{
  if(mAxisDisplayList == 0)
  {
    mAxisDisplayList = glGenLists(1);
    glNewList(mAxisDisplayList, GL_COMPILE );
    
    glPushAttrib(GL_CURRENT_BIT | GL_LINE_BIT | GL_ENABLE_BIT);
    glDisable(GL_LIGHTING);
    glLineWidth(2);
    glBegin(GL_LINES);
    //x
    glColor3d(1.0, 0.0, 0.0);
    glVertex3d(0.0, 0.0, 0.0); glVertex3d(1.0, 0.0, 0.0);
    //y
    glColor3d(0.0, 1.0, 0.0);
    glVertex3d(0.0, 0.0, 0.0); glVertex3d(0.0, 1.0, 0.0);
    //z
    glColor3d(0.0, 0.0, 1.0);
    glVertex3d(0.0, 0.0, 0.0); glVertex3d(0.0, 0.0, 1.0);
    glEnd();
    glPopAttrib();
    
    glEndList();
  }
  else
    glCallList(mAxisDisplayList);
}

void DisplayData::drawBoundingBox(const Point3d& iMin, const Point3d& iMax) const
{
  glPushAttrib (GL_POLYGON_BIT | GL_ENABLE_BIT);
  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  glBegin(GL_QUADS);
    glNormal3f (0.0F, 0.0F, 1.0F);
    glVertex3f (iMin. getX (), iMin. getY (), iMax. getZ ());
    glVertex3f (iMax. getX (), iMin. getY (), iMax. getZ ());
    glVertex3f (iMax. getX (), iMax. getY (), iMax. getZ ());
    glVertex3f (iMin. getX (), iMax. getY (), iMax. getZ ());

    glNormal3f( 0.0F, 0.0F,-1.0F);
    glVertex3f (iMin. getX (), iMin. getY (), iMin. getZ ());
    glVertex3f (iMax. getX (), iMin. getY (), iMin. getZ ());
    glVertex3f (iMax. getX (), iMax. getY (), iMin. getZ ());
    glVertex3f (iMin. getX (), iMax. getY (), iMin. getZ ());

    glNormal3f( 0.0F, 1.0F, 0.0F);
    glVertex3f (iMin. getX (), iMax. getY (), iMax. getZ ());
    glVertex3f (iMax. getX (), iMax. getY (), iMax. getZ ());
    glVertex3f (iMax. getX (), iMax. getY (), iMin. getZ ());
    glVertex3f (iMin. getX (), iMax. getY (), iMin. getZ ());

    glNormal3f( 0.0F,-1.0F, 0.0F);
    glVertex3f (iMin. getX (), iMin. getY (), iMax. getZ ());
    glVertex3f (iMax. getX (), iMin. getY (), iMax. getZ ());
    glVertex3f (iMax. getX (), iMin. getY (), iMin. getZ ());
    glVertex3f (iMin. getX (), iMin. getY (), iMin. getZ ());

    glNormal3f( 1.0F, 0.0F, 0.0F);
    glVertex3f (iMax. getX (), iMin. getY (), iMax. getZ ());
    glVertex3f (iMax. getX (), iMin. getY (), iMin. getZ ());
    glVertex3f (iMax. getX (), iMax. getY (), iMin. getZ ());
    glVertex3f (iMax. getX (), iMax. getY (), iMax. getZ ());

    glNormal3f(-1.0F, 0.0F, 0.0F);
    glVertex3f (iMin. getX (), iMin. getY (), iMax. getZ ());
    glVertex3f (iMin. getX (), iMin. getY (), iMin. getZ ());
    glVertex3f (iMin. getX (), iMax. getY (), iMin. getZ ());
    glVertex3f (iMin. getX (), iMax. getY (), iMax. getZ ());
  glEnd();
  glPopAttrib();
}

void DisplayData::drawCube() const
{
  float hs = 0.5;
  
   if(mCubeDisplayList == 0)
  {
    mCubeDisplayList = glGenLists(1);
    glNewList(mCubeDisplayList, GL_COMPILE );
    glBegin(GL_QUADS);
      glNormal3f( 0.0F, 0.0F, 1.0F);
      glVertex3f( hs, hs, hs); glVertex3f(-hs, hs, hs);
      glVertex3f(-hs,-hs, hs); glVertex3f( hs,-hs, hs);

      glNormal3f( 0.0F, 0.0F,-1.0F);
      glVertex3f(-hs,-hs,-hs); glVertex3f(-hs, hs,-hs);
      glVertex3f( hs, hs,-hs); glVertex3f( hs,-hs,-hs);

      glNormal3f( 0.0F, 1.0F, 0.0F);
      glVertex3f( hs, hs, hs); glVertex3f( hs, hs,-hs);
      glVertex3f(-hs, hs,-hs); glVertex3f(-hs, hs, hs);

      glNormal3f( 0.0F,-1.0F, 0.0F);
      glVertex3f(-hs,-hs,-hs); glVertex3f( hs,-hs,-hs);
      glVertex3f( hs,-hs, hs); glVertex3f(-hs,-hs, hs);

      glNormal3f( 1.0F, 0.0F, 0.0F);
      glVertex3f( hs, hs, hs); glVertex3f( hs,-hs, hs);
      glVertex3f( hs,-hs,-hs); glVertex3f( hs, hs,-hs);

      glNormal3f(-1.0F, 0.0F, 0.0F);
      glVertex3f(-hs,-hs,-hs); glVertex3f(-hs,-hs, hs);
      glVertex3f(-hs, hs, hs); glVertex3f(-hs, hs,-hs);
    glEnd();
  }
  else
    glCallList(mCubeDisplayList);

}
