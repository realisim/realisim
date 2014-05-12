#include "GameEntity.h"
#include "Math/MathUtils.h"
#include "Engine.h"

using namespace realisim;
	using namespace platform;
//------------------------------------------------------------------------------
//--- GameEntity
//------------------------------------------------------------------------------
GameEntity::GameEntity() : 
 mSpriteTokens(),
 mCurrentSpriteToken(),
 mBoundingBox( Point2d(0.0), Vector2d( 5, 5 ) ),
 mBoundingCircle( Point2d(0.0), 5 ),
 mPosition( 0.0 ),
 mVelocity( 0.0 ),
 mMaximumVelocity( 400, 1000 ),
 mAcceleration( 0.0 ),
 mMaximumAcceleration( 800, 800 ),
 mCollisionElasticity( 0.0 ),
 mCollisionSearchGrid(0.0, 0.0),
 mIntersections(),
 mIsGravityApplied(true),
 mpEngine(0),
 mSprite(),
 mMarkedForDeletion(false)
{}
//------------------------------------------------------------------------------
GameEntity::~GameEntity() {}
//------------------------------------------------------------------------------
void GameEntity::addIntersection( const Intersection2d& iV )
{ mIntersections.push_back(iV); }
//------------------------------------------------------------------------------
void GameEntity::clearIntersections()
{ mIntersections.clear(); }
//------------------------------------------------------------------------------
const Vector2d& GameEntity::getAcceleration() const
{ return mAcceleration; }
//------------------------------------------------------------------------------
const math::Rectangle GameEntity::getBoundingBox() const
{
	return Rectangle( getPosition() - mBoundingBox.size() / 2.0,
    mBoundingBox.size() ); 
}
//------------------------------------------------------------------------------
const math::Circle GameEntity::getBoundingCircle() const
{ return Circle( getPosition(), mBoundingCircle.getRadius() );  }
//------------------------------------------------------------------------------
const Vector2d& GameEntity::getMaximumAcceleration() const
{ return mMaximumAcceleration; }
//------------------------------------------------------------------------------
const Vector2d& GameEntity::getMaximumVelocity() const
{ return mMaximumVelocity; }
//------------------------------------------------------------------------------
const Point2d& GameEntity::getPosition() const
{ return mPosition; }
//------------------------------------------------------------------------------
QString GameEntity::getSpriteToken(int i) const
{
  map<int, QString>::const_iterator it = mSpriteTokens.find(i);
  return it != mSpriteTokens.end() ? it->second : QString();
}
//------------------------------------------------------------------------------
const Vector2d& GameEntity::getVelocity() const
{ return mVelocity; }
//------------------------------------------------------------------------------
bool GameEntity::hasIntersections() const
{
	bool r = false;
	for( int i = 0; i < getNumberOfIntersections(); ++i )
  { r |= mIntersections[i].hasContacts(); }
  return r;
}
//------------------------------------------------------------------------------
void GameEntity::setAcceleration( const Vector2d& iV )
{ mAcceleration = iV; }
//------------------------------------------------------------------------------
void GameEntity::setBoundingBox( const Rectangle& iV )
{ 
	mBoundingBox = iV;
  mBoundingCircle.setRadius( sqrt( pow(iV.width()/2.0, 2) + 
  	pow( iV.height() / 2.0, 2 ) ) );
  
  const Stage& s = mpEngine->getStage();
  const Vector2d bbSize = mBoundingBox.size();
  int kernel = max( bbSize.x(), bbSize.y() ) / 
  	max( s.getCellSize().x(), s.getCellSize().y() ) * 3;
  kernel = max(kernel, 3); //le kernel doit au minimum etre de 3.
  if( kernel % 2 == 0 ) ++kernel; //kernel ne doit pas etre pair
	mCollisionSearchGrid.set( kernel, kernel );
}
//------------------------------------------------------------------------------
void GameEntity::setBoundingCircle( const Circle& iV )
{ mBoundingCircle = iV; }
//------------------------------------------------------------------------------
void GameEntity::setMaximumAcceleration( const Vector2d& iV )
{ mMaximumAcceleration = iV; }
//------------------------------------------------------------------------------
void GameEntity::setMaximumVelocity( const Vector2d& iV )
{ mMaximumVelocity = iV; }
//------------------------------------------------------------------------------
void GameEntity::setPosition( const Point2d& iV )
{ mPosition = iV; }
//------------------------------------------------------------------------------
void GameEntity::setSprite( QString iV )
{ 
	mCurrentSpriteToken = iV;
  if( mpEngine )
  { 
  	mSprite = mpEngine->getSpriteCatalog().getSprite( getCurrentSpriteToken() );
    mSprite.startAnimation();
  }
  Vector2i frameSize = mSprite.isValid() ? mSprite.getFrameSize() : Vector2i(40, 40);
  setBoundingBox( Rectangle(Point2d( 0.0 ), frameSize) );
}
//------------------------------------------------------------------------------
void GameEntity::setSpriteToken( int i, QString iV )
{ mSpriteTokens[i] = iV; }
//------------------------------------------------------------------------------
void GameEntity::setVelocity( const Vector2d& iV )
{ mVelocity = iV; }

//------------------------------------------------------------------------------
//--- Actor
//------------------------------------------------------------------------------
Actor::Actor() : GameEntity(),
 mName("no name"),
 mHealth( 100.0 ),
 mState( sIdle ),
 mWeapon(),
 mAimingDirection( 1.0, 0.0 )
{}
//------------------------------------------------------------------------------
Actor::~Actor() {}
//------------------------------------------------------------------------------
void Actor::attack()
{
	Weapon w = getWeapon();
  if( w.canFire() )
  {
    Projectile* p = w.fire( getAimingDirection() ); 
    p->setPosition( getPosition() );
    mpEngine->addProjectile( p );
    
    setWeapon( w );
  }
}
//------------------------------------------------------------------------------
double Actor::getHealth() const
{ return mHealth; }
//------------------------------------------------------------------------------
QString Actor::getName() const
{ return mName; }
//------------------------------------------------------------------------------
Actor::state Actor::getState() const
{ return mState; }
//------------------------------------------------------------------------------
const Weapon& Actor::getWeapon() const
{ return mWeapon; }
//------------------------------------------------------------------------------
void Actor::moveLeft()
{
	Vector2d a = getAcceleration();
  state s = getState();
  const double maxHAccel = getMaximumAcceleration().x();
  
  switch ( s ) 
  {
  case sIdle :
    a -= Vector2d(maxHAccel, 0);
    s = sRunningLeft;
    break;
  case sRunningLeft :
  case sRunningRight :
  case sFalling :
  case sJumping :
    a -= Vector2d(maxHAccel, 0);
    break;
  default: break;
  }
  setAcceleration( a );
  setState( s );
}

//------------------------------------------------------------------------------
void Actor::moveRight()
{
	Vector2d a = getAcceleration();
  Actor::state s = getState();
  const double maxHAccel = getMaximumAcceleration().x();
  
  switch ( s ) {
  case sIdle :
      a += Vector2d(maxHAccel, 0);
      s = sRunningRight;
    break;
  case sRunningLeft :
  case sRunningRight :
  case sFalling :
  case sJumping :
    a += Vector2d(maxHAccel, 0);
    break;
  default: break;
  }
  setAcceleration( a );
  setState( s );
}

//------------------------------------------------------------------------------
void Actor::moveUp()
{
  Vector2d v = getVelocity();
  Actor::state s = getState();
  switch ( s ) {
  case sIdle :
  case sRunningLeft :
  case sRunningRight :
    v += Vector2d(0, 400);
    s = sJumping;
    break;
  case sJumping:
  	v += Vector2d(0, 8);
  break;
  default: break;
  }
  setVelocity( v );
  setState( s );
}
//------------------------------------------------------------------------------
void Actor::setHealth( double iV )
{ mHealth = iV; }
//------------------------------------------------------------------------------
void Actor::setName( QString iV )
{ mName = iV; }
//------------------------------------------------------------------------------
void Actor::setState( state iV )
{ 
	if( mState != iV )
  {
    switch (getState()) 
    {
      case sHit: 
        switch (iV) 
        {
          case sHit: break;
          default: if( mHitTimer.elapsed() > 500 ) {mState = iV;} break;
        }
      break;
      default:
        switch (iV) 
        {
          case sHit: mHitTimer.start(); mState = iV; break;
          default: mState = iV; break;
        } 
        break;
    }
    setSprite( getSpriteToken( mState ) );
  }
}
//------------------------------------------------------------------------------
void Actor::setWeapon( const Weapon& iW )
{ mWeapon = iW; }
//------------------------------------------------------------------------------
void Actor::updateState()
{
	Vector2d accel = getAcceleration();
  Vector2d vel = getVelocity();
  Point2d pos = getPosition();
  
	if( isEqual( vel.y(), 0.0 ) && hasIntersections() &&
  	getIntersection(0).getContact(0).y() < getPosition().y() )
  { 
  	vel.x() > 0 ? setState( Actor::sRunningRight ) : 
      setState( Actor::sRunningLeft );
  }
  
  //friction en x. seulement si il n'y a pas d'input usagé
  //ou changment de direciton
  bool applyFriction = ! ( (mpEngine->isKeyPressed( Qt::Key_Left ) || 
  	mpEngine->isKeyPressed( Qt::Key_Right ) ) ) || 
    accel.x() / fabs(accel.x()) !=  vel.x() / fabs(vel.x());
  if(applyFriction && 
  	(getState() == Actor::sRunningLeft ||
     getState() == Actor::sRunningRight) )
		vel.setX( vel.x() * 0.8 );

  if( vel.y() < 0.0 )
  {
  	switch (getState()) {
      case Actor::sIdle:
      case Actor::sRunningLeft:
      case Actor::sRunningRight:
      case Actor::sJumping:
      	setState( Actor::sFalling );
        break;
      default: break;
    }
  }
  
  if( vel.norm() < 0.01 )
  { setState( Actor::sIdle ); }
  
  setPosition( pos );
  setVelocity( vel );
}

//------------------------------------------------------------------------------
//--- Player
//------------------------------------------------------------------------------
Player::Player() : Actor()
{ 
  setName( "Player1" );
  setSpriteToken( Actor::sIdle, "player idle");
  setSpriteToken( Actor::sRunningLeft, "player run left");
  setSpriteToken( Actor::sRunningRight, "player run right");
  setSpriteToken( Actor::sFalling, "player idle");
  setSpriteToken( Actor::sJumping, "player idle");
  setSpriteToken( Actor::sHit, "player idle");
  
Weapon we = getWeapon();
we.setType( Weapon::tGrenade );
setWeapon( we );
}

//------------------------------------------------------------------------------
void Player::handleUserInput()
{
	assert( mpEngine );
  
  Vector2d aimingDir = getAimingDirection();
  //input usagé
  if( mpEngine->isKeyPressed( Qt::Key_Left ) )
  { 
  	moveLeft();
    aimingDir.set( -1.0, 0.0 );
  }
  if( mpEngine->isKeyPressed( Qt::Key_Right ) )
  {
  	moveRight();
    aimingDir.set( 1.0, 0.0 );
  }
  if( mpEngine->isKeyPressed( Qt::Key_Up ) )
  {
    aimingDir.set( 0.0, 1.0 );
  }
  if( mpEngine->isKeyPressed( Qt::Key_Down ) )
  {
    aimingDir.set( 0.0, -1.0 );
  }
  if( mpEngine->isKeyPressed( Qt::Key_Z ) )
  { moveUp(); }
  if( mpEngine->isKeyPressed( Qt::Key_X, true ) )
  { attack(); }
  
  setAimingDirection( aimingDir );
}


//------------------------------------------------------------------------------
void Player::update()
{
  Physics& p = mpEngine->getPhysics();
  Stage& s = mpEngine->getStage();
  handleUserInput();
  p.update( *this );
  p.resolveCollisions( *this, s );
  
  for( int i = 0; i < s.getNumberOfMonsters(); ++i )
  { p.resolveCollisions( *this, s.getMonster(i) ); }
  
	Actor::updateState();
}

//------------------------------------------------------------------------------
//--- Monster
//------------------------------------------------------------------------------
Monster::Monster() : Actor()
{ setType(mtBrownSlime); }

Monster::Monster(monsterType iT) : Actor()
{ setType(iT); }

//------------------------------------------------------------------------------
void Monster::setType( monsterType iType )
{
	mType = iType;
  switch (mType) 
  {
    case mtBrownSlime:
      setName( "Brown Slime" );
      setSpriteToken( Actor::sIdle, "monstre bidon1 idle");
      setSpriteToken( Actor::sRunningLeft, "monstre bidon1 run left");
      setSpriteToken( Actor::sRunningRight, "monstre bidon1 run right");
      setSpriteToken( Actor::sFalling, "monstre bidon1 idle");
      setSpriteToken( Actor::sJumping, "monstre bidon1 idle");
      setSpriteToken( Actor::sHit, "monstre bidon1 idle");
      setSpriteToken( Actor::sDead, "explosion");
      setHealth(20);
      setMaximumVelocity( Vector2d(0, 1000) );
      break;
    case mtBigGreen:
      setName( "Big Green" );
      setSpriteToken( Actor::sIdle, "monstre bidon2 idle");
      setSpriteToken( Actor::sRunningLeft, "monstre bidon2 run left");
      setSpriteToken( Actor::sRunningRight, "monstre bidon2 run right");
      setSpriteToken( Actor::sFalling, "monstre bidon2 idle");
      setSpriteToken( Actor::sJumping, "monstre bidon2 idle");
      setSpriteToken( Actor::sHit, "monstre bidon2 idle");
      setSpriteToken( Actor::sDead, "explosion");
      setHealth(50);
      setMaximumVelocity( Vector2d(20, 1000) );
      break;
    default: break;
  }
}

//------------------------------------------------------------------------------
void Monster::update()
{ 
  Physics& p = mpEngine->getPhysics();
  Stage& s = mpEngine->getStage();
  updateAi();
  p.update( *this );
  p.resolveCollisions( *this, s );
  
  Actor::updateState();
  if( getHealth() <= 0.0 )
  {
  	setState( sDead );
    //ajout d'une animation d'éclatement de l'acteur.
    mpEngine->addAnimation( getSprite(), getPosition() );
		markForDeletion( true );  
  }
}

//------------------------------------------------------------------------------
void Monster::updateAi()
{
	assert(mpEngine);
  //on les fait courrir après le joeur
  Point2d playerPos = mpEngine->getPlayer().getPosition();
  Point2d pos = getPosition();
  Vector2d dir = playerPos - pos;
  if( dir.x() < 0 ) moveLeft();
  else moveRight();
  
  if( dir.y() > 0 ) moveUp();
}

//------------------------------------------------------------------------------
//--- Projectile
//------------------------------------------------------------------------------
Projectile::Projectile() : GameEntity(),
  mType( Weapon::tNone ),
  mDamage( 1.0 ),
  mState( sIdle ),
  mStart(),
  mLifeSpan(1000)
{ mStart.start(); }

Projectile::~Projectile()
{}

//------------------------------------------------------------------------------
int Projectile::getRemainingLife() const
{ return getLifeSpan() - mStart.elapsed(); }

//------------------------------------------------------------------------------
void Projectile::setState( state iS )
{
	if( mState != iS )
  {
  	mState = iS;
    setSprite( getSpriteToken( mState ) );
  }
}

//------------------------------------------------------------------------------
void Projectile::setType( Weapon::type iT )
{
	mType = iT;
  setMaximumVelocity( Vector2d( 2000, 2000 ) );
  setMaximumAcceleration( Vector2d( 2000, 2000 ) );
  switch ( getType() ) 
  {
    case Weapon::tPellet:
      setSpriteToken( sHorizontal, "pellet bullet horizontal");
      setSpriteToken( sVertical, "pellet bullet vertical");
      setSpriteToken( sExploding, "pellet bullet explode");
      setDamage( 10.0 );
      setVelocity( Vector2d(800, 800) );
      setLifeSpan(400);
      applyGravity(false);
      setCollisionElasticity( 0.0 );
    break;
    case Weapon::tGrenade:
      setSpriteToken( sHorizontal, "pellet bullet horizontal");
      setSpriteToken( sVertical, "pellet bullet vertical");
      setSpriteToken( sExploding, "explosion");
      setDamage( 5.0 );
      setVelocity( Vector2d(600, 400) );
      setLifeSpan(2000);
      applyGravity(true);
      setCollisionElasticity( 0.4 );
    break;
    default: break;
  }
}

//------------------------------------------------------------------------------
void Projectile::update()
{
  Physics& p = mpEngine->getPhysics();
  Stage& s = mpEngine->getStage();
  
  p.update( *this );
  p.resolveCollisions( *this, s );
  
  for( int i = 0; i < s.getNumberOfMonsters(); ++i )
  { p.resolveCollisions( *this, s.getMonster(i) ); }
  
	updateState();

  switch( getType() )
  {
  case Weapon::tPellet:
  {
    if( hasIntersections() || getRemainingLife() <= 0)
  	{
      setState( sExploding );
      markForDeletion( true );
      
      Point2d pos = hasIntersections() ? getIntersection(0).getContact(0) :
       getPosition();
      //ajout d'une animation d'éclatement du projectile.
      mpEngine->addAnimation( getSprite(), pos );
    }
  }
  break;
  case Weapon::tGrenade:
  {
  	if( getRemainingLife() <= 0 )
    {
      setState( sExploding );
      //p.explode( getIntersection(0).getContact(0) )
      //ajout d'une animation d'éclatement du projectile.
      mpEngine->addAnimation( getSprite(), getPosition() );
      markForDeletion( true );
    }
  }break;
  default: break;
  }

}

//------------------------------------------------------------------------------
void Projectile::updateState()
{
  Vector2d v = getVelocity();
  if( isEqual( v.x(), 0.0 ) )
    setState( sVertical );
  else
    setState( sHorizontal );
}

//------------------------------------------------------------------------------
//---Weapon
//------------------------------------------------------------------------------
Weapon::Weapon() : mType( tNone ),
mLastFire(),
mFireRate( 1.0 )
{ mLastFire.start(); }

Weapon::~Weapon()
{}

//------------------------------------------------------------------------------
bool Weapon::canFire() const
{	return mLastFire.elapsed() > (1.0 / getFireRate() * 1000);}
//------------------------------------------------------------------------------
Projectile* Weapon::fire( const Vector2d& iDir )
{
	Projectile* p = new Projectile();
  mLastFire.start();
  switch (getType()) 
  {
    case tPellet:
    {
    	p->setType( tPellet );
      Vector2d v = p->getVelocity();
      p->setVelocity( Vector2d( v.x() * iDir.x(), v.y() * iDir.y() ) );
    }break;
    case tGrenade:
    {
      p->setType( tGrenade );
      Vector2d v = p->getVelocity();
    	v.setX( v.x() * iDir.x() );
      p->setVelocity(v);
    }break;
    default: break;
  }
  return p;
}
//------------------------------------------------------------------------------
double Weapon::getFireRate() const
{ return mFireRate; }
//------------------------------------------------------------------------------
Weapon::type Weapon::getType() const
{ return mType; }
//------------------------------------------------------------------------------
void Weapon::setFireRate( double f )
{ mFireRate = f; }
//------------------------------------------------------------------------------
void Weapon::setType(type t)
{ 
	mType = t;
  switch (getType()) 
  {
    case tPellet:
    	setFireRate( 8.0 );
    break;
    case tGrenade:
    	setFireRate( 12.0 );
    break;
    default: break;
  }
}
