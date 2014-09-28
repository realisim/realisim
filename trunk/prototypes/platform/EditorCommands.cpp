
#include "EditorCommands.h"
#include "Math/MathUtils.h"

using namespace realisim;
	using namespace math;
	using namespace platform;

//-----------------------------------------------------------------------------
// addLayer
//-----------------------------------------------------------------------------
void CommandAddLayer::execute()
{
	mStage.addLayer();
  mLayerIndex = mStage.getNumberOfLayers() - 1;
}

//-----------------------------------------------------------------------------
void CommandAddLayer::undo()
{ mStage.removeLayer(mLayerIndex); }

//-----------------------------------------------------------------------------
// removeLayer
//-----------------------------------------------------------------------------
void CommandRemoveLayer::execute()
{ mStage.removeLayer(mLayerIndex); }

//-----------------------------------------------------------------------------
void CommandRemoveLayer::undo()
{
  mStage.addLayer();
  mLayerIndex = mStage.getNumberOfLayers() - 1;
}

//-----------------------------------------------------------------------------
// cellEdition
//-----------------------------------------------------------------------------
void CommandCellEdition::apply( Stage::Layer* l, int iIndex, 
	const CellContent& iCell )
{
	Stage& s = mEngine.getStage();
  /*seul le layer data affecte le terrain... Les
    autres layers sont decoratives... */
  if( l->mName == "data"/*kDataLayerName*/ )
  { s.mTerrain[iIndex] = iCell.mCellType; }
  l->mData[iIndex] = iCell.mSpriteTokenIndex;
}

void CommandCellEdition::execute()
{  }

//-----------------------------------------------------------------------------
void CommandCellEdition::redo()
{
	Vector2d center;
	Stage& s = mEngine.getStage();
	Stage::Layer* l = s.getLayer( mCurrentLayer );
  map<int, CellContent>::iterator it = mCurrentCells.begin();
	for( ; it != mCurrentCells.end(); ++it )
  { 
  	apply( l, it->first,it->second );
    center += s.getCellPixelCoordinate(it->first);
  }
  
  center /= (double)mCurrentCells.size();
  mEngine.getPlayer().setPosition( toPoint( center ) );
  mEngine.moveGameCameraTo( toPoint( center ) );
}

//-----------------------------------------------------------------------------
void CommandCellEdition::undo()
{
	Vector2d center;
	Stage& s = mEngine.getStage();
	Stage::Layer* l = s.getLayer( mCurrentLayer );
  map<int, CellContent>::iterator it = mPreviousCells.begin();
	for( ; it != mPreviousCells.end(); ++it )
  {
  	apply( l, it->first,it->second );
    center += s.getCellPixelCoordinate(it->first);
  }
  
  center /= (double)mCurrentCells.size();
  mEngine.getPlayer().setPosition( toPoint( center ) );
  mEngine.moveGameCameraTo( toPoint( center ) );
}

//-----------------------------------------------------------------------------
void CommandCellEdition::update()
{
	Stage& stage = mEngine.getStage();
	Point2i mousePos = mEngine.getMouse().getPosition();
  Point3d gl = mEngine.getGameCamera().pixelToGL( mousePos.x(),
  	mousePos.y() );
  Vector2i c = stage.getCellCoordinate( Point2d( gl.getX(), gl.getY() ) );
  
  /*Si l'index courant est dans le terrain, on fait l'édition, sinon, on
    agrandit le terrain.*/
  if( stage.hasCell( c ) )
  {
    int index = stage.getCellIndex( c.x(), c.y() );
    Stage::cellType ct = mEngine.getEditingTool();
    QString spriteToken = mEngine.getEditingSpriteToken();
    int spriteTokenIndex = 255;
    Stage::Layer* l = stage.getLayer( mEngine.getCurrentLayer() );
    for( int i = 0; i < (int)l->mTokens.size(); ++i )
    {
      if( l->mTokens[i] == spriteToken )
      {
        spriteTokenIndex = i;
        break;
      }
    }
    
    mPreviousCells.insert( make_pair( index, CellContent( 
    	(Stage::cellType)stage.mTerrain.at(index), l->mData[index] ) ) );
    if( mEngine.getMouse().isButtonPressed( Qt::LeftButton ) )
    { 
    	apply( l, index, CellContent( ct, spriteTokenIndex ) );
      mCurrentCells.insert( make_pair( index, CellContent( 
        (Stage::cellType)stage.mTerrain.at(index), l->mData[index] ) ) );
    }
    else if( mEngine.getMouse().isButtonPressed( Qt::RightButton ) )
    { 
    	apply( l, index, CellContent( Stage::ctEmpty, 255 ) );
      mCurrentCells.insert( make_pair( index, CellContent( 
        (Stage::cellType)stage.mTerrain.at(index), l->mData[index] ) ) );
    }
    
  }
  else 
  { printf( "Faudrait agrandir le terrain cell coord: %d, %d\n", c.x(), c.y() ); }
}

