
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
Engine::Stage::Stage() : mCellSize( 24, 24 ), 
	mTerrainSize( 50, 50 ),
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
//printf("player pos (%f, %f), kernelSize (%d, %d) \n", iP.x(), iP.y(),
//	iK.x(), iK.y());
  //on trouve la coordonnée de la cellule pour la poisition iP
  Vector2i playerCell( iP.x() / cellSize().x(), iP.y() / cellSize().y());
//printf("player cell (%d, %d)\n", playerCell.x(), playerCell.y() );

  Vector2i terrainCell;
  for( int j = -iK.y() / 2; j <= iK.y() / 2; ++j )
  	for( int i = -iK.x() / 2; i <= iK.x() / 2; ++i )
    {
    	terrainCell = playerCell + Vector2i( i, j );
//printf("terrainCell cell (%d, %d)\n", terrainCell.x(), terrainCell.y() );
//      if( terrainCell.x() >= 0 && terrainCell.x() < terrainSize().x() &&
//         terrainCell.y() >= 0 && terrainCell.y() < terrainSize().y() )
//      {
//printf("terrainCell index (%d)\n", terrainCell.y() * terrainSize().x() + terrainCell.x() );
//      	r.push_back( terrainCell.y() * terrainSize().x() + terrainCell.x() );
//      }
			int clampX = min( max( terrainCell.x(), 0 ), terrainSize().x() );
      int clampY = min( max( terrainCell.y(), 0 ), terrainSize().y() );
			r.push_back( clampY * terrainSize().x() + clampX );
    }
  
  return r;
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
  mKeys()
{
	int w = 800, h = 600;
	mGameCamera.setOrthoProjection( w, h, 0.5, 200 );
  mGameCamera.setWindowSize(w, h);
  mGameCamera.set( Point3d( 0.0, 0.0, 5.0 ),
  	Point3d( 0.0, 0.0, 0.0 ),
    Vector3d( 0.0, 1.0, 0.0 ) );
    
  //des valeur bidon dans le terrain
  for( int i = 0; i < getStage().terrainSize().x(); ++i )
  	mStage.mTerrain[i] = Stage::ctGround ;
  mStage.mTerrain[ 10 * getStage().terrainSize().x() + 3  ]  = Stage::ctGround ;
  mStage.mTerrain[ 10 * getStage().terrainSize().x() + 5  ]  = Stage::ctGround ;
  mStage.mTerrain[ 10 * getStage().terrainSize().x() + 7  ]  = Stage::ctGround ;
    
  //init de la position du joueur
  mPlayer.position = Point2d( 100, 100 );
  
	goToState( sMainMenu );
}

Engine::~Engine()
{
	mClients.clear();
}

//------------------------------------------------------------------------------
Engine::configureMenuItem Engine::getCurrentConfigureMenuItem() const
{ return mConfigureMenuItem; }

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
Engine::mainMenuItem Engine::getCurrentMainMenuItem() const
{ return mMainMenuItem; }

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
Engine::pauseMenuItem Engine::getCurrentPauseMenuItem() const
{ return mPauseMenuItem; }

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
//vector<Tile> Engine::getTiles( const Point2d& iO, const Point2d& iE) const
//{
//	int tileW = 1024, tileH = 1024;
//	vector<Tile> r;
//  Point2i o( iO );
//  Point2i e( iE );
//  
//  int startXIndex = round(o.x() / (double)tileW - 0.5);
//  int endXIndex = round(e.x() / (double)tileW - 0.5);
//  int startYIndex = round(o.y() / (double)tileH - 0.5);
//  int endYIndex = round(e.y() / (double)tileH - 0.5);
//  
////printf("startx %d endx %d starty %d end%d\n", startXIndex, endXIndex,
////	startYIndex, endYIndex);
//  for( int j = startYIndex; j <= endYIndex; ++j )
//  {
//  	for( int i = startXIndex; i <= endXIndex; ++i )
//    {
//    	Tile t;
//      t.setOrigin( Point2d( tileW * i, tileH * j ) );
//      t.setSize( Vector2d( tileW, tileH ) );
//      r.push_back( t );
//    }
//  }
//  return r;
//}
  
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
    
  //verification des collisions
  mPlayer.intersection.clear();
  int iKernel = 3;
  /*On va chercher les cellules (3 x 3) autour de la position du joueur*/
  vector<int> cells = mStage.getCellIndices( mPlayer.position, Vector2i(iKernel) );
  bool left = false, right = false, bottom = false, top = false;
  
  for( size_t i = 0; i < cells.size(); ++i )
  {
    if( (uchar)mStage.terrain().at( cells[i] ) != Engine::Stage::ctEmpty )
    {
printf("intersection on cells %d\n", cells[i]);
    	Intersection2d intersection = intersects( 
	      Circle( mPlayer.position, getPlayerCollisionRadius() ),
      	Rectangle( Point2d(
          cells[i] % mStage.terrainSize().x() * mStage.cellSize().x(),
          cells[i] / mStage.terrainSize().x() * mStage.cellSize().y()),
          mStage.cellSize() ) );
      mPlayer.intersection.add( intersection );
    }
  }
  
  //déplacement du joueur
  mPlayer.position = mPlayer.position + d;

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
	const bool debug = false;
  bool iterate = false;
  if( isKeyPressed( Qt::Key_Escape ) ) goToState( sPaused );
  
  Vector2d d(0.0);
  //input usagé
  if( isKeyPressed( Qt::Key_A ) ) d -= Vector2d(2, 0);
  if( isKeyPressed( Qt::Key_D ) ) d += Vector2d(2, 0);
  if( isKeyPressed( Qt::Key_W ) ) d += Vector2d(0, 3);
  if( isKeyPressed( Qt::Key_S ) ) d -= Vector2d(0, 2);
  if( isKeyPressed( Qt::Key_Plus, true ) ) iterate = true; 
   
   
 	//application de la gravité
  d += Vector2d(0.0, -2);

	if( debug && !iterate ) return;

  //verification des collisions
  mPlayer.intersection.clear();
  int iKernel = 3;
  double dt = 1.0;
	bool bottomLeft, left, topLeft, top, topRight, right, bottomRight, bottom;
  bottomLeft = left = topLeft = top = topRight = right = bottomRight = bottom = false;
  /*On va chercher les cellules (3 x 3) autour de la position du joueur*/
  vector<int> cells = mStage.getCellIndices( mPlayer.position, Vector2i(iKernel) );
  for( size_t i = 0; i < cells.size(); ++i )
  {
    if( (uchar)mStage.terrain().at( cells[i] ) != Engine::Stage::ctEmpty )
    {
    	Circle c( mPlayer.position + d, getPlayerCollisionRadius() );
    	Rectangle cell( Point2d(
          cells[i] % mStage.terrainSize().x() * mStage.cellSize().x(),
          cells[i] / mStage.terrainSize().x() * mStage.cellSize().y()),
          mStage.cellSize() );
    	Intersection2d intersection = intersects( c, cell );
      
      if( intersection.hasPoints() )
      {
printf("player pos %f, %f and collision at %f, %f\n",
	mPlayer.position.x() + d.x(),
  mPlayer.position.y() + d.y(),
  intersection.point(0).x(), intersection.point(0).y() );
  			
        //on trouve le temps exact de la collision
        double inc = 0.5;
        dt = inc;
        while( inc > 1.0e-8 )
        {
        	Circle c( mPlayer.position + d * dt, getPlayerCollisionRadius() );
          Intersection2d i2 = intersects( c, cell );
          inc /= 2.0;
          if( i2.hasPoints() )
          {
          	dt -= inc;
            intersection = i2;
          }
          else
          	dt += inc;
        }
        
  			//int c = cells[i] - getStage().getCellIndex( mPlayer.position );
//        Vector2i contactingCell(c % getStage().terrainSize().x(),
//        	c / getStage().terrainSize().x() );
				
        Vector2i contactingCell = 
        	getStage().getCellCoordinate( cells[i] ) -
          getStage().getCellCoordinate(mPlayer.position) ;
          
        left |= contactingCell.x() < 0 ;
        right |= contactingCell.x() > 0 ;
        bottom |= contactingCell.y() < 0;
        top |= contactingCell.y() > 0;
printf( "collision avec cellule %d, %d par rapport au joueur\n",
	contactingCell.x(), contactingCell.y() );
      }    
      mPlayer.intersection.add(intersection);  
    }
  }
  
printf("left %d, right %d, bottom %d, top %d\n",
	left ? 1:0,
  right ? 1:0,
  bottom ? 1:0,
  top ? 1:0);

	Vector2d f;
	for( int i = 0; i < mPlayer.intersection.numberOfPoints(); ++i )
  { f += mPlayer.intersection.normal(i); }

  //déplacement du joueur
  Vector2d dsurf = d.projectOn((1 - dt) * f);
  mPlayer.position = mPlayer.position + d - d.projectOn((1 - dt) * f);
printf("position apres collision: %f, %f, dt %f,"
  "projection d sur f %f, %f\n", mPlayer.position.x(), mPlayer.position.y(),
  dt,dsurf.x(), dsurf.y() ); 

  //deplacement de la camera pour suivre le joueur
  Matrix4d m = mGameCamera.getTransformationToGlobal();
  m.setTranslation( Point3d( mPlayer.position.x(), mPlayer.position.y(), 0.0 ) );
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
void Engine::keyPressed( int iKey )
{ mKeys[iKey] = true; }

//------------------------------------------------------------------------------
void Engine::keyReleased( int iKey )
{ mKeys[iKey] = false; }
  
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