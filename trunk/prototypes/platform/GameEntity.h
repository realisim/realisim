/* */

#ifndef realisim_platform_GameEntity_hh
#define realisim_platform_GameEntity_hh

#include "Math/intersection.h"
#include "Math/Point.h"
#include "Math/Vect.h"
#include "3d/Sprite.h"
#include <QString>
#include <vector>

namespace realisim
{
namespace platform 
{
using namespace std;
using namespace math;
class Engine;
class Projectile;

//------------------------------------------------------------------------------
//--- GameEntity
//------------------------------------------------------------------------------
class GameEntity
{
public:
	GameEntity();
  virtual ~GameEntity() = 0;
  
  void addForce( const Vector2d& );
  void addIntersection( const Intersection2d& );
  void applyGravity(bool i) {mIsGravityApplied = i;}
  void clearIntersections();
  Vector2d getAcceleration() const;
  const Rectangle getBoundingBox() const;
  const Circle getBoundingCircle() const;
  double getCollisionElasticity() const {return mCollisionElasticity;}
  const Vector2d& getCollisionSearchGrid() const { return mCollisionSearchGrid; }
  QString getCurrentSpriteToken() const {return mCurrentSpriteToken;}
  Vector2d getForce(int i) const;
  double getFrictionCoefficient() const {return mFrictionCoefficient;}
  Intersection2d getIntersection(int i) const {return mIntersections[i];}
  const Vector2d& getMaximumAcceleration() const;
  const Vector2d& getMaximumVelocity() const;  
  int getNumberOfForces() const { return mForces.size(); }
  int getNumberOfIntersections() const {return mIntersections.size();}
  const Point2d& getPosition() const;
  const treeD::Sprite& getSprite() const { return mSprite; }
  QString getSpriteToken( int ) const;
  const Vector2d& getVelocity() const;
  bool hasIntersections() const;
  bool isGravityApplied() const {return mIsGravityApplied;}
  bool isMarkedForDeletion() const { return mMarkedForDeletion; }
  void markForDeletion( bool iD ) { mMarkedForDeletion = iD; }
  //void setAcceleration( const Vector2d& );
  void resetForces();
  void setBoundingBox( const Rectangle& );
  void setBoundingCircle( const Circle& );
  void setCollisionElasticity(double iE) {mCollisionElasticity = iE;}
  void setEngine( realisim::platform::Engine* iE ) {mpEngine = iE;}
  void setFrictionCoefficient( double iC ) {mFrictionCoefficient = iC;}
  void setMaximumAcceleration( const Vector2d& );
  void setMaximumVelocity( const Vector2d& );
  void setPosition( const Point2d& );
  void setSprite( QString );
  void setSpriteToken( int, QString );
  void setVelocity( const Vector2d& );
  virtual void update() = 0;
  
protected:
  std::map<int, QString> mSpriteTokens;
  QString mCurrentSpriteToken;
  Rectangle mBoundingBox;
  Circle mBoundingCircle;
  Point2d mPosition;
  Vector2d mVelocity;
  Vector2d mMaximumVelocity;
//  Vector2d mAcceleration;
  std::vector<Vector2d> mForces;
  Vector2d mMaximumAcceleration;
  double mCollisionElasticity;
  double mFrictionCoefficient;
  Vector2d mCollisionSearchGrid;
  vector<Intersection2d> mIntersections;
  bool mIsGravityApplied;
  realisim::platform::Engine* mpEngine;
  treeD::Sprite mSprite;
  bool mMarkedForDeletion;
  
};

//------------------------------------------------------------------------------
// --- Weapon
//------------------------------------------------------------------------------
class Weapon : public GameEntity
{
public:
  Weapon();
  virtual ~Weapon();
  
  enum type{ tNone, tPellet, tGrenade };
  
  bool canFire() const;
  Projectile* fire( const Vector2d& );
  double getFireRate() const;
  type getType() const;
  void setFireRate( double );
  void setType(type t);
  virtual void update();
  
protected:
	type mType;
  QTime mLastFire;
  double mFireRate;
//  int mRounds;
};

//------------------------------------------------------------------------------
// --- Projectile
//------------------------------------------------------------------------------
class Projectile : public GameEntity
{
public:
  Projectile();
  virtual ~Projectile();
  
  enum projectileType{ ptBasic ,ptBullet, ptGrenade };
  enum state{ sIdle, sHorizontal, sVertical, sExploding };
  
  double getDamage() const { return mDamage; }
  double getExplosionDamage() const { return mExplosionDamage; }
  projectileType getType() const {return mType;}
  void setDamage( double d ) { mDamage = d; }
  void setExplosionDamage( double d ) { mExplosionDamage = d; }
  void setType( projectileType t);
  virtual void update();
 	int getLifeSpan() const {return mLifeSpan;}
 	void setLifeSpan( int i ) { mLifeSpan = i; }
  
protected:
  int getRemainingLife() const;
	void setState( state );
  void updateState();
  
	projectileType mType;
  double mDamage;
  double mExplosionDamage;
  state mState;
	QTime mStart;
  int mLifeSpan; //ms
};

//------------------------------------------------------------------------------
//--- Actor
//------------------------------------------------------------------------------
class Actor : public GameEntity
{
public:
	Actor();
  virtual ~Actor() = 0;
  
  enum state{ sIdle, sRunningLeft, sRunningRight, sFalling,
    sJumping, sHit, sPushingLeft, sPushingRight, sHittingCeiling,
    sDead };
  
  void attack();
  Vector2d getAimingDirection() const {return mAimingDirection;}
  double getHealth() const;
  QString getName() const;
  state getState() const;
  const Weapon& getWeapon() const;
  void moveLeft();
  void moveRight();
  void jump();
  void setAimingDirection( const Vector2d& d ) {mAimingDirection = d;}
  void setHealth( double );
  void setName( QString );
  void setState( state );
  virtual void setWeapon( const Weapon& );
  
protected:
  void updateState();
	
  QString mName;
  double mHealth;
  state mState;
  QTime mHitTimer;
  Weapon mWeapon;
  Vector2d mAimingDirection;
};

//------------------------------------------------------------------------------
//--- Player
//------------------------------------------------------------------------------
class Player : public Actor
{
public:
  Player();
  virtual ~Player() {}
  virtual void update();
  
  void addWeaponToBag( const Weapon& );
  int getNumberOfWeaponsInBag() const {return mWeaponBag.size();}
  Weapon getWeaponFromBag(int i) const { return mWeaponBag[i]; }
  Weapon getWeaponFromBag( Weapon::type ) const;
  bool hasWeaponInBag( Weapon::type t) const { return getWeaponIndex(t) != -1; }
	virtual void setWeapon( const Weapon& );
  
protected:
	int getWeaponIndex( Weapon::type ) const;
  void handleUserInput();
    
  std::vector<Weapon> mWeaponBag;
  QTime mDeathTimer;
};

//------------------------------------------------------------------------------
//--- Monster
//------------------------------------------------------------------------------
class Monster : public Actor
{
  public:
    enum monsterType{ mtBrownSlime, mtBigGreen };
    
    Monster();
    Monster(monsterType);
    virtual ~Monster() {}
    
    monsterType getType() const {return mType;}
    void setType(monsterType);
    virtual void update();
    
  protected:
    void updateAi();
  	monsterType mType;
};


} //namespace platform
} //namespace realisim

#endif
