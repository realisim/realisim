
#include <algorithm>
#include "Engine.h"
#include "Math/MathUtils.h"
#include "Math/Primitives.h"
#include "utils/utilities.h"

using namespace std;
using namespace realisim;
	using namespace math;
	using namespace treeD;
	using namespace platform;

namespace 
{
	const int kStageHeader = 0x9ab36ef2;
  const int kStageVersion = 5;
  const int kStageFistCompatibleVersion = 1;
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
void Stage::add( Weapon* a )
{ 
  a->setEngine(mpEngine);
	mWeapons.push_back( a );
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
Weapon& Stage::getWeapon( int i )
{ return *mWeapons[i]; }

//------------------------------------------------------------------------------
int Stage::getNumberOfActors() const
{ return mActors.size(); }

//------------------------------------------------------------------------------
int Stage::getNumberOfMonsters() const
{ return mMonsters.size(); }

//------------------------------------------------------------------------------
int Stage::getNumberOfWeapons() const
{ return mWeapons.size(); }

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
void Stage::removeWeapon( int i )
{
	Weapon* m = mWeapons[i];
	mWeapons.erase( std::find( mWeapons.begin(), mWeapons.end(), m ) );
  delete m;
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
void Engine::addAnimation( const Sprite& iS, const Point2d& iPos )
{
  Animation* ani = new Animation();
  ani->setSprite( iS );
  ani->setPosition( iPos );  
  addAnimation( ani );
}

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
        	mTimerId = startTimer( getPhysics().getTimeIncrement() * 1000.0 );
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
{ 
	goToState(sMainMenu);
	loadStage("stage.bin");
}

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
  if( isKeyPressed( Qt::Key_Escape ) ) goToState( sPaused );
  
  for( int i = 0; i < (int)mEntities.size(); ++i )
  { mEntities[i]->update(); }

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
	state previousState = getState();
  goToState(sPaused);
  
	setSpriteCatalog("level1.cat"); //faudrait le lire du stage
	mStage.clear();
  mStage = iS;
  refreshGameEntityList();
  mVisibleCells.clear();
  
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

Weapon* w = new Weapon();
w->setType(Weapon::tPellet);
mStage.add( w );
w = new Weapon();
w->setType(Weapon::tGrenade);
mStage.add( w );

mStage.getWeapon(0).setPosition( Point2d(100, 100) );
mStage.getWeapon(1).setPosition( Point2d(200, 100) );

	startLevel();
  
  send( eStageLoaded );
  goToState(previousState);
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
  /*On enleve le joueur de la liste quand il est marqué pour etre effacé (
    typiquement quand le joueur meurt...) ainsi les collisions et tout le 
    tralala, fonctionne comme si le joueur n'était pas présent.*/
  if( !mPlayer.isMarkedForDeletion() )
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
  //ajout/retrait des armes
  vector<Weapon*>::iterator itWeapons = mStage.mWeapons.begin();
  while( itWeapons != mStage.mWeapons.end() )
  {
    if( (*itWeapons)->isMarkedForDeletion() )
    {
      delete *itWeapons;
      itWeapons = mStage.mWeapons.erase( itWeapons );
    }
    else
    {
    	mEntities.push_back( *itWeapons );
    	++itWeapons;
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
  printf( "num animation %d\n", mAnimations.size() );
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
void Engine::startLevel( int iDelay )
{
	mStartLevelDelay = iDelay;
	mStartLevelTimer.start();
}

//------------------------------------------------------------------------------
void Engine::startLevel()
{
	mStartLevelTimer = QTime();
  
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
  	//--- debuggage --
    #ifndef NDEBUG
    bool debug = false; bool iterate = false;  
    if( isKeyPressed( Qt::Key_D ) ) debug = !debug;
    if( debug && isKeyPressed( Qt::Key_Plus, true ) ) iterate = true;
    if( debug && !iterate ) return;
    #endif
    //--- fin debuggage

  	switch (getState()) 
    {
    	case sIdle: break;
      case sMainMenu: handleMainMenu(); break;
      case sConfigureMenu: handleConfigureMenu(); break;
      case sPlaying: 
        if( !mStartLevelTimer.isNull() && 
        	mStartLevelTimer.elapsed() > mStartLevelDelay )
        { startLevel(); }
      	handlePlaying();
        break;
      case sEditing: handleEditing(); break;
      case sPaused: handlePauseMenu(); break;
      default: break;
    }
    
    computeVisibleCells();
  	send( eFrameDone );
    refreshGameEntityList();
    
//    /*Le reset des forces devrait etre dans le update des entity, mais pour
//    faciliter le deboggage (affichage des forces) on le fait ici, apres le
//    message eFrameDone.*/
//    for( int i = 0; i < (int)mEntities.size(); ++i )
//	  { mEntities[i]->resetForces(); }
  }
}

//------------------------------------------------------------------------------
void Engine::unregisterClient( Client* ipC )
{
	vector<Client*>::iterator it = find(mClients.begin(), mClients.end(), ipC);
  if( it != mClients.end() )
  	mClients.erase( it );
}