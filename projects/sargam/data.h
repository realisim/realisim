//
//  data.h

#ifndef realisim_sargam_data_h
#define realisim_sargam_data_h

#include <QString>
#include <vector>

namespace realisim
{
namespace sargam
{

enum noteValue{ nvSa = 1, nvRe, nvGa, nvMa, nvPa, nvDha, nvNi, nvComma, nvChick,
  nvRest };
enum noteModification{ nmKomal, nmShuddh, nmTivra };
enum strokeType{ stDa, stRa, stDiri, stNone };
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

//------------------------------------------------------------------------------
class NoteLocator
{
public:
  NoteLocator();
  NoteLocator( int, int );
  
  int getBar() const;
  int getIndex() const;
  bool isValid() const;
  
protected:
  int mBar;
  int mIndex;
};
bool operator<( const NoteLocator&, const NoteLocator& );

//------------------------------------------------------------------------------
class Composition
{
public:
  Composition();

protected:
  struct Bar
  {
  public:
    Bar();
    std::vector<Note> mNotes;
    std::vector< std::vector<int> > mMatras;
    std::vector<int> mGraceNotes;
  };
  
  struct Ornements
  {
    Ornements();
    std::vector< std::vector<NoteLocator> > mNotes;
  };
  
  struct Line
  {
    Line();
    
    QString mText;
    std::vector<int> mBarIndices;
  };
  
  std::vector<Note> mScale;
  std::vector<Bar> mBars;
  std::vector<Line> mLines;
  std::vector<Ornements> mOrnements;
};
  
}
}

#endif
