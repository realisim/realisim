/*
 *  commands, translate.h
 *  Created by Pierre-Olivier Beaudoin on 09-08-05.
 */
 
 #ifndef realedit_commands_translate_hh
 #define realedit_commands_translate_hh

#include "math/Vect.h"
namespace realEdit{class Controller;}
#include "utils/Command.h"

namespace realEdit
{
namespace commands 
{
  using namespace realisim::math;
  using namespace std;
  class Translate : public realisim::utils::Command
  {
   public:
     
     explicit Translate(Controller&);
     virtual ~Translate();
     
     virtual void execute();
     virtual void update(const Vector3d&);
     
  protected:
    virtual void undo();
    
    Controller& mC;
    Vector3d mDelta;
    Vector3d mTotalDelta;
  };
}
}

#endif