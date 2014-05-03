
#include <algorithm>
#include "Engine.h"
#include "Math/MathUtils.h"
#include "Math/Primitives.h"
#include "utils/utilities.h"
#include <QPainter>

using namespace std;
using namespace realisim;
	using namespace math;
	using namespace treeD;
	using namespace platform;

namespace 
{
	const double kDt = 0.015; //temps pour chaque step de l'engin en secondes
	const int kStageHeader = 0x9ab36ef2;
  
  const int kStageVersion = 5;
  const int kStageFistCompatibleVersion = 1;
}

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
//const vector<Intersection2d>& GameEntity::getIntersections() const
//{ return mIntersections; }
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
void Actor::update()
{
	Vector2d accel = getAcceleration();
  Vector2d vel = getVelocity();
  Point2d pos = getPosition();
  
	if( isEqual( vel.y(), 0.0 ) && hasIntersections() &&
  	getIntersection(0).getPoint(0).y() < getPosition().y() )
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
void Actor::updateAi()
{}

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
we.setType( Weapon::tPellet );
setWeapon( we );
}

//------------------------------------------------------------------------------
void Player::update()
{ Actor::update(); }

//------------------------------------------------------------------------------
void Player::updateAi()
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
	Actor::update();
  
  if( getHealth() <= 0.0 )
  {
  	setState( sDead );
    //ajout d'une animation d'éclatement de l'acteur.
    Animation* ani = new Animation();
    ani->setPosition( getPosition() );
    ani->setSprite( getSprite() );
    mpEngine->addAnimation( ani );
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
  mState( sIdle )
{}

Projectile::~Projectile()
{}

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
  switch ( getType() ) 
  {
    case Weapon::tPellet:
      setSpriteToken( sHorizontal, "pellet bullet horizontal");
      setSpriteToken( sVertical, "pellet bullet vertical");
      setSpriteToken( sExploding, "pellet bullet explode");
      setDamage( 10.0 );
      setMaximumVelocity( Vector2d( 400, 400 ) );
      setMaximumAcceleration( Vector2d( 400, 400 ) );
      applyGravity(false);
    break;
    default: break;
  }
}

//------------------------------------------------------------------------------
void Projectile::update()
{
  if( hasIntersections() )
  {
  	setState( sExploding );
    //ajout d'une animation d'éclatement du projectile.
    Animation* ani = new Animation();
    ani->setPosition( getIntersection(0).getPoint(0) );
    ani->setSprite( getSprite() );
    mpEngine->addAnimation( ani );
    markForDeletion( true );
  }
}

//------------------------------------------------------------------------------
void Projectile::updateAi()
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
      //p.setSpriteToken( p.getSpriteName() + " horizontal");
//    	if( isEqual( iDir.x(), 0.0 )  )
//    		p.setSpriteToken( p.getSpriteName() + " vertical");
      Vector2d v = p->getMaximumVelocity();
      p->setVelocity( Vector2d( v.x() * iDir.x(), v.y() * iDir.y() ) );
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
    default: break;
  }
}

//------------------------------------------------------------------------------
//---Stage
//------------------------------------------------------------------------------
Stage::Stage() :
  mpEngine(0),
	mName( "stage" ),
  mCellSize( 32, 32 ), 
	mTerrainSize( 30, 40 ),
	mTerrain( mTerrainSize.x() * mTerrainSize.y(), Stage::ctEmpty ),
  mLayers(),
  mActors()
{ mLayers.push_back( new Layer( getTerrainSize() ) ); }

Stage::Stage( QString iName, Vector2i iSize ) : 
  mpEngine(0),
	mName( iName ),
	mCellSize( 32, 32 ), 
	mTerrainSize( iSize ),
	mTerrain( mTerrainSize.x() * mTerrainSize.y(), Stage::ctEmpty ),
  mLayers(),
  mBackgroundToken(),
  mActors()
{ mLayers.push_back( new Layer( getTerrainSize() ) ); }

Stage::Stage( const Stage& iS) :
  mpEngine( iS.mpEngine ),
	mName( iS.getName() ),
  mCellSize( iS.getCellSize() ), 
	mTerrainSize( iS.getTerrainSize() ),
	mTerrain( iS.getTerrain() ),
  mLayers(),
  mBackgroundToken( iS.getBackgroundToken() ),
  mActors()
{ 
	for( int i = 0; i < iS.getNumberOfLayers(); ++i )
  { mLayers.push_back( new Layer( *iS.mLayers[i] ) ); }
    
//  for( int i = 0; i < iS.getNumberOfActors(); ++i )
//  { mActors.push_back( new Actor( *iS.mActors[i] ) ); }
	for( int i = 0; i < iS.getNumberOfMonsters(); ++i )
  { add( new Monster( *iS.mMonsters[i] ) ); }
}

Stage& Stage::operator=( const Stage& iS )
{
	mpEngine = iS.mpEngine;
	mName = iS.getName();
  mCellSize = iS.getCellSize();
  mTerrainSize = iS.getTerrainSize();
  mTerrain = iS.getTerrain();

  for( int i = 0; i < iS.getNumberOfLayers(); ++i )
  	mLayers.push_back( new Layer( *iS.mLayers[i] ) );
    
  mBackgroundToken = iS.getBackgroundToken();
  
//  for( int i = 0; i < iS.getNumberOfActors(); ++i )
//  { mActors.push_back( new Actor( *iS.mActors[i] ) ); }
	for( int i = 0; i < iS.getNumberOfMonsters(); ++i )
  { add( new Monster( *(iS.mMonsters[i]) ) ); }
  
	return *this;
}

Stage::~Stage()
{ clear(); }

//------------------------------------------------------------------------------
void Stage::add( Monster* a )
{ 
  a->setEngine(mpEngine);
	mActors.push_back( a );
  mMonsters.push_back( a );
}

//------------------------------------------------------------------------------
void Stage::addLayer()
{ mLayers.push_back( new Layer( getTerrainSize() ) ); }

//------------------------------------------------------------------------------
void Stage::addToken( int iLayer, QString iToken )
{
	vector<QString>& t = mLayers[iLayer]->mTokens;	
	if( std::find( t.begin(), t.end(), iToken ) == t.end() ) 
		t.push_back( iToken );
}

//------------------------------------------------------------------------------
void Stage::clear()
{
	mName = QString();
	mCellSize.set(0, 0);
  mTerrainSize.set(0, 0);
  mTerrain = QByteArray();
	for( int i = 0; i < getNumberOfLayers(); ++i )
  { delete mLayers[i]; }
  mLayers.clear();
  mBackgroundToken = QString();
  for( int i = 0; i < getNumberOfActors(); ++i )
  { delete mActors[i]; }
  mActors.clear();
  mMonsters.clear();
}

//------------------------------------------------------------------------------
/*retourne tous les indices de le map qui ont la valeure iCt*/
std::vector<int> Stage::find( cellType iCt ) const
{
	vector<int> r;
  for(int i = 0; i < mTerrain.size(); ++i)
  	if( (uchar)mTerrain[i] == iCt ) r.push_back(i);
  return r;
}

//------------------------------------------------------------------------------
void Stage::fromBinary( QByteArray iBa )
{
	clear();
  QDataStream in(&iBa, QIODevice::ReadOnly);
  
  qint32 header;
  quint32 version, firstCompatibleVersion, cellSizeX, cellSizeY,
  	terrainSizeX, terrainSizeY;
  in >> header;
  if (header != kStageHeader) 
  { 
  	printf("Format de fichier invalide."); 
    *this = Stage();
  	return;
  }
  in >> version;
  in >> firstCompatibleVersion;

	if( version >= firstCompatibleVersion )
  {
  	if( version >= 4 ) 
    { in >> mName; }
    else { mName = "stage"; }
  
    in >> cellSizeX;
    in >> cellSizeY;
    mCellSize.set( cellSizeX, cellSizeY );
    
    in >> terrainSizeX;
    in >> terrainSizeY;
    mTerrainSize.set( terrainSizeX, terrainSizeY );
    
    in >> mTerrain;
    
    if( version >= 2 )
    	in >> mBackgroundToken;
     
    if( version < 3 )
    	mLayers.push_back( new Layer( mTerrainSize ) );
      
    // ajout des layers
    if( version >= 3 )
    {
    	unsigned int numLayers = 0, numTokens = 0;
      QString token;
      in >> numLayers;
      for( uint i = 0; i < numLayers; ++i )
      {
      	Layer* l = new Layer( mTerrainSize );
        in >> l->mData;
        in >> numTokens;
        for( uint j = 0; j < numTokens; ++j )
        {
        	in >> token;
          l->mTokens.push_back( token );
        }
        
        if( version > 4 )
        { bool v; in >> v; l->mVisibility = v; }
        
        mLayers.push_back( l );
      }
    }
  }
  else 
  {
    printf( "La version de fichier du Stage est trop récente et n'est pas "
      "supportée." );
  }
}

//------------------------------------------------------------------------------
Actor& Stage::getActor( int i )
{ return *mActors[i]; }

//------------------------------------------------------------------------------
QString Stage::getBackgroundToken() const
{ return mBackgroundToken; }

//------------------------------------------------------------------------------
Vector2i Stage::getCellCoordinate( const Point2d& p ) const
{ return Vector2i( (int)p.x() / getCellSize().x(), p.y() / getCellSize().y() ); }

//------------------------------------------------------------------------------
Vector2i Stage::getCellCoordinate( int index ) const
{ return Vector2i(  index % getTerrainSize().x(), index / getTerrainSize().x() ); }

//------------------------------------------------------------------------------
/*Retourne l'index de la cellule au pixel p*/
int Stage::getCellIndex( const Point2d& p ) const
{ 
	Vector2i c = getCellCoordinate( p );
  return getCellIndex( c.x(), c.y() );
}

//------------------------------------------------------------------------------
//retourn l'index de la cellule x, y
int Stage::getCellIndex( int x, int y ) const
{ return y * getTerrainSize().x() + x; }

//------------------------------------------------------------------------------
vector<int> Stage::getCellIndices( const Point2d& iP, const Vector2i& iK ) const
{
	vector<int> r;
  //on trouve la coordonnée de la cellule pour la poisition iP
  Vector2i c = getCellCoordinate( iP );

  Vector2i terrainCell;
  for( int j = -iK.y() / 2; j <= iK.y() / 2; ++j )
  	for( int i = -iK.x() / 2; i <= iK.x() / 2; ++i )
    {
    	terrainCell = c + Vector2i( i, j );

			if( terrainCell.x() >= 0 && terrainCell.x() < getTerrainSize().x() &&
      	 terrainCell.y() >= 0 && terrainCell.y() < getTerrainSize().y() )
      r.push_back( terrainCell.y() * getTerrainSize().x() + terrainCell.x() );
    }
  
  return r;
}

//------------------------------------------------------------------------------
Vector2i Stage::getCellPixelCoordinate( int iIndex ) const
{
	Vector2i r = getCellCoordinate( iIndex );
  return getCellPixelCoordinate( r.x(), r.y() );
}

//------------------------------------------------------------------------------
Vector2i Stage::getCellPixelCoordinate( int iX, int iY ) const
{  return Vector2i( iX * getCellSize().x(), iY * getCellSize().y() ); }

//------------------------------------------------------------------------------
Monster& Stage::getMonster( int i )
{ return *mMonsters[i]; }

//------------------------------------------------------------------------------
int Stage::getNumberOfActors() const
{ return mActors.size(); }

//------------------------------------------------------------------------------
int Stage::getNumberOfMonsters() const
{ return mMonsters.size(); }

//------------------------------------------------------------------------------
/*retourne le token de sprite pour le layer iLayer de la cellule iIndex*/
QString Stage::getToken( int iLayer, int iIndex ) const
{
	QString r;
  if( iLayer >= 0 && iLayer < getNumberOfLayers() )
  {
  	int v = mLayers[iLayer]->mData[iIndex];
    if( v >= 0 && v < (int)mLayers[iLayer]->mTokens.size() )
  		r = mLayers[iLayer]->mTokens[v];
  }
  return r;
}

//------------------------------------------------------------------------------
/*retourne les token de sprites pour le layer iLayer*/
vector<QString> Stage::getTokens( int iLayer ) const
{
	vector<QString> r;
  if( iLayer >= 0 && iLayer < getNumberOfLayers() )
  	r = mLayers[iLayer]->mTokens;
  return r;
}

//------------------------------------------------------------------------------
bool Stage::hasCell( const Vector2i& iC ) const
{
	return iC.x() >= 0 && iC.x() < getTerrainWidth() &&
  	iC.y() >= 0 && iC.y() < getTerrainHeight();
}

//------------------------------------------------------------------------------
bool Stage::isLayerVisible( int i ) const
{
	bool r = false;
	if( i >= 0 && i < getNumberOfLayers() ) 
  { r = mLayers[i]->mVisibility; } 
	return r;
}

//------------------------------------------------------------------------------
void Stage::removeActor( int i )
{
  Actor* a = mActors[i];
	{
  	Monster* dc = dynamic_cast<Monster*> (a);
    if(dc)
    { mMonsters.erase( std::find( mMonsters.begin(), mMonsters.end(), dc ) ); }
  }

  delete a;
  mActors.erase( mActors.begin() + i );
}

//------------------------------------------------------------------------------
void Stage::removeLayer( int i )
{}

//------------------------------------------------------------------------------
void Stage::removeMonster( int i )
{
	Monster* m = mMonsters[i];
  removeActor( distance( mActors.begin(), 
  	std::find( mActors.begin(), mActors.end(), m ) ) );
}

//------------------------------------------------------------------------------
void Stage::setBackgroundToken( QString iBt )
{ mBackgroundToken = iBt; }

//------------------------------------------------------------------------------
void Stage::setLayerAsVisible( int iL, bool iV/*=true*/ )
{
	if( iL >= 0 && iL < getNumberOfLayers() )
  { mLayers[iL]->mVisibility = iV; }
}

//------------------------------------------------------------------------------
QByteArray Stage::toBinary() const
{
  QByteArray r;
  QDataStream out(&r, QIODevice::WriteOnly);

  //header
  out << (quint32)kStageHeader;
  //version courante
  out << (quint32)kStageVersion;
  out << (quint32)kStageFistCompatibleVersion;
  out.setVersion(QDataStream::Qt_4_7);
	
  //nom
  out << mName;
  
	//getCellSize
  out << (quint32)mCellSize.x();
  out << (quint32)mCellSize.y();
  //terrain size
  out << (quint32)mTerrainSize.x();
  out << (quint32)mTerrainSize.y();
  //terrain data
  out << mTerrain;
  
  //backgroundToken
  out << mBackgroundToken;
  
  //layers
  out << (quint32)getNumberOfLayers();
  for( int i = 0; i < getNumberOfLayers(); ++i )
  {
  	Layer* pl = mLayers[i];
  	out << pl->mData;
    out << (quint32)pl->mTokens.size();
    for( int j = 0; j < (int)pl->mTokens.size(); ++j )
    {
    	out << pl->mTokens[j];      
    }
    out << pl->mVisibility;
  }
  return r;
}

//------------------------------------------------------------------------------
unsigned char Stage::value(int iX, int iY) const
{ return value( iY * getTerrainSize().x() + iX ); }

//------------------------------------------------------------------------------
unsigned char Stage::value(int iIndex) const
{ return (unsigned char)getTerrain()[iIndex]; }

//------------------------------------------------------------------------------
//---Physics
//------------------------------------------------------------------------------
Physics::Physics()
{}
Physics::~Physics()
{}

//------------------------------------------------------------------------------
void Physics::resolveCollisions( GameEntity& iGe, Stage& iStage )
{
  //verification des collisions
  //on flush les collisions précedentes de l'acteur
  iGe.clearIntersections();
  
  const Vector2d bbSize = iGe.getBoundingBox().size();
  int kernel = max( bbSize.x(), bbSize.y() ) / 
  	max( iStage.getCellSize().x(), iStage.getCellSize().y() ) * 3;
  if( kernel % 2 == 0 ) ++kernel; //kernel ne doit pas etre pair
  vector<int> cells = iStage.getCellIndices( iGe.getPosition(), Vector2i(kernel) );
  vector< pair<int,Rectangle> > collidingCells;

  for( uint i = 0; i < cells.size(); ++i )
  {
  	if( iStage.value( cells[i] ) == Stage::ctGround )
    {
      //la coordonnée pixel de la cellule
      Vector2i cpc = iStage.getCellPixelCoordinate( cells[i] );
      Rectangle cellRect( toPoint(cpc), iStage.getCellSize() );
      Rectangle aRect = iGe.getBoundingBox();
      
      Intersection2d x = intersect( aRect, cellRect );
      if( x.hasContacts() )
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
      Intersection2d x = intersect( aRect, it->second );
      if( x.hasContacts() )
      {
        iGe.addIntersection( x );
        
        Point2d pos = iGe.getPosition();
        Vector2d v = iGe.getVelocity();
        Vector2d penetration = x.getPenetration();
        
        if( pc.y() == cc.y())
        {
        	penetration.setY( 0.0 );          
          v.setX( 0.0 );
        }
        else if( pc.x() == cc.x() )
        {
          penetration.setX( 0.0 );
          v.setY( 0.0 );
        }       	
          
        iGe.setPosition( pos - penetration );
        iGe.setVelocity( v );
        
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
        Intersection2d x = intersect( aRect, it->second );
        if( x.hasContacts() )
        {
          iGe.addIntersection( x );
          
          Point2d pos = iGe.getPosition();
          Vector2d v = iGe.getVelocity();
          Vector2d penetration = x.getPenetration();
          
          if( fabs(penetration.x()) <= fabs(penetration.y()) ) 
          { 
            penetration.setY( 0.0 );
            v.setX( 0.0 );
          }
          else 
          { 
            penetration.setX( 0.0 );
            v.setY(0.0);
          }     	
            
          iGe.setPosition( pos - penetration );
          iGe.setVelocity( v );
        }
      	it = collidingCells.erase(it);
      }
      else
      {++it;}
    }
		++count;
  }
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
void Physics::resolveCollisions( Projectile& p, Actor& a)
{
  if( intersects( p.getBoundingCircle(), a.getBoundingCircle() ) )
  {
    Intersection2d z = intersect( p.getBoundingBox(), a.getBoundingBox() );
    if( z.hasContacts() )
    { 
      p.addIntersection( z );
      
      //gestion du dommage pour l'acteur.
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
  v += a * kDt;
  v.setX( v.x() >= 0.0 ? 
  	min(maxHv, v.x()) : max(-maxHv, v.x()) );
  v.setY( v.y() >= 0.0 ? 
  	min(maxVv, v.y()) : max(-maxVv, v.y()) );
  
  iGe.setPosition( p + v * kDt );
  iGe.setVelocity( v );
  iGe.setAcceleration( a );
}

//------------------------------------------------------------------------------
//---Engine
//------------------------------------------------------------------------------
Engine::Engine() : QObject(), 
  mPhysics(),
  mState( sIdle ),
	mClients(),
  mTimerId(0),
  mMainMenuItem( mmiStart ),
  mConfigureMenuItem( cmiDifficulty ),
  mPauseMenuItem( pmiBack ),
  mKeys(),
  mMouseButtons(),
  mMouseWheelDelta(0.0),
  mMousePos( -1, -1 ),
  mMouseDelta(0, 0),
  mEditingTool( Stage::ctGround ),
  mCurrentLayer(0)
{
	int w = 800, h = 600;
  //mGameCamera.setProjection(-w / 2.0, w / 2.0,
//   -h/2.0, h/2.0, 0.0, 200, Camera::Projection::tOrthogonal, true);

	mGameCamera.setOrthoProjection( w, h, 0.0, 200 );
  mGameCamera.setWindowSize(w, h);
  mGameCamera.set( Point3d( 0.0, 0.0, 5.0 ),
  	Point3d( 0.0, 0.0, 0.0 ),
    Vector3d( 0.0, 1.0, 0.0 ) );
  
	goToState( sMainMenu );
}

Engine::~Engine()
{
	mClients.clear();
  
  for( int i = 0; i < getNumberOfProjectiles(); ++i )
  { delete mProjectiles[i]; mProjectiles.clear(); }
}

//------------------------------------------------------------------------------
void Engine::addError( QString e ) const
{ 
	mErrors += mErrors.isEmpty() ? e : "\n" + e;
  const_cast<Engine*>(this)->send( eErrorRaised );
}

//------------------------------------------------------------------------------
void Engine::addProjectile( Projectile* p )
{
	p->setEngine(this);
	mProjectiles.push_back(p);
}

//------------------------------------------------------------------------------
void Engine::addAnimation( Animation* a )
{ mAnimations.push_back(a); }

//------------------------------------------------------------------------------
void Engine::computeVisibleCells()

{
	const Camera& c = getGameCamera();
  const Stage& s = mStage;
  Point2d look( c.getTransformationToGlobal().getTranslation().getX(),
    c.getTransformationToGlobal().getTranslation().getY() );
  mVisibleCells = s.getCellIndices( look, Vector2i(
  	(int)ceil( c.getVisibleWidth() / s.getCellSize().x() ) + 1,
    (int)ceil( c.getVisibleHeight() / s.getCellSize().y() ) ) + 1 );

}

//------------------------------------------------------------------------------
QString Engine::getAndClearLastErrors() const
{
  QString r = mErrors;
  mErrors.clear();
  return r;
}

//------------------------------------------------------------------------------
int Engine::getCurrentLayer() const
{ return mCurrentLayer; }

//------------------------------------------------------------------------------
std::vector<QString> Engine::getConfigureMenuItems() const
{
  vector<QString> r( cmiCount, "" );
  for( int i = 0; i < cmiCount; ++i)
  {
  	switch (i) 
    {
      case cmiDifficulty: r[i] = "Difficulté"; break;
      case cmiBack: r[i] = "Retour au menu principal"; break;
      default: break;
    }
  }
  return r;
}

//------------------------------------------------------------------------------
Engine::configureMenuItem Engine::getCurrentConfigureMenuItem() const
{ return mConfigureMenuItem; }
  
//------------------------------------------------------------------------------
Engine::mainMenuItem Engine::getCurrentMainMenuItem() const
{ return mMainMenuItem; }

//------------------------------------------------------------------------------
Engine::pauseMenuItem Engine::getCurrentPauseMenuItem() const
{ return mPauseMenuItem; }

//------------------------------------------------------------------------------
vector<QString> Engine::getMainMenuItems() const
{
	vector<QString> r( mmiCount, "" );
  for( int i = 0; i < mmiCount; ++i)
  {
  	switch (i) 
    {
      case mmiStart: r[i] = "Jouer"; break;
      case mmiConfigure: r[i] = "Configuration"; break;
      case mmiQuit: r[i] = "Quitter"; break;
      default: break;
    }
  }
  return r;
}

//------------------------------------------------------------------------------
double Engine::getMouseWheelDelta(bool iConsume/*=true*/)
{ 
	double r = mMouseWheelDelta;
	if( iConsume ) {mMouseWheelDelta = 0.0;}
  return r;
}

//------------------------------------------------------------------------------
vector<QString> Engine::getPauseMenuItems() const
{
	vector<QString> r( pmiCount, "" );
  for( int i = 0; i < pmiCount; ++i)
  {
  	switch (i) 
    {
      case pmiBack: r[i] = "Retour au jeu"; break;
      case pmiEdit: r[i] = "Edition"; break;
      case pmiQuit: r[i] = "Quitter"; break;
      default: break;
    }
  }
  return r;
}

//------------------------------------------------------------------------------
realisim::utils::SpriteCatalog& Engine::getSpriteCatalog()
{ return mSpriteCatalog; }

//------------------------------------------------------------------------------
/*retourne l'index de toutes les cellules visibles de la tuile*/
const vector<int>& Engine::getVisibleCells() const
{ return mVisibleCells; }

//------------------------------------------------------------------------------
void Engine::goToState( state iS )
{
	if( getState() == iS )	return;

	switch (getState()) 
  {
  	case sIdle:
    	switch (iS) 
      {
        case sMainMenu:
        	mTimerId = startTimer( kDt * 1000.0 );
          mState = sMainMenu;
          break;
        default: break;
      }
    break;
  	case sMainMenu:
      switch (iS) 
      {
        case sIdle:
        	if (mTimerId != 0) { killTimer(mTimerId); mTimerId = 0; }
          mState = sIdle;
          break;
        case sConfigureMenu:
          mState = sConfigureMenu;
          break;
        case sPlaying:
          mState = sPlaying;
          break;
        case sQuitting:
          mState = sQuitting;
          send( eQuit );
          break;
        default: break;
      }
      break;
    case sConfigureMenu:
      switch (iS) 
      {
        case sMainMenu:
          mState = sMainMenu;
          break;
        default: break;
      }
      break;
    case sPlaying:
      switch (iS) 
      {
        case sPaused:
          mState = sPaused;
          break;
        default: break;
      }
      break;
    case sPaused:
      switch (iS) 
      {
      	case sPlaying:
          mState = sPlaying;
          break;
        case sEditing:
          mState = sEditing;
          break;
        case sQuitting:
          mState = sQuitting;
          send( eQuit );
          break;
        default: break;
      }
      break;
    case sEditing:
      switch (iS) 
      {
      	case sPaused:
        	{
          	QString p = mStageFilePath.isEmpty() ? mStage.getName() + ".bin" :
            	mStageFilePath;
            saveStage( p );
	          mState = sPaused;
          }
          break;
        default: break;
      }
      break;
    default: break;
  }
  
  send( eStateChanged );
}

//------------------------------------------------------------------------------
void Engine::graphicsAreReady()
{ loadStage("stage.bin"); }

//------------------------------------------------------------------------------
bool Engine::hasError() const
{ return mErrors.isEmpty(); }

//------------------------------------------------------------------------------
void Engine::handleConfigureMenu()
{
	//on commence par regarder l'input usager
  bool up = isKeyPressed( Qt::Key_Up, true );
  bool down = isKeyPressed( Qt::Key_Down, true );
  bool enter = isKeyPressed( Qt::Key_Return, true );
  
  switch (mConfigureMenuItem) 
  {
    case cmiDifficulty:
    	if(up) mConfigureMenuItem = cmiBack;
      if(down) mConfigureMenuItem = cmiBack;
    break;
    case cmiBack:
    	if(up) mConfigureMenuItem = cmiDifficulty;
      if(down) mConfigureMenuItem = cmiDifficulty;  
      if(enter) goToState(sMainMenu);
    break;
    default: break;
  }
}

//------------------------------------------------------------------------------
void Engine::handleEditing()
{	
  if( isKeyPressed( Qt::Key_Escape ) ) goToState( sPaused );
  
  Vector2d d(0.0);
  //input usagé
  if( isKeyPressed( Qt::Key_Left ) ) d -= Vector2d(5, 0);
  if( isKeyPressed( Qt::Key_Right ) ) d += Vector2d(5, 0);
  if( isKeyPressed( Qt::Key_Up ) ) d += Vector2d(0, 5);
  if( isKeyPressed( Qt::Key_Down ) ) d -= Vector2d(0, 5);
  
  //handleMapCollisions();
  
  //application de la gravité
  //d += Vector2d(0.0, -2);
  //déplacement du joueur
  mPlayer.setPosition( mPlayer.getPosition() + d );

  Point3d gl = mGameCamera.pixelToGL( getMousePos().x(), getMousePos().y() );
  Vector2i c = mStage.getCellCoordinate( Point2d( gl.getX(), gl.getY() ) );
  
  /*Si l'index courant est dans le terrain, on fait l'édition, sinon, on
    agrandit le terrain.*/
  if( mStage.hasCell( c ) )
  {
    int index = mStage.getCellIndex( c.x(), c.y() );
  	Stage::Layer* l = mStage.mLayers[ getCurrentLayer() ];
    if( isMousePressed( Qt::LeftButton ) )
    {
      mStage.mTerrain[index] = getEditingTool();    
      for( int i = 0; i < (int)l->mTokens.size(); ++i )
      {
        if( l->mTokens[i] == getEditingSpriteToken() )
        {
          l->mData[index] = i;
          break;
        }
      }
    }
    else if( isMousePressed( Qt::RightButton ) )
    {
      mStage.mTerrain[index] = Stage::ctEmpty;
      l->mData[index] = 255;
    }
  }
  else 
  { printf( "cell coord: %d, %d\n", c.x(), c.y() ); }


  //deplacement de la camera pour suivre le joueur
  Matrix4d m = mGameCamera.getTransformationToGlobal();
  m.translate( Vector3d( d.x(), d.y(), 0.0 ) );
  mGameCamera.setTransformationToGlobal( m );
}

//------------------------------------------------------------------------------
void Engine::handleMainMenu()
{
	//on commence par regarder l'input usager
  bool up = isKeyPressed( Qt::Key_Up, true );
  bool down = isKeyPressed( Qt::Key_Down, true );
  bool enter = isKeyPressed( Qt::Key_Return, true );
  
  switch (mMainMenuItem) 
  {
    case mmiStart:
    	if(up) mMainMenuItem = mmiQuit;
      if(down) mMainMenuItem = mmiConfigure;
      if(enter) goToState(sPlaying);
    break;
    case mmiConfigure:
    	if(up) mMainMenuItem = mmiStart;
      if(down) mMainMenuItem = mmiQuit;    
      if(enter) goToState(sConfigureMenu);
    break;
    case mmiQuit:
    	if(up) mMainMenuItem = mmiConfigure;
      if(down) mMainMenuItem = mmiStart;
      if(enter) goToState(sQuitting);
    break;
    default: break;
  }
}

//------------------------------------------------------------------------------
void Engine::handlePauseMenu()
{
	//on commence par regarder l'input usager
  bool up = isKeyPressed( Qt::Key_Up, true );
  bool down = isKeyPressed( Qt::Key_Down, true );
  bool enter = isKeyPressed( Qt::Key_Return, true );
  
  switch (mPauseMenuItem) 
  {
    case pmiBack:
    	if(up) mPauseMenuItem = pmiQuit;
      if(down) mPauseMenuItem = pmiEdit;
      if(enter) goToState(sPlaying);
    break;
    case pmiEdit:
    	if(up) mPauseMenuItem = pmiBack;
      if(down) mPauseMenuItem = pmiQuit;  
      if(enter) goToState(sEditing);
    break;
    case pmiQuit:
    	if(up) mPauseMenuItem = pmiEdit;
      if(down) mPauseMenuItem = pmiBack;  
      if(enter) goToState(sQuitting);
    break;
    default: break;
  }
}

//------------------------------------------------------------------------------
void Engine::handlePlaying()
{	
	//--- debuggage --
	const bool debug = false; bool iterate = false;
  if( isKeyPressed( Qt::Key_Plus, true ) ) iterate = true;
  if( debug && !iterate ) return;
  //--- fin debuggage
  
  if( isKeyPressed( Qt::Key_Escape ) ) goToState( sPaused );
  
  for( int i = 0; i < (int)mEntities.size(); ++i )
  {
  	/*remise des force à zero sur tout les acteurs en debut d'iteration.*/
    mEntities[i]->setAcceleration( Vector2d(0.0) );
  	mEntities[i]->updateAi();
    mPhysics.update( *mEntities[i] );
    mPhysics.resolveCollisions( *mEntities[i], getStage() );
  }
  
  //gestions des collisions acteur et monstre
  for( int i = 0; i < getStage().getNumberOfMonsters(); ++i)
  { 
  	mPhysics.resolveCollisions( mPlayer, getStage().getMonster(i) );
    //gestion des collisions projectiles et monstre
    for( int j = 0; j < getNumberOfProjectiles(); ++j )
    { mPhysics.resolveCollisions( getProjectile(j), getStage().getMonster(i) ); }
  }
  
  for( int i = 0; i < (int)mEntities.size(); ++i )
  { mEntities[i]->update(); }

  //deplacement de la camera pour suivre le joueur
  moveGameCamera();
  
  refreshGameEntityList();
}

//------------------------------------------------------------------------------
bool Engine::isKeyPressed( Qt::Key iK, bool iReset /*=false*/ )
{
	bool r = false;
  map<int, bool>::iterator it;
  if( (it = mKeys.find( iK )) != mKeys.end() )
  { 
  	r = it->second;
    if( iReset ) mKeys[iK] = false;
  }
  return r;
}

//------------------------------------------------------------------------------
bool Engine::isMousePressed( Qt::MouseButtons iB, bool iReset /*=false*/ )
{
	bool r = false;
  map<int, bool>::iterator it;
  if( (it = mMouseButtons.find( iB )) != mMouseButtons.end() )
  { 
  	r = it->second;
    if( iReset ) mMouseButtons[iB] = false;
  }
  return r;
}

//------------------------------------------------------------------------------
bool Engine::isVisible( const GameEntity& iA ) const
{
	bool r = false;
	const vector<int>& vc = getVisibleCells();
  if( !vc.empty() )
  {
    Rectangle rect;
    rect.setBottomLeft( toPoint(mStage.getCellPixelCoordinate( vc.front() ) ) );
    rect.setTopRight( toPoint(mStage.getCellPixelCoordinate( vc.back() ) + 
      mStage.getCellSize()) );
    
    r = rect.contains( iA.getPosition() );
  }
  
  return r;
}

//------------------------------------------------------------------------------
void Engine::keyPressed( int iKey )
{ mKeys[iKey] = true; }

//------------------------------------------------------------------------------
void Engine::keyReleased( int iKey )
{ mKeys[iKey] = false; }

//------------------------------------------------------------------------------  
void Engine::loadStage( const Stage& iS )
{
	setSpriteCatalog("level1.cat"); //faudrait le lire du stage
	mStage.clear();
  mStage = iS;
  
//  for( int i = 0; i < 250; ++i)
//  {
//  	s.addActor( new Monster( Monster::mtBrownSlime ) );
//    s.getActor(i).setPosition( Point2d(200 + i * 50, 100) );
//  }

mStage.add( new Monster( Monster::mtBrownSlime ) );
mStage.add( new Monster( Monster::mtBigGreen ) );
mStage.add( new Monster( Monster::mtBrownSlime ) );
mStage.add( new Monster( Monster::mtBigGreen ) );
mStage.getActor(0).setPosition( Point2d(200, 100) );
mStage.getActor(1).setPosition( Point2d(400, 800) );
mStage.getActor(2).setPosition( Point2d(280, 100) );
mStage.getActor(3).setPosition( Point2d(800, 800) );


  //init du joeur de la position du joueur
  mPlayer = Player();
  mPlayer.setEngine( this );
  mPlayer.setSprite( mPlayer.getSpriteToken( mPlayer.getState() ) );
  vector<int> start = getStage().find( Stage::ctStart );
  if( !start.empty() )
  	mPlayer.setPosition( toPoint(
    	getStage().getCellPixelCoordinate( start[0] ) + 
      getStage().getCellSize() / 2 ) );
  else
	  mPlayer.setPosition( Point2d( 10, 10 ) );
  
	//on met le joueur et la camera dans la premiere case
  Matrix4d m = mGameCamera.getTransformationToGlobal();
  m.setTranslation( Point3d( mPlayer.getPosition().x(), 
  	mPlayer.getPosition().y(), 0.0 ) );
  mGameCamera.setTransformationToGlobal( m );
  
  send( eStageLoaded );
}
//------------------------------------------------------------------------------
void Engine::loadStage( QString iPath )
{
	Stage s;
  s.fromBinary( utils::fromFile( iPath ) );
  s.setEngine(this);
  loadStage(s);
  mStageFilePath = iPath;
}  
  
//------------------------------------------------------------------------------
void Engine::mouseMoved( Point2i iPos )
{	
  if( mMousePos.x() != -1 && mMousePos.y() != -1 )
		mMouseDelta = iPos - mMousePos;
  mMousePos = iPos;
}

//------------------------------------------------------------------------------
void Engine::mousePressed( int iButton )
{ mMouseButtons[iButton] = true; }

//------------------------------------------------------------------------------
void Engine::mouseReleased( int iButton )
{ mMouseButtons[iButton] = false; }

//------------------------------------------------------------------------------
/*iD est l'angle en degré de rotation de la molette*/
void Engine::mouseWheelMoved( double iD )
{ mMouseWheelDelta = iD; }

//------------------------------------------------------------------------------
void Engine::moveGameCamera()  
{
  Matrix4d m = mGameCamera.getTransformationToGlobal();
  Point2d desired = mPlayer.getPosition();
  Point2d final = desired;
	Vector2d viewSize( mGameCamera.getVisibleWidth(),
  	mGameCamera.getVisibleHeight() );
  Rectangle desiredView(
    Point2d( desired.x() - mGameCamera.getVisibleWidth() / 2 ,
    	desired.y() - mGameCamera.getVisibleHeight() / 2 ),
    viewSize );
    
  const Stage& s = getStage();
  
  Point2d stageLowerLeft = toPoint( s.getCellPixelCoordinate(
  	s.getCellIndex( 0, 0 ) ) );
  Point2d stageUpperRight = toPoint( s.getCellPixelCoordinate(
  	s.getCellIndex( s.getTerrainSize().x() - 1, s.getTerrainSize().y() - 1 ) ) );
  stageUpperRight = stageUpperRight + Vector2d( s.getCellSize() );
	Rectangle stageRect( stageLowerLeft, stageUpperRight );

	//le stage fit en entier dans la vue
	if( stageRect.width() < desiredView.width() &&
  	stageRect.height() < desiredView.height() )
  {
  	final = Point2d( stageRect.left() + stageRect.width() / 2.0,
  		stageRect.bottom() + stageRect.height() / 2.0 );
  }
  else //le stage fit pas dans la vue
  {
    if( stageLowerLeft.x() > desiredView.left() )
    {
      desiredView.setLeft( stageLowerLeft.x() );
      desiredView.setRight( stageLowerLeft.x() + viewSize.x() );
    }
    if( stageLowerLeft.y() > desiredView.bottom() )
    {
      desiredView.setBottom( stageLowerLeft.y() );
      desiredView.setTop( stageLowerLeft.y() + viewSize.y() );
    }
    
    if( stageUpperRight.x() < desiredView.right() )
    {
      desiredView.setRight( stageUpperRight.x() );
      desiredView.setLeft( stageUpperRight.x() - viewSize.x() );
    }
    if( stageUpperRight.y() < desiredView.top() )
    {
      desiredView.setTop( stageUpperRight.y() );
      desiredView.setBottom( stageUpperRight.y() - viewSize.y() );
    }
    
    final = Point2d( desiredView.left() + desiredView.width() / 2.0,
  		desiredView.bottom() + desiredView.height() / 2.0 );
  }
  
  m.setTranslation( Point3d(final.x(), final.y(), 0.0) );
  mGameCamera.setTransformationToGlobal( m );
}

//------------------------------------------------------------------------------
void Engine::newStage( QString iName, int iX, int iY )
{
	loadStage( Stage( iName, Vector2i( iX, iY ) ) );
  mStageFilePath = QString();
}

//------------------------------------------------------------------------------
/*Cette méthode sert a faire quelques opérations de maintenance en début 
  d'itération de simulation de jeu. Elle met à jour la liste des entités de jeu
  et fait aussi le nettoyage des entités mortes.*/
void Engine::refreshGameEntityList()
{
  mEntities.clear();
  mEntities.push_back( &mPlayer );

  //ajout des acteurs et retrait des morts
  vector<Actor*>::iterator itActor = mStage.mActors.begin();
  while( itActor != mStage.mActors.end() )
  {
  	if( (*itActor)->isMarkedForDeletion() )
    {
    	{
      	Monster* dc = dynamic_cast<Monster*> (*itActor);
        if( dc )
        { mStage.mMonsters.erase( find( mStage.mMonsters.begin(),
        	mStage.mMonsters.end(), dc) ); }
      }
    	delete *itActor;
    	itActor = mStage.mActors.erase( itActor );
    }
    else
    {
      mEntities.push_back(*itActor);
    	itActor++;
    }
  }

	//on enleve le projectiles mort
	vector<Projectile*>::iterator itProjectile = mProjectiles.begin();
  while( itProjectile != mProjectiles.end() )
  {
  	if( (*itProjectile)->isMarkedForDeletion() )
    {
      delete *itProjectile;
      itProjectile = mProjectiles.erase( itProjectile );
    }
    else
    {
    	mEntities.push_back( *itProjectile );
    	++itProjectile;
    }
  }
  
  //on enleve les animations terminées
  vector<Animation*>::iterator itAnims = mAnimations.begin();
  while( itAnims != mAnimations.end() )
  {
  	if( (*itAnims)->isDone() )
    { 
    	delete *itAnims;
    	itAnims = mAnimations.erase( itAnims );
    }
    else{ ++itAnims; }
  }
}

//------------------------------------------------------------------------------
void Engine::registerClient( Client* ipC )
{
	vector<Client*>::iterator it = find(mClients.begin(), mClients.end(), ipC);
  if( it == mClients.end() )
  	mClients.push_back( ipC );
}

//------------------------------------------------------------------------------
void Engine::saveStage( QString iPath )
{ utils::toFile( iPath, getStage().toBinary() ); }  

//------------------------------------------------------------------------------
void Engine::send( event iE )
{
	for( size_t i = 0; i < mClients.size(); ++i )
  { mClients[i]->gotEvent( iE ); }
}

//------------------------------------------------------------------------------
void Engine::setCurrentLayer( int iL )
{
	if( iL >= 0 && iL < mStage.getNumberOfLayers() ) 
		mCurrentLayer = iL;
  else
  	mCurrentLayer = 0;
}

//------------------------------------------------------------------------------
void Engine::setSpriteCatalog( QString iPath )
{
	QByteArray ba = utils::fromFile(iPath);
	mSpriteCatalog.fromBinary( ba );
}

//------------------------------------------------------------------------------
QString Engine::toString( Stage::cellType iCt )
{
	QString r("indéfini");
  switch (iCt) 
  {
    case Stage::ctEmpty: r = "vide"; break;
    case Stage::ctStart: r = "départ"; break;
    case Stage::ctWayPoint: r = "waypoint"; break;
    case Stage::ctGround: r = "sol"; break;
    default: break;
  }
  return r;
}

//------------------------------------------------------------------------------
void Engine::timerEvent( QTimerEvent* ipE )
{
	if( ipE->timerId() == mTimerId )
  {
  	switch (getState()) 
    {
    	case sIdle: break;
      case sMainMenu: handleMainMenu(); break;
      case sConfigureMenu: handleConfigureMenu(); break;
      case sPlaying: handlePlaying(); break;
      case sEditing: handleEditing(); break;
      case sPaused: handlePauseMenu(); break;
      default: break;
    }
    
    computeVisibleCells();
  	send( eFrameDone );
  }
}

//------------------------------------------------------------------------------
void Engine::unregisterClient( Client* ipC )
{
	vector<Client*>::iterator it = find(mClients.begin(), mClients.end(), ipC);
  if( it != mClients.end() )
  	mClients.erase( it );
}