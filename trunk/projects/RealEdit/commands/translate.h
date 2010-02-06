/*
 *  commands, translate.h
 *  Created by Pierre-Olivier Beaudoin on 09-08-05.
 */
 
 #ifndef realedit_commands_translate_hh
 #define realedit_commands_translate_hh

namespace realEdit{class EditionData;}
#include "utils/Command.h"
#include "RealEdit/Controller.h"
#include "math/Vect.h"

namespace realEdit
{
namespace commands 
{
  using namespace realisim::math;
  using namespace std;
  class Translate : public realisim::utils::Command
  {
   public:
     
     explicit Translate(EditionData&, const Vector3d&,
       Controller::mode);
     virtual ~Translate();
     
     virtual void execute();
     virtual void undo();
     virtual void update(const Vector3d&);
     
  protected:
    realEdit::EditionData& mEditionData;
    Vector3d mDelta;
    Vector3d mTotalDelta;
    Controller::mode mMode;
  };
}
}

#endif