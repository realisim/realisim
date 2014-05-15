
#include "Engine.h"
#include "GameEntity.h"
#include "Math/MathUtils.h"
#include "Physics.h"

using namespace realisim;
	using namespace math;
	using namespace platform;

//------------------------------------------------------------------------------
//---Physics
//------------------------------------------------------------------------------
Physics::Physics()
{}
Physics::~Physics()
{}

//------------------------------------------------------------------------------
void Physics::applyFriction( GameEntity& iGe )
{
  Vector2d v = iGe.getVelocity();
  Vector2d a = iGe.getAcceleration();
  /*On applique la friction si l'entité à une collision avec le sol.
    
  */
  bool touchesGround = false;
  if( iGe.hasIntersections() )
  {
  	for(int i = 0; i < iGe.getNumberOfIntersections(); ++i && !touchesGround )
    {
    	Intersection2d x = iGe.getIntersection(i);
    	for( int j = 0; j < x.getNumberOfContacts(); ++j && !touchesGround )
      {
      	if(x.getNormal(j) * Vector2d(0.0, 1.0) > 0)
        { touchesGround = true; }
      }
    }
  }
  bool applyFriction = touchesGround;
    
  if(applyFriction )
		v.setX( v.x() * ( 1.0 - iGe.getFrictionCoefficient() ) );
    
  iGe.setVelocity( v );
}

//------------------------------------------------------------------------------
/*retourne la penetration de r1 dans r2.*/
Vector2d Physics::penetration( const Rectangle& r1, const Rectangle& r2 )
{
  double olx = axisOverLap( r1.bottomLeft().x(), r1.bottomRight().x(),
    r2.bottomLeft().x(), r2.bottomRight().x() );
  double oly = axisOverLap( r1.bottomLeft().y(), r1.topLeft().y(),
    r2.bottomLeft().y(), r2.topLeft().y() );
  return Vector2d( olx, oly );
}

//------------------------------------------------------------------------------
/*Cette methode calcule la direction de reflexion pour un un vecteur incident
 iI par rapport à la normale iN.
 Note, iN doit être normalisé*/
Vector2d Physics::reflect( const Vector2d& iI, const Vector2d& iN,
	double iElasticity )
{
	assert( isEqual(iN.norm(), 1.0) );
  return iI - ( (iElasticity + 1.0) * (iI * iN)*(iN) );
}

//------------------------------------------------------------------------------
void Physics::resolveCollisions( GameEntity& iGe, Stage& iStage )
{  
  //verification des collisions
  //on flush les collisions précedentes de l'acteur
  iGe.clearIntersections();  
  vector<int> cells = iStage.getCellIndices( iGe.getPosition(), 
  	iGe.getCollisionSearchGrid() );
  vector< pair<int,Rectangle> > collidingCells;

  for( uint i = 0; i < cells.size(); ++i )
  {
  	if( iStage.value( cells[i] ) == Stage::ctGround )
    {
      //la coordonnée pixel de la cellule
      Vector2i cpc = iStage.getCellPixelCoordinate( cells[i] );
      Rectangle cellRect( toPoint(cpc), iStage.getCellSize() );
      Rectangle aRect = iGe.getBoundingBox();
      
      if( intersects( aRect, cellRect ) )
      { collidingCells.push_back( make_pair(cells[i], cellRect) ); }
    }
  }
  
  /*On commence par reglé les collisions directement sous et sur les
    coté de l'acteur.*/
  vector< pair<int,Rectangle> >::iterator it = collidingCells.begin();
  for( ; it != collidingCells.end(); )
  {
  	Rectangle aRect = iGe.getBoundingBox();
    Vector2i cc = iStage.getCellCoordinate( it->first );
    Vector2i pc = iStage.getCellCoordinate( iGe.getPosition() );
    
    if( pc.x() == cc.x() || pc.y() == cc.y() )
    {      
      if( intersects( aRect, it->second ) )
      {
      	Intersection2d x;
        Point2d pos = iGe.getPosition();
        Vector2d v = iGe.getVelocity();
        Vector2d pen = penetration( aRect, it->second );
        Vector2d normal;
        if( pc.y() == cc.y())
        {
        	normal = pc.x() > cc.x() ? Vector2d(1.0, 0.0) : 
          	Vector2d(-1.0, 0.0);
        	pen.setY( 0.0 );
          v = reflect( v, normal, iGe.getCollisionElasticity() );
        }
        else if( pc.x() == cc.x() )
        {
        	normal = pc.y() > cc.y() ? Vector2d(0.0, 1.0) :
            Vector2d(0.0, -1.0);
          pen.setX( 0.0 );
          v = reflect( v, normal, iGe.getCollisionElasticity() );
        }       	
        x.add( it->second.bottomLeft() + 
        	Vector2d(iStage.getCellSize() / 2.0), normal );
        iGe.setPosition( pos - pen );
        iGe.setVelocity( v );
        iGe.addIntersection( x );
        
        it = collidingCells.erase(it);
      }
      else {++it;}
    }
    else 
    { ++it; }
  }
  
  /*Ensuite pour toutes les cellules en collisions qui restent, on parcour
    a partir du centre de lacteur vers lextérieur.*/
  int count = 1;
  while( !collidingCells.empty() )
  {
    it = collidingCells.begin();
    for( ; it != collidingCells.end(); )
    {      
      Vector2i cc = iStage.getCellCoordinate( it->first );
      Vector2i pc = iStage.getCellCoordinate( iGe.getPosition() );
      
      if( abs(pc.y() - cc.y()) == count || 
        abs(pc.x() - cc.x()) == count )
      {
      	Rectangle aRect = iGe.getBoundingBox();
        if( intersects( aRect, it->second ) )
        {
        	Intersection2d x;
          Point2d pos = iGe.getPosition();
          Vector2d v = iGe.getVelocity();
          Vector2d pen = penetration( aRect, it->second );
          Vector2d normal;
          
          if( fabs(pen.x()) <= fabs(pen.y()) ) 
          { 
            normal = pc.x() > cc.x() ? Vector2d(1.0, 0.0) : 
              Vector2d(-1.0, 0.0);
            pen.setY( 0.0 );
            v = reflect( v, normal, iGe.getCollisionElasticity() );
          }
          else 
          { 
            normal = pc.y() > cc.y() ? Vector2d(0.0, 1.0) :
              Vector2d(0.0, -1.0);
            pen.setX( 0.0 );
            v = reflect( v, normal, iGe.getCollisionElasticity() );
          }
          x.add( it->second.bottomLeft() + 
  	      	Vector2d(iStage.getCellSize() / 2.0), normal );
          iGe.setPosition( pos - pen );
          iGe.setVelocity( v );
          iGe.addIntersection( x );
        }
      	it = collidingCells.erase(it);
      }
      else
      {++it;}
    }
		++count;
  }
  
  applyFriction(iGe);
}

//------------------------------------------------------------------------------
void Physics::resolveCollisions( Player& p, Monster& m)
{
  if( intersects( p.getBoundingCircle(), m.getBoundingCircle() ) )
  {
    Intersection2d z = intersect( p.getBoundingBox(), m.getBoundingBox() );
    if( z.hasContacts() )
    {
      p.addIntersection( z );
      p.setState( Actor::sHit );
    }
  }
}

//------------------------------------------------------------------------------
/* Cette méthode sert à gérer la collision des projectiles avec le stage. Elle
  est particulière parce que les projectiles peuvent aller tres vite et la
  collision générique resolveCollisions( GameEntity& p, Stage& s) gère mal ce
  cas.*/
void Physics::resolveCollisions( Projectile& p, Stage& s)
{
	p.clearIntersections();
  vector<int> cells = s.getCellIndices( p.getPosition(),
  	p.getCollisionSearchGrid() );

	Vector2d v = p.getVelocity();
  Rectangle bBox = p.getBoundingBox();
  Point2d previousPos = p.getPosition() - v * getTimeIncrement();
	Intersection2d x;
  bool hasIntersection = false;
  for( uint i = 0; i < cells.size(); ++i )
  {
  	if( s.value( cells[i] ) == Stage::ctGround )
    {
      //la coordonnée pixel de la cellule
      Vector2i cpc = s.getCellPixelCoordinate( cells[i] );
      Rectangle cellRect( toPoint(cpc), s.getCellSize() );      
      if( intersects( bBox, cellRect ) )
      {
        //on trouve le point precis d'intersetion avec la map
        Line2d l( previousPos, v );
        x.add(intersect(l, cellRect));
        hasIntersection = true;
      }
    }
  }

	/*si il y a au moins une intersection entre la ligne de direction du
    projectile et la map, on vient trouver le point d'intersection le plus
    près du projectile. Si ce dernier est contenu par le bbox du projectile,
    on corrige la velocité/position du projectile. Lorsqu'il n'y a pas
    d'intersection entre la ligne de direction du projectile et la map, mais que
    hasIntersection est vrai, on applique la resolution de collisiion générique.
    Ce cas est utilisé lorsque le projectile est relativement gros.*/
  if( x.hasContacts() )
  {
    int closestIndex = std::numeric_limits<int>::max();
    double closestDistance = std::numeric_limits<double>::max();
    for( int i = 0; i < x.getNumberOfContacts(); ++i )
    {
      double d = (previousPos - x.getContact(i)).normSquare();
      if( d < closestDistance )
      { closestIndex = i; closestDistance = d; }
    }
    
    Point2d c = x.getContact( closestIndex );
    if( bBox.contains(c) )
    {
      Vector2d v = p.getVelocity();
      Vector2d n = x.getNormal( closestIndex );
      
      v = reflect(v, n, p.getCollisionElasticity());
      p.setVelocity(v);    
      Vector2d pProjn = fabs(toVector(p.getPosition()) * n) * n;
      Vector2d cProjn = fabs(toVector(c) * n) * n;
      Vector2d bbProjn = fabs((p.getBoundingBox().size() * n)) * n /2.0;
      Vector2d d = bbProjn - (pProjn - cProjn);
      p.setPosition( p.getPosition() + d );
      Intersection2d interFinal;
      p.addIntersection( Intersection2d( c, n ) );
    }
  }
  else if( hasIntersection )
  { resolveCollisions((GameEntity&)p, s); }
  
  if( hasIntersection )
  {
  	applyFriction(p);
  }
  //p.addIntersection(x); //pour debogage
}

//------------------------------------------------------------------------------
void Physics::resolveCollisions( Projectile& p, Actor& a)
{
  if( intersects( p.getBoundingCircle(), a.getBoundingCircle() ) )
  {
    Intersection2d z = intersect( p.getBoundingBox(), a.getBoundingBox() );
    if( z.hasContacts() )
    { 
    	p.addIntersection( z );
      //gestion des dommages
      a.setHealth( a.getHealth() - p.getDamage() );
    }
  }
}

//------------------------------------------------------------------------------
void Physics::update( GameEntity& iGe )
{ 
  Point2d p = iGe.getPosition();
	Vector2d v = iGe.getVelocity();
	Vector2d a = iGe.getAcceleration();
  double maxHv = iGe.getMaximumVelocity().x();
  double maxVv = iGe.getMaximumVelocity().y();
  
  //application de la gravité
  if( iGe.isGravityApplied() )
  { a += Vector2d(0.0, -980); }

	//déplacement du joueur a la position désiré
  v += a * getTimeIncrement();
  v.setX( v.x() >= 0.0 ? 
  	min(maxHv, v.x()) : max(-maxHv, v.x()) );
  v.setY( v.y() >= 0.0 ? 
  	min(maxVv, v.y()) : max(-maxVv, v.y()) );
  
  iGe.setPosition( p + v * getTimeIncrement() );
  iGe.setVelocity( v );
  iGe.setAcceleration( a );
}
