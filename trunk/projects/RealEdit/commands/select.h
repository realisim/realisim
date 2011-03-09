/*
 *  commands, selection.h
 *  Created by Pierre-Olivier Beaudoin on 09-08-05.
 */
 
 #ifndef realedit_commands_Select_hh
 #define realedit_commands_Select_hh

namespace realEdit{class Controller;}
#include "utils/Command.h"
#include <set>
#include <vector>

namespace realEdit
{
namespace commands 
{
  using namespace std;
  class Select : public realisim::utils::Command
  {
    public:
      enum mode{mAdditive, mNormal, mSubtractive};

      explicit Select(Controller&);
      virtual ~Select();
      virtual void execute();
      virtual void update(const unsigned int, mode);
      virtual void update(const std::vector<unsigned int>&, mode);
      virtual void undo();
     
    private:
      Controller& mC;
      set<unsigned int> mPreviousSelection;
      vector<pair<unsigned int, mode> > mSelection;
  };
}
}

#endif