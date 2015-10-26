#include <algorithm>
#define _USE_MATH_DEFINES //pour M_PI
#include <math.h>
#include "engine.h"
#include <random>
#include "utilities3d.h"

using namespace sharkTank;
using namespace std;
using namespace resonant;
   using namespace coreLibrary;


const int kTimeIncrementInMsec = 15;

//--- fish constants
const int kNumberOfFishInFlock = 500;
const double kFishMinimalNeighboutSearchRadius = 6.0;
const double kFishMaxAccel = 500.0;
const double kFishMaxVelocity = 32.0;
const double kEvadeSharkFactor = 15;
const double kFishBoundingSphereRadius = 1.732 * .5; //sqrt(3)*coté du cube

//--- shark constant
const double kSharkMinimalNeighboutSearchRadius = 60.0;
const double kWaterFrictionCoeff = 1.05;
const double kSharkBoundingSphereRadius = 1.732 * 3; //sqrt(3)*coté du cube

const double kNoVelocity = 0.2;

namespace
{
   double clamp( double iToClamp, double iMin, double iMax )
   { return min(max(iToClamp, iMin), iMax); }
}
//-----------------------------------------------------------------------------
//--- fishFoodNode
//-----------------------------------------------------------------------------
void fishFoodNode::begin()
{
   const point3 p = mpFishFood->getPosition();
   glDisable(GL_LIGHTING);
   glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
   glColor3ub( 200, 200, 0 );
   glPushMatrix();
   glTranslated( p.x(), p.y(), p.z() );
   utilities::drawBox( point3(-.5, -.5, -.5), point3(.5, .5, .5) );   
   glPopMatrix();
   glEnable(GL_LIGHTING);
   glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}
//-----------------------------------------------------------------------------
//--- fishNode
//-----------------------------------------------------------------------------
GLuint fishNode::mDisplayList = 0;
fishNode::fishNode(QString iToken, fish* ipF) :
   drawableNode(iToken),
   mpFish(ipF)
{}
//-----------------------------------------------------------------------------
fishNode::~fishNode()
{ glDeleteLists(mDisplayList, 1); mDisplayList = 0; }
//-----------------------------------------------------------------------------
void fishNode::begin()
{
   if( mDisplayList == 0 )
   {
      //create the list
      mDisplayList = glGenLists(1);

      //content of the list
      // compile the display list, store a triangle in it
      glNewList(mDisplayList, GL_COMPILE);
      utilities::drawBox( point3(-0.5, -0.5, -0.5), point3(0.5, 0.5, 0.5) );
      glEndList();
   }

   matrix4 t( mpFish->getPosition()-point3::origin );
   t *= mpFish->getOrientation();
   glColor3ub(215, 215, 215);
   glPushMatrix();
   glMultMatrixd( (GLdouble*)&t );
   glCallList(mDisplayList);   
   glPopMatrix();

   //glDisable(GL_LIGHTING);
   ////food force
   //glColor3ub( 200, 200, 0 ); //jaune
   //lineSegment ff( mpFish->getPosition(), mpFish->getPosition() + mpFish->getFoodForce() );
   //utilities::drawLineSegment( ff );

   ////grouping force
   //glColor3ub( 0, 255, 0 ); //vert
   //lineSegment gf( mpFish->getPosition(), mpFish->getPosition() + mpFish->getGroupingForce() );
   //utilities::drawLineSegment( gf );
   ////keep your distance
   //glColor3ub( 0, 0, 255 ); //bleu
   //lineSegment kydf( mpFish->getPosition(), mpFish->getPosition() + mpFish->getSeparationForce() );
   //utilities::drawLineSegment( kydf );
   //glEnable(GL_LIGHTING);
}
//-----------------------------------------------------------------------------
//--- fishFood
//-----------------------------------------------------------------------------
void fishFood::update()
{
   //--- check for collisions avec fish   
   if( isMarkedForDeletion() ) return;

   vector<fish*>& vf = mEngine.getFishes();
   foreach(fish* f, vf)
   {
      if( (f->getPosition() - getPosition()).norm() < 2*kFishBoundingSphereRadius )
      { markForDeletion(); break; }
   } 
}


//-----------------------------------------------------------------------------
//--- sharkNode
//-----------------------------------------------------------------------------
GLuint sharkNode::mDisplayList = 0;
sharkNode::sharkNode(QString iToken, shark* s) :
drawableNode(iToken),
   mpShark(s)
{}
//-----------------------------------------------------------------------------
sharkNode::~sharkNode()
{ glDeleteLists(mDisplayList, 1); mDisplayList = 0; }
//-----------------------------------------------------------------------------
void sharkNode::begin()
{
   if( mDisplayList == 0 )
   {
      //create the list
      mDisplayList = glGenLists(1);

      //content of the list
      // compile the display list, store a triangle in it
      glNewList(mDisplayList, GL_COMPILE);
      utilities::drawBox( point3(-3, -3, -3), point3(3, 3, 3) );
      glEndList();
   }

   glColor3ub(200.0, 0.0, 0.0);
   matrix4 t( mpShark->getPosition() - point3::origin );
   t *= mpShark->getOrientation();
   glPushMatrix();
   glMultMatrixd( (GLdouble*)&t );
   glCallList(mDisplayList);
   glPopMatrix();

   glDisable(GL_LIGHTING);
   //eatingforce
   glColor3ub( 200, 200, 0 ); //jaune
   lineSegment ff( mpShark->getPosition(),
      mpShark->getPosition() + mpShark->getEatingForce() );
   utilities::drawLineSegment( ff );

   //totalforce
   glColor3ub( 0, 0, 200 ); //jaune
   lineSegment af( mpShark->getPosition(),
      mpShark->getPosition() + mpShark->getAppliedForce() );
   utilities::drawLineSegment( af );

   //velocity
   glColor3ub( 0, 200, 200 ); //jaune
   lineSegment vf( mpShark->getPosition(),
      mpShark->getPosition() + mpShark->getVelocity() );
   utilities::drawLineSegment( vf );


   /*axe d.orientation*/
   matrix4 o = mpShark->getOrientation();
   vector3 x( o(0,0), o(1,0), o(2,0) );
   vector3 y( o(0,1), o(1,1), o(2,1) );
   vector3 z( o(0,2), o(1,2), o(2,2) );
   glColor3ub(255, 0, 0);
   lineSegment lsX( mpShark->getPosition(),
      mpShark->getPosition() + 10 * x );
   utilities::drawLineSegment( lsX );

   glColor3ub(0, 255, 0);
   lineSegment lsY( mpShark->getPosition(),
      mpShark->getPosition() + 10 * y );
   utilities::drawLineSegment( lsY );

   glColor3ub(0, 0, 255);
   lineSegment lsZ( mpShark->getPosition(),
      mpShark->getPosition() + 10 * z );
   utilities::drawLineSegment( lsZ );

   glEnable(GL_LIGHTING);
}

//-----------------------------------------------------------------------------
//--- fish
//-----------------------------------------------------------------------------
fish::fish(engine& e) : actor(e),
mSearchNeighbourRadius(kFishMinimalNeighboutSearchRadius)
{}
//-----------------------------------------------------------------------------
void fish::update()
{
   //updatePhysics -
   mEngine.updatePhysics( this );
   
   //--- update behavioral forces
   mFoodForce = vector3::zero;
   mGroupingForce = vector3::zero;
   mSeparationForce = vector3::zero;
   mEvadeForce = vector3::zero;

   //-- regroupement des poissons ensemble
   vector<fish*>& vf = mEngine.getFishes();
   point3 mCenterOfMass = point3::origin;
   double numberOfNeighbours = 0;
   foreach( fish* pFish, vf )
   {
      if( pFish == this ){ continue; }

      vector3 d = pFish->getPosition() - getPosition();
      double dNorm = d.norm();
      if( dNorm <= 2*kFishBoundingSphereRadius ) //collision
      {
         mSeparationForce += d * -1 * 1.0/dNorm * 100;
      }      

      //on vient de trouver un ami
      if(dNorm < getSearchNeighbourRadius())
      {
         mCenterOfMass += (pFish->getPosition()-point3::origin);
         numberOfNeighbours++;

         //on applique la force de separation si les amis sont trop près
         if( dNorm <= mEngine.getFishMinimalSeparationDistance() )
         {
            mSeparationForce += d * -1 * 1.0/dNorm;
         }
      }
   }   

   /*La force de regroupement est vers le centre de masse des voisins
     trouvé. Si on ne trouve pas un nombre minimal de voisin, on augmente
     le rayon de recherche afin de ne pas reste seul.*/
   if( numberOfNeighbours > mEngine.getFishMinimalNumberOfNeighbourToFlock() )
   {
      mCenterOfMass.set( mCenterOfMass.x() / numberOfNeighbours,
         mCenterOfMass.y() / numberOfNeighbours,
         mCenterOfMass.z() / numberOfNeighbours );
      mGroupingForce = (mCenterOfMass - getPosition());
      setSearchNeighbourRadius( max( getSearchNeighbourRadius() / 2.0, kFishMinimalNeighboutSearchRadius ) );
   }
   else
   { setSearchNeighbourRadius( getSearchNeighbourRadius() * 2.0 ); }

   //--- force d'attraction de la bouffe.
   fishFood* ff = mEngine.getClosestFood( getPosition() );
   if( ff )
   { mFoodForce = ff->getPosition() - getPosition(); }

   //--- force d'évasion du requin
   vector<shark*>& vs = mEngine.getSharks();
   foreach( shark* s, vs )
   {
      vector3 sharkV = s->getPosition() - getPosition();
      double safeDistance = 2 * kSharkBoundingSphereRadius + kFishBoundingSphereRadius;
      if( sharkV.norm() < safeDistance )
      {
         mEvadeForce += -sharkV * kEvadeSharkFactor; 
      }
   }

   setAppliedForce( mFoodForce + mSeparationForce + mGroupingForce + mEvadeForce );
}
//-----------------------------------------------------------------------------
//--- shark
//-----------------------------------------------------------------------------
shark::shark(engine& e) : actor(e),
  mSearchNeighbourRadius(kSharkMinimalNeighboutSearchRadius),
  mFovAngle(65.0)
{}
//-----------------------------------------------------------------------------
void shark::update()
{
   mEngine.updatePhysics(this);

   mEatingForce = vector3::zero;
   mWanderingForce = vector3::zero;
   
   vector<fish*>& vf = mEngine.getFishes();
   point3 cm(0.0, 0.0, 0.0);
   double numberOfPrey = 0;
   foreach(fish* f, vf)
   {
      if( f->isMarkedForDeletion() ){ continue; }

      vector3 preyV = f->getPosition() - getPosition();
      const double d = preyV.norm();
      if( d < getSearchNeighbourRadius() && 
       acos(preyV.normalize() * getVelocity().normalize()) < getFovAngle()*M_PI/180.0 / 2.0 )
      { 
         cm += f->getPosition() - point3::origin;
         numberOfPrey++;

         //collision avec le poisson... on le bouffe!
         if( d < kSharkBoundingSphereRadius + kFishBoundingSphereRadius )
         { f->markForDeletion(); }
      }
   }

   if( numberOfPrey > 0 )
   {
      cm.set( cm.x()/numberOfPrey, cm.y()/numberOfPrey, cm.z()/numberOfPrey );
      setSearchNeighbourRadius( max(getSearchNeighbourRadius() / 2.0, kSharkMinimalNeighboutSearchRadius) );
      setFovAngle( max(getFovAngle() - 0.5, 25.0) );
      mEatingForce = cm - getPosition();
      mCircleAround = point3::origin;
   }
   else
   {
      if( mCircleAround.isEqualTo(point3::origin, 1e-5 ) )
      { mCircleAround = getPosition(); }
      setSearchNeighbourRadius( getSearchNeighbourRadius() * 2.0 );
      setFovAngle( max(getFovAngle() + 0.5, 65.0) );
      //start circling aroung the last position at 2Pi per 10sec...
      double e = mEngine.getElapsedTime() / 1000.0 * 2 * M_PI / 10.0;
      double r = 20.0;
      double a = 5;
      point3 pointOnCircle( mCircleAround.x() + cos(e)*r, 
      mCircleAround.y() + sin(e)*r, 
      mCircleAround.z() + sin(e)*a );
      mWanderingForce = pointOnCircle - getPosition();
   }  
   setAppliedForce(mEatingForce + mWanderingForce); 
}
//-----------------------------------------------------------------------------
//--- engine
//-----------------------------------------------------------------------------
engine::engine() : QObject(),
mState( sStopped ),
mTimerId(0),
mFishMinimalSeparationDistance(4.0),
mFishMinimalNumberOfNeighbourToFlock(10)
{
   srand((unsigned int)time(NULL));
}
//-----------------------------------------------------------------------------
engine::~engine()
{
   send( mDestroyingEngine );
   mListeners.clear();

   for(auto it = mFishes.begin(); it != mFishes.end(); ++it)
   { delete *it; }
   mFishes.clear();

   for(auto it = mFishFood.begin(); it != mFishFood.end(); ++it)
   { delete *it; }
   mFishFood.clear();
}
//-----------------------------------------------------------------------------
void engine::addShark()
{
   //on genere une position alleatoire
   default_random_engine generator(rand());
   uniform_int_distribution<double> accelDistribution(kFishMaxAccel * .8, kFishMaxAccel * 1.2);
   uniform_int_distribution<double> speedDistribution(kFishMaxVelocity * .8, kFishMaxVelocity * 1.2);

   uniform_int_distribution<double> sharkSpace(-100, 100);
   shark* s = new shark(*this);
   s->setPosition( point3( sharkSpace(generator), 
      sharkSpace(generator), 
      sharkSpace(generator) ) );
   s->setMaxAccel( 4*accelDistribution(generator) );
   s->setMaxVelocity( 2*speedDistribution(generator) );
   mSharks.push_back(s);

   sharkNode* sn = new sharkNode( "shark "+QString::number((int)s), s );
   mSceneGraph.addNode(sn);
}
//-----------------------------------------------------------------------------
void engine::addFood()
{
   //on genere une position alleatoire   
   std::default_random_engine generator(rand());
   std::uniform_int_distribution<double> spaceDistribution(-200,200);

   //fish food
   fishFood* ff = new fishFood(*this);
   ff->setPosition( point3( spaceDistribution(generator),
      spaceDistribution(generator), 
      spaceDistribution(generator) ) );
   mFishFood.push_back( ff );

   fishFoodNode* ffn = new fishFoodNode( "fishFood " + 
      QString::number((int)ff), ff );
   mSceneGraph.addNode( ffn );
}
//-----------------------------------------------------------------------------
void engine::addListener(listener* l)
{
   auto it = find( mListeners.begin(), mListeners.end(), l );
   if( it == mListeners.end() )
   { mListeners.push_back(l); }
}
//-----------------------------------------------------------------------------
fishFood* engine::getClosestFood(point3 p)
{
   fishFood* r = 0;
   double smallestD = numeric_limits<double>::max();
   foreach( fishFood* ff, mFishFood )
   {
      double d = (ff->getPosition() - p).norm();
      if( d < smallestD )
      { smallestD = d; r = ff; }
   }
   return r;
}
//-----------------------------------------------------------------------------
// return the number of milliseconds since initialization
int engine::getElapsedTime() const
{ return mTime.elapsed(); }
//-----------------------------------------------------------------------------
void engine::goToState(state s)
{
   if( mState == s )
   { return; }
   
   switch (mState)
   {
   case sStopped: 
      switch ( s )
      {
      case sStarted:
         initialize();
         mTimerId = startTimer(kTimeIncrementInMsec);
         mState = s;
      break;
      default : break;
      }
   break;

   case sPaused: 
      switch ( s )
      {
      case sStarted: break;
      default : break;
      }
      break;

   case sStarted: 
      switch ( s )
      {
      case sPaused: break;
      case sStopped:
         killTimer(mTimerId);
         mTimerId = 0;
         mState = s;
      break;
      default : break;
      }
      break;

   default : break;
   }
   send( mStateChanged );
}
//-----------------------------------------------------------------------------
void engine::initialize()
{
   mTime.start();

   //on genere une position alleatoire
   default_random_engine generator(rand());
   uniform_int_distribution<double> flockDistribution(-20,20);
   uniform_int_distribution<double> accelDistribution(kFishMaxAccel * .8, kFishMaxAccel * 1.2);
   uniform_int_distribution<double> speedDistribution(kFishMaxVelocity * .8, kFishMaxVelocity * 1.2);

   //les fishs
   for(int i = 0; i < kNumberOfFishInFlock; ++i)
   {
      fish* f = new fish(*this);
      f->setPosition( point3( flockDistribution(generator), 
         flockDistribution(generator), 
         flockDistribution(generator) ) );
      f->setMaxAccel( accelDistribution(generator) );
      f->setMaxVelocity( speedDistribution(generator) );
      mFishes.push_back(f);

      fishNode* fn = new fishNode( "fish " + QString::number((int)f), f );
      mSceneGraph.addNode( fn );
   }
}
//-----------------------------------------------------------------------------
void engine::nudgeFish()
{
   std::default_random_engine generator(rand());
   std::uniform_int_distribution<int> fishIndex(0, (int)mFishes.size() - 1);
   std::uniform_int_distribution<double> nudgeForce(-1000, 1000);

   int i = fishIndex(generator);
   fish* f = mFishes[i];

   vector3 force( nudgeForce(generator), nudgeForce(generator), nudgeForce(generator) );
   f->setGroupingForce( force );
}
//-----------------------------------------------------------------------------
void engine::remove(fish* f)
{
   auto it = find(mFishes.begin(), mFishes.end(), f);
   if( it != mFishes.end() )
   {
      QString token = "fish " + QString::number((int)*it);
      mSceneGraph.removeNode(token);
      mFishes.erase(it);
   }
}
//-----------------------------------------------------------------------------
void engine::remove(fishFood* ff)
{
   auto it = find(mFishFood.begin(), mFishFood.end(), ff);
   if( it != mFishFood.end() )
   {
      QString token = "fishFood " + QString::number((int)*it);
      mSceneGraph.removeNode(token);
      mFishFood.erase(it);
   }
}
//-----------------------------------------------------------------------------
void engine::removeListener(listener* l)
{
   auto it = find( mListeners.begin(), mListeners.end(), l );
   if( it != mListeners.end() )
   { mListeners.erase(it); }
}
//-----------------------------------------------------------------------------
void engine::send(message m)
{
   for(size_t i = 0; i < mListeners.size(); ++i )
   { mListeners[i]->received(m); }
}

//-----------------------------------------------------------------------------
void engine::start()
{   
   goToState(sStarted);
}
//-----------------------------------------------------------------------------
void engine::timerEvent(QTimerEvent* ipE)
{
   if( ipE->timerId() == mTimerId )
   {
      //fish
      foreach(fish* pFish, mFishes)
      { pFish->update(); }

      //food
      foreach(fishFood* ff, mFishFood)
      { ff->update(); }

      //sharks
      foreach(shark* s, mSharks)
      { s->update(); }

      //--- cleanup, on enleve les elements marké a détruires.
      foreach(fish* f, mFishes)
      {
         if( f->isMarkedForDeletion() )
         { remove(f); }
      }

      foreach(fishFood* ff, mFishFood)
      {
         if( ff->isMarkedForDeletion() )
         { remove(ff); }
      }

      send(mFrameReady);
   }
}
//-----------------------------------------------------------------------------
void engine::updatePhysics(actor* ipA)
{
   vector3 friction = !ipA->getVelocity().isEqualTo(vector3::zero, 1e-5) ?
      -ipA->getVelocity().normalize()*kWaterFrictionCoeff : vector3::zero;
   vector3 f = ipA->getAppliedForce() + friction;
   const double mass = 1.0;
   const double timeIncInSec = kTimeIncrementInMsec/1000.0;
   vector3 a = f/mass;
   double aNorm = a.norm();
   const double maxAccel = ipA->getMaxAccel();
   if( !isEqualTo(aNorm, 0.0, 1e-5) )
   { a = a.normalize() * clamp( aNorm, -maxAccel, maxAccel ); }
   ipA->setAcceleration( a );

   vector3 v = ipA->getVelocity() + ipA->getAcceleration() * timeIncInSec;
   double vNorm = v.norm();
   const double maxVel = ipA->getMaxVelocity();
   if( !isEqualTo(vNorm, 0.0, 1e-5) )
   { v = v.normalize() * clamp( vNorm, -maxVel, maxVel ); }
   ipA->setVelocity( v );
   ipA->setPosition( ipA->getPosition() + ipA->getVelocity() * timeIncInSec );

   //--- calcule de lorientation
   /* Le systeme de coordonne est comme suit:
   
         z
         ^
         |
         |
         |---------> y
        /
       /
      /
     x
      x est dans la meme diretion que la velocity.
   */
   matrix4 o = ipA->getOrientation();
   matrix4 r;
   if( v.norm() > kNoVelocity )
   {
      vector3 y( 0, 1, 0 );
      vector3 z( 0, 0, 1);
      vector3 x( 1, 0, 0);
      x = v;
      x.normalize();
      y = z ^ x;
      y.normalize();
      z = x ^ y;
      z.normalize();
      o = matrix4 (x, y, z);
      //langle entre la force appliquée et le vecteur velocity donne le 
      //tanguage
      //double cosTheta = ipA->getAppliedForce().normalize() * ipA->getVelocity().normalize();
//printf("Theta %.4f\n", acos(cosTheta) * 180.0 / M_PI);
      //r = matrix4(vector3(1, 0 , 0), acos(cosTheta));
   }      
   ipA->setOrientation( o * r );
}
////-----------------------------------------------------------------------------
//void engine::
//{ }

//-----------------------------------------------------------------------------
//--- engine::listener
//-----------------------------------------------------------------------------
engine::listener::listener()
{}

