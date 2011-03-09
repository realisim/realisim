#ifndef realedit_commands_remove_hh
#define realedit_commands_remove_hh

/*Permet d'enlever un point, segment ou polygone du
  modèle courant.
*/
namespace realEdit{class Controller;}
#include "utils/Command.h"
#include "DataModel/DataModel.h"
#include <vector>
#include <set>

namespace realEdit
{
namespace commands 
{
  class Remove : public realisim::utils::Command
  {
    public:
      explicit Remove(Controller&);
      virtual ~Remove();

      virtual void execute();
      virtual void redo();
      virtual void undo();
     
    private:    
      virtual void removePoint(const RealEditPoint&);
      virtual void removePolygon(const RealEditPolygon&);
      virtual void removeSegment(const RealEditSegment&);

      Controller& mController;
      std::set<unsigned int> mSelection;
      std::vector<RealEditPoint> mPoints;
      std::vector<RealEditPolygon> mPolygons;
      std::vector<RealEditSegment> mSegments;
  };
}
}

#endif