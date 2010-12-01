/*
 *  Vierwer.h
 *  Realisim
 *
 *  Created by Pierre-Olivier Beaudoin on 10-05-30.
 */
#ifndef RealEdit_PaletteTools_hh
#define RealEdit_PaletteTools_hh

namespace realEdit {class Controller;}
#include "DataModel/EditionData.h"
#include "QtGui/QWidget" 

namespace realEdit
{
namespace palette 
{

class Tools : public QWidget
{
public:
  Tools(QWidget* = 0);
  virtual ~Tools();

  virtual void setController(Controller&);

protected:
  Controller* mpController;
};


}
}

#endif

