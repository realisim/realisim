/* */

#ifndef realisim_platform_EditorCommands_hh
#define realisim_platform_EditorCommands_hh

#include "Engine.h"
#include "Math/Point.h"
#include <map>
#include "utils/Command.h"

namespace realisim
{
namespace platform 
{

//------------------------------------------------------------------------------
class CommandAddLayer : public realisim::utils::Command
{
public:
	CommandAddLayer( Stage& iS ) :
  mStage(iS), mLayerIndex(-1) {}
  
  virtual void execute();
  virtual void undo();
  
protected:
	Stage& mStage;
  int mLayerIndex;
};

//------------------------------------------------------------------------------
class CommandRemoveLayer : public realisim::utils::Command
{
public:
	CommandRemoveLayer( Stage& iS, int iIndex ) :
  mStage(iS), mLayerIndex(iIndex) {}
  
  virtual void execute();
  virtual void undo();
  
protected:
	Stage& mStage;
  int mLayerIndex;
};

//------------------------------------------------------------------------------
class CommandCellEdition : public realisim::utils::Command
{
public:
	CommandCellEdition( Engine& iE ) : mEngine(iE),
    mCurrentLayer( mEngine.getCurrentLayer() ) {}
  
  virtual void execute();
  virtual void undo();
  virtual void redo();
  void update();
  
protected:
	struct CellContent
  {
  	CellContent( Stage::cellType iCt, int iSt ) : 
    	mCellType(iCt), mSpriteTokenIndex(iSt) {}
    
    Stage::cellType mCellType;
    int mSpriteTokenIndex;
  };

	void apply( Stage::Layer*, int, const CellContent& );

	Engine& mEngine;
  int mCurrentLayer;
  std::map< int, CellContent > mPreviousCells;
  std::map< int, CellContent > mCurrentCells;
};


} //namespace platform
} //namespace realisim

#endif
