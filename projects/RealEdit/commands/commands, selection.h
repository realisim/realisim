/*
 *  commands, selection.h
 *  Created by Pierre-Olivier Beaudoin on 09-08-05.
 */
 
 #ifndef realedit_commands_selection_hh
 #define realedit_commands_selection_hh

namespace realEdit{class EditionData;}
#include "Command.h"
#include <vector>

namespace realEdit
{
namespace commands 
{
  using namespace std;
  class Selection : public realisim::utils::Command
  {
   public:
     explicit Selection(EditionData&, const vector<unsigned int>&);
     virtual ~Selection();
     
     virtual void execute();
     virtual void undo();
     
  private:    
    realEdit::EditionData& mEditionData;
    vector<unsigned int> mPreviousSelection;
    vector<unsigned int> mSelection;
  };
}
}

#endif