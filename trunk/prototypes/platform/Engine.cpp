
#include <algorithm>
#include "Engine.h"
#include "Math/MathUtils.h"
#include "Math/Primitives.h"
#include <QPainter>

using namespace std;
using namespace realisim;
	using namespace math;
	using namespace treeD;
	using namespace platform;

const double kDt = 0.015; //temps pour chaque step de l'engin en secondes

//------------------------------------------------------------------------------
//---Stage
//------------------------------------------------------------------------------
Engine::Stage::Stage() : mCellSize( 32, 32 ), 
	mTerrainSize( 400, 400 ),
	mTerrain( mTerrainSize.x() * mTerrainSize.y(), Stage::ctEmpty )
{}

//------------------------------------------------------------------------------
Vector2i Engine::Stage::getCellCoordinate( const Point2d& p ) const
{ return Vector2i( (int)p.x() / cellSize().x(), p.y() / cellSize().y() ); }

//------------------------------------------------------------------------------
Vector2i Engine::Stage::getCellCoordinate( int index ) const
{ return Vector2i( index % terrainSize().x(), index / terrainSize().y() ); }

//------------------------------------------------------------------------------
int Engine::Stage::getCellIndex( const Point2d& p ) const
{ 
	return (int)p.y() / cellSize().y() * terrainSize().x() + 
  	(int)p.x() / cellSize().x();
}

//------------------------------------------------------------------------------
vector<int> Engine::Stage::getCellIndices( const Point2d& iP, const Vector2i& iK ) const
{
	vector<int> r;
  //on trouve la coordonnée de la cellule pour la poisition iP
  Vector2i playerCell( iP.x() / cellSize().x(), iP.y() / cellSize().y());

  Vector2i terrainCell;
  for( int j = -iK.y() / 2; j <= iK.y() / 2; ++j )
  	for( int i = -iK.x() / 2; i <= iK.x() / 2; ++i )
    {
    	terrainCell = playerCell + Vector2i( i, j );

			int clampX = min( max( terrainCell.x(), 0 ), terrainSize().x() - 1 );
      int clampY = min( max( terrainCell.y(), 0 ), terrainSize().y() - 1 );
			r.push_back( clampY * terrainSize().x() + clampX );
    }
  
  return r;
}

//------------------------------------------------------------------------------
Vector2i Engine::Stage::getCellPixelCoordinate( int iIndex ) const
{
	Vector2i r = getCellCoordinate( iIndex );
  return Vector2i( r.x() * cellSize().x(), r.y() * cellSize().y() );
}

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
  mMouseDelta(0, 0)
{

	int w = 800, h = 600;
	mGameCamera.setOrthoProjection( w, h, 0.0, 200 );
  mGameCamera.setWindowSize(w, h);
  mGameCamera.set( Point3d( 0.0, 0.0, 5.0 ),
  	Point3d( 0.0, 0.0, 0.0 ),
    Vector3d( 0.0, 1.0, 0.0 ) );
    
  //des valeur bidon dans le terrain
  for( int i = 0; i < getStage().terrainSize().x(); ++i )
  	mStage.mTerrain[i] = Stage::ctGround ;
  mStage.mTerrain[ getStage().terrainSize().x() + 15  ]  = Stage::ctGround ;
  mStage.mTerrain[ getStage().terrainSize().x() + 22  ]  = Stage::ctGround ;
  mStage.mTerrain[ getStage().terrainSize().x() + 30  ]  = Stage::ctGround ;
  mStage.mTerrain[ 10 * getStage().terrainSize().x() + 3  ]  = Stage::ctGround ;
  mStage.mTerrain[ 10 * getStage().terrainSize().x() + 5  ]  = Stage::ctGround ;
  mStage.mTerrain[ 10 * getStage().terrainSize().x() + 7  ]  = Stage::ctGround ;
    
  //init de la position du joueur
  mPlayer.mPosition = Point2d( 100, 100 );
  
	goToState( sMainMenu );
}

Engine::~Engine()
{
	mClients.clear();
}

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
  if( isKeyPressed( Qt::Key_A ) ) d -= Vector2d(2, 0);
  if( isKeyPressed( Qt::Key_D ) ) d += Vector2d(2, 0);
  if( isKeyPressed( Qt::Key_W ) ) d += Vector2d(0, 3);
  if( isKeyPressed( Qt::Key_S ) ) d -= Vector2d(0, 2);
  
  //application de la gravité
  //d += Vector2d(0.0, -2);
  //déplacement du joueur
  mPlayer.mPosition = mPlayer.mPosition + d;

	if( isMousePressed( Qt::LeftButton ) )
  {
  	Point3d gl = mGameCamera.pixelToGL( getMousePos().x(), getMousePos().y() );
  	int index = mStage.getCellIndex( Point2d( gl.getX(), gl.getY() ) );
    mStage.mTerrain[index] = Stage::ctGround;
    send(eStageChanged);
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
    if( (uchar)mStage.terrain().at( cells[i] ) != Engine::Stage::ctEmpty )
    {
    	Vector2i cellPixelCoordinate = mStage.getCellPixelCoordinate( cells[i] );
    	Rectangle r( toPoint(cellPixelCoordinate),
        mStage.cellSize() );

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
          getStage().cellSize() / 2.0 , Vector2d( 0.0, 1.0 ) );
        mPlayer.mIntersection.add(intersection);
        
        //on replace le joueur
        Vector2i cellSize = getStage().cellSize();
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
  bool applyFriction = ! ( isKeyPressed( Qt::Key_A ) ||
  	isKeyPressed( Qt::Key_D ) );
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
  if( isKeyPressed( Qt::Key_W ) )
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
  mPlayer.mPosition = mPlayer.mPosition + mPlayer.mVelocity * kDt;
printf("velocity %f, %f acceleration %f, %f\n", mPlayer.mVelocity.x(),
	mPlayer.mVelocity.y(), mPlayer.mAcceleration.x(), mPlayer.mAcceleration.y() );
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
  Matrix4d m = mGameCamera.getTransformationToGlobal();
  m.setTranslation( Point3d( mPlayer.mPosition.x(), mPlayer.mPosition.y(), 0.0 ) );
  mGameCamera.setTransformationToGlobal( m );
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
void Engine::registerClient( Client* ipC )
{
	vector<Client*>::iterator it = find(mClients.begin(), mClients.end(), ipC);
  if( it == mClients.end() )
  	mClients.push_back( ipC );
}

//------------------------------------------------------------------------------
void Engine::send( event iE )
{
	for( size_t i = 0; i < mClients.size(); ++i )
  { mClients[i]->gotEvent( iE ); }
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