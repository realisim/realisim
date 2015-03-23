//
//  data.cpp

#include "data.h"

using namespace realisim;
using namespace sargam;

//------------------------------------------------------------------------------
//--- Note
//------------------------------------------------------------------------------
Note::Note( noteValue iV) :
mValue( iV ),
mOctave( 0 ),
mModification( nmShuddh )
{}
//------------------------------------------------------------------------------
Note::Note( noteValue iV, int iO, noteModification iM ) :
  mValue( iV ),
  mOctave( iO ),
  mModification( iM )
{}
//------------------------------------------------------------------------------
bool Note::canBeKomal() const
{
  bool r = false;
  if( mModification != nmKomal &&
     (mValue == nvRe || mValue == nvGa || mValue == nvDha || mValue == nvNi ) )
  { r = true; }
  return r;
}
//------------------------------------------------------------------------------
bool Note::canBeTivra() const
{
  bool r = false;
  if( mModification != nmTivra && mValue == nvMa ){ r = true; }
  return  r;
}
//------------------------------------------------------------------------------
noteModification Note::getModification() const
{ return mModification; }
//------------------------------------------------------------------------------
int Note::getOctave() const
{ return mOctave; }
//------------------------------------------------------------------------------
noteValue Note::getValue() const
{ return mValue; }
//------------------------------------------------------------------------------
void Note::setModification( noteModification iM )
{ mModification = iM; }
//------------------------------------------------------------------------------
void Note::setOctave( int iO )
{ mOctave = iO; }
//------------------------------------------------------------------------------
void Note::setValue( noteValue iV )
{ mValue = iV; }

//------------------------------------------------------------------------------
//--- NoteLocator
//------------------------------------------------------------------------------
NoteLocator::NoteLocator() :
 mBar( -1 ),
 mIndex( -1 )
{}
//------------------------------------------------------------------------------
NoteLocator::NoteLocator( int iBar, int iIndex ) :
mBar( iBar ),
mIndex( iIndex )
{}
//------------------------------------------------------------------------------
int NoteLocator::getBar() const
{ return mBar; }
//------------------------------------------------------------------------------
int NoteLocator::getIndex() const
{ return mIndex; }
//------------------------------------------------------------------------------
bool NoteLocator::isValid() const
{ return mBar != -1 && mIndex != -1; }
//------------------------------------------------------------------------------
bool operator<( const NoteLocator& iLhs, const NoteLocator& iRhs )
{
  bool r = iLhs.getBar() < iRhs.getBar();
  if( iLhs.getBar() == iRhs.getBar() )
  { r = iLhs.getIndex() < iRhs.getIndex(); }
  return r;
}

