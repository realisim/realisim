#ifndef realedit_commands_extrude_hh
#define realedit_commands_extrude_hh

/*Permet d'extruder une selection de polygones

  membres:
    mContour: Les points du/des contours ordonnés (voir makeOrderedContour)
    mAddedPoints: Les points ajouté afin des faire les surfaces extrudées.
    mAddedPolygons: Les polygones ajoutés afin de faire les surface extrudées.
    mNewSelection: Les polygones qui seront sélectionnés sur les surface extrudées.
    mpRmove: La commande qui sert à enlever les polygones de la sélection 
      initiale. La sélection sur laquelle la commande extrude est invoquée.
*/

namespace realEdit{class Controller;}
#include "commands/remove.h"
#include "utils/Command.h"
#include "DataModel/DataModel.h"
#include <vector>

namespace realEdit
{
namespace commands 
{
  class Extrude : public realisim::utils::Command
  {
    public:
      explicit Extrude(Controller&);
      virtual ~Extrude();

      virtual void execute();    
      virtual void redo();
      virtual void undo();
    
    private:
      void firstSegment(const RealEditSegment&);
      Vector3d getExtrusionDirection();
      bool keepSegmentOrder(const RealEditSegment&);
      bool makeOrderedContour();
      void nextSegment(std::vector<RealEditSegment>&);

      Controller& mController;
      std::vector<RealEditPoint> mContour;
      std::vector<RealEditPoint> mAddedPoints;
      std::vector<RealEditPolygon> mAddedPolygons;
      std::set<unsigned int> mNewSelection;
      Remove* mpRemove;
  };
}
}

#endif