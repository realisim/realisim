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
// mAcceleration( 0.0 ),
 mForces(),
 mMaximumAcceleration( 800, 800 ),
 mCollisionElasticity( 0.0 ),
 mFrictionCoefficient( 0.2 ),
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
void GameEntity::addForce( const Vector2d& iF )
{ mForces.push_back(iF); }
//------------------------------------------------------------------------------
void GameEntity::addIntersection( const Intersection2d& iV )
{ mIntersections.push_back(iV); }
//------------------------------------------------------------------------------
void GameEntity::clearIntersections()
{ mIntersections.clear(); }
//------------------------------------------------------------------------------
Vector2d GameEntity::getAcceleration() const
{ 
	Vector2d r;
  for( int i = 0; i < getNumberOfForces(); ++i) {r+=mForces[i];}
	return r; 
}
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
Vector2d GameEntity::getForce(int i) const 
{return mForces[i]; } 
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
void GameEntity::resetForces()
{ mForces.clear(); }
//------------------------------------------------------------------------------
//void GameEntity::setAcceleration( const Vector2d& iV )
//{ mAcceleration = iV; }
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
	Vector2d f;
  state s = getState();
  const double maxHAccel = getMaximumAcceleration().x();
  
  switch ( s ) 
  {
  case sIdle :
    f = -Vector2d(maxHAccel, 0);
    s = sRunningLeft;
    break;
  case sRunningLeft :
  case sRunningRight :
  case sFalling :
  case sJumping :
    f = -Vector2d(maxHAccel, 0);
    break;
  default: break;
  }
  addForce( f );
  setState( s );
}

//------------------------------------------------------------------------------
void Actor::moveRight()
{
	Vector2d f;
  Actor::state s = getState();
  const double maxHAccel = getMaximumAcceleration().x();
  
  switch ( s ) {
  case sIdle :
      f = Vector2d(maxHAccel, 0);
      s = sRunningRight;
    break;
  case sRunningLeft :
  case sRunningRight :
  case sFalling :
  case sJumping :
    f = Vector2d(maxHAccel, 0);
    break;
  default: break;
  }
  addForce( f );
  setState( s );
}

//------------------------------------------------------------------------------
void Actor::jump()
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

printf("ps %d, ns %d\n", getState(), iV);
	bool addAnimation = false;
	if( mState != iV )
  {
    switch (getState()) 
    {    
    	case sDead: break; // on ne sort pas de sDead	
      case sHit: 
        switch (iV) 
        {
          case sHit: break;
          case sDead: 
          	mState = sDead;
            addAnimation = true;
						markForDeletion( true );
            break;
          default: if( mHitTimer.elapsed() > 500 ) {mState = iV;} break;
        }
      break;
      default:
        switch (iV) 
        {
        	case sDead:
            mState = sDead;
            addAnimation = true;
						markForDeletion( true );
            break;
          case sHit: mHitTimer.start(); mState = iV; break;
          default: mState = iV; break;
        } 
        break;
    }
    setSprite( getSpriteToken( mState ) );
    if( addAnimation ) { printf("add anim mstate = %d\n", mState);mpEngine->addAnimation( getSprite(), getPosition() ); }
  }
}
//------------------------------------------------------------------------------
void Actor::setWeapon( const Weapon& iW )
{ mWeapon = iW; }
//------------------------------------------------------------------------------
void Actor::updateState()
{
  const Vector2d& v = getVelocity();
  
	if( isEqual( v.y(), 0.0 ) && hasIntersections() &&
  	getIntersection(0).getContact(0).y() < getPosition().y() )
  { 
  	v.x() > 0 ? setState( Actor::sRunningRight ) : 
      setState( Actor::sRunningLeft );
  }
  
  if( v.y() < 0.0 )
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
  
  if( v.normSquare() < 0.01 )
  { setState( Actor::sIdle ); }
  
  if( getHealth() <= 0.0 )
  { setState( sDead ); }
}

//------------------------------------------------------------------------------
//--- Player
//------------------------------------------------------------------------------
Player::Player() : Actor(),
	mDeathTimer( QTime() )
{ 
  setName( "Player1" );
  setSpriteToken( Actor::sIdle, "player idle");
  setSpriteToken( Actor::sRunningLeft, "player run left");
  setSpriteToken( Actor::sRunningRight, "player run right");
  setSpriteToken( Actor::sFalling, "player idle");
  setSpriteToken( Actor::sJumping, "player idle");
  setSpriteToken( Actor::sHit, "player idle");
  setSpriteToken( Actor::sDead, "explosion");
  setFrictionCoefficient( 0.2 );
  
  //Weapon w; w.setType(Weapon::tPellet);
  //addWeapon( w );
  //setWeapon(w);
  //w.setType(Weapon::tGrenade);
  //addWeapon( w );
  //setWeapon(w);
  //setWeapon( getWeapon(Weapon::tGrenade) );
}

//------------------------------------------------------------------------------
void Player::addWeaponToBag( const Weapon& iW)
{
	int i = getWeaponIndex( iW.getType() );
	if( i != -1 )
  {
//  	mWeaponBag[i].setNumberOfAmmo = mWeaponBag[i].getNumberOfAmmo() + 
//    	iW.getNumberOfAmmo();
  }
  else{ mWeaponBag.push_back( iW ); }
}

//------------------------------------------------------------------------------
Weapon Player::getWeaponFromBag( Weapon::type iT ) const
{
	Weapon r;
  int i = getWeaponIndex( iT);
  if( i != -1 ) r = getWeaponFromBag( i );
  return r;
}

//------------------------------------------------------------------------------
int Player::getWeaponIndex( Weapon::type iT ) const
{
	int r = -1;
  for(int i = 0; i < getNumberOfWeaponsInBag(); ++i)
  { 
  	if( mWeaponBag[i].getType() == iT )
    { r = i; break; }
  }
  return r;
}

//------------------------------------------------------------------------------
void Player::handleUserInput()
{
	assert( mpEngine );
  
  Vector2d aimingDir = getAimingDirection();
  //input usager pour déplacement
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
  { aimingDir.set( 0.0, 1.0 );}
  if( mpEngine->isKeyPressed( Qt::Key_Down ) )
  { aimingDir.set( 0.0, -1.0 ); }

  if( mpEngine->isKeyPressed( Qt::Key_Z ) )
  { jump(); }
  if( mpEngine->isKeyPressed( Qt::Key_X, true ) )
  { attack(); }
  
  setAimingDirection( aimingDir );
  
  //input usager pour changement d'arme
  if( mpEngine->isKeyPressed( Qt::Key_1 ) )
  { 
  	if( hasWeaponInBag( Weapon::tPellet ) )
    { setWeapon( getWeaponFromBag( Weapon::tPellet ) ); }
  }
  if( mpEngine->isKeyPressed( Qt::Key_2 ) )
  {
  	if( hasWeaponInBag( Weapon::tGrenade ) )
    { setWeapon( getWeaponFromBag( Weapon::tGrenade ) ); }
  }
  
  double frictionCoefficient = 0.2;
  if( mpEngine->isKeyPressed( Qt::Key_Left ) || 
  	mpEngine->isKeyPressed( Qt::Key_Right ) )
  {frictionCoefficient = 0.0;}
	setFrictionCoefficient( frictionCoefficient );
}

//------------------------------------------------------------------------------
void Player::setWeapon( const Weapon& iW )
{
	int i = getWeaponIndex( iW.getType() );
	if( i != -1 )
  { mWeaponBag[i] = iW; }
  else
  { mWeaponBag.push_back(iW); }
  mWeapon = iW;
}

//------------------------------------------------------------------------------
void Player::update()
{
  Physics& p = mpEngine->getPhysics();
  Stage& s = mpEngine->getStage();
  handleUserInput();
  p.update( *this );
  p.resolveCollisions( *this, s );
  
  //intersection avec les monstres
  for( int i = 0; i < s.getNumberOfMonsters(); ++i )
  {
  	Monster& m = s.getMonster(i);
    Intersection2d x = p.checkCollisions( *this, m );
  	if( x.hasContacts() )
    {
    	addIntersection(x);
      for( int i = 0; i < x.getNumberOfContacts(); ++i )
      { addForce( x.getNormal(i) * 4000 ); }
      
      if( getState() != Actor::sHit )
      { setHealth( getHealth() - 20 ); }
    	setState( Actor::sHit );      
    }
  }
  
  //intersection avec les armes
  for( int i = 0; i < s.getNumberOfWeapons(); ++i )
  {
  	Weapon& w = s.getWeapon(i);
  	if( p.checkCollisions( *this, w ).hasContacts() )
    {
    	setWeapon(w);
    	w.markForDeletion(true);
    }
  }
  
  Actor::updateState();  
  if( getState() == sDead )
  {
    for( int i = 0; i < getNumberOfWeaponsInBag(); ++i )
    {
      Weapon* w = new Weapon( getWeaponFromBag(i) );
      w->setPosition( getPosition() );
      w->setVelocity( Vector2d( 1000 * i, 400 ) );
      mpEngine->getStage().add( w );
    }
    mWeaponBag.clear();
    mpEngine->startLevel( 2000 );
  }
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
      setMaximumAcceleration( Vector2d(0, 1000) );
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
      setMaximumAcceleration( Vector2d(20, 1000) );
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
  
  if( dir.y() > 0 ) jump();
}

//------------------------------------------------------------------------------
//--- Projectile
//------------------------------------------------------------------------------
Projectile::Projectile() : GameEntity(),
  mType( Weapon::tNone ),
  mDamage( 1.0 ),
  mExplosionDamage(0.0),
  mState( sIdle ),
  mStart(),
  mLifeSpan(1000)
{ mStart.start(); }

Projectile::~Projectile()
{}

//------------------------------------------------------------------------------
int Projectile::getRemainingLife() const
{ return getLifeSpan() < 0 ? 1 : getLifeSpan() - mStart.elapsed(); }

//------------------------------------------------------------------------------
void Projectile::setState( state iS )
{
	if( mState != iS )
  {
  	mState = iS;
    setSprite( getSpriteToken( mState ) );
    
    switch (mState) 
    {
      case sExploding:
        //ajout d'une animation d'éclatement du projectile.
        mpEngine->addAnimation( getSprite(), getPosition() );
        markForDeletion( true );
        break;
      default: break;
    }
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
      setLifeSpan(400);
      applyGravity(false);
      setCollisionElasticity( 0.0 );
    break;
    case Weapon::tGrenade:
      setSpriteToken( sHorizontal, "weapon grenade");
      setSpriteToken( sVertical, "weapon grenade");
      setSpriteToken( sExploding, "explosion");
      setDamage( 0.0 );
      setExplosionDamage( 200 );
      setLifeSpan(3000);
      applyGravity(true);
      setCollisionElasticity( 0.4 );
      setFrictionCoefficient( 0.5 );
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
  { 
  	Monster& m = s.getMonster(i);
    Intersection2d x = p.checkCollisions( *this, m );
  	if( x.hasContacts() )
    {
    	addIntersection(x);
    	m.setHealth( m.getHealth() - getDamage() );
    }
  }
	updateState();
}

//------------------------------------------------------------------------------
void Projectile::updateState()
{
  Vector2d v = getVelocity();
  if( isEqual( v.x(), 0.0 ) )
    setState( sVertical );
  else
    setState( sHorizontal );
  
  switch( getType() )
  {
  case Weapon::tPellet:
  {
    if( hasIntersections() || getRemainingLife() <= 0)
  	{ setState( sExploding ); }
  }
  break;
  case Weapon::tGrenade:
  {
  	if( getRemainingLife() <= 0 )
    {
      setState( sExploding );      
      Physics& p = mpEngine->getPhysics();
      p.explode( getPosition(), 32 * 8, getExplosionDamage(), *mpEngine );
    }
  }break;
  default: break;
  }
}

//------------------------------------------------------------------------------
//---Weapon
//------------------------------------------------------------------------------
Weapon::Weapon() : mType( tNone ),
mLastFire(),
mFireRate( 0.0 )
{ mLastFire.start(); }

Weapon::~Weapon()
{}

//------------------------------------------------------------------------------
bool Weapon::canFire() const
{	return getFireRate() > 0 && mLastFire.elapsed() > (1.0 / getFireRate() * 1000);}
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
      Vector2d v = 800 * iDir;
      p->setVelocity( v );
    }break;
    case tGrenade:
    {
      p->setType( tGrenade );
      Vector2d v( 600 * iDir.x(), 200 );
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
    	setSpriteToken( 0, "icon weapon pellet" );
    	setFireRate( 8.0 );
    break;
    case tGrenade:
    	setSpriteToken( 0, "icon weapon grenade" );
    	setFireRate( 2.0 );
    break;
    default: break;
  }
}
//------------------------------------------------------------------------------
void Weapon::update()
{
	if( !getSprite().isValid() )
  { setSprite( getSpriteToken(0) ); }
  
  Physics& p = mpEngine->getPhysics();
  Stage& s = mpEngine->getStage();
  
  p.update( *this );
  p.resolveCollisions( *this, s );
}
