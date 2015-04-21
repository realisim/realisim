/* */

#include "MainDialog.h"
#include "math/intersection.h"
#include "math/Line.h"
#include "math/Primitives.h"
#include "math/MathUtils.h"
#include "math/Point.h"
#include "math/Vect.h"
#include "utils/utilities.h"
#include <QPrintDialog>
#include <QPrintPreviewDialog>
#include <QPrinter>
#include <QPrinterInfo>
#include <set>
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */

using namespace std;
using namespace realisim;
  using namespace math;
  using namespace utils;
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
  const int kNoBarHover = -5;
  
  const QString kSargamScaleLabel("Scale: ");
  const QString kTarabTuningLabel("Tarab tuning: ");
}

realisim::sargam::Composition PartitionViewer::mDummyComposition;

//-----------------------------------------------------------------------------
// --- partition viewer
//-----------------------------------------------------------------------------
PartitionViewer::PartitionViewer( QWidget* ipParent ) :
  QWidget( ipParent ),
  mpTitleEdit(0),
  mpLineTextEdit(0),
  mIsDebugging( false),
  mPageSizeId( QPageSize::Letter ),
  mLayoutOrientation( QPageLayout::Portrait ),
  mNumberOfPages(0),
  mCurrentBar( -1 ),
  mCurrentNote( -1 ),
  mOctave( 0 ),
  mEditingLineIndex( -1 ),
  mEditingTitle( false ),
  mAddLineTextHover( -1 ),
  mBarHoverIndex( kNoBarHover ),
  x( &mDummyComposition ),
  mDefaultLog(),
  mpLog( &mDefaultLog ),
  mIsVerbose( false )
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
{ setNumberOfPage( getNumberOfPages() + 1 ); }
//-----------------------------------------------------------------------------
void PartitionViewer::clear()
{
  setNumberOfPage(1);
  mBars.clear();
  mLines.clear();
  mBarsPerPage.clear();
  mOrnements.clear();
  mCurrentBar = -1;
  mCurrentNote = -1;
  mSelectedNotes.clear();
  setBarAsDirty(sbScale, true);
  setBarAsDirty(sbTarabTuning, true);
  
  if( isVerbose() )
  { getLog().log( "PartitionViewer: clear." ); }
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
  /*Cette commande ne peut pas etre executée sur les barres speciales*/
  if( getCurrentBar() < 0 ){ return; }
  
  int cb = getCurrentBar();
  addBar( cb );
  setCurrentNote( -1 );
  setCurrentBar( cb + 1 );
  clearSelection();
  setBarAsDirty( getCurrentBar(), true );
  updateUi();
  
  if( isVerbose() )
  { getLog().log( "PartitionViewer: commandAddBar." ); }
}
//-----------------------------------------------------------------------------
void PartitionViewer::commandAddBarBefore()
{
  /*Cette commande ne peut pas etre executée sur les barres speciales*/
  if( getCurrentBar() < 0 ){ return; }
  
  if( x->isStartOfLine( getCurrentBar() ) )
  {
    int cl = x->findLine( getCurrentBar() );
    QString t = x->getLineText( cl );
    x->eraseLine( cl );
    addBar( getCurrentBar() - 1 );
    x->addLine( getCurrentBar(), t );
  }
  else
  { addBar( getCurrentBar() - 1 ); }
  
  setCurrentNote( -1 );
  setCurrentBar( getCurrentBar() );
  clearSelection();
  setBarAsDirty( getCurrentBar(), true );
  updateUi();
  
  if( isVerbose() )
  { getLog().log( "PartitionViewer: commandAddBarBefore." ); }
}
//-----------------------------------------------------------------------------
/*Les notes de la selection deviennent des graces notes si elle ne le sont
 pas déjà.*/
void PartitionViewer::commandAddGraceNotes()
{
  /*Cette commande ne peut pas etre executée sur les barres speciales*/
  if( getCurrentBar() < 0 ){ return; }
  
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
  
  if( isVerbose() )
  { getLog().log( "PartitionViewer: commandAddGraceNotes." ); }
}
//-----------------------------------------------------------------------------
void PartitionViewer::commandAddLine()
{
  /*Cette commande ne peut pas etre executée sur les barres speciales*/
  if( getCurrentBar() < 0 ){ return; }
  
  x->addLine( getCurrentBar() );
  setCurrentNote( -1 );
  setBarAsDirty( getCurrentBar(), true );
  
  if( isVerbose() )
  { getLog().log( "PartitionViewer: commandAddLine." ); }
}
//-----------------------------------------------------------------------------
void PartitionViewer::commandAddMatra()
{
  /*Cette commande ne peut pas etre executée sur les barres speciales*/
  if( getCurrentBar() < 0 ){ return; }
  
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
  
  if( isVerbose() )
  { getLog().log( "PartitionViewer: commandAddMatra." ); }
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
  
  if( isVerbose() )
  { getLog().log( "PartitionViewer: commandAddNote." ); }
}
//-----------------------------------------------------------------------------
void PartitionViewer::commandAddOrnement( ornementType iOt )
{
  /*Cette commande ne peut pas etre executée sur les barres speciales*/
  if( getCurrentBar() < 0 ){ return; }
  
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
  
  if( isVerbose() )
  { getLog().log( "PartitionViewer: commandAddOrnement." ); }
}
//-----------------------------------------------------------------------------
void PartitionViewer::commandAddStroke( strokeType iSt )
{
  /*Cette commande ne peut pas etre executée sur les barres speciales*/
  if( getCurrentBar() < 0 ){ return; }
  
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
  
  if( isVerbose() )
  { getLog().log( "PartitionViewer: commandAddStroke." ); }
}
//-----------------------------------------------------------------------------
void PartitionViewer::commandBreakMatrasFromSelection()
{
  /*Cette commande ne peut pas etre executée sur les barres speciales*/
  if( getCurrentBar() < 0 ){ return; }
  
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
  
  if( isVerbose() )
  { getLog().log( "PartitionViewer: commandBreakMatrasFromSelection." ); }
}
//-----------------------------------------------------------------------------
void PartitionViewer::commandBreakOrnementsFromSelection()
{
  /*Cette commande ne peut pas etre executée sur les barres speciales*/
  if( getCurrentBar() < 0 ){ return; }
  
  for( int i = 0; i < mSelectedNotes.size(); ++i )
  {
    int bar = mSelectedNotes[i].first;
    int index = mSelectedNotes[i].second;
    int ornementIndex = x->findOrnement( bar, index );
    if( ornementIndex != -1 )
    { eraseOrnement( ornementIndex ); }
  }
  
  if( isVerbose() )
  { getLog().log( "PartitionViewer: commandBreakOrnementsFromSelection." ); }
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
  
  if( isVerbose() )
  { getLog().log( "PartitionViewer: commandDecreaseOctave." ); }
}
//-----------------------------------------------------------------------------
void PartitionViewer::commandErase()
{
  if( hasSelection() )
  {
    /*On efface les notes en partant de la fin parce que la selection contient
     les indices des notes, ainsi en commençant par la fin, nous n'avons pas à
     ajuster les indices.*/
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
      /*On efface la ligne si la note courante est -1 et que la barre est
        un début de ligne. Par contre, on ne peut pas effacer la ligne 0.*/
      if( getCurrentNote() == -1 && x->isStartOfLine( cb ) &&
         x->findLine( cb ) != 0 )
      { x->eraseLine( x->findLine( cb ) ); }
      else
      {
        x->eraseNote( cb, getCurrentNote() );
        setCurrentNote( getCurrentNote() - 1 );
      }
    }
    else
    {
      /*On ne peut pas effacer la derniere barre...*/
      if( x->getNumberOfBars() > 1 )
      {
        eraseBar( cb );
        setCurrentBar( cb - 1 );
        setCurrentNote( x->getNumberOfNotesInBar( getCurrentBar() ) - 1 );
      }
    }
    setBarAsDirty( getCurrentBar(), true );
  }
  updateUi();
  
  if( isVerbose() )
  { getLog().log( "PartitionViewer: commandErase." ); }
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
  
  if( isVerbose() )
  { getLog().log( "PartitionViewer: commandIncreaseOctave." ); }
}
//-----------------------------------------------------------------------------
void PartitionViewer::commandRemoveSelectionFromGraceNotes()
{
  /*Cette commande ne peut pas etre executée sur les barres speciales*/
  if( getCurrentBar() < 0 ){ return; }
  
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
  
  if( isVerbose() )
  { getLog().log( "PartitionViewer: commandRemoveSelectionFromGraceNotes." ); }
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
  
  if( isVerbose() )
  { getLog().log( "PartitionViewer: commandShiftNote." ); }
}
//-----------------------------------------------------------------------------
void PartitionViewer::createUi()
{
  //titre
  /* Astuce: les line edit on comme parent le parent du widget... ainsi
     les touches pesees lors de l'édition* n'interfere jamais avec la
     méthode keyPressEvent de la classe PartitionViewer. */
  mpTitleEdit = new QLineEdit( this->parentWidget() );
  mpTitleEdit->setFont( mTitleFont );
  mpTitleEdit->hide();
  connect( mpTitleEdit, SIGNAL( editingFinished() ),
          this, SLOT( stopTitleEdit() ) );
  connect( mpTitleEdit, SIGNAL( textChanged(const QString&)),
          this, SLOT( resizeLineEditToContent() ) );
  
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
void PartitionViewer::draw( QPaintDevice* iPaintDevice ) const
{
  QPainter p( iPaintDevice );
  p.setBackgroundMode( Qt::OpaqueMode );
  QBrush b( Qt::white );
  QPen pen = p.pen();
  p.setBackground( b );
  
  //--- render pages
  drawPages( &p );
  
  //--- titre
  drawTitle( &p );
  
  //--- rendu des textes pour barres speciales (sargam scale, tarab tuning...)
  p.setPen( Qt::black );
  p.setFont( mBarFont );
  p.drawText( getRegion( rSargamScaleLabel ), Qt::AlignCenter, kSargamScaleLabel );
  p.drawText( getRegion( rTarabTuningLabel ), Qt::AlignCenter, kTarabTuningLabel );
  
  //render bars
  drawSpecialBars( &p );
  for( int i = 0; i < x->getNumberOfBars(); ++i )
  {
    drawBar( &p, i );
    if( x->isStartOfLine( i ) )
    { drawLine( &p, x->findLine( i ) ); }
  }
  
  drawCursor( &p );
  drawSelectedNotes( &p );
  
  //on dessine le contour de la barre survolee si ce nest pas la courant
  if( mBarHoverIndex != getCurrentBar() )
  { drawBarContour( &p, mBarHoverIndex, QColor( 125, 125, 125, 120 ) ); }
  //on dessine le contour de la barre courante
  drawBarContour( &p, getCurrentBar(), QColor( 0, 10, 210, 120 ) );
  
  drawPageFooters( &p );
}
//-----------------------------------------------------------------------------
void PartitionViewer::drawBar( QPainter* iP, int iBar ) const
{
  iP->save();
  
  const Bar& b = getBar( iBar );
  if( !b.mIsWayTooLong )
  {
    QPoint topLeft = b.mScreenLayout.topLeft();
    QTransform transfo;
    transfo.translate( topLeft.x(), topLeft.y() );
    iP->setWorldTransform( transfo, true );
    
    iP->setRenderHints( iP->renderHints() | QPainter::Antialiasing );
    QPen pen = iP->pen();
    pen.setColor( Qt::black );
    
    //les notes
    for( int j = 0; j < x->getNumberOfNotesInBar( iBar ); ++j )
    {
      QString s = noteToString( x->getNote( iBar, j ) );
      QFont f = x->isGraceNote( iBar, j ) ? mGraceNotesFont : mBarFont;
      iP->setFont(f);
      iP->drawText( b.getNoteRect(j), s);
    }
    
    //la barre vertical
    int x1 = getBarRegion( brSeparatorX );
    int y1 = 0.15 * kBarHeight;
    int y2 = getBarRegion( brNoteBottomY );
    iP->drawLine( x1, y1, x1, y2 );
    
    //--- render beat groups
    for( int i = 0; i < b.mMatraGroupsRect.size(); ++i )
    {
      QRect r = b.mMatraGroupsRect[i];
      iP->drawArc( r, -10 * 16, -170 * 16 );
    }
    
    //--- render Ornements: meends and krintans
    for( int i = 0; i < x->getNumberOfOrnements(); ++i )
    {
      if( x->ornementAppliesToBar( i, iBar ) )
      {
        const Ornement& m = mOrnements[i];
        QRect r =  m.mFullOrnement;
        
        iP->save();
        iP->setClipRect( QRect( QPoint(0, 0), b.mScreenLayout.size() ) );
        iP->translate( m.getOffset( iBar ), 0 );
        iP->translate( m.getDestination( iBar ), 0 );
        if( x->getOrnementType(i) == otMeend )
        { iP->drawArc( r, 10 * 16, 170 * 16 ); }
        else //krintan
        {
          iP->drawLine( r.bottomLeft(), r.topLeft() );
          iP->drawLine( r.topLeft(), r.topRight() );
          iP->drawLine( r.topRight(), r.bottomRight() );
        }
        
        iP->restore();
      }
    }
    
    //render strokes
    iP->setFont(mStrokeFont);
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
      iP->drawText( r2, Qt::AlignCenter, a );
    }
  }
  else
  {
    iP->save();
    iP->resetTransform();
    QPen pen = iP->pen();
    pen.setStyle( Qt::DashLine );
    pen.setColor( Qt::red );
    iP->setPen( pen );
    iP->drawRoundedRect(b.mScreenLayout, 2, 2);
    iP->drawText(b.mScreenLayout, Qt::AlignCenter, "Multi-lines bar are not "
                 "supported yet...");
    iP->restore();
  }
  
  iP->restore();
}
//-----------------------------------------------------------------------------
void PartitionViewer::drawBarContour( QPainter* iP, int iBarIndex, QColor iCol ) const
{
  if( iBarIndex > sbScale && hasFocus() )
  {
    QPen pen = iP->pen();
    iP->save();
    iP->setRenderHints( QPainter::Antialiasing );
    pen.setColor( iCol );
    pen.setWidth( 1 );
    iP->setPen( pen );
    const Bar& b = getBar( iBarIndex );
    iP->drawRoundedRect( b.mScreenLayout, 2, 2 );
    iP->restore();
  }
}
//-----------------------------------------------------------------------------
void PartitionViewer::drawCursor( QPainter* iP ) const
{
  QPen pen = iP->pen();
  //le curseur dans la barre courante s'il n'y a pas de selection
  //et que le focus y est
  pen.setColor( Qt::black );
  pen.setWidth( 1 );
  iP->setPen( pen );
  if( !hasSelection() && hasFocus() )
  { iP->drawLine( getCursorLine() ); }
}
//-----------------------------------------------------------------------------
void PartitionViewer::drawLine( QPainter* iP, int iLine ) const
{
  QPen pen = iP->pen();

  pen.setColor( Qt::black );
  iP->setPen( pen );
  iP->setFont( mLineFont );
  const Line& l = mLines[ iLine ];
  //numero de ligne. les lignes commencent a 1!!!
  iP->drawText( l.mLineNumberRect, QString::number( iLine + 1 ) );
  //le hot spot
  if( mAddLineTextHover == iLine )
  {
    iP->save();
    QPen pen = iP->pen();
    pen.setStyle( Qt::DashLine );
    pen.setColor( Qt::gray );
    iP->setPen( pen );
    iP->drawRoundedRect(l.mHotSpot, 2, 2);
    iP->restore();
  }
  
  //text de la ligne
  if( mEditingLineIndex != iLine )
  { iP->drawText( l.mTextScreenLayout.bottomLeft(), x->getLineText(iLine) ); }

}
//-----------------------------------------------------------------------------
void PartitionViewer::drawPages( QPainter* iP ) const
{
  for( int i = 0; i < getNumberOfPages(); ++i)
  {
    //on dessine le background blanc de la page
    iP->setPen( Qt::white );
    iP->setBrush( Qt::white );
    iP->drawRect( getPageRegion( prPage, i ) );
    
    //--- tout le text de la page
    iP->setPen( Qt::black );
    iP->setBrush( Qt::NoBrush );
  }
}
//-----------------------------------------------------------------------------
void PartitionViewer::drawPageFooter( QPainter* iP, int i ) const
{
  //on dessine le bas de page
  //l'indice de page commence à 0, mais on veut que montrer page 1.
  QString text;
  text.sprintf( "%s: %d out of %d", x->getTitle().toStdString().c_str(),  i + 1,
    getNumberOfPages() );
  iP->setFont(QFont("Arial", 10));
  iP->setPen( Qt::gray );
  iP->drawText( getPageRegion( prPageFooter, i ), Qt::AlignCenter, text );
}
//-----------------------------------------------------------------------------
void PartitionViewer::drawPageFooters( QPainter* iP ) const
{
  for( int i = 0; i < getNumberOfPages(); ++i )
  { drawPageFooter( iP, i ); }
}
//-----------------------------------------------------------------------------
void PartitionViewer::drawSelectedNotes( QPainter* iP ) const
{
  iP->save();
  iP->setRenderHints( iP->renderHints() | QPainter::Antialiasing );
  
  map<int, vector<int> > m = splitPerBar(mSelectedNotes);
  map<int, vector<int> >::iterator it = m.begin();
  for( ; it != m.end(); ++it )
  {
    QRect r;
    const Bar& b = getBar( it->first );
    const vector<int>& vn = it->second;
    for( int i = 0 ; i < vn.size(); ++i )
    {
      int noteIndex = vn[i];
      if( r.isNull() )
      { r = b.getNoteRect(noteIndex); }
      else{ { r = r.united(b.getNoteRect(noteIndex)); } }
    }
    
    QTransform transfo;
    QPoint topLeft = b.mScreenLayout.topLeft();
    transfo.translate( topLeft.x(), topLeft.y() );
    iP->setWorldTransform( transfo );
    
    QPen pen = iP->pen();
    pen.setColor( QColor( 0, 0, 255, 185 ) );
    iP->setPen( pen );
    if( !r.isNull() )
    {
      r.adjust( -2, -2, 2, 2 );
      iP->drawRoundedRect( r, 2, 2 );
    }
  }
  
  iP->restore();
}
//-----------------------------------------------------------------------------
void PartitionViewer::drawSpecialBars( QPainter* iP ) const
{
  drawBar( iP, sbScale );
  drawBar( iP, sbTarabTuning );
}
//-----------------------------------------------------------------------------
void PartitionViewer::drawTitle( QPainter* iP ) const
{
  iP->save();
  
  if( !hasTitleEditionPending() )
  {
    iP->setFont( mTitleFont );
    iP->setPen( Qt::black );
    iP->drawText( mTitleScreenLayout, Qt::AlignCenter, x->getTitle() );
  }
  
  iP->restore();
}
//-----------------------------------------------------------------------------
/*Efface la barre iBar des donnees et des donnees d'affichage.*/
void PartitionViewer::eraseBar( int iBar )
{
  if( iBar >= 0 && iBar < x->getNumberOfBars() )
  {
    mBars.erase( mBars.begin() + iBar );
    if( x->getNumberOfBars() > 0 ){ setBarAsDirty(0, true); }
  }
  x->eraseBar( iBar );
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
  for( int i = 0; i < kNumBars; ++i )
  {
    addBar( x->getNumberOfBars() );
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
    case sbScale: r = &mScale; break;
    case sbTarabTuning: r = &mTarabTuning; break;
    default: r = &mBars[iBar]; break;
  }
  return *r;
}
//-----------------------------------------------------------------------------
vector<int> PartitionViewer::getBarsFromPage( int iPage ) const
{
  vector<int> r;
  if( iPage >= 0 && iPage < getNumberOfPages() )
  { r = mBarsPerPage.at(iPage); }
  return r;
}
//-----------------------------------------------------------------------------
Composition PartitionViewer::getComposition() const
{ return *x; }
//-----------------------------------------------------------------------------
QLine PartitionViewer::getCursorLine() const
{
  QLine l;
  const Bar& b = getBar( getCurrentBar() );
  const int cn = getCurrentNote();
  if( cn >= 0 && cn < b.mNoteScreenLayouts.size() )
  {
    QRect r = b.mNoteScreenLayouts[getCurrentNote()];
    l.setPoints( r.topRight(), r.bottomRight() );
  }
  else
  {
    int x = b.mScreenLayout.left() + getBarRegion( brNoteStartX );
    int y = b.mScreenLayout.top() + getBarRegion( brNoteTopY );
    int h = getBarRegion( brNoteBottomY ) - getBarRegion( brNoteTopY );
    l.setPoints( QPoint(x,y) , QPoint( x, y + h ) );
  }
  return l;
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
QPageLayout::Orientation PartitionViewer::getLayoutOrientation() const
{ return mLayoutOrientation; }
//-----------------------------------------------------------------------------
const Log& PartitionViewer::getLog() const
{ return *mpLog; }
//-----------------------------------------------------------------------------
Log& PartitionViewer::getLog()
{ return const_cast<Log&>( const_cast< const PartitionViewer* >(this)->getLog() ); }
//-----------------------------------------------------------------------------
int PartitionViewer::getInterNoteSpacing(int iBar, int iIndex1, int iIndex2 ) const
{
  
  int r = kInterNoteGap;
  int matra1 = x->findMatra( iBar, iIndex1 );
  int matra2 = x->findMatra( iBar, iIndex2 );
  //les notes d'un même matra
  if( matra1 == matra2 && matra1 != -1 && matra2 != -1 )
  {
    r = 0;
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
      int w = getPageSizeInInch().width() * logicalDpiX();
      int h = getPageSizeInInch().height() * logicalDpiY();
      int t = iPage * h + iPage * kInterPageGap;
      r.setTop( t );
      r.setWidth( w ); r.setHeight( h );
    }break;
    case prBody:
    {
      int margin = cmToPixel( kPageMarginInCm );
      int sargamScaleBottom = iPage == 0 ? getRegion(rTarabTuning).bottom() + kSpacing : 0;
      int startOfBody = max( margin, sargamScaleBottom );
      int paperWidth = getPageSizeInInch().width() * logicalDpiX();
      int paperHeight = getPageSizeInInch().height() * logicalDpiY();
      int topOfPage = getPageRegion( prPage, iPage ).top();//iPage * paperHeight + iPage * kInterPageGap;
      r.setLeft( margin );
      r.setWidth( paperWidth - 2*margin );
      r.setTop( topOfPage + startOfBody );
      r.setBottom( topOfPage + paperHeight - margin - kPageFooter );
    }break;
    case prPageFooter:
    {
      int margin = cmToPixel( kPageMarginInCm );
      r = getPageRegion( prPage, iPage );
      r.setBottom( r.bottom() - margin );
      r.setTop( r.bottom() - kPageFooter );
    }break;
  }
  
  return r;
}
//-----------------------------------------------------------------------------
QPageSize::PageSizeId PartitionViewer::getPageSizeId() const
{ return mPageSizeId; }
//-----------------------------------------------------------------------------
QSizeF PartitionViewer::getPageSizeInInch() const
{
  QSizeF s = QPageSize::size( mPageSizeId, QPageSize::Inch);
  
  if( getLayoutOrientation() == QPageLayout::Landscape )
  { s = QSizeF( s.height(), s.width() ); }
  
  return s;
}
//-----------------------------------------------------------------------------
QRect PartitionViewer::getRegion( region iR ) const
{
  QRect r(0, 0, 1, 1);
  
  switch( iR )
  {
    case rPartition:
    {
      int w = getPageSizeInInch().width() * logicalDpiX();
      int h = getNumberOfPages() * getPageSizeInInch().height() * logicalDpiY() +
      std::max(getNumberOfPages() - 1, 0) * kInterPageGap;
      r.setWidth( w ); r.setHeight( h );
    }break;
    case rTitle:
    {
      int w = getPageSizeInInch().width() * logicalDpiX();
      int h = QFontMetrics(mTitleFont).height();
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
      r.setWidth( fm.width( kSargamScaleLabel ) );
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
    case rTarabTuningLabel:
    {
      QFontMetrics fm( mBarFont );
      QRect t = getRegion( rSargamScaleLabel );
      r = t;
      r.translate( 0, t.height() + kSpacing );
      int margin = cmToPixel( kPageMarginInCm );
      r.setLeft( margin );
      r.setHeight( kBarHeight );
      r.setWidth( fm.width( kTarabTuningLabel ) );
    } break;
    case rTarabTuning:
    {
      QFontMetrics fm( mBarFont );
      QRect t = getRegion( rSargamScaleLabel );
      int margin = cmToPixel( kPageMarginInCm );
      r = t;
      r.translate( 0, t.height() + kSpacing );
      r.setLeft( getRegion( rTarabTuningLabel ).right() );
      r.setRight( getPageSizeInInch().width() * logicalDpiX() - margin );
      r.setHeight( kBarHeight );
    } break;
  }
  return r;
}
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
  if( isVerbose() )
  { getLog().log( "PartitionViewer: key %s pressed.", QKeySequence( ipE->key(),
    QKeySequence::NativeText ).toString().toStdString().c_str() ); }
  
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
    {
      /* 1. S'il n'y a pas de selection:
             Shift est enfoncé, donc on selectionne la note courante.
             Shift n'est pas enfoncé, donc on déplace la note courante
         2. S'il y a une selection:
             Shift est enfoncé, on agrandit la selection par la gauche.
             Shift n'est pas enfoncé, la selection disparait et la note courante
               devient la premiere note de la selection
       */
      bool shiftPressed = (ipE->modifiers() & Qt::ShiftModifier);
      if( !hasSelection() )
      {
        if( shiftPressed )
        {
          int bar = getCurrentBar();
          int index = mCurrentNote;
          if( index == -1 && (bar - 1) < x->getNumberOfBars() )
          {
            bar--;
            index = x->getNumberOfNotesInBar(bar) - 1;
          }
          addNoteToSelection( bar, index );
        }
        else
        {
          mCurrentNote = std::max( --mCurrentNote, -2 );
          if( getCurrentNote() == -2 && getCurrentBar() > sbScale )
          {
            setCurrentBar( getCurrentBar() - 1 );
            setCurrentNote( x->getNumberOfNotesInBar( getCurrentBar() ) - 1 );
          }
        }
      }
      else
      {
        if( shiftPressed )
        {
          //ajoute la note a gauche de la premiere note selectionnee
          int bar = mSelectedNotes[0].first;
          int index = mSelectedNotes[0].second;
          index = max( index - 1, -1 );
          if( index < 0 && bar > 0 )
          {
            bar--;
            index = x->getNumberOfNotesInBar(bar) - 1;
          }
          addNoteToSelection( bar, index );
        }
        else
        {
          int index = max( -1, mSelectedNotes[0].second - 1 );
          setCurrentBar( mSelectedNotes[0].first );
          setCurrentNote( index );
          clearSelection();
        }
      }
    } break;
    case Qt::Key_Right:
    {
      /* 
       1. S'il n'y a pas de selection:
         Shift est enfoncé, donc on selectionne la note courante.
         Shift n'est pas enfoncé, donc on déplace la note courante
       2. S'il y a une selection:
         Shift est enfoncé, on agrandit la selection par la gauche.
         Shift n'est pas enfoncé, la selection disparait et la note courante
           devient la premiere note de la selection */
      bool shiftPressed = (ipE->modifiers() & Qt::ShiftModifier);
      if( !hasSelection() )
      {
        int bar = getCurrentBar();
        int nextNote = mCurrentNote + 1;
        
        if( shiftPressed )
        {
          if( nextNote > x->getNumberOfNotesInBar( bar ) - 1 &&
             x->getNumberOfBars() > bar + 1 )
          {
            bar++;
            nextNote = 0;
          }
          if( x->getNumberOfNotesInBar( bar ) > nextNote )
          { addNoteToSelection( bar, nextNote ); }
        }
        else
        {
          if( nextNote > x->getNumberOfNotesInBar( bar ) - 1 &&
             x->getNumberOfBars() > bar + 1 )
          {
            bar++;
            nextNote = -1;
          }
          nextNote = min( nextNote, x->getNumberOfNotesInBar( bar ) - 1 );
          setCurrentBar( bar );
          setCurrentNote( nextNote );
        }
      }
      else
      {
        int bar = mSelectedNotes[ mSelectedNotes.size() - 1 ].first;
        int index = mSelectedNotes[ mSelectedNotes.size() - 1 ].second;
        if( shiftPressed )
        {
          index += 1;
          if( index > x->getNumberOfNotesInBar(bar) - 1 &&
             x->getNumberOfBars() > bar + 1 )
          {
            bar++;
            index = 0;
          }
          addNoteToSelection( bar, index );
        }
        else
        {
          setCurrentBar( bar );
          setCurrentNote( index );
          clearSelection();
        }
      }
    } break;
    case Qt::Key_Space:
      if( (ipE->modifiers() & Qt::ShiftModifier) )
      { commandAddBarBefore(); }
      else { commandAddBar(); }
      break;
    case Qt::Key_Backspace: commandErase(); break;
    case Qt::Key_Return: commandAddLine(); break;
    case Qt::Key_Plus: commandIncreaseOctave(); break;
    case Qt::Key_Minus: commandDecreaseOctave(); break;
    case Qt::Key_Shift: break;
    default: break;
  }
  updateUi();
  emit ensureVisible( getCursorLine().p2() );
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
  
  //intersection avec le titre
  if( mTitleScreenLayout.contains( pos ) )
  { cs = Qt::IBeamCursor; }
  
  //intersection avec les barres
  for( int i = 0; i < x->getNumberOfBars(); ++i )
  {
    const Bar& b = getBar(i);
    if( b.mScreenLayout.contains( pos ) )
    { mBarHoverIndex = i; updateUi(); }
    else if ( mBarHoverIndex == i )
    { mBarHoverIndex = kNoBarHover; updateUi(); }
  }
  
  //intersection avec les lignes
  for( int i = 0; i < x->getNumberOfLines(); ++i )
  {
    if( mLines[i].mHotSpot.contains( pos ) )
    { mAddLineTextHover = i; updateUi(); }
    else if(mAddLineTextHover == i)
    { mAddLineTextHover = -1; updateUi(); }
      
    if( mLines[i].mTextScreenLayout.contains( pos ) )
    { cs = Qt::IBeamCursor; }
  }
  
  setCursor( cs );
  
  if( isDebugging() )
  { updateUi(); }
}
//-----------------------------------------------------------------------------
void PartitionViewer::mouseReleaseEvent( QMouseEvent* ipE )
{
  QPoint pos = ipE->pos();
  
  //intersection avec le titre
  if( mTitleScreenLayout.contains( pos ) )
  { startTitleEdit(); }
  
  //intersection avec les barres
  if( mBarHoverIndex != kNoBarHover )
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
    
    if( mLines[i].mTextScreenLayout.contains( pos ) )
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
  draw( this );
  
  //--- render debug infos
  if( isDebugging() )
  {
    QPainter p(this);
    p.setFont(QFont("Arial", 10));
    p.setBrush( Qt::NoBrush );
    
    p.setPen( Qt::blue );
    p.drawRect( mTitleScreenLayout );
    
    p.setPen( Qt::yellow );
    for( int i = 0; i < getNumberOfPages(); ++i)
    {
      if( i == 0 )
      {
        p.drawRect( getRegion( rTitle ) );
        p.drawRect( getRegion( rSargamScaleLabel ) );
        p.drawRect( getRegion( rSargamScale ) );
        p.drawRect( getRegion( rTarabTuningLabel ) );
        p.drawRect( getRegion( rTarabTuning ) );
      }
      p.drawRect( getPageRegion( prBody, i ) );
      p.drawRect( getPageRegion( prPageFooter, i ) );
    }
    
    //on dessine le layout des bars...
    for( int i = 0; i < x->getNumberOfBars(); ++i )
    {
      p.setPen( Qt::green );
      Bar& b = getBar(i);
      //le layput page de la bar
      p.drawRect( b.mScreenLayout );
      
      //le layout page des notes
      for( int j = 0; j < b.mNoteScreenLayouts.size(); ++j )
      { p.drawRect( b.mNoteScreenLayouts[j] ); }
      
      //texte de debuggage
      p.setPen(Qt::gray);
      p.drawText( b.mScreenLayout, Qt::AlignCenter, QString::number(i) );
      QString s;
      if( getCurrentBar() == i )
      { s += QString().sprintf("current note: %d\n", getCurrentNote() ); }
      s += QString().sprintf("number of notes: %d", x->getNumberOfNotesInBar( i ) );
      p.drawText( b.mScreenLayout, Qt::AlignBottom | Qt::AlignRight, s );
    }
    
    p.setPen( Qt::blue );
    //layout des lignes
    for( int i = 0; i < x->getNumberOfLines(); ++i )
    {
      const Line& l = mLines[i];
      p.drawRect( l.mLineNumberRect );
      p.drawRect( l.mHotSpot );
      p.drawRect( l.mTextScreenLayout );
    }
    
    //--- different texte...
    p.setPen(Qt::gray);
    //posiition de la souris, dans le coin sup gauche...
    {
      p.save();
      QBrush b( Qt::white );
      p.setBackgroundMode( Qt::OpaqueMode );
      p.setBackground( b );
      QRect r = ipE->region().boundingRect();
      QPoint pos = mapFromGlobal( QCursor::pos() );
      QString s;
      s.sprintf( "Mouse pos: %d, %d\n"
                "Region to repaint: %d, %d, %d, %d",
                pos.x(), pos.y(),
                r.left(), r.top(), r.right(), r.bottom() );
      p.drawText( r.topLeft() + QPoint(10, 10), s );
      p.restore();
    }
    
    QString s;
    s.sprintf("Partition size: %d, %d\n",
              getRegion( rPartition ).width(),
              getRegion( rPartition ).height() );
    p.drawText(rect(), Qt::AlignBottom | Qt::AlignRight, s);
    
    /*print des barres par pages...*/
//    for( int i = 0; i < getNumberOfPages(); ++i )
//    {
//      vector<int> bars = getBarsFromPage( i );
//      for( int j = 0; j < bars.size(); ++j )
//      { printf( "page %d, bar %d\n", i, bars[j] ); }
//    }
  }
}
//-----------------------------------------------------------------------------
void PartitionViewer::print( QPrinter* iPrinter )
{
  QPageSize::PageSizeId previousPageSizeId = getPageSizeId();
  QPageLayout::Orientation previousOrientation = getLayoutOrientation();
  
  QPageLayout pl = iPrinter->pageLayout();
  QMarginsF minMargin = pl.minimumMargins();
  QMarginsF maxMargin = pl.maximumMargins();
  
  setPageSize( pl.pageSize().id() );
  setLayoutOrientation( pl.orientation() );

  QPainter p( iPrinter );
  
  //--- titre
  drawTitle( &p );
  
  //--- rendu des textes pour barres speciales (sargam scale, tarab tuning...)
  p.setFont( mBarFont );
  p.drawText( getRegion( rSargamScaleLabel ), Qt::AlignCenter, kSargamScaleLabel );
  p.drawText( getRegion( rTarabTuningLabel ), Qt::AlignCenter, kTarabTuningLabel );
  
  drawSpecialBars( &p );
  
  for( int i = 0; i < getNumberOfPages(); ++i )
  {
    p.save();
    QTransform pageTransfo;
    QPoint pageTop = getPageRegion( prPage, i ).topLeft();
    pageTop.setY( pageTop.y() - i * kInterPageGap );
    
    pageTransfo.translate( -pageTop.x(), -pageTop.y() );
    p.setWorldTransform( pageTransfo );
    
    vector<int> bars = getBarsFromPage( i );
    for( int j = 0; j < bars.size(); ++j )
    {
      drawBar( &p, bars[j] );
      if( x->isStartOfLine( bars[j] ) )
      { drawLine( &p, x->findLine( bars[j] ) ); }
    }
  
    drawPageFooter( &p, i );
    
    p.restore();
    if( i < getNumberOfPages() - 1 )
    { iPrinter->newPage(); }
  }
  
  //on remet la taille du papier...
  setPageSize( previousPageSizeId );
  setLayoutOrientation( previousOrientation );
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
    updateUi();
  }
}
//-----------------------------------------------------------------------------
void PartitionViewer::setAsDebugging( bool iD )
{ mIsDebugging = iD; updateUi(); }
//-----------------------------------------------------------------------------
void PartitionViewer::setAsVerbose( bool iV )
{ mIsVerbose = iV; }
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
/*La note courante ne doit pas etre inférieur a -1 */
void PartitionViewer::setCurrentNote( int iN )
{ mCurrentNote = max( iN, -1 ); updateUi(); }
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
  
  setCurrentBar( 0 );
  setCurrentNote( - 1 );
  updateUi();
}
//-----------------------------------------------------------------------------
void PartitionViewer::setLayoutOrientation( QPageLayout::Orientation iO )
{
  mLayoutOrientation = iO;
  updateLayout();
  QRect p = getRegion( rPartition );
  resize( p.width()+1, p.height()+1 );
  updateUi();
}
//-----------------------------------------------------------------------------
void PartitionViewer::setLog( Log* ipLog )
{
  if( ipLog )
  {
    ipLog->takeEntriesFrom( *mpLog );
    mpLog = ipLog;
  }
  else{ ipLog = &mDefaultLog; }
}
//-----------------------------------------------------------------------------
void PartitionViewer::setPageSize( QPageSize::PageSizeId iId )
{
  mPageSizeId = iId;
  updateLayout();
  QRect p = getRegion( rPartition );
  resize( p.width()+1, p.height()+1 );
  updateUi();
}
//-----------------------------------------------------------------------------
void PartitionViewer::setNumberOfPage( int iN )
{ mNumberOfPages = iN; }
//-----------------------------------------------------------------------------
/*Permet de separer en vecteur de note index par barre une selection iV qui
  contient des notes sur plusieurs barres.*/
map< int, vector<int> > PartitionViewer::splitPerBar(
  vector< std::pair<int, int> > iV ) const
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
  mpLineTextEdit->setText( x->getLineText(iLineIndex) );
  mEditingLineIndex = iLineIndex;
  updateLayout(); //pour afficher correctement le lineEdit
  updateUi();
}
//-----------------------------------------------------------------------------
void PartitionViewer::startTitleEdit()
{
  mpTitleEdit->setFocus();
  mpTitleEdit->setText( x->getTitle() );
  mEditingTitle = true;
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
    setBarAsDirty( 0, true );
    updateUi();
    setFocus();
  }
}
//-----------------------------------------------------------------------------
void PartitionViewer::stopTitleEdit()
{
  if( hasTitleEditionPending() )
  {
    x->setTitle( mpTitleEdit->text() );
    mEditingTitle = false;
    //on met la barre dirty pour forcer le updatePageLayout
    setBarAsDirty( 0, true );
    updateUi();
    setFocus();
  }
}

//-----------------------------------------------------------------------------
QString PartitionViewer::strokeToString( strokeType iSt ) const
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
void PartitionViewer::updateBarLayout()
{
  setNumberOfPage( 1 );
  mBarsPerPage.clear();
  
  mLayoutCursor = getPageRegion( prBody, 0 ).topLeft();
  for( int i = 0; i < x->getNumberOfBars(); ++i )
  {
    Bar& b = getBar(i);
    b.mScreenLayout = QRect();
    b.mNoteScreenLayouts.clear();
    b.mIsWayTooLong = false;
    
    int pageIndex = toPageIndex( mLayoutCursor );
    QRect pageBody = getPageRegion(  prBody, pageIndex );
    //Si cest le debut d'une ligne, on saute 1.x ligne, sauf si
    //cest la ligne 0
    if( x->isStartOfLine( i ) && x->findLine( i ) != -1)
    {
      int lineIndex = x->findLine( i );
      int verticalShift = lineIndex != 0 ? 1.05*kBarHeight : 0;
      if( !(x->getLineText( lineIndex ).isEmpty()) ||
         mEditingLineIndex == lineIndex )
      { verticalShift += QFontMetrics(mLineFont).height(); }
      mLayoutCursor.setX( pageBody.left() );
      mLayoutCursor.setY( mLayoutCursor.y() + verticalShift );
    }
    
    //--- le layout des bars
    QRect fullLayout = b.mRect;
    while( fullLayout.width() )
    {
      pageIndex = toPageIndex( mLayoutCursor );
      pageBody = getPageRegion(  prBody, pageIndex );
      
      QRect pageLayout = fullLayout;
      pageLayout.translate( mLayoutCursor );
      
      /*Le layout depasse dans le bas de la page... on ajoute une page*/
      if( !pageBody.contains( pageLayout ) &&
         !pageBody.contains( pageLayout.bottomLeft() ) )
      {
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
      //En ce moment ( 14 avril 2015 ), ce n'est pas supporté, donc on
      //coupe le layout au bout de la page et on set un flag qui mettra
      //un message a l'usage lors du rendu ( voir méthode drawBar() ).
      else if( !pageBody.contains( pageLayout ) )
      {
        //on coupe le layout au bout de la page
        fullLayout.setWidth( pageBody.width() - 1 );
        b.mIsWayTooLong = true;
      }
      else //le layout de la barre entre à l'écran sans problème
      {
        //on fait le layout des notes restantes pour cette barre
        for( int j = 0; j < x->getNumberOfNotesInBar(i); ++j )
        {
          QRect n = b.getNoteRect(j);
          n.translate( pageLayout.topLeft() );
          b.mNoteScreenLayouts.push_back( n );
        }
        
        mLayoutCursor += QPoint( fullLayout.width(), 0 );
        fullLayout.setWidth(0);
        b.mScreenLayout = pageLayout;
        
        mBarsPerPage[ pageIndex ].push_back( i );
      }
    }
  }
}
//-----------------------------------------------------------------------------
void PartitionViewer::updateLayout()
{
  //layout du titre
  QRect l = QFontMetrics( mTitleFont ).boundingRect( x->getTitle() );
  QRect titleRegion = getRegion( rTitle );
  int dx = titleRegion.center().x() - l.width() / 2 ;
  int dy = titleRegion.center().y() - l.height() / 2 ;
  mTitleScreenLayout = l;
  mTitleScreenLayout.translate( QPoint( dx, dy ) - l.topLeft() );
  
  //layout des barres spéciale (gamme, accordage tarab, etc...)
  updateSpecialBarLayout( sbScale );
  updateSpecialBarLayout( sbTarabTuning );
  
  //--- le layout des barres du sargam
  updateBarLayout();
  
  //--- le layout des ornements
  updateOrnementLayout();
  
  //--- le layout des ligne, il vient apres le layout des barres parce
  //qu'il en est dependant
  updateLineLayout();
}
//-----------------------------------------------------------------------------
void PartitionViewer::updateLineLayout()
{
  mLines.clear();
  mLines.resize( x->getNumberOfLines() );
  for( int i = 0; i < x->getNumberOfLines(); ++i )
  {
    Line& l = mLines[i];
    const Bar& b = getBar( x->getLineFirstBar(i) );
    QRect bl = b.mScreenLayout;
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
      l.mTextScreenLayout = r;
    }else { l.mTextScreenLayout = QRect(); }
    
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
/*Cette méthode est un peu tricky, parce que les ornements peuvent dépasser
  les frontières d'une barre. Puisque le systeme d'affichage est fait pour
 dessiner une barre à la fois, il faut tenir compte de quelle region de
 l'ornement sera dessiner dans la barre. 
 
 mFullOrnement: sert à définir la
   taille totale de l'ornement, c'est dans ce rectangle qu'il sera dessiner.
 mOffsets: sert a tenir le décalage à appliquer lorsqu'on dessine l'ornement
   pour une barre donnée.
 mDestination: sert à tenir la position (en x) à laquelle l'ornement doit
   être dessiner.
 */
void PartitionViewer::updateOrnementLayout()
{
  for( int iOrn = 0; iOrn < x->getNumberOfOrnements(); ++iOrn )
  {
    Ornement& o = mOrnements[iOrn];
    o.mOffsets.clear();
    o.mDestinations.clear();
    o.mFullOrnement = QRect();
    
    //update rects...
    vector<int> bi = x->getBarsInvolvedByOrnement( iOrn );
    o.mOffsets.resize( bi.size() );
    o.mDestinations.resize( bi.size() );
    int barIndex = -1;
    int widthAccum = 0;
    int height = x->getOrnementType(iOrn) == otMeend ? kMeendHeight : kKrintanHeight;
    for( int i = 0; i < bi.size(); ++i )
    {
      barIndex = bi[i];
      int left = numeric_limits<int>::max();
      int right = numeric_limits<int>::min();
      
      /*Ici, on détermine la taille (left, right) de l'ornement dans la barre
       i.*/
      for( int j = 0; j < x->getNumberOfNotesInOrnement(iOrn); ++j )
      {
        NoteLocator nl = x->getNoteLocatorFromOrnement(iOrn, j);
        /*Si les notes de l'ornements s'applique à la barre barIndex, elles
         contribueront à définir la taille de l'ornement dans la barre barIndex*/
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
      
      o.mDestinations[i] = make_pair( barIndex, left );
      o.mOffsets[i] = make_pair( barIndex, -widthAccum );
      widthAccum += right - left;
    }
    o.mFullOrnement = QRect( QPoint(0, getBarRegion( brOrnementY ) ), QSize(widthAccum, height) );
  }
}
//-----------------------------------------------------------------------------
void PartitionViewer::updateSpecialBarLayout( specialBar barIndex )
{
  Bar& b = getBar( barIndex );
  b.mScreenLayout = QRect();
  b.mNoteScreenLayouts.clear();
  QRect l = b.mRect;
  QPoint topLeft;
  switch (barIndex)
  {
    case sbScale: topLeft = getRegion( rSargamScale ).topLeft(); break;
    case sbTarabTuning: topLeft = getRegion( rTarabTuning ).topLeft(); break;
    default: break;
  }
  
  l.translate( topLeft );
  b.mScreenLayout = l;
  //on fait le layout des notes pour cette barre
  for( int j = 0; j < x->getNumberOfNotesInBar(barIndex); ++j )
  {
    QRect n = b.getNoteRect(j);
    n.translate( topLeft );
    b.mNoteScreenLayouts.push_back(n);
  }
}
//-----------------------------------------------------------------------------
void PartitionViewer::updateUi()
{
  /*
   */
  bool shouldUpdateLayout = false;
  for( int i = sbScale; i < x->getNumberOfBars(); ++i )
  {
    Bar& b = getBar(i);
    if( b.mIsDirty )
    {
      updateBar( i );
      b.mIsDirty = false;
      shouldUpdateLayout = true;
    }
  }
  if( shouldUpdateLayout ){ updateLayout(); }
  
  //placer le ui l'écran
  //titre
  if( hasTitleEditionPending() )
  {
    QRect r = getRegion( rTitle );
    QPoint c = r.center() - QPoint( mpTitleEdit->width() / 2,
      mpTitleEdit->height() / 2 );
    //voir la creation de mpTitleEdit pour explication mapping.
    c = mapToParent( c );
    mpTitleEdit->move( c );
    mpTitleEdit->show();
  }
  else{ mpTitleEdit->hide(); }
  
  //place la lineEdit pour les ligne
  if( hasLineEditionPending() )
  {
    QPoint p = mLines[mEditingLineIndex].mTextScreenLayout.topLeft();
    //voir la creation de mpLineTextEdit pour explication mapping.
    p = mapToParent(p);
    mpLineTextEdit->move( p );
    mpLineTextEdit->show();
  }
  else { mpLineTextEdit->hide(); }
  
  
  /* On redimensionne le widget pour garantir que toutes les pages seront
     affichees. */
  QRect p = getRegion( rPartition );
  resize( p.width()+1, p.height()+1 );
  
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
int PartitionViewer::Ornement::getDestination( int iBar ) const
{
  int r = 0;
  for( int i = 0; i < mDestinations.size(); ++i )
  { if( mDestinations[i].first == iBar ) { r = mDestinations[i].second; break; } }
  return r;
}
//-----------------------------------------------------------------------------
int PartitionViewer::Ornement::getOffset( int iBar ) const
{
  int r = 0;
  for( int i = 0; i < mOffsets.size(); ++i )
  { if( mOffsets[i].first == iBar ) { r = mOffsets[i].second; break; } }
  return r;
}

//-----------------------------------------------------------------------------
// --- MAIN WINDOW
//-----------------------------------------------------------------------------
MainDialog::MainDialog() : QMainWindow(),
  mpScrollArea(0),
  mpPartitionViewer(0),
  mSettings( QSettings::UserScope, "Realisim", "Sargam" ),
  mLog(),
  mIsVerbose( false )
{
  createUi();
  
  //--- init log
  QDir logDir( "./logs" );
  bool canLogToFile = true;
  if( !logDir.exists() )
  {
    canLogToFile = QDir(".").mkdir( "logs" );
  }
  if( canLogToFile )
  {
    mLog.logToFile( true, "logs/" + QDateTime::currentDateTime().toString(
      "yyyy-MM-dd_hh_mm_ss" ) + ".txt" );
  }
  else
  {
    getLog().log( "Impossible to log to file. Could not create directory "
                     "'logs'. Try creating it manually beside the executables." );
  }
  mLog.log( "Sargam started. version %d", getVersion() );
  mpPartitionViewer->setLog( &mLog );

//!!! pour le beta, on met a verbose...
setAsVerbose( true );
  
  //--- init viewer
  loadSettings();
  newFile();
  resize( mpPartitionViewer->width() + 35, 800 );
}
//-----------------------------------------------------------------------------
void MainDialog::createUi()
{
  QWidget* pMainWidget = new QWidget( this );
  setCentralWidget(pMainWidget);
  
  //--- la barre de menu
  //--- file
  QMenuBar* pMenuBar = new QMenuBar(this);
  QMenu* pFile = pMenuBar->addMenu("File");
  pFile->addAction( QString("&New"), this, SLOT( newFile() ),
                   QKeySequence::New );
  
  pFile->addAction( QString("&Open..."), this, SLOT( openFile() ),
                   QKeySequence::Open );
  pFile->addAction( QString("&Save"), this, SLOT( save() ),
                   QKeySequence::Save );
  pFile->addAction( QString("Save As..."), this, SLOT( saveAs() ),
                   QKeySequence::SaveAs );
  pFile->addSeparator();
  pFile->addAction( QString("Print..."), this, SLOT( print() ),
                   QKeySequence::Print );
  pFile->addAction( QString("Print preview..."), this, SLOT( printPreview() ) );
  
  //--- view
  QMenu* pPreferences = pMenuBar->addMenu("pPreferences");
  pPreferences->addAction( "Options...", this, SLOT( preferences() ) );
  
  //debug action
  QShortcut* pRandomPart = new QShortcut( QKeySequence( Qt::CTRL + Qt::SHIFT + Qt::Key_G), this );
  connect( pRandomPart, SIGNAL(activated()), this, SLOT(generateRandomPartition()) );
  
  QShortcut* pDebugS = new QShortcut( QKeySequence( Qt::CTRL + Qt::SHIFT + Qt::Key_D), this );
  connect( pDebugS, SIGNAL(activated()), this, SLOT(toggleDebugging()) );
  
  setMenuBar( pMenuBar );
  
  //--- le reste du Ui
  QHBoxLayout* pLyt = new QHBoxLayout(pMainWidget);
  pLyt->setMargin(0);
  pLyt->setSpacing(0);
  
  mpScrollArea = new QScrollArea( pMainWidget );
  mpScrollArea->setAlignment( Qt::AlignHCenter );
  {
    mpPartitionViewer = new PartitionViewer( pMainWidget );
    mpPartitionViewer->setAsDebugging( true );
    mpPartitionViewer->setFocus();
    connect( mpPartitionViewer, SIGNAL( ensureVisible(QPoint) ),
            this, SLOT( ensureVisible(QPoint) ) );
    
    mpScrollArea->setWidget( mpPartitionViewer );
  }
  pLyt->addWidget( mpScrollArea );
}
//-----------------------------------------------------------------------------
void MainDialog::ensureVisible( QPoint p )
{ mpScrollArea->ensureVisible( p.x(), p.y() ); }
//-----------------------------------------------------------------------------
void MainDialog::fillPageSizeCombo( QComboBox* ipC )
{
  mAvailablePageSizeIds.clear();
  //ajout de letter par defaut...
  mAvailablePageSizeIds.push_back( QPageSize::Letter );
  
  QList<QPrinterInfo> pis = QPrinterInfo::availablePrinters();
  for( int i = 0; i < pis.size(); ++i )
  {
    QList<QPageSize> lps = pis.at(i).supportedPageSizes();
    for( int j = 0; j < lps.size(); ++j )
    { mAvailablePageSizeIds.push_back( lps.at(j).id() ); }
  }
  
  sort( mAvailablePageSizeIds.begin(), mAvailablePageSizeIds.end() );
  mAvailablePageSizeIds.erase( unique( mAvailablePageSizeIds.begin(),
    mAvailablePageSizeIds.end() ), mAvailablePageSizeIds.end() );
  
  for( int i = 0; i < mAvailablePageSizeIds.size(); ++i )
  { ipC->insertItem(i, QPageSize::name( mAvailablePageSizeIds[i] ) ); }
}
//-----------------------------------------------------------------------------
void MainDialog::generatePrintPreview( QPrinter* iP )
{ mpPartitionViewer->print( iP ); }
//-----------------------------------------------------------------------------
void MainDialog::generateRandomPartition()
{
  mpPartitionViewer->generateRandomPartition();
  
  if( isVerbose() )
  { getLog().log( "MainDialog: random partition generated." ); }
}
//-----------------------------------------------------------------------------
bool MainDialog::isVerbose() const
{ return mIsVerbose; }
//-----------------------------------------------------------------------------
void MainDialog::loadSettings()
{
	mLastSavePath = mSettings.value( "lastSavePath" ).toString();
//  mAddTexturePath = mSettings.value( "addTexturePath" ).toString();
//  mOpenCatalogPath = mSettings.value( "openCatalogPath" ).toString();
//  mRefreshTexturePath = mSettings.value( "refreshTexturePath" ).toString();
  if( isVerbose() )
  { getLog().log( "MainDialog: settings loaded." ); }
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
  
  if( isVerbose() )
  { getLog().log( "MainDialog: new file." ); }
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
    
    if( mComposition.hasError() )
    {
      getLog().log( "Errors while opening file %s: %s.", s.toStdString().c_str(),
        mComposition.getAndClearLastErrors().toStdString().c_str() );
    }
  }
  
  if( isVerbose() )
  { getLog().log( "MainDialog: file %s opened.", s.toStdString().c_str() ); }
}
//-----------------------------------------------------------------------------
void MainDialog::preferences()
{
  QDialog d( this );
  QComboBox* pPageSizeCombo = new QComboBox(&d);
  QButtonGroup* pOrientation = new QButtonGroup( &d );
  
  QVBoxLayout* pMainLyt = new QVBoxLayout( &d );
  pMainLyt->setMargin(2); pMainLyt->setSpacing(2);
  {
    //--- taille du papier
    QHBoxLayout* pPaperLyt = new QHBoxLayout();
    {
      pPaperLyt->setMargin(2); pPaperLyt->setSpacing(2);
      QLabel* pLabel = new QLabel( "Page size: ", &d );
      
      fillPageSizeCombo( pPageSizeCombo );
      int currrentIndex = distance( mAvailablePageSizeIds.begin(),
            std::find( mAvailablePageSizeIds.begin(), mAvailablePageSizeIds.end(),
              mpPartitionViewer->getPageSizeId() ) );
      pPageSizeCombo->setCurrentIndex( currrentIndex );
      
      pPaperLyt->addWidget(pLabel);
      pPaperLyt->addWidget(pPageSizeCombo);
    }
    
    //--- Portrait, landscape
    QHBoxLayout* pOrientationLyt = new QHBoxLayout();
    {
      QCheckBox* pPortrait = new QCheckBox( "Portrait", &d );
      QCheckBox* pLandscape = new QCheckBox( "Landscape", &d );
      pOrientation->addButton( pPortrait, 0 );
      pOrientation->addButton( pLandscape, 1 );
      
      if( mpPartitionViewer->getLayoutOrientation() == QPageLayout::Portrait )
      { pPortrait->setCheckState( Qt::Checked ); }
      else{ pLandscape->setCheckState( Qt::Checked ); }
      
      pOrientationLyt->addStretch(1);
      pOrientationLyt->addWidget(pPortrait);
      pOrientationLyt->addWidget(pLandscape);
    }
    
    //--- ok, Cancel
    QHBoxLayout* pBottomButLyt = new QHBoxLayout();
    pBottomButLyt->setMargin(2); pBottomButLyt->setSpacing(2);
    {
      QPushButton* pOk = new QPushButton( "Ok", &d );
      connect( pOk, SIGNAL( clicked() ), &d, SLOT( accept() ) );
      
      QPushButton* pCancel = new QPushButton( "Cancel", &d );
      connect( pCancel, SIGNAL( clicked() ), &d, SLOT( reject() ) );
      
      pBottomButLyt->addStretch(1);
      pBottomButLyt->addWidget(pOk);
      pBottomButLyt->addWidget(pCancel);
    }
    pMainLyt->addLayout( pPaperLyt );
    pMainLyt->addLayout( pOrientationLyt );
    pMainLyt->addStretch(1);
    pMainLyt->addLayout( pBottomButLyt );
  }
  
  if (d.exec() == QDialog::Accepted)
  {
    mpPartitionViewer->setPageSize( mAvailablePageSizeIds[ pPageSizeCombo->currentIndex() ] );
    
    QPageLayout::Orientation o;
    if( pOrientation->checkedId() == 0 ){ o = QPageLayout::Portrait; }
    else{ o = QPageLayout::Landscape; }
    mpPartitionViewer->setLayoutOrientation( o );
  }
  
  if( isVerbose() )
  { getLog().log( "MainDialog: view options closed." ); }
}
//-----------------------------------------------------------------------------
void MainDialog::print()
{
  QPrinter p;
  //appliquer les configurations de visualisation a l'imprimante
  QPrintDialog d( &p, this );
  if (d.exec() == QDialog::Accepted)
  { mpPartitionViewer->print( &p ); }
  
  if( isVerbose() )
  { getLog().log( "MainDialog: print."); }
}
//-----------------------------------------------------------------------------
void MainDialog::printPreview()
{
  QPrinter p;
  //appliquer les configurations de visualisation a l'imprimante
  QPrintPreviewDialog d( &p, this );
  connect( &d, SIGNAL( paintRequested(QPrinter*) ),
          this, SLOT( generatePrintPreview(QPrinter*) ) );
  if (d.exec() == QDialog::Accepted)
  { mpPartitionViewer->print( &p ); }
  disconnect(&d, SIGNAL( paintRequested(QPrinter*) ),
             this, SLOT( generatePrintPreview(QPrinter*) ) );
  
  if( isVerbose() )
  { getLog().log( "MainDialog: print preview."); }
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
  
  if( isVerbose() )
  { getLog().log( "MainDialog: saved." ); }
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
  
  if( isVerbose() )
  { getLog().log( "MainDialog: save as %s.", mSaveFileName.toStdString().c_str() ); }
}
//-----------------------------------------------------------------------------
void MainDialog::saveSettings()
{
	mSettings.setValue( "lastSavePath", mLastSavePath );
//	mSettings.setValue( "addTexturePath", mAddTexturePath );
//  mSettings.setValue( "openCatalogPath", mOpenCatalogPath );
//  mSettings.setValue( "refreshTexturePath", mRefreshTexturePath );
  if( isVerbose() )
  { getLog().log( "MainDialog: save settings." ); }
}
//-----------------------------------------------------------------------------
void MainDialog::setAsVerbose( bool iV )
{
  mIsVerbose = iV;
  mpPartitionViewer->setAsVerbose( iV );
  
  getLog().log( "verbose set to: %s", iV?"true":"false" );
}
//-----------------------------------------------------------------------------
void MainDialog::toggleDebugging()
{
  mpPartitionViewer->setAsDebugging( !mpPartitionViewer->isDebugging() );
  
  if( isVerbose() )
  { getLog().log( "MainDialog: debugging toggled to %s.", mpPartitionViewer->isDebugging()?"true":"false" ); }
}
//-----------------------------------------------------------------------------
void MainDialog::updateUi()
{
  
}

