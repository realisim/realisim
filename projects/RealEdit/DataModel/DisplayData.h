/*
 *  EditionData.h
 *  Project
 *
 *  Created by Pierre-Olivier Beaudoin on 19/11/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef RealEdit_DisplayData_hh
#define RealEdit_DisplayData_hh

namespace RealEdit
{
  class DisplayData;
}

class RealEdit::DisplayData
{
public:
  DisplayData();
  ~DisplayData();
  
  void drawCube() const;
  void initDisplayList();
  
private:
  unsigned int mCube;
};

#endif
