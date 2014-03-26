
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
//--- Actor
//------------------------------------------------------------------------------
Actor::Actor() : 
 mName("no name"),
 mSpriteName( "no sprite name" ),
 mSpriteToken( "no sprite token" ),
 mBoundingBox( Point2d(0.0), Vector2d( 5, 5 ) ),
 mBoundingCircle( Point2d(0.0), 5 ),
 mHealth( 100.0 ),
 mPosition( 0.0 ),
 mVelocity( 0.0 ),
 mMaximumVelocity( 400, 1000 ),
 mAcceleration( 0.0 ),
 mMaximumAcceleration( 800, 800 ),
 mState( sIdle ),
 mIntersections()
{}
//------------------------------------------------------------------------------
Actor::Actor( const Actor& iA ) : 
 mName( iA.getName() ),
 mSpriteName( iA.getSpriteName() ),
 mSpriteToken( iA.getSpriteToken() ),
 mBoundingBox( iA.getBoundingBox() ),
 mBoundingCircle( iA.getBoundingCircle() ),
 mHealth( iA.getHealth() ),
 mPosition( iA.getPosition() ),
 mVelocity( iA.getVelocity() ),
 mMaximumVelocity( iA.getMaximumVelocity() ),
 mAcceleration( iA.getAcceleration() ),
 mMaximumAcceleration( iA.getMaximumAcceleration() ),
 mState( iA.getState() ),
 mIntersections( iA.getIntersections() )
{}
//------------------------------------------------------------------------------
Actor& Actor::operator=( const Actor& iA )
{
  mName = iA.getName();
  mSpriteName = iA.getSpriteName();
  mSpriteToken = iA.getSpriteToken();
  mBoundingBox = iA.getBoundingBox();
  mBoundingCircle = iA.getBoundingCircle();
  mHealth = iA.getHealth();
  mPosition = iA.getPosition();
  mVelocity = iA.getVelocity();
  mAcceleration = iA.getAcceleration();
  mMaximumVelocity = iA.getMaximumVelocity();
  mMaximumAcceleration = iA.getMaximumAcceleration();
  mState = iA.getState();
  mIntersections = iA.getIntersections();
  return *this;
}
//------------------------------------------------------------------------------
Actor::~Actor() {}
//------------------------------------------------------------------------------
void Actor::addIntersection( const Intersection2d& iV )
{ mIntersections.push_back(iV); }
//------------------------------------------------------------------------------
void Actor::clearIntersections()
{ mIntersections.clear(); }
//------------------------------------------------------------------------------
const Vector2d& Actor::getAcceleration() const
{ return mAcceleration; }
//------------------------------------------------------------------------------
const math::Rectangle Actor::getBoundingBox() const
{
	return Rectangle( getPosition() - mBoundingBox.size() / 2,
    mBoundingBox.size() ); 
}
//------------------------------------------------------------------------------
const math::Circle Actor::getBoundingCircle() const
{ return Circle( getPosition(), mBoundingCircle.getRadius() );  }
//------------------------------------------------------------------------------
double Actor::getHealth() const
{ return mHealth; }
//------------------------------------------------------------------------------
const vector<Intersection2d>& Actor::getIntersections() const
{ return mIntersections; }
//------------------------------------------------------------------------------
QString Actor::getName() const
{ return mName; }
//------------------------------------------------------------------------------
const Vector2d& Actor::getMaximumAcceleration() const
{ return mMaximumAcceleration; }
//------------------------------------------------------------------------------
const Vector2d& Actor::getMaximumVelocity() const
{ return mMaximumVelocity; }
//------------------------------------------------------------------------------
const Point2d& Actor::getPosition() const
{ return mPosition; }
//------------------------------------------------------------------------------
QString Actor::getSpriteName() const
{ return mSpriteName; }
//------------------------------------------------------------------------------
QString Actor::getSpriteToken() const
{ return mSpriteToken; }
//------------------------------------------------------------------------------
Actor::state Actor::getState() const
{ return mState; }
//------------------------------------------------------------------------------
const Vector2d& Actor::getVelocity() const
{ return mVelocity; }
//------------------------------------------------------------------------------
void Actor::setAcceleration( const Vector2d& iV )
{ mAcceleration = iV; }
//------------------------------------------------------------------------------
void Actor::setBoundingBox( const Rectangle& iV )
{ 
	mBoundingBox = iV;
  mBoundingCircle.setRadius( sqrt( pow(iV.width()/2.0, 2) + 
  	pow( iV.height() / 2.0, 2 ) ) );
}
//------------------------------------------------------------------------------
void Actor::setBoundingCircle( const Circle& iV )
{ mBoundingCircle = iV; }
//------------------------------------------------------------------------------
void Actor::setHealth( double iV )
{ mHealth = iV; }
//------------------------------------------------------------------------------
void Actor::setName( QString iV )
{ mName = iV; }
//------------------------------------------------------------------------------
void Actor::setMaximumAcceleration( const Vector2d& iV )
{ mMaximumAcceleration = iV; }
//------------------------------------------------------------------------------
void Actor::setMaximumVelocity( const Vector2d& iV )
{ mMaximumVelocity = iV; }
//------------------------------------------------------------------------------
void Actor::setPosition( const Point2d& iV )
{ mPosition = iV; }
//------------------------------------------------------------------------------
void Actor::setSpriteName( QString iV )
{ mSpriteName = iV; }
//------------------------------------------------------------------------------
void Actor::setSpriteToken( QString iV )
{ mSpriteToken = iV; }
//------------------------------------------------------------------------------
void Actor::setState( state iV )
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
}
//------------------------------------------------------------------------------
void Actor::setVelocity( const Vector2d& iV )
{ mVelocity = iV; }
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//---Engine::Stage
//------------------------------------------------------------------------------
realisim::platform::Actor Engine::Stage::mDummyActor;

Engine::Stage::Stage() :
	mName( "stage" ),
  mCellSize( 32, 32 ), 
	mTerrainSize( 30, 40 ),
	mTerrain( mTerrainSize.x() * mTerrainSize.y(), Stage::ctEmpty ),
  mLayers(),
  mActors()
{ mLayers.push_back( new Layer( getTerrainSize() ) ); }

Engine::Stage::Stage( QString iName, Vector2i iSize ) : 
	mName( iName ),
	mCellSize( 32, 32 ), 
	mTerrainSize( iSize ),
	mTerrain( mTerrainSize.x() * mTerrainSize.y(), Stage::ctEmpty ),
  mLayers(),
  mBackgroundToken(),
  mActors()
{ mLayers.push_back( new Layer( getTerrainSize() ) ); }

Engine::Stage::Stage( const Stage& iS) :
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
    
  for( int i = 0; i < iS.getNumberOfActors(); ++i )
  { mActors.push_back( new Actor( *iS.mActors[i] ) ); }
}

Engine::Stage& Engine::Stage::operator=( const Stage& iS )
{
	mName = iS.getName();
  mCellSize = iS.getCellSize();
  mTerrainSize = iS.getTerrainSize();
  mTerrain = iS.getTerrain();

  for( int i = 0; i < iS.getNumberOfLayers(); ++i )
  	mLayers.push_back( new Layer( *iS.mLayers[i] ) );
    
  mBackgroundToken = iS.getBackgroundToken();
  
  for( int i = 0; i < iS.getNumberOfActors(); ++i )
  { mActors.push_back( new Actor( *iS.mActors[i] ) ); }
	return *this;
}

Engine::Stage::~Stage()
{ clear(); }

//------------------------------------------------------------------------------
void Engine::Stage::addActor()
{ mActors.push_back( new Actor() ); }

//------------------------------------------------------------------------------
void Engine::Stage::addLayer()
{ mLayers.push_back( new Layer( getTerrainSize() ) ); }

//------------------------------------------------------------------------------
void Engine::Stage::addToken( int iLayer, QString iToken )
{
	vector<QString>& t = mLayers[iLayer]->mTokens;	
	if( std::find( t.begin(), t.end(), iToken ) == t.end() ) 
		t.push_back( iToken );
}

//------------------------------------------------------------------------------
void Engine::Stage::clear()
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
}

//------------------------------------------------------------------------------
/*retourne tous les indices de le map qui ont la valeure iCt*/
std::vector<int> Engine::Stage::find( cellType iCt ) const
{
	vector<int> r;
  for(int i = 0; i < mTerrain.size(); ++i)
  	if( (uchar)mTerrain[i] == iCt ) r.push_back(i);
  return r;
}

//------------------------------------------------------------------------------
void Engine::Stage::fromBinary( QByteArray iBa )
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
Actor& Engine::Stage::getActor( int i )
{
	Actor* r = &mDummyActor; 
	if( i >= 0 && i < getNumberOfActors() )
  { r = mActors[i]; }
  return *r;
}

//------------------------------------------------------------------------------
QString Engine::Stage::getBackgroundToken() const
{ return mBackgroundToken; }

//------------------------------------------------------------------------------
Vector2i Engine::Stage::getCellCoordinate( const Point2d& p ) const
{ return Vector2i( (int)p.x() / getCellSize().x(), p.y() / getCellSize().y() ); }

//------------------------------------------------------------------------------
Vector2i Engine::Stage::getCellCoordinate( int index ) const
{ return Vector2i(  index % getTerrainSize().x(), index / getTerrainSize().x() ); }

//------------------------------------------------------------------------------
/*Retourne l'index de la cellule au pixel p*/
int Engine::Stage::getCellIndex( const Point2d& p ) const
{ 
	Vector2i c = getCellCoordinate( p );
  return getCellIndex( c.x(), c.y() );
}

//------------------------------------------------------------------------------
//retourn l'index de la cellule x, y
int Engine::Stage::getCellIndex( int x, int y ) const
{ return y * getTerrainSize().x() + x; }

//------------------------------------------------------------------------------
vector<int> Engine::Stage::getCellIndices( const Point2d& iP, const Vector2i& iK ) const
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
Vector2i Engine::Stage::getCellPixelCoordinate( int iIndex ) const
{
	Vector2i r = getCellCoordinate( iIndex );
  return getCellPixelCoordinate( r.x(), r.y() );
}

//------------------------------------------------------------------------------
Vector2i Engine::Stage::getCellPixelCoordinate( int iX, int iY ) const
{  return Vector2i( iX * getCellSize().x(), iY * getCellSize().y() ); }

//------------------------------------------------------------------------------
/*retourne le token de sprite pour le layer iLayer de la cellule iIndex*/
QString Engine::Stage::getToken( int iLayer, int iIndex ) const
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
vector<QString> Engine::Stage::getTokens( int iLayer ) const
{
	vector<QString> r;
  if( iLayer >= 0 && iLayer < getNumberOfLayers() )
  	r = mLayers[iLayer]->mTokens;
  return r;
}

//------------------------------------------------------------------------------
bool Engine::Stage::hasCell( const Vector2i& iC ) const
{
	return iC.x() >= 0 && iC.x() < getTerrainWidth() &&
  	iC.y() >= 0 && iC.y() < getTerrainHeight();
}

//------------------------------------------------------------------------------
bool Engine::Stage::isLayerVisible( int i ) const
{
	bool r = false;
	if( i >= 0 && i < getNumberOfLayers() ) 
  { r = mLayers[i]->mVisibility; } 
	return r;
}

//------------------------------------------------------------------------------
void Engine::Stage::removeActor( int i )
{
	if( &getActor(i) != &mDummyActor )
  {
  	delete mActors[i];
    mActors.erase( mActors.begin() + i );
  }
}

//------------------------------------------------------------------------------
void Engine::Stage::removeLayer( int i )
{}

//------------------------------------------------------------------------------
void Engine::Stage::setBackgroundToken( QString iBt )
{ mBackgroundToken = iBt; }

//------------------------------------------------------------------------------
void Engine::Stage::setLayerAsVisible( int iL, bool iV/*=true*/ )
{
	if( iL >= 0 && iL < getNumberOfLayers() )
  { mLayers[iL]->mVisibility = iV; }
}

//------------------------------------------------------------------------------
QByteArray Engine::Stage::toBinary() const
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
unsigned char Engine::Stage::value(int iX, int iY) const
{ return value( iY * getTerrainSize().x() + iX ); }

//------------------------------------------------------------------------------
unsigned char Engine::Stage::value(int iIndex) const
{ return (unsigned char)getTerrain()[iIndex]; }


//------------------------------------------------------------------------------
//---Engine
//------------------------------------------------------------------------------
Engine::Engine() : QObject(), mState( sIdle ),
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

	loadStage("stage.bin");
  
	goToState( sMainMenu );
}

Engine::~Engine()
{
	mClients.clear();
}

//------------------------------------------------------------------------------
void Engine::addError( QString e ) const
{ 
	mErrors += mErrors.isEmpty() ? e : "\n" + e;
  const_cast<Engine*>(this)->send( eErrorRaised );
}

//------------------------------------------------------------------------------
void Engine::afterCollision( Actor& iA )
{
	Vector2d accel = iA.getAcceleration();
  Vector2d vel = iA.getVelocity();
  Point2d pos = iA.getPosition();
  
	if( isEqual( vel.y(), 0.0 ) )
  { iA.setState( Actor::sWalking ); }
  
  //friction en x. seulement si il n'y a pas d'input usagé
  //ou changment de direciton
  bool applyFriction = ! ( (isKeyPressed( Qt::Key_A ) || 
  	isKeyPressed( Qt::Key_D ) ) ) || 
    accel.x() / fabs(accel.x()) !=  vel.x() / fabs(vel.x());
  if(applyFriction && iA.getState() == Actor::sWalking )
		vel.setX( vel.x() * 0.8 );

  if( vel.y() < 0.0 )
  {
  	switch (iA.getState()) {
      case Actor::sIdle:
      case Actor::sWalking:
      case Actor::sJumping:
      	iA.setState( Actor::sFalling );
        break;
      default: break;
    }
  }
  
  if( vel.norm() < 0.1 )
  { iA.setState( Actor::sIdle ); }
  
  iA.setPosition( pos );
  iA.setVelocity( vel );
}

//------------------------------------------------------------------------------
void Engine::applyPhysics( Actor& iA )
{
	Point2d p = iA.getPosition();
	Vector2d v = iA.getVelocity();
	Vector2d a = iA.getAcceleration();
  double maxHv = iA.getMaximumVelocity().x();
  double maxVv = iA.getMaximumVelocity().y();
  
  //application de la gravité
  a += Vector2d(0.0, -980);

	//déplacement du joueur a la position désiré
  v += a * kDt;
  v.setX( v.x() >= 0.0 ? 
  	min(maxHv, v.x()) : max(-maxHv, v.x()) );
  v.setY( v.y() >= 0.0 ? 
  	min(maxVv, v.y()) : max(-maxVv, v.y()) );
  
  iA.setPosition( p + v * kDt );
  iA.setVelocity( v );
  iA.setAcceleration( a );
//printf("velocity %f, %f acceleration %f, %f\n", mPlayer.mVelocity.x(),
//	mPlayer.mVelocity.y(), mPlayer.mAcceleration.x(), mPlayer.mAcceleration.y() );
}

//------------------------------------------------------------------------------
QString Engine::getAndClearLastErrors() const
{
  QString r = mErrors;
  mErrors.clear();
  return r;
}

//------------------------------------------------------------------------------
vector<Rectangle> Engine::getCollisionRectangles( const Actor& iA,
	Stage::cellType iCt )
{
	vector<Rectangle> r;
  const Vector2d bbSize = iA.getBoundingBox().size();
  int kernel = max( bbSize.x(), bbSize.y() ) / 
  	max( mStage.getCellSize().x(), mStage.getCellSize().y() ) * 3;
  if( kernel % 2 == 0 ) ++kernel; //kernel ne doit pas etre pair
  vector<int> cells = mStage.getCellIndices( iA.getPosition(), Vector2i(kernel) );
  
  //pour chaque lignes
  for(int j = 0; j < kernel; ++j)
  {
  	int lastI = -5;
    int count = 0;
    int index;
  	for(int i = 0; i < kernel; ++i)
  	{
    	index = j * kernel + i;
  		if( index < (int)cells.size() && mStage.value( cells[ index ] ) == iCt )
      {
      	if( i == lastI + 1 )
        {
          lastI = i;
        	++count;
        	r[ r.size() - 1 ].setWidth( count * mStage.getCellSize().x() );
        }
        else 
        {
          lastI = i;
          count = 1;
          Vector2i cellPixelCoordinate = mStage.getCellPixelCoordinate( 
          	cells[ index ] );
          Rectangle cellRect( toPoint(cellPixelCoordinate), mStage.getCellSize() );
          r.push_back( cellRect );
        }
      }
  	}
  }
  
  //pour chaque colonnes
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
vector<int> Engine::getVisibleCells() const
{
	const Camera& c = getGameCamera();
  const Stage& s = mStage;
  Point2d look( c.getTransformationToGlobal().getTranslation().getX(),
    c.getTransformationToGlobal().getTranslation().getY() );
  return s.getCellIndices( look, Vector2i(
  	(int)ceil( c.getVisibleWidth() / s.getCellSize().x() ) + 1,
    (int)ceil( c.getVisibleHeight() / s.getCellSize().y() ) ) + 1 );
}

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
bool Engine::hasError() const
{ return mErrors.isEmpty(); }

//------------------------------------------------------------------------------
void Engine::handleActorCollisions()
{
	//on test le joueurs par rapport a tous les autres acteurs
  handleActorCollisions( mPlayer );
}

//------------------------------------------------------------------------------
void Engine::handleActorCollisions( Actor& iA )
{
	for( int i = 0; i < mStage.getNumberOfActors(); ++i )
  {
  	Actor* a = mStage.mActors[i];
    if( &iA == a ) continue;
    if( intersects( iA.getBoundingCircle(), a->getBoundingCircle() ) )
    {
    	Intersection2d z = intersect( iA.getBoundingBox(), a->getBoundingBox() );
      if( z.hasIntersections() )
      {
				iA.addIntersection( z );
        iA.setState( Actor::sHit );
      }
    }
  }
}

//------------------------------------------------------------------------------
void Engine::handleActorInput( Actor& iA )
{
  //on les fait courrir après le joeur
  Point2d playerPos = getPlayer().getPosition();
  Point2d pos = iA.getPosition();
  Vector2d dir = playerPos - pos;
  if( dir.x() < 0 ) moveLeft( iA );
  else moveRight( iA );
  
  if( dir.y() > 0 ) moveUp( iA );

	applyPhysics( iA );
  updateSpriteToken( iA );
}

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
  if( isKeyPressed( Qt::Key_A ) ) d -= Vector2d(5, 0);
  if( isKeyPressed( Qt::Key_D ) ) d += Vector2d(5, 0);
  if( isKeyPressed( Qt::Key_W ) ) d += Vector2d(0, 5);
  if( isKeyPressed( Qt::Key_S ) ) d -= Vector2d(0, 5);
  
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
void Engine::handleMapCollisions()
{
	//le joueur
  handleMapCollisions( mPlayer );
  
	//les enemies
  for( int i = 0; i < mStage.getNumberOfActors(); ++i )
  { handleMapCollisions( mStage.getActor( i ) ); }
}

//------------------------------------------------------------------------------
void Engine::handleMapCollisions( Actor& iA )
{
  //verification des collisions
  //on flush les collisions précedentes de l'acteur
  iA.clearIntersections();
  
  vector<Rectangle> agg = getCollisionRectangles( iA, Engine::Stage::ctGround );
  
  for( size_t i = 0; i < agg.size(); ++i )
  {
    Rectangle playerRect = iA.getBoundingBox();

    Intersection2d intersection = intersect( playerRect, agg[i] );
    if( intersection.hasIntersections() )
    {
      intersection.add( agg[i].getCenter() );
      iA.addIntersection( intersection );
      
      Point2d pos = iA.getPosition();
      Vector2d v = iA.getVelocity();
      Vector2d penetration = intersection.getPenetration();
      Vector2d displacement;
      if( fabs(penetration.x()) <= fabs(penetration.y()) ) 
      { 
      	displacement.set( penetration.x(), 0.0 );
        iA.setVelocity( Vector2d( 0.0, v.y() ) );
      }
      else 
      { 
        displacement.set( 0.0, penetration.y() );
        iA.setVelocity( Vector2d( v.x(), 0.0 ) );
      }
      iA.setPosition( pos - displacement );
    }
  }
  afterCollision(iA);
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
void Engine::handlePlayerInput()
{
  //input usagé
  if( isKeyPressed( Qt::Key_A ) )
  { moveLeft( mPlayer ); }
  if( isKeyPressed( Qt::Key_D ) )
  { moveRight( mPlayer ); }
  if( isKeyPressed( Qt::Key_W, true ) )
  { moveUp( mPlayer ); }
  applyPhysics( mPlayer );
  updateSpriteToken( mPlayer );
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
  
  //reset les forces sur les acteurs
  mPlayer.setAcceleration( Vector2d( 0.0 ) );
  for( int i = 0; i < getStage().getNumberOfActors(); ++i)
  { mStage.getActor( i ).setAcceleration( Vector2d( 0.0 ) ) ; }
  
  //le joueur
  handlePlayerInput();  
  //les acteurs
  for( int i = 0; i < getStage().getNumberOfActors(); ++i)
  { handleActorInput( mStage.getActor(i) );  }  
  handleActorCollisions();
  handleMapCollisions();

  //deplacement de la camera pour suivre le joueur
  moveGameCamera();
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
void Engine::keyPressed( int iKey )
{ mKeys[iKey] = true; }

//------------------------------------------------------------------------------
void Engine::keyReleased( int iKey )
{ mKeys[iKey] = false; }

//------------------------------------------------------------------------------  
void Engine::loadStage( const Stage& iS )
{
	mStage.clear();
  mStage = iS;

  //init de la position du joueur
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
void Engine::moveLeft( Actor& iActor )
{
	Vector2d a = iActor.getAcceleration();
  Actor::state s = iActor.getState();
  const double maxHAccel = iActor.getMaximumAcceleration().x();
  
  switch ( s ) 
  {
  case Actor::sIdle :
    a -= Vector2d(maxHAccel, 0);
    s = Actor::sWalking;
    break;
  case Actor::sWalking :
  case Actor::sFalling :
  case Actor::sJumping :
    a -= Vector2d(maxHAccel, 0);
    break;
  default: break;
  }
  iActor.setAcceleration( a );
  iActor.setState( s );
}

//------------------------------------------------------------------------------
void Engine::moveRight( Actor& iActor )
{
	Vector2d a = iActor.getAcceleration();
  Actor::state s = iActor.getState();
  const double maxHAccel = iActor.getMaximumAcceleration().x();
  
  switch ( s ) {
  case Actor::sIdle :
      a += Vector2d(maxHAccel, 0);
      s = Actor::sWalking;
    break;
  case Actor::sWalking :
  case Actor::sFalling :
  case Actor::sJumping :
    a += Vector2d(maxHAccel, 0);
    break;
  default: break;
  }
  iActor.setAcceleration( a );
  iActor.setState( s );
}

//------------------------------------------------------------------------------
void Engine::moveUp( Actor& iActor )
{
  Vector2d v = iActor.getVelocity();
  Actor::state s = iActor.getState();
  switch ( s ) {
  case Actor::sIdle :
  case Actor::sWalking :
    v += Vector2d(0, 540);
    s = Actor::sJumping;
    break;
  default: break;
  }
  iActor.setVelocity( v );
  iActor.setState( s );
}

//------------------------------------------------------------------------------
void Engine::newStage( QString iName, int iX, int iY )
{
	loadStage( Stage( iName, Vector2i( iX, iY ) ) );
  mStageFilePath = QString();
	//mStage.clear();
  //mStage = Stage( iName, Vector2i( iX, iY ) );
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

//------------------------------------------------------------------------------
void Engine::updateSpriteToken( Actor& iA )
{
	utils::SpriteCatalog& sc = mSpriteCatalog;
  Sprite* s;
  QString st;
  switch (iA.getState()) 
  {
    case Actor::sIdle: st = iA.getSpriteName() + " idle"; break;
    case Actor::sWalking:
      {
      const Vector2d& v = iA.getVelocity();
      st = v.x() > 0.0 ? iA.getSpriteName() + " run right" :
        iA.getSpriteName() +  " run left";
      }
      break;
    default: st = iA.getSpriteName() + " idle"; break;
  }
  
  s = &(sc.getSprite( st) );
  iA.setSpriteToken( st );
  iA.setBoundingBox( Rectangle(Point2d( 0.0 ), s->getFrameSize()) );
}