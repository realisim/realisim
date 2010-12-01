/*
 *  commands, selection.h
 *  Created by Pierre-Olivier Beaudoin on 09-08-05.
 */
 
 #ifndef realedit_commands_selection_hh
 #define realedit_commands_selection_hh

namespace realEdit{class EditionData;}
#include "utils/Command.h"
#include <set>
#include <vector>

namespace realEdit
{
namespace commands 
{
  using namespace std;
  class Selection : public realisim::utils::Command
  {
   public:
     enum mode{mAdditive, mNormal, mSubtractive};
     
     explicit Selection(EditionData&, const unsigned int, mode);
     virtual ~Selection();
     
     virtual void execute();
     virtual void update(const unsigned int, mode);
     
  protected:
    virtual void undo();
    
    realEdit::EditionData& mEditionData;
    set<unsigned int> mPreviousSelection;
    vector<pair<unsigned int, mode> > mSelection;
  };
}
}

#endif