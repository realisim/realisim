/*
 *  assembly, rotate.cpp
 *  Project
 *
 *  Created by Pierre-Olivier Beaudoin on 09-08-09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#include "assembly, rotate.h"
#include "ObjectNode.h"
#include "EditionData.h"
#include "MathUtils.h"
#include "Point.h"
#include "Quaternion.h"
#include <vector>

using namespace realisim::math;
using namespace realEdit;
  using namespace commands;
    using namespace assembly;
using namespace std;
  
Rotate::Rotate(EditionData& iEd) :
  mEditionData(iEd)
{}

Rotate::~Rotate()
{}

//------------------------------------------------------------------------------
void Rotate::execute()
{
  ObjectNode* n = mEditionData.getCurrentNode();
  Path p(n);
  //Matrix4d transfo = p.getSceneTransformation();
  //Quat4d nodeTransfo(transfoNoTranslation);
  Vector3d rotationAxis = Vector3d(0.0, 1.0, 0.0) * p.getSceneToNode();
  n->rotate(PI/8.0, rotationAxis);
}

//------------------------------------------------------------------------------
void Rotate::undo()
{
  ObjectNode* n = mEditionData.getCurrentNode();
  n->rotate(-PI/4.0,
    Vector3d( 0.0, 0.0, 1.0 ));
}

