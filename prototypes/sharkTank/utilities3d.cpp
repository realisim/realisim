#include "utilities3d.h"
//--- glew.h doit être inclus avant qgl.h (a cause de gl.h)
#ifdef WIN32
   #ifndef __glew_h__
      #include <GL/glew.h> //nécessaire pour l'utilisation de GL_BGRA
   #endif   
#endif
#include "qgl.h"

using namespace std;
using namespace resonant;
   using namespace utilities;

//-----------------------------------------------------------------------------
//--- fonctions utilitaires
//-----------------------------------------------------------------------------
void utilities::drawAxis()
{
   glColor3ub(255, 0, 0);
   glBegin( GL_LINES );
   glVertex3d( 0.0, 0.0, 0.0 );
   glVertex3d( 1.0, 0.0, 0.0 );
   glEnd();
   glColor3ub(0, 255, 0);
   glBegin( GL_LINES );
   glVertex3d( 0.0, 0.0, 0.0 );
   glVertex3d( 0.0, 1.0, 0.0 );
   glEnd();
   glColor3ub(0, 0, 255);
   glBegin( GL_LINES );
   glVertex3d( 0.0, 0.0, 0.0 );
   glVertex3d( 0.0, 0.0, 1.0 );
   glEnd();
}
//-----------------------------------------------------------------------------
/*Permet de dessiner une boite. p1 représente le coin inférieur gauche sur le
  plan z le plus rapproché. le coin p2 représente le coin supérieur droit sur le plan z le
  plus éloigné.*/
void utilities::drawBox(point3 p1, point3 p2)
{
   /* a,b,c,d,e,f,g et h repésentent les 8 coins. comme suit:
       h-----g
   d---|-c   |
   |   | |   |
   |   e |---f
   a-----b
   
   */
   const point3 a = p1;
   const point3 b(p2.x(), p1.y(), p1.z());
   const point3 c(p2.x(), p2.y(), p1.z());
   const point3 d(p1.x(), p2.y(), p1.z());
   const point3 e(p1.x(), p1.y(), p2.z());
   const point3 f(p2.x(), p1.y(), p2.z());
   const point3 g = p2;
   const point3 h(p1.x(), p2.y(), p2.z());
   glBegin( GL_QUADS );
   //-z
   glNormal3d( 0.0, 0.0, -1 );
   glVertex3dv((GLdouble*)&e);
   glVertex3dv((GLdouble*)&h);
   glVertex3dv((GLdouble*)&g);
   glVertex3dv((GLdouble*)&f);

   //z
   glNormal3d( 0.0, 0.0, 1 );
   glVertex3dv((GLdouble*)&a);
   glVertex3dv((GLdouble*)&b);
   glVertex3dv((GLdouble*)&c);
   glVertex3dv((GLdouble*)&d);

   //-x
   glNormal3d( -1.0, 0.0, 0.0 );
   glVertex3dv((GLdouble*)&a);
   glVertex3dv((GLdouble*)&e);
   glVertex3dv((GLdouble*)&h);
   glVertex3dv((GLdouble*)&d);

   //x
   glNormal3d( 1.0, 0.0, 0.0 );
   glVertex3dv((GLdouble*)&b);
   glVertex3dv((GLdouble*)&f);
   glVertex3dv((GLdouble*)&g);
   glVertex3dv((GLdouble*)&c);

   //-y
   glNormal3d( 0.0, -1.0, 0.0 );
   glVertex3dv((GLdouble*)&a);
   glVertex3dv((GLdouble*)&e);
   glVertex3dv((GLdouble*)&f);
   glVertex3dv((GLdouble*)&b);

   //y
   glNormal3d( 0.0, 1.0, 0.0 );
   glVertex3dv((GLdouble*)&d);
   glVertex3dv((GLdouble*)&c);
   glVertex3dv((GLdouble*)&g);
   glVertex3dv((GLdouble*)&h);
   glEnd();
}
//-----------------------------------------------------------------------------
/*dessine une grille plan centré en x,y,z qui s'étend de -0.5 a 0.5*/
void utilities::drawGrid(const plane& p)
{
   //on trouve un vecteur perpendicualire a la normale
   vector3 n = p.getNormal();
   
   //on trouve un vecteur perpendicualire qui sera la baseX
   double epsilon = 1e-5;
   /*afin d'obtenir un vecteur perpendiculaire, le produit scalaire doit donner
    0. donc
    	1- (ax, by, cz) * (dx, ey, fz) = 0 
      2- ( a*d + b*e + c*z ) = 0 
      si d = b et que e = -a et que z = 0,
      3- a*b + b*(-a) + 0 = 0
      Bref, en permuttant deux valeurs et en inversant une des deux et remplacant
      la troisieme par 0, on obtient toujours un vecteur perpendiculaire.*/
    vector3 baseX(1.0, 0.0, 0.0);
    if( !isEqualTo( n.dx(), 0.0, epsilon ) )
    	baseX.set( n.dy(), -n.dx(), 0.0 );
    else if( !isEqualTo( n.dy(), 0.0, epsilon ) ) 
    	baseX.set( -n.dy(), n.dx(), 0.0 );
    else if( !isEqualTo( n.dz(), 0.0, epsilon ) )
		baseX.set( 0.0, n.dz(), -n.dy() );

   vector3 baseY = baseX^n;
   matrix4 m( baseX.normalize(), baseY.normalize(), n );
   matrix4 t( p.getPoint()- point3::origin );
   m = t*m;
   const int kInterspace = 50;
   for(int i = -25; i <= 25; ++i )
   {
      point3 p1( i*kInterspace, i*kInterspace, 0 );
      p1 = m.transform(p1);
      drawLine( line(p1, baseX) );
      drawLine( line(p1, baseY) );
   }
}
//-----------------------------------------------------------------------------
//dessine une ligne centré en x,y,z qui s'étend de chaque coté
void utilities::drawLine(const line& l)
{
   const point3 a = l.getPoint() - l.getDirection()*1e6;
   const point3 b = l.getPoint() + l.getDirection()*1e6;
   glBegin(GL_LINES);
      glVertex3dv((GLdouble*)&a);
      glVertex3dv((GLdouble*)&b);
   glEnd();
}
//-----------------------------------------------------------------------------
void utilities::drawLineSegment(const lineSegment& ls)
{
   glBegin(GL_LINES);
   glVertex3dv((GLdouble*)&(ls.getFirstPoint()));
   glVertex3dv((GLdouble*)&(ls.getSecondPoint()));
   glEnd();
}
//-----------------------------------------------------------------------------
void utilities::drawPoint(const point3& p)
{
   glBegin(GL_POINTS);
      glVertex3dv((GLdouble*)&p);
   glEnd();
}
//-----------------------------------------------------------------------------
void utilities::drawPolygon(const polygon& p)
{
   glBegin(GL_POLYGON);
      glNormal3dv( (GLdouble*)&(p.getNormal()) );
      for(int i = 0; i < p.getNumberOfVertices(); ++i)
      { glVertex3dv((GLdouble*)&(p.getVertex(i))); }
   glEnd();
}
//-----------------------------------------------------------------------------
void utilities::drawRectangle(point2 p1, point2 p2)
{
   glBegin( GL_QUADS );
   glVertex2d(p1.x(), p1. y());
   glVertex2d(p2.x(), p1.y());
   glVertex2d(p2.x(), p2.y());
   glVertex2d(p1.x(), p2.y());
   glEnd();
}