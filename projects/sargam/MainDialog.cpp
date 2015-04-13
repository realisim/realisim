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
  const double kPageMarginInCm = 2.0;
  const int kPageFooter = 15;
  //bar
  const int kBarHeight = 60;
  const int kInterNoteGap = 8;
  const int kMeendHeight = 10;
  const int kKrintanHeight = 6;
  const int kBeatGroupHeight = 10;

}

realisim::sargam::Composition PartitionViewer::mDummyComposition;

//-----------------------------------------------------------------------------
// --- partition viewer
//-----------------------------------------------------------------------------
PartitionViewer::PartitionViewer( QWidget* ipParent ) :
  QWidget( ipParent ),
  mpTitleLe(0),
  mSargamScaleLabel( "no text for sargamScale" ),
  mpLineTextEdit(0),
  mIsDebugging( false),
  mPaperSize( 8.5, 11 ),
  mNumberOfPages(0),
  mCurrentBar( -1 ),
  mCurrentNote( -1 ),
  mOctave( 0 ),
  mEditingLineIndex( -1 ),
  mAddLineTextHover( -1 ),
  mBarHoverIndex(-1),
  x( &mDummyComposition )
{
  setMouseTracking( true );
  setStyleSheet("QLineEdit { border: none }");
  setFocusPolicy( Qt::StrongFocus );
  srand( time(NULL) );
  mTitleFont = QFont( "Arial", 24 );
  mTitleFont.setBold( true );
  mBarFont = QFont( "Arial", 14 );
  mGraceNotesFont = QFont( "Arial", 10 );
  mLineFont = QFont( "Arial", 12 );
  mStrokeFont = QFont( "Arial", 10 );

  createUi();
  addPage();
  
  //generateRandomPartition();
  updateUi();
}

PartitionViewer::~PartitionViewer()
{}

//-----------------------------------------------------------------------------
/*Ajoute une barre a la suite de la barre iBarIndex*/
void PartitionViewer::addBar( int iBarIndex )
{
  x->addBar( iBarIndex );
  
  //on ajoute une barre pour les donnees d'affichage.
  vector<Bar>::iterator it = mBars.begin() + iBarIndex + 1;
  if( it < mBars.end() )
  { mBars.insert(it, Bar()); }
  else{ mBars.push_back( Bar() ); }
}
//-----------------------------------------------------------------------------
void PartitionViewer::addNoteToSelection( int iBar, int iNote )
{
  mSelectedNotes.push_back( make_pair( iBar, iNote ) );
  sort( mSelectedNotes.begin(), mSelectedNotes.end() );
  /*On empeche quil y ait 2 fois la meme note dans la selection.*/
  mSelectedNotes.erase(
    std::unique( mSelectedNotes.begin(), mSelectedNotes.end() ),
    mSelectedNotes.end() );
  
  /*On met la barre dirty parce que le rectangle de selection est dessine
    sur la barre (voir methode renderBarOffScreen).*/
  setBarAsDirty( iBar, true );
}
//-----------------------------------------------------------------------------
void PartitionViewer::addPage()
{
  mNumberOfPages++;
  QRect p = getRegion( rPartition );
  resize( p.width()+1, p.height()+1 );
}
//-----------------------------------------------------------------------------
void PartitionViewer::clear()
{
  mNumberOfPages = 1;
  mpTitleLe->setText( "Untitled" );
  mBars.clear();
  mLines.clear();
  mOrnements.clear();
  mCurrentBar = -1;
  mCurrentNote = -1;
  mSelectedNotes.clear();
}
//-----------------------------------------------------------------------------
void PartitionViewer::clearSelection()
{
  for( int i = 0; i < mSelectedNotes.size(); ++i )
  { setBarAsDirty( mSelectedNotes[i].first , true ); }
  mSelectedNotes.clear();
}
//-----------------------------------------------------------------------------
int PartitionViewer::cmToPixel( double iCm ) const
{ return logicalDpiX() * iCm / 2.54; }
//-----------------------------------------------------------------------------
void PartitionViewer::commandAddBar()
{
  addBar( getCurrentBar() );
  setCurrentNote( -1 );
  setCurrentBar( getCurrentBar() + 1 );
  clearSelection();
  updateUi();
}
//-----------------------------------------------------------------------------
/*Les notes de la selection deviennent des graces notes si elle ne le sont
 pas déjà.*/
void PartitionViewer::commandAddGraceNotes()
{
  if( hasSelection() )
  {
    for( int i = 0; i < mSelectedNotes.size(); ++i )
    {
      int barIndex = mSelectedNotes[i].first;;
      int noteIndex = mSelectedNotes[i].second;
      x->addGraceNote( barIndex, noteIndex );
      setBarAsDirty( barIndex, true );
    }
  }
  updateUi();
}

//-----------------------------------------------------------------------------
void PartitionViewer::commandAddLine()
{
  commandAddBar();
  x->addLine( getCurrentBar() );
//  mLines.push_back( Line() );
  setBarAsDirty( getCurrentBar(), true );
}
//-----------------------------------------------------------------------------
void PartitionViewer::commandAddMatra()
{
  if( hasSelection() )
  {
    /*On commence par chercher si il y a deja un matra associe aux
     note du matra qu'on est en train d'ajouter. Si oui, on enleve le matra
     deja existants.*/
    for( int i = 0; i < mSelectedNotes.size(); ++i )
    {
      int bar = mSelectedNotes[i].first;
      int noteIndex = mSelectedNotes[i].second;
      if( x->isNoteInMatra( bar, noteIndex) )
      {
        x->eraseMatra( bar, x->findMatra( bar, noteIndex ) );
        setBarAsDirty( bar, true );
      }
    }
    
    map<int, vector<int> > m = splitPerBar( mSelectedNotes );
    map<int, vector<int> >::const_iterator it = m.begin();
    for( ; it != m.end(); ++it )
    {
      x->addMatra( it->first, it->second );
      setBarAsDirty( it->first, true );
    }
  }
  updateUi();
}
//-----------------------------------------------------------------------------
void PartitionViewer::commandAddNote( noteValue iN )
{
  if( hasSelection() )
  { commandErase(); }
  Note n = makeNoteFromScale( iN );
  n.setOctave( mOctave );
  x->addNote( getCurrentBar(), getCurrentNote(), n );
  mCurrentNote++;
  clearSelection();
  setBarAsDirty(getCurrentBar(), true);
  updateUi();
}
//-----------------------------------------------------------------------------
void PartitionViewer::commandAddOrnement( ornementType iOt )
{
  if( hasSelection() )
  {
    /*On commence par chercher si il y a deja un ornement associe aux
     note de lornement qu'on est en train d'ajouter. Si oui, on enleve lornement
     deja existants.*/
    for( int i = 0; i < mSelectedNotes.size(); ++i )
    {
      int bar = mSelectedNotes[i].first;
      int noteIndex = mSelectedNotes[i].second;
      if( x->isNoteInOrnement( bar, noteIndex) )
      { eraseOrnement( x->findOrnement( bar, noteIndex ) ); }
    }
    
    mOrnements.push_back( Ornement() );
    x->addOrnement( iOt, toNoteLocator( mSelectedNotes ) );
    setBarAsDirty( x->getBarsInvolvedByOrnement(
      x->findOrnement( mSelectedNotes[0].first, mSelectedNotes[0].second) ), true );
  }
}
//-----------------------------------------------------------------------------
void PartitionViewer::commandAddStroke( strokeType iSt )
{
  vector< pair<int, int> > v;
  if( hasSelection() )
  {
    for( int i = 0; i < mSelectedNotes.size(); ++i )
    { v.push_back( mSelectedNotes[i] ); }
  }
  else{ v.push_back( make_pair( getCurrentBar(), getCurrentNote() ) ); }
 
  for( int i = 0; i < v.size(); ++i )
  {
    int bar = v[i].first;
    int index = v[i].second;
    
    /*On commence par chercher si il y a deja un stroke associe aux
      notes du stroke qu'on est en train d'ajouter. Si oui, on enleve les strokes
     deja existants.*/
    if( x->hasStroke( bar, index ) )
    {
      x->eraseStroke( bar, x->findStroke( bar, index ) );
      setBarAsDirty( bar, true );
    }
  }

  //on ajoute les strokes par barre
  map< int, vector<int> > m = splitPerBar( v );
  map< int, vector<int> >::const_iterator it = m.begin();
  for( ; it != m.end(); ++it )
  {
    int bar = it->first;
    x->addStroke( bar, iSt, it->second );
    setBarAsDirty( bar, true );
  }
  
  
}
//-----------------------------------------------------------------------------
void PartitionViewer::commandBreakMatrasFromSelection()
{
  for( int i = 0; i < mSelectedNotes.size(); ++i )
  {
    int bar = mSelectedNotes[i].first;
    int index = mSelectedNotes[i].second;
    int matraIndex = x->findMatra( bar, index );
    if( matraIndex != -1 )
    {
      x->eraseMatra( bar, matraIndex );
      setBarAsDirty( bar, true );
    }
  }
}
//-----------------------------------------------------------------------------
void PartitionViewer::commandBreakOrnementsFromSelection()
{
  for( int i = 0; i < mSelectedNotes.size(); ++i )
  {
    int bar = mSelectedNotes[i].first;
    int index = mSelectedNotes[i].second;
    int ornementIndex = x->findOrnement( bar, index );
    if( ornementIndex != -1 )
    { eraseOrnement( ornementIndex ); }
  }
}
//-----------------------------------------------------------------------------
void PartitionViewer::commandDecreaseOctave()
{
  vector< pair<int, int> > v;
  if( hasSelection() )
  {
    for( int i = 0; i < mSelectedNotes.size(); ++i )
    { v.push_back( mSelectedNotes[i] ); }
  }
  else{ v.push_back( make_pair( getCurrentBar(), getCurrentNote() ) ); }
  
  for( int i = 0; i < v.size(); ++i )
  {
    Note n = x->getNote( v[i].first, v[i].second );
    n.setOctave( n.getOctave() - 1 );
    x->setNote( v[i].first, v[i].second, n );
    setBarAsDirty( v[i].first, true );
  }
  mOctave = std::max( --mOctave, -1 );
}
//-----------------------------------------------------------------------------
void PartitionViewer::commandErase()
{
  if( hasSelection() )
  {
    for( int i = mSelectedNotes.size() - 1; i >= 0; --i )
    {
      int bar = mSelectedNotes[i].first;
      int index = mSelectedNotes[i].second;
      x->eraseNote( bar, index );
      
      if( x->isNoteInOrnement(bar, index) )
      {
        setBarAsDirty( x->getBarsInvolvedByOrnement(
          x->findOrnement(bar, index) ), true );
      }
    }
    setCurrentBar( mSelectedNotes[0].first );
    /*Le -1 est parce que la premiere note selectionnée est effacée, il faut
      donc aller à l'autre d'avant.*/
    setCurrentNote( mSelectedNotes[0].second - 1);
    clearSelection();
  }
  else
  {
    int cb = getCurrentBar();
    if( x->getNumberOfNotesInBar(cb) > 0 )
    {
      x->eraseNote( cb, getCurrentNote() );
      mCurrentNote--;
    }
    else
    {
      x->eraseBar( cb );
      setCurrentBar( cb - 1 );
      setCurrentNote( x->getNumberOfNotesInBar( getCurrentBar() ) - 1 );
    }
    setBarAsDirty(cb, true);
  }
  updateUi();
}
//-----------------------------------------------------------------------------
void PartitionViewer::commandIncreaseOctave()
{
  vector< pair<int, int> > v;
  if( hasSelection() )
  {
    for( int i = 0; i < mSelectedNotes.size(); ++i )
    { v.push_back( mSelectedNotes[i] ); }
  }
  else{ v.push_back( make_pair( getCurrentBar(), getCurrentNote() ) ); }
  
  for( int i = 0; i < v.size(); ++i )
  {
    Note n = x->getNote( v[i].first, v[i].second );
    n.setOctave( n.getOctave() + 1 );
    x->setNote( v[i].first, v[i].second, n );
    setBarAsDirty( v[i].first, true );
  }
  mOctave = std::min( ++mOctave, 1 );
}
//-----------------------------------------------------------------------------
void PartitionViewer::commandRemoveSelectionFromGraceNotes()
{
  for( int i = 0; i < mSelectedNotes.size(); ++i )
  {
    int bar = mSelectedNotes[i].first;
    int index = mSelectedNotes[i].second;
    if( x->isGraceNote( bar, index ) )
    {
      x->eraseGraceNote( bar, index );
      setBarAsDirty( bar, true );
    }
  }
  updateUi();
}
//-----------------------------------------------------------------------------
void PartitionViewer::commandShiftNote()
{
  vector< pair<int, int> > l;
  if( hasSelection() )
  { l = mSelectedNotes; }
  else
  { l.push_back( make_pair( getCurrentBar(), getCurrentNote()) ); }
 
  for( int i = 0; i < l.size(); ++i )
  {
    int bar = l[i].first;
    int index = l[i].second;
    Note n = x->getNote( bar, index );
    if( n.getModification() == nmKomal || n.getModification() == nmTivra )
    { n.setModification( nmShuddh ); }
    else if( n.canBeKomal() )
    { n.setModification( nmKomal); }
    else if( n.canBeTivra() )
    { n.setModification( nmTivra ); }
    x->setNote( bar, index, n );
    setBarAsDirty( bar, true );
  }
  
}
//-----------------------------------------------------------------------------
void PartitionViewer::createUi()
{
  //titre
  mpTitleLe = new QLineEdit( this );
  connect( mpTitleLe, SIGNAL( textChanged(const QString&)),
          this, SLOT( resizeLineEditToContent() ) );
  mpTitleLe->setFont( mTitleFont );
  mpTitleLe->setText( "Untitled" );

  //scale label
  mSargamScaleLabel = "Scale: ";
  
  //line edit pour titre des lignes
  mpLineTextEdit = new QLineEdit( this->parentWidget() );
  mpLineTextEdit->setFont( mLineFont );
  mpLineTextEdit->hide();
  connect( mpLineTextEdit, SIGNAL( editingFinished() ),
          this, SLOT( stopLineTextEdit() ) );
  connect( mpLineTextEdit, SIGNAL( textChanged(const QString&)),
          this, SLOT( resizeLineEditToContent() ) );
}
//-----------------------------------------------------------------------------
void PartitionViewer::drawBarContour( QPainter& iP, int iBarIndex, QColor iCol )
{
  if( iBarIndex != -1 && hasFocus() )
  {
    QPen pen = iP.pen();
    iP.save();
    iP.setRenderHints( QPainter::Antialiasing );
    pen.setColor( iCol );
    pen.setWidth( 1 );
    iP.setPen( pen );
    Bar& b = getBar( iBarIndex );
    for( int i = 0; i < b.mPageLayout.size(); ++i )
    { iP.drawRoundedRect( b.mPageLayout[i], 2, 2 ); }
    iP.restore();
  }

}
//-----------------------------------------------------------------------------
/*Efface la barre iBar des donnees et des donnees d'affichage.*/
void PartitionViewer::eraseBar( int iBar )
{
  x->eraseBar( iBar );
  if( iBar >= 0 && iBar < x->getNumberOfBars() )
  {
    mBars.erase( mBars.begin() + iBar );
    if( x->getNumberOfBars() > 0 ){ setBarAsDirty(0, true); }
  }
}
//-----------------------------------------------------------------------------
/*Efface l'ornement iIndex de données d'affichage et des données.*/
void PartitionViewer::eraseOrnement( int iIndex )
{
  if( iIndex >= 0 && iIndex < mOrnements.size() )
  {
    setBarAsDirty( x->getBarsInvolvedByOrnement( iIndex ), true );
    mOrnements.erase( mOrnements.begin() + iIndex );
    x->eraseOrnement( iIndex );
  }
}
//-----------------------------------------------------------------------------
void PartitionViewer::generateRandomPartition()
{
  clear();
  const int kNumBars = 52;
  mBars.resize( kNumBars );
  for( int i = 0; i < x->getNumberOfBars() - 1; ++i )
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
      x->addNote( i, Note( (noteValue)note, octave ) );
    }
  }
  updateUi();
}
//-----------------------------------------------------------------------------
PartitionViewer::Bar& PartitionViewer::getBar( int iBar )
{
  return const_cast<Bar&>(
    const_cast< const PartitionViewer* >(this)->getBar(iBar) );
}
//-----------------------------------------------------------------------------
const PartitionViewer::Bar& PartitionViewer::getBar( int iBar ) const
{
  const Bar* r = 0;
  switch( iBar )
  {
    case bScale: r = &mScale; break;
    default: r = &mBars[iBar]; break;
  }
  return *r;
}
//-----------------------------------------------------------------------------
Composition PartitionViewer::getComposition() const
{ return *x; }
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
    case brStrokeY: r = kBarHeight - fm.height(); break;
    case brOrnementY: r = getBarRegion(brGraceNoteTopY) - 8; break;
    case brMatraGroupY: r = getBarRegion(brNoteBottomY) - 4; break;
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
  int matra1 = x->findMatra( iBar, iIndex1 );
  int matra2 = x->findMatra( iBar, iIndex2 );
  //les notes d'un même matra
  if( matra1 == matra2 && matra1 != -1 && matra2 != -1 )
  {
    r = 2;
    //si ce sont des graceNotes
    if( x->isGraceNote( iBar, iIndex1 ) && x->isGraceNote( iBar, iIndex2 ) )
    { r = 1; }
  }
  return r;
}
//-----------------------------------------------------------------------------
int PartitionViewer::getNumberOfPages() const
{ return mNumberOfPages; }
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
      int margin = cmToPixel( kPageMarginInCm );
      r.setLeft( margin );
      r.setHeight( kBarHeight );
      r.setWidth( fm.width( mSargamScaleLabel ) );
    } break;
    case rSargamScale:
    {
      QFontMetrics fm( mBarFont );
      QRect t = getRegion( rTitle );
      int margin = cmToPixel( kPageMarginInCm );
      r = t;
      r.translate( 0, t.height() + kSpacing );
      r.setLeft( getRegion( rSargamScaleLabel ).right() );
      r.setRight( t.right() - margin );
      r.setHeight( kBarHeight );
    } break;
  }
  return r;
}
//-----------------------------------------------------------------------------
QString PartitionViewer::getTitle() const
{ return mpTitleLe->text(); }
//-----------------------------------------------------------------------------
bool PartitionViewer::hasSelection() const
{ return mSelectedNotes.size() > 0; }
//-----------------------------------------------------------------------------
bool PartitionViewer::isDebugging() const
{ return mIsDebugging; }
//-----------------------------------------------------------------------------
bool PartitionViewer::isNoteSelected(int iBar, int iNoteIndex ) const
{
  bool r = false;
  for( int i = 0; i < mSelectedNotes.size(); ++i )
  {
    if( iBar == mSelectedNotes[i].first && iNoteIndex == mSelectedNotes[i].second )
    { r = true; break; }
  }
  return r;
}
//-----------------------------------------------------------------------------
bool PartitionViewer::hasLineEditionPending() const
{ return mEditingLineIndex != -1; }
//-----------------------------------------------------------------------------
void PartitionViewer::keyPressEvent( QKeyEvent* ipE )
{
  switch ( ipE->key() )
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
    case Qt::Key_B: //Bend - meend
      if( (ipE->modifiers() & Qt::ShiftModifier) )
      { commandBreakOrnementsFromSelection(); }
      else{ commandAddOrnement( otMeend ); }
      break;
    case Qt::Key_E: commandAddStroke( stDiri ); break;
    case Qt::Key_G: //grace note
      if( (ipE->modifiers() & Qt::ShiftModifier) )
      { commandRemoveSelectionFromGraceNotes(); }
      else { commandAddGraceNotes(); }
      break;
    case Qt::Key_K: //krintan
      if( (ipE->modifiers() & Qt::ShiftModifier) )
      { commandBreakOrnementsFromSelection(); }
      else{ commandAddOrnement( otKrintan ); }
      break;
    case Qt::Key_M: //matra
      if( (ipE->modifiers() & Qt::ShiftModifier) )
      { commandBreakMatrasFromSelection(); }
      else{ commandAddMatra(); }
      break;
    case Qt::Key_Q: commandAddStroke( stDa ); break;
    case Qt::Key_R: commandAddNote( nvRest );break;
    case Qt::Key_S: commandShiftNote();break;
    case Qt::Key_W: commandAddStroke( stRa ); break;
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
        if( getCurrentNote() == -2 && getCurrentBar() > bScale )
        {
          setCurrentBar( getCurrentBar() - 1 );
          setCurrentNote( x->getNumberOfNotesInBar( getCurrentBar() ) - 1 );
        }
      }
      else
      {
        setCurrentBar( mSelectedNotes[0].first );
        setCurrentNote( mSelectedNotes[0].second - 1);
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
        int maxIndex = x->getNumberOfNotesInBar( getCurrentBar() ) - 1;
        ++mCurrentNote;
        if( getCurrentNote() > maxIndex &&
           getCurrentBar() < x->getNumberOfBars() - 1 )
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
        setCurrentBar( mSelectedNotes[0].first );
        setCurrentNote( mSelectedNotes[ mSelectedNotes.size() - 1 ].second );
        clearSelection();
      }
      break;
    case Qt::Key_Space:
      commandAddBar();
      break;
    case Qt::Key_Backspace: commandErase(); break;
    case Qt::Key_Return: commandAddLine(); break;
    case Qt::Key_Plus: commandIncreaseOctave(); break;
    case Qt::Key_Minus: commandDecreaseOctave(); break;
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
  vector<Note> v = x->getScale();
  for( int i = 0; i < v.size(); ++i )
  {
    if( v[i].getValue() == r.getValue() )
    { r.setModification( v[i].getModification() ); break; }
  }
  return r;
}
//-----------------------------------------------------------------------------
void PartitionViewer::mouseMoveEvent( QMouseEvent* ipE )
{
  QPoint pos = ipE->pos();
  Qt::CursorShape cs = Qt::ArrowCursor;
  
  //intersection avec les barres
  for( int i = 0; i < x->getNumberOfBars(); ++i )
  {
    const Bar& b = getBar(i);
    for( int j = 0; j < b.mPageLayout.size(); ++j )
    {
      if( b.mPageLayout[j].contains( pos ) )
      { mBarHoverIndex = i; updateUi(); }
      else if ( mBarHoverIndex == i )
      { mBarHoverIndex = -1; updateUi(); }
    }
  }
  
  //intersection avec les lignes
  for( int i = 0; i < x->getNumberOfLines(); ++i )
  {
    if( mLines[i].mHotSpot.contains( pos ) )
    { mAddLineTextHover = i; updateUi(); }
    else if(mAddLineTextHover == i)
    { mAddLineTextHover = -1; updateUi(); }
      
    if( mLines[i].mTextRect.contains( pos ) )
    { cs = Qt::IBeamCursor; }
  }
  
  setCursor( cs );
}
//-----------------------------------------------------------------------------
void PartitionViewer::mouseReleaseEvent( QMouseEvent* ipE )
{
  QPoint pos = ipE->pos();
  
  //intersection avec les barres
  if( mBarHoverIndex != -1 )
  {
    setCurrentBar( mBarHoverIndex );
    setCurrentNote( x->getNumberOfNotesInBar( mBarHoverIndex ) - 1 );
    updateUi();
  }
  
  //intersection avec les lignes
  for( int i = 0; i < x->getNumberOfLines(); ++i )
  {
    if( mLines[i].mHotSpot.contains( pos ) )
    {
      mAddLineTextHover = -1;
      setBarAsDirty( i, true );
      startLineTextEdit( i );
    }
    
    if( mLines[i].mTextRect.contains( pos ) )
    { startLineTextEdit( i ); }
  }
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
    case nvRe: if(nm == nmKomal){r = "r";}else {r = "R";} break;
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
  for( int i = bScale; i < x->getNumberOfBars(); ++i )
  {
    Bar& b = getBar(i);
    //on blit le pixmap dans le widget.
    QRect cut( 0, 0, 0, 0 );
    for( int j = 0; j < b.mPageLayout.size(); ++j )
    {
      cut.setSize( b.mPageLayout[j].size() );
      p.drawPixmap( b.mPageLayout[j], b.mPixmap, cut );
      cut.setLeft( cut.width() + 1 );
      
      //le curseur dans la barre courante s'il n'y a pas de selection
      //et que le focus y est
      pen.setColor( Qt::black );
      pen.setWidth( 1 );
      p.setPen( pen );
      if( !hasSelection() && getCurrentBar() == i && hasFocus() )
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
  
  //on dessine le contour de la barre survolee si ce nest pas la courant
  if( mBarHoverIndex != getCurrentBar() )
  { drawBarContour( p, mBarHoverIndex, QColor( 125, 125, 125, 120 ) ); }
  //on dessine le contour de la barre courante
  drawBarContour( p, getCurrentBar(), QColor( 0, 10, 210, 120 ) );
  
  //rendu des lignes, numero et texte
  for( int i = 0; i < x->getNumberOfLines(); ++i )
  {
    pen.setColor( Qt::black );
    p.setPen( pen );
    p.setFont( mLineFont );
    const Line& l = mLines[i];
    //numero de ligne. les lignes commencent a 1!!!
    p.drawText( l.mLineNumberRect, QString::number( i + 1 ) );
    //le hot spot
    if( mAddLineTextHover == i )
    {
      p.drawRect( l.mHotSpot );
    }
    //text de la ligne
    p.drawText( l.mTextRect.bottomLeft(), x->getLineText(i) );
  }
  
  //--- render debug infos
  if( isDebugging() )
  {
    p.setFont(QFont("Arial", 10));
    QPen pen = p.pen();
    pen.setColor(Qt::blue);
    p.setPen(pen);
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
    for( int i = 0; i < x->getNumberOfBars(); ++i )
    {
      Bar& b = getBar(i);
      //le layput page de la bar
      for( int j = 0; j < b.mPageLayout.size(); ++j )
      {
        p.drawRect( b.mPageLayout[j] );
        p.drawText( b.mPageLayout[j], Qt::AlignCenter, QString::number(i) );
        QString s;
        if( getCurrentBar() == i )
        { s += QString().sprintf("current note: %d\n", getCurrentNote() ); }
        s += QString().sprintf("number of notes: %d", x->getNumberOfNotesInBar( i ) );
        p.drawText( b.mPageLayout[j], Qt::AlignBottom | Qt::AlignRight, s );
      }
      //le layout page des notes
      for( int j = 0; j < b.mNotesPageLayout.size(); ++j )
      { p.drawRect( b.mNotesPageLayout[j] ); }
    }
    
    //layout des lignes
    for( int i = 0; i < x->getNumberOfLines(); ++i )
    {
      const Line& l = mLines[i];
      p.drawRect( l.mLineNumberRect );
      p.drawRect( l.mHotSpot );
      p.drawRect( l.mTextRect );
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
  Bar& b = getBar( iBar );
  b.mPixmap = QPixmap( b.mRect.width(), b.mRect.height() );
  b.mPixmap.fill( Qt::white );
  QPainter bp( &b.mPixmap );
  bp.setRenderHints( bp.renderHints() | QPainter::Antialiasing );
  QPen pen = bp.pen();
  
  //les notes
  /*On utilise la layout lineaire pour dessiner la bar dans le pixmap
   hors ecran. La soustraction "int tx = noteLinearLayout.left() - barLinear.left();"
   est pour déplacer le contenu lineaire de la bar à zéro.*/
  for( int j = 0; j < x->getNumberOfNotesInBar( iBar ); ++j )
  {
    QString s = noteToString( x->getNote( iBar, j ) );
    QFont f = x->isGraceNote( iBar, j ) ? mGraceNotesFont : mBarFont;
    bp.setFont(f);
    bp.drawText( b.getNoteRect(j), s);
  }
  
  //la barre vertical
  int x1 = getBarRegion( brSeparatorX );
  int y1 = 0.15 * kBarHeight;
  int y2 = getBarRegion( brNoteBottomY );
  bp.drawLine( x1, y1, x1, y2 );
  
  //b.mPixmap.toImage().save( "bar" + QString::number(i) + ".png", "PNG" );
  
  //--- render beat groups
  for( int i = 0; i < b.mMatraGroupsRect.size(); ++i )
  {
    QRect r = b.mMatraGroupsRect[i];
    bp.drawArc( r, -10 * 16, -170 * 16 );
  }
  
  //--- render Ornements: meends and krintans
  for( int i = 0; i < x->getNumberOfOrnements(); ++i )
  {
    //Ornement& m = mOrnements[i];
    if( x->ornementAppliesToBar( i, iBar ) )
    {
      Ornement& m = mOrnements[i];
      QPixmap pix( m.mFullOrnement.width(), m.mFullOrnement.height() );
      pix.fill( QColor( 0, 0, 0, 0 ) );
      QPainter mp( &pix );
      mp.setRenderHints( mp.renderHints() | QPainter::Antialiasing );
      QPen ornementPen;
      ornementPen.setColor( Qt::black );
      ornementPen.setWidth( 1 );
      mp.setPen( ornementPen );
      mp.setBrush( Qt::NoBrush );
      if( x->getOrnementType(i) == otMeend )
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
  
  //render strokes
  bp.setFont(mStrokeFont);
  for( int i = 0; i < x->getNumberOfStrokesInBar(iBar); ++i )
  {
    QRect r, r2;
    for( int j = 0; j < x->getNumberOfNotesInStroke(iBar, i); ++j )
    {
      int noteIndex = x->getNoteIndexFromStroke(iBar, i, j);
      if( r.isNull() )
      { r = b.getNoteRect( noteIndex ); }
      else { r = r.united( b.getNoteRect( noteIndex ) ); }
    }
    r2.setTop( getBarRegion( brStrokeY ) );
    r2.setLeft( r.left() );
    r2.setWidth( r.width() );
    r2.setHeight( QFontMetrics(mBarFont).height() );
    QString a = strokeToString( x->getStrokeType(iBar, i) );
    bp.drawText( r2, Qt::AlignCenter, a );
  }
  
  //render selection
  {
    QRect r;
    for( int i = 0; i < mSelectedNotes.size(); ++i )
    {
      int barIndex = mSelectedNotes[i].first;
      if( barIndex == iBar )
      {
        Bar& b = getBar( barIndex );
        int noteIndex = mSelectedNotes[i].second;
        if( r.isNull() )
        { r = b.getNoteRect(noteIndex); }
        else{ { r = r.united(b.getNoteRect(noteIndex)); } }
      }
    }
    
    pen.setColor( QColor( 0, 0, 255, 185 ) );
    bp.setPen( pen );
    if( !r.isNull() )
    {
      r.adjust( -2, -2, 2, 2 );
      bp.drawRoundedRect( r, 2, 2 );
    }
  }

}
//-----------------------------------------------------------------------------
void PartitionViewer::resizeLineEditToContent()
{
  QLineEdit* le = dynamic_cast<QLineEdit*>( QObject::sender() );
  if( le )
  {
    QFontMetrics fm( le->font() );
    le->resize( std::max( fm.width( le->text() ) * 1.1,
                         (double)fm.width( "short" ) ), le->height() );
  }
}
//-----------------------------------------------------------------------------
void PartitionViewer::restoreFocus()
{ setFocus(); }
//-----------------------------------------------------------------------------
void PartitionViewer::setAsDebugging( bool iD )
{ mIsDebugging = iD; updateUi(); }
//-----------------------------------------------------------------------------
void PartitionViewer::setBarAsDirty( int iBar, bool iD )
{ getBar(iBar).mIsDirty = iD; }
//-----------------------------------------------------------------------------
void PartitionViewer::setBarAsDirty( vector<int> iV, bool iD )
{
  for( int i = 0; i < iV.size(); ++i )
  { setBarAsDirty( iV[i], iD ); }
}
//-----------------------------------------------------------------------------
void PartitionViewer::setCurrentBar( int iB )
{ mCurrentBar = iB; updateUi(); }
//-----------------------------------------------------------------------------
void PartitionViewer::setCurrentNote( int iN )
{ mCurrentNote = iN; updateUi(); }
//-----------------------------------------------------------------------------
void PartitionViewer::setComposition(Composition* ipC)
{
  clear();
  x = ipC != 0 ? ipC : &mDummyComposition;
  mBars.resize( ipC->getNumberOfBars() );
  
  //--- lines
  mLines.resize( ipC->getNumberOfLines() );
  
  //--- ornements
  mOrnements.resize( ipC->getNumberOfOrnements() );
  
  //--- title
  mpTitleLe->setText( ipC->getTitle() );
  
  //--- scale
//  mScale.mNotes = ipC->getScale();
//  setBarAsDirty( bScale, true );
  
  setCurrentBar( 0 );
  setCurrentNote( - 1 );
  updateUi();
}
//-----------------------------------------------------------------------------
/*Permet de separer en vecteur de note index par barre une selection iV qui
  contient des notes sur plusieurs barres.*/
map< int, vector<int> > PartitionViewer::splitPerBar(
  vector< std::pair<int, int> > iV )
{
  map< int, vector<int> > r;
  
  int bar = -1, noteIndex = -1;
  vector< int > bg;
  for( int i = 0; i < iV.size(); ++i )
  {
    if( bar != iV[i].first && !bg.empty() )
    {
      r[bar] = bg;
      bg.clear();
    }
    bar = iV[i].first;
    noteIndex = iV[i].second;
    bg.push_back( noteIndex );
  }
  r[bar] = bg;
  
  return r;
}
//-----------------------------------------------------------------------------
void PartitionViewer::startLineTextEdit( int iLineIndex )
{
  mpLineTextEdit->setFocus();
  const Line& l = mLines[iLineIndex];
  mpLineTextEdit->setMinimumWidth( l.mTextRect.width() );
  mpLineTextEdit->setText( x->getLineText(iLineIndex) );
  mEditingLineIndex = iLineIndex;
  updateUi();
}
//-----------------------------------------------------------------------------
void PartitionViewer::stopLineTextEdit()
{
  if( hasLineEditionPending() )
  {
    x->setLineText( mEditingLineIndex, mpLineTextEdit->text() );
    mEditingLineIndex = -1;
    //on met la barre dirty pour forcer le updatePageLayout
    setBarAsDirty( x->getLineFirstBar(mEditingLineIndex), true );
    updateUi();
    setFocus();
  }
}
//-----------------------------------------------------------------------------
QString PartitionViewer::strokeToString( strokeType iSt )
{
  QString r;
  switch (iSt)
  {
    case stDa: r = "\x7c"; break;
    case stRa: r = "\xE2\x80\x95"; break;
    case stDiri: r = "\xE2\x88\xA7"; break;
    case stNone: break;
    default: break;
  }
  return r;
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
vector<NoteLocator> PartitionViewer::toNoteLocator(
  const vector< pair<int, int> > iV ) const
{
  vector<NoteLocator> r;
  for( int i = 0; i < iV.size(); ++i )
  { r.push_back( NoteLocator( iV[i].first, iV[i].second ) ); }
  return r;
}
//-----------------------------------------------------------------------------
/* voir la doc de Qt (http://doc.qt.io/qt-5/qrect.html#details) pour le +1 après
 chaque fonction right() et bottom()*/
void PartitionViewer::updateBar( int iBar )
{
  const int kMargin = 2;
  const int kMinBarWidth = 50;
  
  Bar& b = getBar(iBar);
  //--- definition des rect contenants les notes
  b.mNotesRect.clear();
  QFontMetrics fm( mBarFont );
  QFontMetrics gfm( mGraceNotesFont );
  int separatorPos = getBarRegion( brNoteStartX );
  int barWidth = separatorPos + 2*kMargin;
  int cursorX = kMargin + separatorPos;
  for( int j = 0; j < x->getNumberOfNotesInBar( iBar ); ++j )
  {
    QString n = noteToString( x->getNote( iBar, j ) );
    int noteLenght = x->isGraceNote( iBar, j ) ? gfm.width(n) + 1 : fm.width( n ) + 1;
    int noteHeight = x->isGraceNote( iBar, j )  ? gfm.height() : fm.height();
    int posY = x->isGraceNote( iBar, j ) ?
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
  for( int i = 0; i < x->getNumberOfMatraInBar(iBar); ++i )
  {
    int left = numeric_limits<int>::max();
    int right = numeric_limits<int>::min();
    //vector<int>& v = b.mMatraGroups[i];
    for( int j = 0; j < x->getNumberOfNotesInMatra(iBar, i); ++j )
    {
      int noteIndex = x->getNoteIndexFromMatra(iBar, i, j);
      left = std::min( left, getBar(iBar).getNoteRect(noteIndex).left() );
      right = std::max( right, getBar(iBar).getNoteRect(noteIndex).right() );
    }
    b.mMatraGroupsRect.push_back( QRect( QPoint( left, getBarRegion( brMatraGroupY ) ),
      QSize( right - left, kBeatGroupHeight ) ) );
  }
}
//-----------------------------------------------------------------------------
void PartitionViewer::updatePageLayouts()
{
  //layout des barres spéciale (gamme, accordage tarab, etc...)
  Bar& b = mScale;
  b.mPageLayout.clear();
  b.mNotesPageLayout.clear();
  QRect l = b.mRect;
  QPoint sargamScaleTopLeft = getRegion( rSargamScale ).topLeft();
  l.translate( sargamScaleTopLeft );
  b.mPageLayout.push_back( l );
  //on fait le layout des notes pour cette barre
  for( int j = 0; j < x->getScale().size(); ++j )
  {
    QRect n = b.getNoteRect(j);
    n.translate( sargamScaleTopLeft );
    b.mNotesPageLayout.push_back(n);
  }
  
  //--- le layout des barres du sargam
  mLayoutCursor = getPageRegion( prBody, 0 ).topLeft();
  for( int i = 0; i < x->getNumberOfBars(); ++i )
  {
    Bar& b = getBar(i);
    b.mPageLayout.clear();
    b.mNotesPageLayout.clear();
    
    int pageIndex = toPageIndex( mLayoutCursor );
    QRect pageBody = getPageRegion(  prBody, pageIndex );
    //Si cest le debut d'une ligne, on saute 1.x ligne, sauf si
    //cest la ligne 0
    if( x->isStartOfLine( i ) && x->findLine( i ) != -1)
    {
      int lineIndex = x->findLine( i );
      int verticalShift = lineIndex != 0 ? 1.2*kBarHeight : 0;
      if( !(x->getLineText( lineIndex ).isEmpty()) ||
         mEditingLineIndex == lineIndex )
      { verticalShift += QFontMetrics(mLineFont).height(); }
      mLayoutCursor.setX( pageBody.left() );
      mLayoutCursor.setY( mLayoutCursor.y() + verticalShift );
    }
    
    //--- le layout des bars
    QRect fullLayout = b.mRect;
    int noteHandledIndex = -1;
    int layoutWidthConsumed = 0;
    while( fullLayout.width() )
    {
      pageIndex = toPageIndex( mLayoutCursor );
      pageBody = getPageRegion(  prBody, pageIndex );
      
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
      //ne rentre pas dans le reste de la ligne, mais peu rentrer à l'écran,
      //on saute une ligne...
      else if( !pageBody.contains( pageLayout ) &&
              pageLayout.width() < pageBody.width() )
      {
        mLayoutCursor.setX( pageBody.left() );
        mLayoutCursor.setY( mLayoutCursor.y() + kBarHeight );
      }
      //ne rentre pas dans ce qui reste et est trop grand de toute facon...
      //ilfaut couper le layout.
      else if( !pageBody.contains( pageLayout ) )
      {
        //on coupe le layout pour ne pas depasser la page
        int whereToCut = pageBody.right();
        
        //on fait le layout des notes pour cette barre et on s'assure qu'on ne
        //coupera pas une note en deux
        for( int j = 0; j < x->getNumberOfNotesInBar(i); ++j )
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
        for( int j = 0; j < x->getNumberOfNotesInBar(i); ++j )
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
  
  //--- le layout des ligne, il vient apres le layout des barres parce
  //qu'il en est dependant
  mLines.clear();
  mLines.resize( x->getNumberOfLines() );
  for( int i = 0; i < x->getNumberOfLines(); ++i )
  {
    Line& l = mLines[i];
    const Bar& b = getBar( x->getLineFirstBar(i) );
    QRect bl = b.mPageLayout[0];
    QFontMetrics fm(mLineFont);
    
    //le hotSpot pour ajouter le texte
    bool hasText = !( x->getLineText(i).isEmpty() );
    if( !hasText )
    {
      QRect r( QPoint(), QSize( fm.width( "T" ), fm.height() ) );
      //le hot spot doit etre carre
      int m = std::max( r.width(), r.height() );
      r.setSize( QSize(m, m) );
      r.translate( bl.left() - r.width() - 5, bl.top() );
      l.mHotSpot = r;
    }else{ l.mHotSpot = QRect(); }
    
    //le rect pour le texte de la ligne
    if( hasText || mEditingLineIndex == i )
    {
      QRect r( QPoint(), QSize(
        std::max( fm.width( x->getLineText(i) ), 50 ), fm.height() ) );
      r.translate( bl.left(), bl.top() - r.height() - 2 );
      l.mTextRect = r;
    }else { l.mTextRect = QRect(); }
    
    //le rect du numero de ligne
    {
    QString s = QString::number( i + 1 );
    QRect r( QPoint(), QSize( fm.width(s), fm.height() ) );
    // on translate le rect contenant la numero de ligne. La position
    // finale est 5 pix a gauche de la marge et centre sur la premiere
    // barre de la ligne.
    r.translate( bl.left() - r.width() - 5,
      bl.top() + bl.height() / 2 - r.height() / 2 );
    l.mLineNumberRect = r;
    }
  }
}
//-----------------------------------------------------------------------------
void PartitionViewer::updateOrnement( int iOrn )
{
  Ornement& o = mOrnements[iOrn];
  o.mCuts.clear();
  o.mBlits.clear();
  o.mFullOrnement = QRect();
  
  //update rects...
  vector<int> bi = x->getBarsInvolvedByOrnement( iOrn );
  o.mCuts.resize( bi.size() );
  o.mBlits.resize( bi.size() );
  int barIndex = -1;
  int widthAccum = 0;
  int height = x->getOrnementType(iOrn) == otMeend ? kMeendHeight : kKrintanHeight;
  for( int i = 0; i < bi.size(); ++i )
  {
    barIndex = bi[i];
    int left = numeric_limits<int>::max();
    int right = numeric_limits<int>::min();
    for( int j = 0; j < x->getNumberOfNotesInOrnement(iOrn); ++j )
    {
      NoteLocator nl = x->getNoteLocatorFromOrnement(iOrn, j);
      if( nl.getBar() == barIndex )
      {
        int noteIndex = nl.getIndex();
        left = std::min( left, getBar(barIndex).getNoteRect(noteIndex).left() );
        right = std::max( right, getBar(barIndex).getNoteRect(noteIndex).right() );
        
        //quand c'est la derniere note de la barre et que le Ornement dépasse dnas la barre
        //suivante, on prend la fin de la barre...
        if( noteIndex == (x->getNumberOfNotesInBar(barIndex) - 1) && i < (bi.size() - 1) )
        { right = getBar(barIndex).mRect.right(); }
        
        //quand c'Est la premiere note de la barre et que le Ornement vient de la barre
        //précédente, le left est le début de la barre
        if( noteIndex == 0 && i > 0 )
        { left = getBar(barIndex).mRect.left(); }
      }
    }
    
    QRect blit( QPoint( left, 0 ), QSize( right - left, height ) );
    o.mBlits[i] = make_pair( barIndex, blit );
    QRect cut( QPoint(0, 0), o.mBlits[i].second.size() );
    if( i != 0 )
    { cut.translate( widthAccum, 0); }
    o.mCuts[i] = make_pair( barIndex, cut );
    widthAccum += cut.width();
  }
  o.mFullOrnement = QRect( QPoint(0, 0), QSize(widthAccum, height) );
}
//-----------------------------------------------------------------------------
void PartitionViewer::updateUi()
{
  /*On commence par mettre a jour les dimensions et position des notes de
   chaque nbar via updateBar().
   Ensuite, si la barre est dirty, on mets a jours les ornements et on fait
   le rendu hors ecran de cette barre. Ceci invalide aussi le layout qui 
   devra etre complement recalcule.
   
   Ensuite on place les widgets classiques a l'ecran.
   */
  bool shouldUpdateLayout = false;
  for( int i = 0; i < x->getNumberOfBars(); ++i )
  {
    Bar& b = getBar(i);
    if( b.mIsDirty )
    { updateBar( i ); }
  }
  
  for( int i = 0; i < x->getNumberOfBars(); ++i )
  {
    Bar& b = getBar(i);
    if( b.mIsDirty )
    {
      for( int j = 0; j < x->getNumberOfOrnements(); ++j )
      {
        if( x->ornementAppliesToBar( j, i ) )
        { updateOrnement( j/*&mOrnements[j]*/ ); }
      }
      renderBarOffscreen(i);
      setBarAsDirty( i, false );
      shouldUpdateLayout = true;
    }
  }
  if( shouldUpdateLayout ){ updatePageLayouts(); }
  
  //placer le ui l'écran
  //titre
  QRect r = getRegion( rTitle );
  QPoint c = r.center() - QPoint( mpTitleLe->width() / 2, mpTitleLe->height() / 2 );
  mpTitleLe->move( c );
  
  //place la lineEdit pour les ligne
  if( hasLineEditionPending() )
  {
    QPoint p = mLines[mEditingLineIndex].mTextRect.topLeft();
    p = mapToParent(p);
    QPoint ajust( 5, 0 );
    mpLineTextEdit->move( p + ajust );
    mpLineTextEdit->show();
  }
  else { mpLineTextEdit->hide(); }
  
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
  pFile->addAction( QString("&New"), this, SLOT( newFile() ),
                   QKeySequence::New );

  pFile->addAction( QString("&Open"), this, SLOT( openFile() ),
                   QKeySequence::Open );
  pFile->addAction( QString("&Save"), this, SLOT( save() ),
                   QKeySequence::Save );
  pFile->addAction( QString("Save As..."), this, SLOT( saveAs() ),
                   QKeySequence::SaveAs );

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
  newFile();
  resize( mpPartitionViewer->width() + 35, 800 );
}
//-----------------------------------------------------------------------------
void MainDialog::generateRandomPartition()
{ mpPartitionViewer->generateRandomPartition(); }
//-----------------------------------------------------------------------------
void MainDialog::loadSettings()
{
	mLastSavePath = mSettings.value( "lastSavePath" ).toString();
//  mAddTexturePath = mSettings.value( "addTexturePath" ).toString();
//  mOpenCatalogPath = mSettings.value( "openCatalogPath" ).toString();
//  mRefreshTexturePath = mSettings.value( "refreshTexturePath" ).toString();
}
//-----------------------------------------------------------------------------
void MainDialog::newFile()
{
  //check modif and save...
  mSaveFileName = QString();
  
  mComposition = Composition();
//!!! les 3 lignes suivantes devraient etre dans le constructeur de Composition
  mComposition.setTitle( "Untitled" );
  mComposition.addBar();
  mComposition.addLine(0);
  mpPartitionViewer->setComposition( &mComposition );
  
  updateUi();
}
//-----------------------------------------------------------------------------
void MainDialog::openFile()
{
 	QString s;
  s = QFileDialog::getOpenFileName(
    this, tr("Opem Composition"),
    mLastSavePath,
    tr("Sargam file (*.srg)"));
  
  if(!s.isEmpty())
  {
    mSaveFileName = s;
    mLastSavePath = QFileInfo(s).absolutePath();
    mComposition.fromBinary( utils::fromFile( s ) );
    mpPartitionViewer->setComposition( &mComposition );
    saveSettings();
  }
}
//-----------------------------------------------------------------------------
void MainDialog::save()
{
	if( mSaveFileName.isEmpty() )
  { saveAs(); }
  else
  {
    utils::toFile( mSaveFileName,
      mpPartitionViewer->getComposition().toBinary() );
  }
  updateUi();
}

//-----------------------------------------------------------------------------
void MainDialog::saveAs()
{
	QString s;
  s = QFileDialog::getSaveFileName(
    this, tr("Save Composition"),
    mLastSavePath + "/untitled.srg",
    tr("Sargam file (*.srg)"));
      
  if(!s.isEmpty())
  {
  	mSaveFileName = s;
    mLastSavePath = QFileInfo(s).absolutePath();
    saveSettings();
    save();
  }
  updateUi();
}

//-----------------------------------------------------------------------------
void MainDialog::saveSettings()
{
	mSettings.setValue( "lastSavePath", mLastSavePath );
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

