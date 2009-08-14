/*
 *  commands, translate.cpp
 *  Created by Pierre-Olivier Beaudoin on 09-08-05.
 */

#include "commands, translate.h"
#include "DataModel.h"
#include "EditionData.h"
#include "Point.h"
#include <vector>

using namespace realisim::math;
using namespace realEdit;
  using namespace commands;
using namespace std;
  
Translate::Translate(EditionData& iEd) :
  mEditionData(iEd)
{}

Translate::~Translate()
{}

//------------------------------------------------------------------------------
void Translate::execute()
{
  vector<RealEditPoint>& v = mEditionData.getSelectedPoints();
  for(unsigned int i = 0; i < v.size(); ++i)
    v[i].set(v[i].pos() + Point3d(5.0, 0.0, 0.0));
}

//------------------------------------------------------------------------------
void Translate::undo()
{
  vector<RealEditPoint>& v = mEditionData.getSelectedPoints();
  for(unsigned int i = 0; i < v.size(); ++i)
    v[i].set(v[i].pos() + Point3d(-5.0, 0.0, 0.0));
}

