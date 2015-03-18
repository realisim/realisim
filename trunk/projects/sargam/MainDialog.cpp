/* */

#include "MainDialog.h"
#include "math/intersection.h"
#include "math/Line.h"
#include "math/Primitives.h"
#include "math/MathUtils.h"
#include "math/Point.h"
#include "math/Vect.h"
#include "utils/utilities.h"
#include <set>
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */

using namespace std;
using namespace realisim;
  using namespace math;
  using namespace sargam;

namespace
{
  const int kSpacing = 5;
  //page
  const int kInterPageGap = 15;
  const int kPageMarginInCm = 1;
  const int kPageFooter = 15;
  //bar
  const int kBarHeight = 60;
  const int kInterNoteGap = 8;
  const int kMeendHeight = 10;
  const int kKrintanHeight = 6;
  const int kBeatGroupHeight = 10;
}

//-----------------------------------------------------------------------------
// --- partition viewer
//-----------------------------------------------------------------------------
PartitionViewer::PartitionViewer( QWidget* ipParent ) :
  QWidget( ipParent ),
  mIsDebugging( false),
  mPaperSize( 8.5, 11 ),
  mNumberOfPages(0),
  mCurrentBar( -1 ),
  mCurrentNote( -1 ),
  mOctave( 0 ),
  mpTitleLe(0),
  mSargamScaleLabel( "no text for sargamScale" )
{
  setStyleSheet("QLineEdit { border: none }");
  setFocusPolicy( Qt::StrongFocus );
  srand( time(NULL) );
  mTitleFont = QFont( "Arial", 24 );
  mTitleFont.setBold( true );
  mBarFont = QFont( "Arial", 14 );
  mGraceNotesFont = QFont( "Arial", 10 );

  createUi();
  addPage();
  
  //ajout des barres par defauts et de la premiere barre du sargam
  for( int i = 0; i <= bFirstSargamBar; ++i )
  { addBar(); }
  
  //valeurs par defaut de la gamme
  Bar& b = mBars[ bScale ];
  for( int i = nvSa; i <= nvNi; ++i )
  { b.mNotes.push_back( Note( (noteValue)i ) ); }
  setBarAsDirty( bScale, true );
  
  setCurrentBar( bFirstSargamBar );
  
  //generateRandomPartition();
  updateUi();
}

PartitionViewer::~PartitionViewer()
{}
//-----------------------------------------------------------------------------
void PartitionViewer::addBar()
{
  mBars.push_back( Bar() );
  updateUi();
}
//-----------------------------------------------------------------------------
/*Les notes de la selection deviennent des graces notes si elle ne le sont
  pas déjà.*/
void PartitionViewer::addGraceNotesFromSelection()
{
  if( hasSelection() )
  {
    for( int i = 0; i < mNotesSelected.size(); ++i )
    {
      int barIndex = mNotesSelected[i].first;;
      int noteIndex = mNotesSelected[i].second;
      Bar& b = mBars[ barIndex ];
      if( !isGraceNote( barIndex, noteIndex ) )
      { b.mGraceNotes.push_back( noteIndex ); }
      
      setBarAsDirty( barIndex, true );
    }
  }
}
//-----------------------------------------------------------------------------
/*ajoute un matra composé du groupe de pair( barIndex, noteIndex )*/
void PartitionViewer::addMatraFromSelection()
{
  if( hasSelection() )
  {
    int bar = -1, noteIndex = -1;
    vector< int > bg;
    for( int i = 0; i < mNotesSelected.size(); ++i )
    {
      //Si la selection est sur plusieurs barres, on ajoute les groupes
      //separement a chaque barre.
      if( bar != mNotesSelected[i].first && !bg.empty() )
      {
        mBars[bar].mMatraGroups.push_back( bg );
        bg.clear();
        setBarAsDirty( bar , true );
      }
      bar = mNotesSelected[i].first;
      noteIndex = mNotesSelected[i].second;
      bg.push_back( noteIndex );
    }
    mBars[bar].mMatraGroups.push_back( bg );
    setBarAsDirty( bar , true );
  }
}
//-----------------------------------------------------------------------------
void PartitionViewer::addNote( Note iNote )
{ addNote( iNote, getCurrentBar(), getCurrentNote() ); }
//-----------------------------------------------------------------------------
/*Insère la note (iNote, iOctave) dans la barre iBar immédiatement après
  l'index iCursorIndex */
void PartitionViewer::addNote( Note iNote,
  int iBar, int iCursorIndex )
{
  if( iBar >= 0 && iBar < getNumberOfBars() )
  {
    int noteIndex = iCursorIndex + 1;
    vector< Note >::iterator pos = mBars[iBar].mNotes.begin() +
      noteIndex;
    
    mBars[iBar].mNotes.insert( pos, iNote );
    //on shift le matra de 1
    shiftMatra( 1, iBar, iCursorIndex );
    //Si la note au curseur, ainsi que la suivante, sont dans le matra, la
    //nouvelle note devra aussi etre dans le matra
    int matra1 = findMatra( iBar, iCursorIndex );
    int matra2 = findMatra( iBar, iCursorIndex + 2 );
    if( matra1 != -1 && matra1 == matra2 )
    { addNoteToMatra( findMatra( iBar, iCursorIndex ), iBar, noteIndex ); }
    
    //on shift les ornements de 1
    shiftOrnement( 1, iBar, iCursorIndex );
    //Si la note au curseur, ainsi que la suivante, sont dans lornement, la
    //nouvelle note devra aussi etre dans l'ornement
    int ornm1 = findOrnement( iBar, iCursorIndex );
    int ornm2 = findOrnement( iBar, iCursorIndex + 2 );
    if( ornm1 != -1 && ornm1 == ornm2 )
    if( isNoteInOrnement( iBar, iCursorIndex) && isNoteInOrnement( iBar, iCursorIndex + 1) )
    { addNoteToOrnement( findOrnement( iBar, iCursorIndex ), iBar, noteIndex ); }
    
    //on shift les graces notes
    shiftGraceNotes( 1, iBar, iCursorIndex );
    //Si la note au curseur, ainsi que la suivante, sont des notes de grace, la
    //nouvelle note devra aussi etre une note de grace
    if( isGraceNote( iBar, iCursorIndex ) && isGraceNote( iBar, iCursorIndex + 1 ) )
    { addToGraceNotes( iBar, noteIndex ); }
    
    setBarAsDirty( iBar, true );
    updateUi();
  }
}
//-----------------------------------------------------------------------------
void PartitionViewer::addNoteToMatra( int iMatraIndex, int iBar, int iNoteIndex )
{
  vector<int>& m = mBars[iBar].mMatraGroups[iMatraIndex];
  m.push_back( iNoteIndex );
  sort( m.begin(), m.end() );
}
//-----------------------------------------------------------------------------
void PartitionViewer::addNoteToOrnement( int iIndex, int iBar, int iNoteIndex )
{
  Ornement& o = mOrnements[iIndex];
  o.mNotes.push_back( make_pair( iBar, iNoteIndex ) );
  sort( o.mNotes.begin(), o.mNotes.end() );
  for( int i = 0; i < o.barsInvolved().size(); ++i )
  { setBarAsDirty( o.barsInvolved()[i] , true); }
}
//-----------------------------------------------------------------------------
void PartitionViewer::addNoteToSelection( int iBar, int iNote )
{
  mNotesSelected.push_back( make_pair( iBar, iNote ) );
  sort( mNotesSelected.begin(), mNotesSelected.end() );
  setBarAsDirty( iBar, true );
}
//-----------------------------------------------------------------------------
void PartitionViewer::addOrnementFromSelection( ornementType iT )
{
  for( int i = 0; i < mNotesSelected.size(); ++i )
  {
    int bar = mNotesSelected[i].first;
    setBarAsDirty( bar , true );
  }
  Ornement t;
  t.mOrnementType = iT;
  t.mNotes = mNotesSelected;
  mOrnements.push_back( t );
}
//-----------------------------------------------------------------------------
void PartitionViewer::addPage()
{
  mNumberOfPages++;
  QRect p = getRegion( rPartition );
  resize( p.width()+1, p.height()+1 );
}
//-----------------------------------------------------------------------------
void PartitionViewer::addToGraceNotes( int iBar, int iNoteIndex )
{
  Bar& b = mBars[ iBar ];
  b.mGraceNotes.push_back( iNoteIndex );
  sort( b.mGraceNotes.begin(), b.mGraceNotes.end() );
}
//-----------------------------------------------------------------------------
void PartitionViewer::clear()
{
  mNumberOfPages = 1;
  mBars.clear();
  mOrnements.clear();
  mCurrentBar = -1;
  mCurrentNote = -1;
}
//-----------------------------------------------------------------------------
void PartitionViewer::clearSelection()
{
  for( int i = 0; i < mNotesSelected.size(); ++i )
  { setBarAsDirty( mNotesSelected[i].first , true ); }
  mNotesSelected.clear();
}
//-----------------------------------------------------------------------------
int PartitionViewer::cmToPixel( double iCm ) const
{ return logicalDpiX() * iCm / 2.54; }
//-----------------------------------------------------------------------------
void PartitionViewer::commandAddNote( noteValue iN )
{
  if( hasSelection() )
  { commandErase(); }
  addNote( makeNoteFromScale( iN ) );
  mCurrentNote++;
  clearSelection();
}
//-----------------------------------------------------------------------------
void PartitionViewer::commandBreakMatrasFromSelection()
{
  for( int i = 0; i < mNotesSelected.size(); ++i )
  {
    int bar = mNotesSelected[i].first;
    int index = mNotesSelected[i].second;
    int matraIndex = findMatra( bar, index );
    if( matraIndex != -1 )
    { eraseMatra( bar, matraIndex ); }
  }
}
//-----------------------------------------------------------------------------
void PartitionViewer::commandBreakOrnementsFromSelection()
{
  for( int i = 0; i < mNotesSelected.size(); ++i )
  {
    int bar = mNotesSelected[i].first;
    int index = mNotesSelected[i].second;
    int ornementIndex = findOrnement( bar, index );
    if( ornementIndex != -1 )
    { eraseOrnement( ornementIndex ); }
  }
}
//-----------------------------------------------------------------------------
void PartitionViewer::commandErase()
{
  if( hasSelection() )
  {
    for( int i = mNotesSelected.size() - 1; i >= 0; --i )
    { eraseNote( mNotesSelected[i].first, mNotesSelected[i].second); }
    setCurrentBar( mNotesSelected[0].first );
    /*Le -1 est parce que la premiere note selectionnée est effacée, il faut
      donc aller à l'autre d'avant.*/
    setCurrentNote( mNotesSelected[0].second - 1);
    clearSelection();
  }
  else
  {
    eraseNote( getCurrentBar(), getCurrentNote() );
    mCurrentNote--;
  };
}
//-----------------------------------------------------------------------------
void PartitionViewer::commandShiftNote()
{
  vector< pair<int, int> > l;
  if( hasSelection() )
  { l = mNotesSelected; }
  else
  { l.push_back( make_pair( getCurrentBar(), getCurrentNote()) ); }
 
  for( int i = 0; i < l.size(); ++i )
  {
    int bar = l[i].first;
    int index = l[i].second;
    Note& n = getNote( bar, index );
    if( n.getModification() == nmKomal || n.getModification() == nmTivra )
    { n.setModification( nmShuddh ); }
    else if( n.canBeKomal() )
    { n.setModification( nmKomal); }
    else if( n.canBeTivra() )
    { n.setModification( nmTivra ); }
    
    setBarAsDirty( bar, true );
  }
  
}
//-----------------------------------------------------------------------------
void PartitionViewer::createUi()
{
  //new
  mpTitleLe = new QLineEdit( this );
  
  //titre
  connect( mpTitleLe, SIGNAL( textChanged(const QString&)),
          this, SLOT( titleChanged(const QString&) ) );
  mpTitleLe->setFont( mTitleFont );
  mpTitleLe->setText( "Untitled" );

  mSargamScaleLabel = "Scale: ";
}
//-----------------------------------------------------------------------------
void PartitionViewer::decreaseOctave()
{ mOctave = std::max( --mOctave, -1 ); }
//-----------------------------------------------------------------------------
void PartitionViewer::eraseGraceNote( int iBar, int iNoteIndex )
{
  Bar& b = mBars[ iBar ];
  for( int i = 0; i < b.mGraceNotes.size(); ++i )
  {
    if( b.mGraceNotes[i] == iNoteIndex )
    { b.mGraceNotes.erase( b.mGraceNotes.begin() + i ); break; }
  }
}
//-----------------------------------------------------------------------------
void PartitionViewer::eraseMatra( int iBar, int iMatraIndex )
{
  Bar& b = mBars[iBar];
  b.mMatraGroups.erase( b.mMatraGroups.begin() + iMatraIndex );
  setBarAsDirty( iBar, true );
}
//-----------------------------------------------------------------------------
void PartitionViewer::eraseNote( int iBar, int iNoteIndex )
{
  if( iBar >= 0 && iBar < getNumberOfBars() &&
     iNoteIndex >= 0 && iNoteIndex < getNumberOfNotesInBar(iBar) )
  {
    Bar& b = mBars[iBar];
    b.mNotes.erase( b.mNotes.begin() + iNoteIndex );

    //si la note est dans le matra, on l'enleve, sinon on shift le groupe
    if( isNoteInMatra( iBar, iNoteIndex ) )
    { eraseNoteFromMatra( iBar, iNoteIndex ); }
    shiftMatra( -1, iBar, iNoteIndex );
    
    //on enleve des ornements
    if( isNoteInOrnement( iBar, iNoteIndex ) )
    { eraseNoteFromOrnement( iBar, iNoteIndex ); }
    shiftOrnement( -1, iBar, iNoteIndex );
    
    //on eneleve les graceNotes
    if( isGraceNote( iBar, iNoteIndex ) )
    { eraseGraceNote( iBar, iNoteIndex ); }
    shiftGraceNotes( -1, iBar, iNoteIndex );
    
    setBarAsDirty( iBar, true);
    updateUi();
  }
}
//-----------------------------------------------------------------------------
void PartitionViewer::eraseNoteFromMatra( int iBar, int iNoteIndex )
{
  Bar& b = mBars[ iBar ];
  //on enleve iNoteIndex de tous les matra de la barre iBar
  for( int i = 0; i < b.mMatraGroups.size(); ++i )
  {
    vector<int>& bg = b.mMatraGroups[i];
    vector<int>::iterator it = bg.begin();
    for( ; it != bg.end(); ++it )
    {
      if( *it == iNoteIndex )
      {
        it = bg.erase( it );
        break;
      }
    }
  }
}
//-----------------------------------------------------------------------------
void PartitionViewer::eraseNoteFromOrnement( int iBar, int iNoteIndex )
{
  //SI on trouve la note dans un ornement, on l'enleve et ensuite on décrémente
  //tous les indices des notes suivantes dans cet ornement.
  for( int i = 0; i < mOrnements.size(); ++i )
  {
    Ornement& o = mOrnements[i];
    vector< pair< int, int > >::iterator it = o.mNotes.begin();
    for( ; it != o.mNotes.end(); ++it )
    {
      if( (*it).first == iBar && (*it).second == iNoteIndex )
      {
        it = o.mNotes.erase( it );
        setBarAsDirty( it->first, true );
        break;
      }
    }
  }
}
//-----------------------------------------------------------------------------
void PartitionViewer::eraseOrnement( int iIndex )
{
  Ornement& o = mOrnements[iIndex];
  for( int i = 0; i < o.mNotes.size(); ++i )
  { setBarAsDirty( o.mNotes[i].first, true ); }
  mOrnements.erase( mOrnements.begin() + iIndex );
}
//-----------------------------------------------------------------------------
/* Cherche dans les matras de iBar pour la note iNoteIndex, si elle est trouvé,
   la fonction retourne l'indice du matra sinon -1.*/
int PartitionViewer::findMatra( int iBar, int iNoteIndex ) const
{
  int r = -1;
  const Bar& b = mBars[iBar];
  for( int i = 0; i < b.mMatraGroups.size(); ++i )
  {
    const vector< int >& g = b.mMatraGroups[i];
    for( int j = 0; j < g.size(); ++j )
    {
      if( g[j] == iNoteIndex ){ r = i; break; }
    }
  }
  return r;
}
//-----------------------------------------------------------------------------
/* Cherche dans les ornement de iBar pour la note iNoteIndex, si elle est trouvé,
 la fonction retourne l'indice du matra sinon -1.*/
int PartitionViewer::findOrnement( int iBar, int iNoteIndex ) const
{
  int r = -1;
  for( int i = 0; i < mOrnements.size(); ++i )
  {
    const Ornement& o = mOrnements[i];
    for( int j = 0; j < o.mNotes.size(); ++j )
    {
      if( o.mNotes[j].first == iBar && o.mNotes[j].second == iNoteIndex )
      { r = i; break; }
    }
  }
  return r;
}
//-----------------------------------------------------------------------------
void PartitionViewer::generateRandomPartition()
{
  clear();
  const int kNumBars = 52;
  mBars.resize( kNumBars );
  for( int i = bFirstSargamBar; i < getNumberOfBars() - 1; ++i )
  {
    double norm = rand() / (double)RAND_MAX;
    int numNotes = 4 + norm * 16;
    for( int j = 0; j < numNotes; ++j )
    {
      norm = rand() / (double)RAND_MAX;
      int note = 1 + norm * 6; //entre 1 et 7
      int octave;
      if( norm < 0.33 ) octave = -1;
      else if( 0.33 <= norm && norm < 0.66 ) octave = 0;
      else octave = 1;
      mBars[i].mNotes.push_back( Note( (noteValue)note, octave ) );
    }
  }
  updateUi();
}
//-----------------------------------------------------------------------------
int PartitionViewer::getBarRegion( barRegion br ) const
{
  int r = 0;
  QFontMetrics fm(mBarFont);
  switch( br )
  {
    case brSeparatorX: r = 5; break;
    case brNoteStartX: r = 10; break;
    case brNoteTopY: r = kBarHeight / 2 - fm.height() / 2; break;
    case brNoteBottomY: r = kBarHeight / 2 + fm.height() / 2; break;
    case brStrokeY: r = kBarHeight / 2 + fm.height() / 2 + 5; break;
    case brOrnementY: r = getBarRegion(brGraceNoteTopY) - 8; break;
    case brMatraGroupY: r = getBarRegion(brNoteBottomY); break;
    case brGraceNoteTopY:
    {
      QFontMetrics fm(mGraceNotesFont);
      r = getBarRegion( brNoteTopY ) - fm.height() / 2;
    } break;
    default: break;
  }
  return r;
}
//-----------------------------------------------------------------------------
int PartitionViewer::getCurrentBar() const
{ return mCurrentBar; }
//-----------------------------------------------------------------------------
int PartitionViewer::getCurrentNote() const
{ return mCurrentNote; }
//-----------------------------------------------------------------------------
int PartitionViewer::getInterNoteSpacing(int iBar, int iIndex1, int iIndex2 ) const
{
  
  int r = kInterNoteGap;
  int matra1 = findMatra( iBar, iIndex1 );
  int matra2 = findMatra( iBar, iIndex2 );
  //les notes d'un même matra
  if( matra1 == matra2 && matra1 != -1 && matra2 != -1 )
  {
    r = 2;
    //si ce sont des graceNotes
    if( isGraceNote( iBar, iIndex1 ) && isGraceNote( iBar, iIndex2 ) )
    { r = 1; }
  }
  return r;
}
//-----------------------------------------------------------------------------
Note& PartitionViewer::getNote( int iBar, int iNote )
{ return mBars[iBar].mNotes[iNote]; }
//-----------------------------------------------------------------------------
const Note& PartitionViewer::getNote( int iBar, int iNote ) const
{ return mBars[iBar].mNotes[iNote]; }
//-----------------------------------------------------------------------------
int PartitionViewer::getNumberOfBars() const
{ return mBars.size(); }
//-----------------------------------------------------------------------------
int PartitionViewer::getNumberOfPages() const
{ return mNumberOfPages; }
//-----------------------------------------------------------------------------
int PartitionViewer::getNumberOfNotesInBar( int iB ) const
{ return mBars[iB].mNotes.size(); }
//-----------------------------------------------------------------------------
int PartitionViewer::getOctave() const
{ return mOctave; }
//-----------------------------------------------------------------------------
QRect PartitionViewer::getPageRegion( pageRegion iR, int iPage /*=0*/ ) const
{
  QRect r(0, 0, 1, 1);
  
  switch( iR )
  {
    case prPage:
    {
      int w = getPaperSizeInInch().width() * logicalDpiX();
      int h = getPaperSizeInInch().height() * logicalDpiY();
      int t = iPage * h + iPage * kInterPageGap;
      r.setTop( t );
      r.setWidth( w ); r.setHeight( h );
    }break;
    case prBody:
    {
      int margin = cmToPixel( kPageMarginInCm );
      int sargamScaleBottom = iPage == 0 ? getRegion(rSargamScale).bottom() + kSpacing : 0;
      int startOfBody = max( margin, sargamScaleBottom );
      int paperWidth = getPaperSizeInInch().width() * logicalDpiX();
      int paperHeight = getPaperSizeInInch().height() * logicalDpiY();
      int topOfPage = getPageRegion( prPage, iPage ).top();//iPage * paperHeight + iPage * kInterPageGap;
      r.setLeft( margin );
      r.setWidth( paperWidth - 2*margin );
      r.setTop( topOfPage + startOfBody );
      r.setBottom( topOfPage + paperHeight - margin );
    }break;
    case prPageFooter:
    {
      r = getPageRegion( prPage, iPage );
      r.setBottom( r.bottom() );
      r.setTop( r.bottom() - kPageFooter );
    }break;
  }
  
  return r;
}

//-----------------------------------------------------------------------------
QSizeF PartitionViewer::getPaperSizeInInch() const
{ return mPaperSize; }
//-----------------------------------------------------------------------------
QRect PartitionViewer::getRegion( region iR ) const
{
  QRect r(0, 0, 1, 1);
  
  switch( iR )
  {
    case rPartition:
    {
      int w = getPaperSizeInInch().width() * logicalDpiX();
      int h = getNumberOfPages() * getPaperSizeInInch().height() * logicalDpiY() +
      std::max(getNumberOfPages() - 1, 0) * kInterPageGap;
      r.setWidth( w ); r.setHeight( h );
    }break;
    case rTitle:
    {
      int w = getPaperSizeInInch().width() * logicalDpiX();
      int h = mpTitleLe->height();
      r.setTop( cmToPixel( kPageMarginInCm ) ); r.setWidth( w ); r.setHeight( h );
    }break;
    case rSargamScaleLabel:
    {
      QFontMetrics fm( mBarFont );
      QRect t = getRegion( rTitle );
      r = t;
      r.translate( 0, t.height() + kSpacing );
      r.setHeight( kBarHeight );
      r.setWidth( fm.width( mSargamScaleLabel ) );
    } break;
    case rSargamScale:
    {
      QFontMetrics fm( mBarFont );
      QRect t = getRegion( rTitle );
      r = t;
      r.translate( 0, t.height() + kSpacing );
      r.setLeft( fm.width( mSargamScaleLabel ) );
      r.setHeight( kBarHeight );
    } break;
  }
  return r;
}
//-----------------------------------------------------------------------------
vector<Note> PartitionViewer::getScale() const
{
  vector<Note> r;
  r = mBars[ bScale ].mNotes;
  return r;
}
//-----------------------------------------------------------------------------
bool PartitionViewer::hasSelection() const
{ return mNotesSelected.size() > 0; }
//-----------------------------------------------------------------------------
bool PartitionViewer::isDebugging() const
{ return mIsDebugging; }
//-----------------------------------------------------------------------------
bool PartitionViewer::isGraceNote(int iBar, int iNoteIndex ) const
{
  const Bar& b = mBars[iBar];
  return ::find( b.mGraceNotes.begin(), b.mGraceNotes.end(), iNoteIndex ) !=
    b.mGraceNotes.end();
}
//-----------------------------------------------------------------------------
bool PartitionViewer::isNoteInMatra(int iBar, int iNoteIndex ) const
{ return findMatra( iBar, iNoteIndex ) != -1; }
//-----------------------------------------------------------------------------
bool PartitionViewer::isNoteInOrnement( int iBar, int iNoteIndex ) const
{ return findOrnement( iBar, iNoteIndex) != -1; }
//-----------------------------------------------------------------------------
bool PartitionViewer::isNoteSelected(int iBar, int iNoteIndex ) const
{
  bool r = false;
  for( int i = 0; i < mNotesSelected.size(); ++i )
  {
    if( iBar == mNotesSelected[i].first && iNoteIndex == mNotesSelected[i].second )
    { r = true; break; }
  }
  return r;
}
//-----------------------------------------------------------------------------
void PartitionViewer::increaseOctave()
{ mOctave = std::min( ++mOctave, 1 ); }
//-----------------------------------------------------------------------------
void PartitionViewer::keyPressEvent( QKeyEvent* ipE )
{
  switch (ipE->key())
  {
    case Qt::Key_1: commandAddNote( nvSa ); break;
    case Qt::Key_2: commandAddNote( nvRe ); break;
    case Qt::Key_3: commandAddNote( nvGa ); break;
    case Qt::Key_4: commandAddNote( nvMa );  break;
    case Qt::Key_5: commandAddNote( nvPa );  break;
    case Qt::Key_6: commandAddNote( nvDha );  break;
    case Qt::Key_7: commandAddNote( nvNi );  break;
    case Qt::Key_Comma: commandAddNote( nvComma ); break;
    case Qt::Key_C: commandAddNote( nvChick ); break;
    case Qt::Key_G:
      if( (ipE->modifiers() & Qt::ShiftModifier) )
      { commandBreakMatrasFromSelection(); }
      else{ addMatraFromSelection();}
      break;
    case Qt::Key_K:
      if( (ipE->modifiers() & Qt::ShiftModifier) )
      { commandBreakOrnementsFromSelection(); }
      else{ addOrnementFromSelection( otKrintan ); }
      break;
    case Qt::Key_M:
      if( (ipE->modifiers() & Qt::ShiftModifier) )
      { commandBreakOrnementsFromSelection(); }
      else{ addOrnementFromSelection( otMeend ); }
      break;
    case Qt::Key_N: addGraceNotesFromSelection(); break;
    case Qt::Key_R: commandAddNote( nvRest );break;
    case Qt::Key_S: commandShiftNote();break;
    case Qt::Key_Left:
      /*S'il ny a pas de selection ou que shift est pesé, on déplace le
       curseur vers la gauche. Lorsqu'il atteint -1, on essait de changer
       de barre si possible et la note courante devient la derniere note
       de la barre précédente.
       S'il y avait une selection et que shift n'est plus enfoncé, on
       enleve la selection et la note courante devient la premieres note
       de la selection
       */
      if( !hasSelection() || (ipE->modifiers() & Qt::ShiftModifier) )
      {
        //ajoute la note courante a la selection si on pese shift
        if( ipE->modifiers() & Qt::ShiftModifier )
        { addNoteToSelection( getCurrentBar(), mCurrentNote ); }
        
        mCurrentNote = std::max( --mCurrentNote, -2 );
        if( getCurrentNote() == -2 && getCurrentBar() > 0 )
        {
          setCurrentBar( getCurrentBar() - 1 );
          setCurrentNote( getNumberOfNotesInBar( getCurrentBar() ) - 1 );
        }
      }
      else
      {
        setCurrentBar( mNotesSelected[0].first );
        setCurrentNote( mNotesSelected[0].second - 1);
        clearSelection();
      }
      break;
    case Qt::Key_Right:
      /*S'il ny a pas de selection ou que shift est pesé, on déplace le
       curseur vers la droite. Lorsqu'il atteint la derniere note de la
       barre, on essait de changer de barre si possible et la note courante
       devient la premiere note de la barre suivante.
       S'il y avait une selection et que shift n'est plus enfoncé, on
       enleve la selection et la note courante devient la derniere note
       de la selection */
      if( !hasSelection() || (ipE->modifiers() & Qt::ShiftModifier) )
      {
        int maxIndex = getNumberOfNotesInBar( getCurrentBar() ) - 1;
        ++mCurrentNote;
        if( getCurrentNote() > maxIndex &&
           getCurrentBar() < getNumberOfBars() - 1 )
        {
          setCurrentBar( getCurrentBar() + 1 );
          setCurrentNote( -1 );
        }
        else { mCurrentNote = std::min( mCurrentNote, maxIndex ); }
        //ajoute la note courante a la selection si on pese shift
        if( ipE->modifiers() & Qt::ShiftModifier )
        { addNoteToSelection( getCurrentBar(), mCurrentNote ); }
      }
      else
      {
        setCurrentBar( mNotesSelected[0].first );
        setCurrentNote( mNotesSelected[ mNotesSelected.size() - 1 ].second );
        clearSelection();
      }
      break;
    case Qt::Key_Space:
      addBar();
      setCurrentNote( -1 );
      setCurrentBar( getNumberOfBars() -1 );
      clearSelection();
      break;
    case Qt::Key_Backspace: commandErase(); break;
    case Qt::Key_Plus: increaseOctave(); break;
    case Qt::Key_Minus: decreaseOctave(); break;
    case Qt::Key_Shift: break;
    default: break;
  }
  updateUi();
}
//-----------------------------------------------------------------------------
void PartitionViewer::keyReleaseEvent( QKeyEvent* ipE )
{
  switch (ipE->key())
  {
    default: break;
  }
}
//-----------------------------------------------------------------------------
Note PartitionViewer::makeNoteFromScale( noteValue iN ) const
{
  Note r( iN, 0, nmShuddh );
  vector<Note> v = getScale();
  for( int i = 0; i < v.size(); ++i )
  {
    if( v[i].getValue() == r.getValue() )
    { r.setModification( v[i].getModification() ); break; }
  }
  return r;
}
//-----------------------------------------------------------------------------
QString PartitionViewer::noteToString( Note iNote ) const
{
  int note = iNote.getValue();
  int octave = iNote.getOctave();
  noteModification nm = iNote.getModification();
  
  QString r;
  switch( note )
  {
    case nvSa: r = "S"; break;
    case nvRe: if(nm == nmKomal){r = "f";}else {r = "R";} break;
    case nvGa: if(nm == nmKomal){r = "g";}else {r = "G";} break;
    case nvMa: if(nm == nmTivra){r = "M";}else {r = "m";} break;
    case nvPa: r = "P"; break;
    case nvDha: if(nm == nmKomal){r = "d";}else {r = "D";} break;
    case nvNi: if(nm == nmKomal){r = "n";}else {r = "N";} break;
    case nvChick: r = "\xE2\x9c\x93"; break; //check
    case nvRest: r = "\xE2\x80\x94"; break; //barre horizontale
    case nvComma: r = ","; break;
    default: break;
  }
  
  //ajout de la notation octave
  if( note != nvChick && note != nvRest && note != nvComma )
  {
    switch (octave)
    {
      case -1: r += "\xCC\xA3"; break;
      case 0: break;
      case 1: r += "\xCC\x87"; break;
      default: break;
    }
  }
  return r;
}
//-----------------------------------------------------------------------------
void PartitionViewer::paintEvent( QPaintEvent* ipE )
{
  QRect r = ipE->region().boundingRect();
  QPainter p(this);
  p.setBackgroundMode( Qt::OpaqueMode );
  QBrush b( Qt::white );
  QPen pen = p.pen();
  p.setBackground( b );
  p.setRenderHints( p.renderHints() | QPainter::Antialiasing );
  
  //--- render pages
  for( int i = 0; i < getNumberOfPages(); ++i)
  {
    //on dessine le background blanc de la page
    p.setPen( Qt::white );
    p.setBrush( Qt::white );
    p.drawRect( getPageRegion( prPage, i ) );
   
    //--- tout le text de la page
    p.setPen( Qt::black );
    p.setBrush( Qt::NoBrush );
    
    //on dessine le bas de page
    //l'indice de page commence à 0, mais on veut que montrer page 1.
    p.setFont(QFont("Arial", 10));
    p.drawText( getPageRegion( prPageFooter, i ), Qt::AlignCenter,
               "page " + QString::number(i) + 1 );
  }
  
  //--- rendu des textes pour barres speciales (sargam scale, tarab tuning...)
  p.setFont( mBarFont );
  p.drawText( getRegion( rSargamScaleLabel ), Qt::AlignCenter, mSargamScaleLabel );
             
  //render bars
  for( int i = 0; i < getNumberOfBars(); ++i )
  {
    Bar& b = mBars[i];
    //on blit le pixmap dans le widget.
    QRect cut( 0, 0, 0, 0 );
    for( int j = 0; j < b.mPageLayout.size(); ++j )
    {
      cut.setSize( b.mPageLayout[j].size() );
      p.drawPixmap( b.mPageLayout[j], b.mPixmap, cut );
      cut.setLeft( cut.width() + 1 );
      
      //le curseur dans la barre courante s'il n'y a pas de selection
      pen.setColor( Qt::black );
      pen.setWidth( 1 );
      p.setPen( pen );
      if( /*!hasSelection() &&*/ getCurrentBar() == i )
      {
        QLine l;
        if( getCurrentNote() >= 0 )
        {
          QRect r = b.mNotesPageLayout[getCurrentNote()];
          l.setPoints( r.topRight(), r.bottomRight() );
        }
        else
        {
          int x = b.mPageLayout[0].left() + getBarRegion( brNoteStartX );
          int y = b.mPageLayout[0].top() + getBarRegion( brNoteTopY );
          int h = getBarRegion( brNoteBottomY ) - getBarRegion( brNoteTopY );
          l.setPoints( QPoint(x,y) , QPoint( x, y + h ) );
        }
        p.drawLine( l );
      }
    }
  }
  
  //on dessine le contour de la barre courante
  //le contoure de la barre selectionné
  if( getCurrentBar() != -1 )
  {
    pen.setColor( QColor( 125, 125, 125, 120 ) );
    pen.setWidth( 2 );
    p.setPen( pen );
    Bar& b = mBars[ getCurrentBar() ];
    for( int i = 0; i < b.mPageLayout.size(); ++i )
    { p.drawRoundedRect( b.mPageLayout[i], 2, 2 ); }
  }
  
  //--- render debug infos
  if( isDebugging() )
  {
    p.setFont(QFont("Arial", 10));
    p.setPen(Qt::blue);
    p.setBrush( Qt::NoBrush );
    for( int i = 0; i < getNumberOfPages(); ++i)
    {
      if( i == 0 )
      {
        p.drawRect( getRegion( rTitle ) );
        p.drawRect( getRegion( rSargamScaleLabel ) );
        p.drawRect( getRegion( rSargamScale ) );
      }
      p.drawRect( getPageRegion( prBody, i ) );
      p.drawRect( getPageRegion( prPageFooter, i ) );
    }
    
    //on dessine le layout des bars...
    for( int i = 0; i < getNumberOfBars(); ++i )
    {
      Bar& b = mBars[i];
      //le layput page de la bar
      for( int j = 0; j < b.mPageLayout.size(); ++j )
      {
        p.drawRect( b.mPageLayout[j] );
        p.drawText( b.mPageLayout[j], Qt::AlignCenter, QString::number(i) );
        QString s;
        if( getCurrentBar() == i )
        { s += QString().sprintf("current note: %d\n", getCurrentNote() ); }
        s += QString().sprintf("number of notes: %d", getNumberOfNotesInBar( i ) );
        p.drawText( b.mPageLayout[j], Qt::AlignBottom | Qt::AlignRight, s );
      }
      //le layout page des notes
      for( int j = 0; j < b.mNotesPageLayout.size(); ++j )
      { p.drawRect( b.mNotesPageLayout[j] ); }
    }
    
    p.setPen(Qt::blue);
    QString s;
    s.sprintf("Partition size: %d, %d\n"
              "Region to repaint: %d, %d, %d, %d",
              getRegion( rPartition ).width(), getRegion( rPartition ).height(),
              r.left(), r.top(), r.right(), r.bottom() );
    p.drawText(rect(), Qt::AlignBottom | Qt::AlignRight, s);
  }
}
//-----------------------------------------------------------------------------
void PartitionViewer::renderBarOffscreen( int iBar )
{
  Bar& b = mBars[ iBar ];
  b.mPixmap = QPixmap( b.mRect.width(), b.mRect.height() );
  b.mPixmap.fill( Qt::white );
  QPainter bp( &b.mPixmap );
  bp.setRenderHints( bp.renderHints() | QPainter::Antialiasing );
  
  //les notes
  /*On utilise la layout lineaire pour dessiner la bar dans le pixmap
   hors ecran. La soustraction "int tx = noteLinearLayout.left() - barLinear.left();"
   est pour déplacer le contenu lineaire de la bar à zéro.*/
  for( int j = 0; j < getNumberOfNotesInBar( iBar ); ++j )
  {
    QString s = noteToString( getNote( iBar, j ) );
    QFont f = isGraceNote( iBar, j ) ? mGraceNotesFont : mBarFont;
    bp.setFont(f);
    bp.drawText( b.getNoteRect(j), s);
  }
  
  //la barre vertical
  QPen pen = bp.pen();
  pen.setWidth( 2 );
  bp.setPen( pen );
  int x1 = getBarRegion( brSeparatorX );
  int y1 = 0.15 * kBarHeight;
  int y2 = kBarHeight - 0.15 * kBarHeight;
  bp.drawLine( x1, y1, x1, y2 );
  
  //b.mPixmap.toImage().save( "bar" + QString::number(i) + ".png", "PNG" );
  
  //--- render beat groups
  pen.setWidth( 1 );
  bp.setPen( pen );
  for( int i = 0; i < b.mMatraGroupsRect.size(); ++i )
  {
    QRect r = b.mMatraGroupsRect[i];
    bp.drawArc( r, -10 * 16, -170 * 16 );
  }
  
  //--- render Ornements: meends and krintans
  for( int i = 0; i < mOrnements.size(); ++i )
  {
    Ornement& m = mOrnements[i];
    if( m.appliesToBar( iBar ) )
    {
      QPixmap pix( m.mFullOrnement.width(), m.mFullOrnement.height() );
      pix.fill( QColor( 0, 0, 0, 0 ) );
      QPainter mp( &pix );
      mp.setRenderHints( mp.renderHints() | QPainter::Antialiasing );
      QPen ornementPen;
      ornementPen.setColor( Qt::black );
      ornementPen.setWidth( 1 );
      mp.setPen( ornementPen );
      mp.setBrush( Qt::NoBrush );
      if( m.mOrnementType == otMeend )
      { mp.drawArc( pix.rect(), 10 * 16, 170 * 16 ); }
      else //krintan
      {
        QRect r = pix.rect(); r.adjust( 1, 1, -1, -1 );
        mp.drawLine( r.bottomLeft(), r.topLeft() );
        mp.drawLine( r.topLeft(), r.topRight() );
        mp.drawLine( r.topRight(), r.bottomRight() );
      }
//pix.toImage().save( "Ornement.png", "PNG" );
      //blit dans le rendu pixmap hors écran de la barre.
      QRect destination = m.getBlit( iBar );
      QRect source = m.getCut( iBar );
      destination.translate( 0, getBarRegion( brOrnementY ) );
      bp.drawPixmap( destination, pix, source );
    }
  }
  
  //render selection
  {
    QRect r;
    for( int i = 0; i < mNotesSelected.size(); ++i )
    {
      int barIndex = mNotesSelected[i].first;
      if( barIndex == iBar )
      {
        Bar& b = mBars[ barIndex ];
        int noteIndex = mNotesSelected[i].second;
        if( r.isNull() )
        { r = b.getNoteRect(noteIndex); }
        else{ { r = r.united(b.getNoteRect(noteIndex)); } }
      }
    }
    
    pen.setColor( QColor( 0, 0, 255, 185 ) );
    pen.setWidth( 2 );
    bp.setPen( pen );
    if( !r.isNull() )
    {
      r.adjust( -2, -2, 2, 2 );
      bp.drawRoundedRect( r, 2, 2 );
    }
  }

}
//-----------------------------------------------------------------------------
void PartitionViewer::setAsDebugging( bool iD )
{ mIsDebugging = iD; updateUi(); }
//-----------------------------------------------------------------------------
void PartitionViewer::setBarAsDirty( int iBar, bool iD )
{
  if( iBar >= 0 && iBar < getNumberOfBars() )
  { mBars[iBar].mIsDirty = iD; }
}
//-----------------------------------------------------------------------------
void PartitionViewer::setCurrentBar( int iB )
{ mCurrentBar = iB; updateUi(); }
//-----------------------------------------------------------------------------
void PartitionViewer::setCurrentNote( int iN )
{ mCurrentNote = iN; updateUi(); }
//-----------------------------------------------------------------------------
void PartitionViewer::shiftGraceNotes( int iN, int iBar, int iFromIndex )
{
  Bar& b = mBars[iBar];
  for( int i = 0; i < b.mGraceNotes.size(); ++i )
  {
    if( b.mGraceNotes[i] > iFromIndex )
    { b.mGraceNotes[i] += iN; }
  }
}
//-----------------------------------------------------------------------------
void PartitionViewer::shiftMatra( int iN, int iBar, int iFromIndex )
{
  Bar& b = mBars[iBar];
  for( int i = 0; i < b.mMatraGroups.size(); ++i )
  {
    vector<int>& m = b.mMatraGroups[i];
    for( int j = 0; j < m.size(); ++j )
    {
      if( m[j] > iFromIndex )
      { m[j] += iN; }
    }
  }
}
//-----------------------------------------------------------------------------
void PartitionViewer::shiftOrnement( int iN, int iBar, int iFromIndex )
{
  for( int i = 0; i < mOrnements.size(); ++i )
  {
    Ornement& o = mOrnements[i];
    for( int j = 0; j < o.mNotes.size(); ++j )
    {
      if( o.mNotes[j].first == iBar && o.mNotes[j].second > iFromIndex )
      { o.mNotes[j].second += iN; }
    }
  }
}
//-----------------------------------------------------------------------------
void PartitionViewer::titleChanged(const QString& iT)
{
  QFontMetrics fm( mpTitleLe->font() );
  mpTitleLe->resize( std::max( fm.width( iT ) * 1.1,
                              (double)fm.width( "short" ) ), mpTitleLe->height() );
  updateUi();
}
//-----------------------------------------------------------------------------
int PartitionViewer::toPageIndex( QPoint iP ) const
{
  int r = 0;
  for( int i = 0; i < getNumberOfPages(); ++i )
  {
    if( getPageRegion( prBody, i ).contains( iP ) )
    { r = i; break; }
  }
  return r;
}
//-----------------------------------------------------------------------------
/* voir la doc de Qt (http://doc.qt.io/qt-5/qrect.html#details) pour le +1 après
 chaque fonction right() et bottom()*/
void PartitionViewer::updateBar( int iBar )
{
  const int kMargin = 2;
  const int kMinBarWidth = 50;

  Bar& b = mBars[ iBar ];
  //--- definition des rect contenants les notes
  b.mNotesRect.clear();
  QFontMetrics fm( mBarFont );
  QFontMetrics gfm( mGraceNotesFont );
  int separatorPos = getBarRegion( brNoteStartX );
  int barWidth = separatorPos + 2*kMargin;
  int cursorX = kMargin + separatorPos;
  for( int j = 0; j < getNumberOfNotesInBar( iBar ); ++j )
  {
    QString n = noteToString( getNote( iBar, j ) );
    int noteLenght = isGraceNote( iBar, j ) ? gfm.width(n) + 1 : fm.width( n ) + 1;
    int noteHeight = isGraceNote( iBar, j )  ? gfm.height() : fm.height();
    int posY = isGraceNote( iBar, j ) ?
      getBarRegion( brGraceNoteTopY ) : getBarRegion( brNoteTopY );
    b.mNotesRect.push_back( QRect( cursorX, posY,
      noteLenght, noteHeight ) );
    cursorX += noteLenght + getInterNoteSpacing( iBar, j, j+1 );
    barWidth = cursorX;
  }
//  barWidth += std::max( (getNumberOfNotesInBar( iBar ) - 1) * kInterNoteGap, 0 );
  
  //--- Rectangle contenant la bar
  QRect barRect( QPoint(0, 0), QSize(std::max( barWidth, kMinBarWidth), kBarHeight ) );
  b.mRect = barRect;
  
  //--- Rectangle contenant les beat groups.
  b.mMatraGroupsRect.clear();
  for( int i = 0; i < b.mMatraGroups.size(); ++i )
  {
    int left = numeric_limits<int>::max();
    int right = numeric_limits<int>::min();
    vector<int>& v = b.mMatraGroups[i];
    for( int j = 0; j < v.size(); ++j )
    {
      int noteIndex = v[j];
      left = std::min( left, mBars[iBar].getNoteRect(noteIndex).left() );
      right = std::max( right, mBars[iBar].getNoteRect(noteIndex).right() );
    }
    b.mMatraGroupsRect.push_back( QRect( QPoint( left, getBarRegion( brMatraGroupY ) ),
      QSize( right - left, kBeatGroupHeight ) ) );
  }
}
//-----------------------------------------------------------------------------
void PartitionViewer::updatePageLayouts()
{
  //layout des barres spéciale (gamme, accordage tarab, etc...)
  Bar& b = mBars[ bScale ];
  b.mPageLayout.clear();
  b.mNotesPageLayout.clear();
  QRect l = b.mRect;
  QPoint sargamScaleTopLeft = getRegion( rSargamScale ).topLeft();
  l.translate( sargamScaleTopLeft );
  b.mPageLayout.push_back( l );
  //on fait le layout des notes restantes pour cette barre
  for( int j = 0; j < getNumberOfNotesInBar(bScale); ++j )
  {
    QRect n = b.getNoteRect(j);
    n.translate( sargamScaleTopLeft );
    b.mNotesPageLayout.push_back( n );
  }
  
  //--- le layout des barres du sargam
  mLayoutCursor = getPageRegion( prBody, 0 ).topLeft();
  for( int i = bFirstSargamBar; i < getNumberOfBars(); ++i )
  {
    Bar& b = mBars[i];
    b.mPageLayout.clear();
    b.mNotesPageLayout.clear();
    
    //--- le layout des bars
    QRect fullLayout = b.mRect;
    int noteHandledIndex = -1;
    int layoutWidthConsumed = 0;
    while( fullLayout.width() )
    {
      int pageIndex = toPageIndex( mLayoutCursor );
      QRect pageBody = getPageRegion(  prBody, pageIndex );
      
      QRect pageLayout = fullLayout;
      pageLayout.translate( mLayoutCursor );
      if( !pageBody.contains( pageLayout ) &&
         !pageBody.contains( pageLayout.bottomLeft() ) )
      {
        //on ajoute une page...
        addPage();
        pageIndex++;
        mLayoutCursor = getPageRegion( prBody, pageIndex ).topLeft();
      }
      else if( !pageBody.contains( pageLayout ) )
      {
        //on coupe le layout pour ne pas depasser la page
        int whereToCut = pageBody.right();
        
        //on fait le layout des notes pour cette barre et on s'assure qu'on ne
        //coupera pas une note en deux
        for( int j = 0; j < getNumberOfNotesInBar(i); ++j )
        {
          QRect n = b.getNoteRect(j);
          n.translate( pageLayout.topLeft() - QPoint( layoutWidthConsumed, 0 ) );
          if( j > noteHandledIndex )
          {
            if( !pageBody.contains( n.topRight() ) )
            {
              whereToCut = std::min(n.left() - 2, whereToCut);
              break;
            }
            else{ b.mNotesPageLayout.push_back( n ); noteHandledIndex++; }
          }
        }
        
        int whatIsLeftOfLayout = pageLayout.right() - whereToCut;
        layoutWidthConsumed = fullLayout.width() - whatIsLeftOfLayout;
        
        //maintenant on coupe et change fullLayout de ligne et on le reaffecte a pageLayout
        fullLayout.setWidth( whatIsLeftOfLayout );
        mLayoutCursor.setX( pageBody.left() + getBarRegion( brNoteStartX ) );
        mLayoutCursor.setY( mLayoutCursor.y() + kBarHeight );
        
        pageLayout.setRight( whereToCut );
        b.mPageLayout.push_back( pageLayout );
      }
      else //le layout de la barre entre à l'écran sans problème
      {
        //on fait le layout des notes restantes pour cette barre
        for( int j = 0; j < getNumberOfNotesInBar(i); ++j )
        {
          QRect n = b.getNoteRect(j);
          n.translate( pageLayout.topLeft() - QPoint( layoutWidthConsumed, 0 ) );
          if( j > noteHandledIndex )
          {
            b.mNotesPageLayout.push_back( n );
            noteHandledIndex++;
          }
        }
        
        mLayoutCursor += QPoint( fullLayout.width(), 0 );
        fullLayout.setWidth(0);
        b.mPageLayout.push_back( pageLayout );
      }
    }
  }
}
//-----------------------------------------------------------------------------
void PartitionViewer::updateOrnement( Ornement* ipM )
{
  ipM->mCuts.clear();
  ipM->mBlits.clear();
  ipM->mFullOrnement = QRect();
  
  //update rects...
  vector<int> bi = ipM->barsInvolved();
  ipM->mCuts.resize( bi.size() );
  ipM->mBlits.resize( bi.size() );
  int barIndex = -1;
  int widthAccum = 0;
  int height = ipM->mOrnementType == otMeend ? kMeendHeight : kKrintanHeight;
  for( int i = 0; i < bi.size(); ++i )
  {
    barIndex = bi[i];
    int left = numeric_limits<int>::max();
    int right = numeric_limits<int>::min();
    for( int j = 0; j < ipM->mNotes.size(); ++j )
    {
      if( ipM->mNotes[j].first == barIndex )
      {
        int noteIndex = ipM->mNotes[j].second;
        left = std::min( left, mBars[barIndex].getNoteRect(noteIndex).left() );
        right = std::max( right, mBars[barIndex].getNoteRect(noteIndex).right() );
        
        //quand c'est la derniere note de la barre et que le Ornement dépasse dnas la barre
        //suivante, on prend la fin de la barre...
        if( noteIndex == (getNumberOfNotesInBar(barIndex) - 1) && i < (bi.size() - 1) )
        { right = mBars[barIndex].mRect.right(); }
        
        //quand c'Est la premiere note de la barre et que le Ornement vient de la barre
        //précédente, le left est le début de la barre
        if( noteIndex == 0 && i > 0 )
        { left = mBars[barIndex].mRect.left(); }
      }
    }
    
    QRect blit( QPoint( left, 0 ), QSize( right - left, height ) );
    ipM->mBlits[i] = make_pair( barIndex, blit );
    QRect cut( QPoint(0, 0), ipM->mBlits[i].second.size() );
    if( i != 0 )
    { cut.translate( widthAccum, 0); }
    ipM->mCuts[i] = make_pair( barIndex, cut );
    widthAccum += cut.width();
  }
  ipM->mFullOrnement = QRect( QPoint(0, 0), QSize(widthAccum, height) );
}
//-----------------------------------------------------------------------------
void PartitionViewer::updateUi()
{
  //placer le ui l'écran
  //titre
  QRect r = getRegion( rTitle );
  QPoint c = r.center() - QPoint( mpTitleLe->width() / 2, mpTitleLe->height() / 2 );
  mpTitleLe->move( c );
  
  //expliquer les deux passes...
  bool shouldUpdateLayout = false;
  for( int i = 0; i < getNumberOfBars(); ++i )
  {
    Bar& b = mBars[i];
    if( b.mIsDirty )
    { updateBar( i ); }
  }
  
  for( int i = 0; i < getNumberOfBars(); ++i )
  {
    Bar& b = mBars[i];
    if( b.mIsDirty )
    {
      for( int j = 0; j < mOrnements.size(); ++j )
      {
        if( mOrnements[j].appliesToBar( i ) )
        { updateOrnement( &mOrnements[j] ); }
      }
      renderBarOffscreen(i);
      setBarAsDirty( i, false );
      shouldUpdateLayout = true;
    }
  }
  
  if( shouldUpdateLayout ){ updatePageLayouts(); }
  
  update();
}
//-----------------------------------------------------------------------------
// --- PARTITIONVIEWER::Bar (vibhag)
//-----------------------------------------------------------------------------
QRect PartitionViewer::Bar::getNoteRect( int iNoteIndex ) const
{
  QRect r;
  if( iNoteIndex >= 0 && iNoteIndex < mNotesRect.size() )
  { r = mNotesRect[iNoteIndex]; }
  return r;
}
//-----------------------------------------------------------------------------
// --- PARTITIONVIEWER::Ornement
//-----------------------------------------------------------------------------
bool PartitionViewer::Ornement::appliesToBar( int iBar ) const
{
  bool r = false;
  for( int i = 0; i < mNotes.size(); ++i )
  { if( mNotes[i].first == iBar ){ r = true; break; } }
  return r;
}
//-----------------------------------------------------------------------------
std::vector<int> PartitionViewer::Ornement::barsInvolved() const
{
  vector<int> r;
  for( int i = 0; i < mNotes.size(); ++i )
  { r.push_back( mNotes[i].first ); }
  r.erase( std::unique( r.begin(), r.end() ), r.end() );
  return r;
}
//-----------------------------------------------------------------------------
QRect PartitionViewer::Ornement::getBlit( int iBar ) const
{
  QRect r;
  for( int i = 0; i < mBlits.size(); ++i )
  { if( mBlits[i].first == iBar ) { r = mBlits[i].second; break; } }
  return r;
}
//-----------------------------------------------------------------------------
QRect PartitionViewer::Ornement::getCut( int iBar ) const
{
  QRect r;
  for( int i = 0; i < mCuts.size(); ++i )
  { if( mCuts[i].first == iBar ) { r = mCuts[i].second; break; } }
  return r;
}

//-----------------------------------------------------------------------------
// --- MAIN WINDOW
//-----------------------------------------------------------------------------
MainDialog::MainDialog() : QMainWindow(),
  mpPartitionViewer(0),
  mSettings( QSettings::UserScope, "Realisim", "Sargam" )
{
  QWidget* pMainWidget = new QWidget( this );
  setCentralWidget(pMainWidget);
  
  //--- la barre de menu
  QMenuBar* pMenuBar = new QMenuBar(pMainWidget);
  QMenu* pFile = pMenuBar->addMenu("File");
//  pFile->addAction( QString("&Open..."), this, SLOT( openCatalogClicked() ),
//  	QKeySequence(Qt::CTRL + Qt::Key_O) );
//  pFile->addAction( QString("&Close"), this, SLOT( closeCatalogClicked() ) );
//  pFile->addAction( QString("&Save"), this, SLOT( save() ),
//  	QKeySequence(Qt::CTRL + Qt::Key_S) );
//  pFile->addAction( QString("Save As..."), this, SLOT( saveAs() ),
//  	QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_S) );

  //debug action
  QShortcut* pRandomPart = new QShortcut( QKeySequence( Qt::CTRL + Qt::SHIFT + Qt::Key_G), this );
  connect( pRandomPart, SIGNAL(activated()), this, SLOT(generateRandomPartition()) );

  QShortcut* pDebugS = new QShortcut( QKeySequence( Qt::CTRL + Qt::SHIFT + Qt::Key_D), this );
  connect( pDebugS, SIGNAL(activated()), this, SLOT(toggleDebugging()) );
  
  setMenuBar( pMenuBar );
  
  //--- le reste du Ui
  QHBoxLayout* pLyt = new QHBoxLayout(pMainWidget);
  pLyt->setMargin(5);
  pLyt->setSpacing(5);
  
  QScrollArea* pScroll = new QScrollArea( pMainWidget );
  {
//    QFrame* pFrame = new QFrame( pMainWidget );
//    QHBoxLayout* pHLyt = new QHBoxLayout( pFrame );
//    {
//      mpPartitionViewer = new PartitionViewer( pFrame );
//      mpPartitionViewer->setAsDebugging( true );
//      pHLyt->addWidget( mpPartitionViewer );
//    }
//    pScroll->setWidget( pFrame );
    
    mpPartitionViewer = new PartitionViewer( pMainWidget );
    mpPartitionViewer->setAsDebugging( true );
    pScroll->setWidget( mpPartitionViewer );
  }
  pLyt->addWidget( pScroll );
  
  loadSettings();
  
  resize( mpPartitionViewer->width() + 35, 800 );
}
//-----------------------------------------------------------------------------
void MainDialog::generateRandomPartition()
{ mpPartitionViewer->generateRandomPartition(); }
//-----------------------------------------------------------------------------
void MainDialog::loadSettings()
{
//	mSaveAsPath = mSettings.value( "saveAsPath" ).toString();
//  mAddTexturePath = mSettings.value( "addTexturePath" ).toString();
//  mOpenCatalogPath = mSettings.value( "openCatalogPath" ).toString();
//  mRefreshTexturePath = mSettings.value( "refreshTexturePath" ).toString();
}

//-----------------------------------------------------------------------------
void MainDialog::save()
{
//	if( mSaveFileName.isEmpty() )
//  { saveAs(); }
//  else
//  { utils::toFile( mSaveFileName, mSpriteCatalog.toBinary() ); }
//  updateUi();
}

//-----------------------------------------------------------------------------
void MainDialog::saveAs()
{
//	QString s;
//  s = QFileDialog::getSaveFileName(
//    this, tr("Save Catalog"),
//    "/home/untitled.cat",
//    tr("Sprite Catalog (*.cat)"));
//      
//  if(!s.isEmpty())
//  {
//  	mSaveFileName = s;
//  	utils::toFile( mSaveFileName, mSpriteCatalog.toBinary() );
//    
//    mSaveAsPath = QFileInfo(s).path();
//	  saveSettings();
//  }
//  updateUi();
}

//-----------------------------------------------------------------------------
void MainDialog::saveSettings()
{
//	mSettings.setValue( "saveAsPath", mSaveAsPath );
//	mSettings.setValue( "addTexturePath", mAddTexturePath );
//  mSettings.setValue( "openCatalogPath", mOpenCatalogPath );
//  mSettings.setValue( "refreshTexturePath", mRefreshTexturePath );
}

//-----------------------------------------------------------------------------
void MainDialog::toggleDebugging()
{ mpPartitionViewer->setAsDebugging( !mpPartitionViewer->isDebugging() ); }

//-----------------------------------------------------------------------------
void MainDialog::updateUi()
{
}

