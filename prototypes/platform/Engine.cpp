
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
  
  const int kStageVersion = 4;
  const int kStageFistCompatibleVersion = 1;
}


//------------------------------------------------------------------------------
//---Stage
//------------------------------------------------------------------------------
Engine::Stage::Stage() :
	mName( "stage" ),
  mCellSize( 32, 32 ), 
	mTerrainSize( 30, 40 ),
	mTerrain( mTerrainSize.x() * mTerrainSize.y(), Stage::ctEmpty ),
  mLayers()
{ mLayers.push_back( new Layer( getTerrainSize() ) ); }

Engine::Stage::Stage( QString iName, Vector2i iSize ) : 
	mName( iName ),
	mCellSize( 32, 32 ), 
	mTerrainSize( iSize ),
	mTerrain( mTerrainSize.x() * mTerrainSize.y(), Stage::ctEmpty ),
  mLayers(),
  mBackgroundToken()
{ mLayers.push_back( new Layer( getTerrainSize() ) ); }

Engine::Stage::Stage( const Stage& iS) :
	mName( iS.getName() ),
  mCellSize( iS.getCellSize() ), 
	mTerrainSize( iS.getTerrainSize() ),
	mTerrain( iS.getTerrain() ),
  mLayers(),
  mBackgroundToken( iS.getBackgroundToken() )
{ 
	for( int i = 0; i < iS.getNumberOfLayers(); ++i )
  	mLayers.push_back( new Layer( *iS.mLayers[i] ) );
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
	return *this;
}

Engine::Stage::~Stage()
{ clear(); }

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
  	delete mLayers[i];
  mLayers.clear();
  mBackgroundToken = QString();
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

			//if( terrainCell.x() >= 0 && terrainCell.x() < terrainSize().x() &&
     // 	 terrainCell.y() >= 0 && terrainCell.y() < terrainSize().y() )
      //r.push_back( terrainCell.y() * terrainSize().x() + terrainCell.x() );
			int clampX = min( max( terrainCell.x(), 0 ), getTerrainSize().x() - 1 );
      int clampY = min( max( terrainCell.y(), 0 ), getTerrainSize().y() - 1 );
			r.push_back( clampY * getTerrainSize().x() + clampX );
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
void Engine::Stage::setBackgroundToken( QString iBt )
{ mBackgroundToken = iBt; }

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
  mMousePos( -1, -1 ),
  mMouseDelta(0, 0),
  mEditingTool( Stage::ctGround ),
  mCurrentLayer(0)
{
	int w = 800, h = 600;
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
void Engine::addTokenToLayer( int iLayer, QString iToken )
{
	if( iLayer >= 0 )
  {
  	mStage.addToken( iLayer, iToken );
  }
  else
  {
  	QString m;
    m.sprintf( "Impossible d'ajouter le sprite %s à la couche %d.",
    	iToken.toStdString().c_str(), iLayer );
  	addError( m );
  }
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
BoundingBox2d Engine::getPlayerBoundingBox() const
{	
	return BoundingBox2d( mPlayer.mPosition - Vector2d( mPlayer.mSize / 2 ),
    Vector2d( mPlayer.mSize ) );
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
        	saveStage( mStage.getName() + ".bin");
          mState = sPaused;
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
  
  //application de la gravité
  //d += Vector2d(0.0, -2);
  //déplacement du joueur
  mPlayer.mPosition = mPlayer.mPosition + d;

  Point3d gl = mGameCamera.pixelToGL( getMousePos().x(), getMousePos().y() );
  int index = mStage.getCellIndex( Point2d( gl.getX(), gl.getY() ) );
  
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
  //verification des collisions
  mPlayer.mIntersection.clear();
  int iKernel = 3;
  /*On va chercher les cellules (3 x 3) autour de la position du joueur sous
    la forme suivante :
     6 7 8
     3 4 5
     0 1 2*/
  vector<int> cells = mStage.getCellIndices( mPlayer.mPosition, Vector2i(iKernel) );
  //on replace les cellule pour gere les collision dans lorder suivant
  /*
     6 1 7
     2 8 3
     4 0 5  */
  bool isOnGround = false;
  vector<int> tmp(cells);
  cells[0] = tmp[1]; cells[1] = tmp[7]; cells[2] = tmp[3];
  cells[3] = tmp[5]; cells[4] = tmp[0]; cells[5] = tmp[2];
  cells[6] = tmp[6]; cells[7] = tmp[8]; cells[8] = tmp[4];
  for( size_t i = 0; i < cells.size(); ++i )
  {
    if( mStage.value( cells[i] ) == Engine::Stage::ctGround )
    {
    	Vector2i cellPixelCoordinate = mStage.getCellPixelCoordinate( cells[i] );
    	Rectangle r( toPoint(cellPixelCoordinate),
        mStage.getCellSize() );

    	//intersection en utilisant minkowski
      BoundingBox2d mink;
      for( int j = 0; j < 4; j++ )
	      for( int i = 0; i < 4; i++ )
        {
        	mink.add( toPoint(getPlayerBoundingBox().point(j) - r.point(i)) );
        }
        
      if( mink.contains(Point2d(0.0), false ) )
      {
      	Intersection2d intersection;
        intersection.add( toPoint(cellPixelCoordinate) + 
          getStage().getCellSize() / 2.0 , Vector2d( 0.0, 1.0 ) );
        mPlayer.mIntersection.add(intersection);
        
        //on replace le joueur
        Vector2i cellSize = getStage().getCellSize();
        switch( i )
        {        
        case 0: //bas
          	mPlayer.mPosition.setY( cellPixelCoordinate.y() + cellSize.y() +
	        	mPlayer.mSize.y() / 2 );
            isOnGround = true;
          break;        
        case 1: //haut
        	mPlayer.mPosition.setY( cellPixelCoordinate.y() -
        		mPlayer.mSize.y() / 2 );
            mPlayer.mVelocity.setY( 0.0 );
            break;
        case 2: //gauche
        	mPlayer.mPosition.setX( cellPixelCoordinate.x() + cellSize.x() +
        		mPlayer.mSize.x() / 2 );
          mPlayer.mVelocity.setX(0.0);
          break;      	
        case 3: //droite
        	mPlayer.mPosition.setX( cellPixelCoordinate.x() -
        		mPlayer.mSize.x() / 2 );
          mPlayer.mVelocity.setX(0.0);
          break;
        case 4: //bas gauche
        case 5: // bas droite
        	if( mPlayer.mState != Player::sJumping )
          {
          	mPlayer.mPosition.setY( cellPixelCoordinate.y() + cellSize.y() +
        		mPlayer.mSize.y() / 2 ); 
            isOnGround = true;
          }
          break;
        case 6: //haut gauche
        case 7: //haut droite
        	if( mPlayer.mState != Player::sFalling )
          {
          	mPlayer.mPosition.setY( cellPixelCoordinate.y() -
            	mPlayer.mSize.y() / 2 );
            isOnGround = true;
          }
        	break; 
        case 8: break;
        default: break;
        }
      }
    }
  }
  
  if( isOnGround )
  {
  	mPlayer.mAcceleration.setY( 0.0 );
    mPlayer.mVelocity.setY( 0.0 );
    mPlayer.mState = Player::sWalking;
  }
  
  //friction en x. seulement si il n'y a pas d'input usagé
  //ou changment de direciton
  bool applyFriction = ! ( (isKeyPressed( Qt::Key_A ) ||
  	isKeyPressed( Qt::Key_D ) ) ) || 
    mPlayer.mAcceleration.x() / fabs(mPlayer.mAcceleration.x()) != 
    mPlayer.mVelocity.x() / fabs(mPlayer.mVelocity.x());
  if(applyFriction && isOnGround)
		mPlayer.mVelocity.setX( mPlayer.mVelocity.x() * 0.8 );

  if( mPlayer.mVelocity.y() < 0.0 )
  {
  	switch (mPlayer.mState) {
      case Player::sIdle:
      case Player::sWalking:
      case Player::sJumping:
      	mPlayer.mState = Player::sFalling;
        break;
      default: break;
    }
  }
  
  if( mPlayer.mVelocity.norm() < 0.1 )
  { mPlayer.mState = Player::sIdle; }
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
  mPlayer.mAcceleration.setX( 0.0 );
  const double maxHAccel = 1000;
  //input usagé
  if( isKeyPressed( Qt::Key_A ) )
  {
    	switch ( mPlayer.mState ) {
    	case Player::sIdle :
        mPlayer.mAcceleration -= Vector2d(maxHAccel, 0);
        mPlayer.mState = Player::sWalking;
        break;
      case Player::sWalking :
      case Player::sFalling :
      case Player::sJumping :
      	mPlayer.mAcceleration -= Vector2d(maxHAccel, 0);
        break;
      default: break;
    }
  }
  if( isKeyPressed( Qt::Key_D ) )
  {
    	switch ( mPlayer.mState ) {
    	case Player::sIdle :
          mPlayer.mAcceleration += Vector2d(maxHAccel, 0);
	        mPlayer.mState = Player::sWalking;
      	break;
      case Player::sWalking :
      case Player::sFalling :
      case Player::sJumping :
      	mPlayer.mAcceleration += Vector2d(maxHAccel, 0);
      	break;
      default: break;
    }
  }
  if( isKeyPressed( Qt::Key_W, true ) )
  {
  	switch ( mPlayer.mState ) {
    	case Player::sIdle :
      case Player::sWalking :
    		mPlayer.mVelocity += Vector2d(0, 500);
        mPlayer.mState = Player::sJumping;
        break;
      default: break;
    }
  }
  
  //application de la gravité
  mPlayer.mAcceleration.setY( mPlayer.mAcceleration.y() - 100 );

	//déplacement du joueur a la position désiré
  mPlayer.mVelocity += mPlayer.mAcceleration * kDt;
  mPlayer.mVelocity.setX( mPlayer.mVelocity.x() >= 0.0 ? 
  	min(400.0, mPlayer.mVelocity.x()) : max(-400.0, mPlayer.mVelocity.x()) );
  mPlayer.mVelocity.setY( mPlayer.mVelocity.y() >= 0.0 ? 
  	min(1400.0, mPlayer.mVelocity.y()) : max(-1400.0, mPlayer.mVelocity.y()) );
  mPlayer.mPosition = mPlayer.mPosition + mPlayer.mVelocity * kDt;
//printf("velocity %f, %f acceleration %f, %f\n", mPlayer.mVelocity.x(),
//	mPlayer.mVelocity.y(), mPlayer.mAcceleration.x(), mPlayer.mAcceleration.y() );
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
  
  handlePlayerInput();
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
  	mPlayer.mPosition = toPoint(
    	getStage().getCellPixelCoordinate( start[0] ) + 
      getStage().getCellSize() / 2 );
  else
	  mPlayer.mPosition = Point2d( 10, 10 );
  
	//on met le joueur et la camera dans la premiere case
  Matrix4d m = mGameCamera.getTransformationToGlobal();
  m.setTranslation( Point3d( mPlayer.mPosition.x(), mPlayer.mPosition.y(), 0.0 ) );
  mGameCamera.setTransformationToGlobal( m );
  
  send( eStageLoaded );
}
//------------------------------------------------------------------------------
void Engine::loadStage( QString iPath )
{
	Stage s;
  s.fromBinary( utils::fromFile( iPath ) );
  loadStage(s);
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
void Engine::moveGameCamera()  
{
  Matrix4d m = mGameCamera.getTransformationToGlobal();
  Point2d desired = mPlayer.mPosition;
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
{
	utils::toFile( iPath, getStage().toBinary() );
}  

//------------------------------------------------------------------------------
void Engine::send( event iE )
{
	for( size_t i = 0; i < mClients.size(); ++i )
  { mClients[i]->gotEvent( iE ); }
}

//------------------------------------------------------------------------------
void Engine::setBackgroundToken( QString iToken )
{ mStage.setBackgroundToken( iToken ); }

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