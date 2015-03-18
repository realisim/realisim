//
//  data.h

#ifndef realisim_sargam_data_h
#define realisim_sargam_data_h

namespace realisim
{
namespace sargam
{

enum noteValue{ nvSa = 1, nvRe, nvGa, nvMa, nvPa, nvDha, nvNi, nvComma, nvChick,
  nvRest };
enum noteModification{ nmKomal, nmShuddh, nmTivra };
//enum ornementType{}'
  
//------------------------------------------------------------------------------
class Note
{
public:
  Note( noteValue );
  Note( noteValue, int, noteModification = nmShuddh);
  
  bool canBeKomal() const; //a mettre dans un utilitaire?
  bool canBeTivra() const;
  noteModification getModification() const;
  int getOctave() const;
  noteValue getValue() const;
  void setModification( noteModification );
  void setOctave( int );
  void setValue( noteValue );
  
protected:
  noteValue mValue;
  int mOctave;
  noteModification mModification;
};
  
////------------------------------------------------------------------------------
//class Ornements
//{
//};
//

////------------------------------------------------------------------------------
//class Bar
//{
//};
//
////------------------------------------------------------------------------------
//class composition
//{
//};
  
}
}

#endif
