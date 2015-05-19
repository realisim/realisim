	/* */

#include "MainDialog.h"
#include "PartitionViewer.h"
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
  using namespace utils;
  using namespace sargam;

namespace
{
  //--- pour PartitionViewer
  const int kSpacing = 5;
  //page
  const int kInterPageGap = 15;
  const double kPageMarginInCm = 1.5;
  const int kPageFooter = 15;
  //bar
  const int kBarHeight = 60;
  const int kInterNoteGap = 5;
  const int kMeendHeight = 10;
  const int kKrintanHeight = 6;
  const int kGamakHeight = 4;
  const int kBeatGroupHeight = 10;
  const int kNoBarIndex = -5;
  
  const QString kSargamScaleLabel("Scale: ");
  const QString kTarabTuningLabel("Tarab tuning: ");
}

realisim::sargam::Composition PartitionViewer::mDummyComposition;
PartitionViewer::Bar PartitionViewer::mDummyBar;

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
  mEditingBarText( -1 ),
  mEditingLineIndex( -1 ),
  mEditingTitle( false ),
  mAddLineTextHover( -1 ),
  mBarHoverIndex( kNoBarIndex ),
  mBarTextHover( -1 ),
  x( &mDummyComposition ),
  mDefaultLog(),
  mpLog( &mDefaultLog ),
  mIsVerbose( false ),
  mHasLogTiming( false )
{
  setMouseTracking( true );
  setFocusPolicy( Qt::StrongFocus );
  srand( time(NULL) );
  
  QString fontFamily( "Arial" );

  /*Mac et windows ne font pas le rendu des police de la meme facon...
  pour faire une histoire courte; Sous mac, le dpi est de 72 et sous windows de
  96. Donc une police de 12 points aurait 12/72 de pouce (donc 1/6 de pouce) de
  haut et à l'écran aurait 12 pixels sous mac et 16 pixels sous windows. C'est
  pourquoi on change les tailles de polices...

  details: http://www.rfwilmut.clara.net/about/fonts.html
  */

#ifdef _WIN32
  int titleSize = 24;
  int barFontSize = 11;
  int barTextSize = 8;
  int graceNoteSize = 8;
  int lineFontSize = 10;
  int strokeFontSize = 8;
#endif

#ifdef __APPLE__
  int titleSize = 24;
  int barFontSize = 14;
  int barTextSize = 10;
  int graceNoteSize = 10;
  int lineFontSize = 12;
  int strokeFontSize = 10;
#endif

  mTitleFont = QFont( fontFamily, titleSize );
  mTitleFont.setBold( true );
  mBarFont = QFont( fontFamily, barFontSize );
  mBarTextFont = QFont( fontFamily, barTextSize );
  mGraceNotesFont = QFont( fontFamily, graceNoteSize );
  mLineFont = QFont( fontFamily, lineFontSize );
  mStrokeFont = QFont( fontFamily, strokeFontSize );
  
  createUi();
  addPage();
  
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
/*Ajoute une barre apres la barre courante, les notes suivant le curseur sont
  ajoutées à la nouvelle barre*/
void PartitionViewer::commandAddBar()
{
  /*Cette commande ne peut pas etre executée sur les barres speciales*/
  if( getCurrentBar() < 0 ){ return; }
  
  // ajout de la barre
  int cb = getCurrentBar();
  addBar( cb );
  clearSelection();
  
  //déplacement des notes dans la nouvelle barre. Le + 1 dans la boucle est pour
  //déplacer les notes qui suivent le curseur.
  vector<NoteLocator> vn;
  int moveFromIndex = getCurrentNote() + 1;
  for( int i = moveFromIndex; i < x->getNumberOfNotesInBar(cb); ++i )
  {
    vn.push_back( NoteLocator( cb, i) );
    x->addNote( cb+1, x->getNote(cb, i) );
  }
  
  //deplace les matras
  moveMatraForward( cb, moveFromIndex );
  //deplace les ornements
  moveOrnementForward( cb, moveFromIndex );
  //déplace les strokes
  moveStrokeForward( cb, moveFromIndex );
  //déplace note de grace
  moveGraceNoteForward(cb, moveFromIndex);
  
  //efface le note de la barre courrante
  for( int i = vn.size() - 1; i >= 0; --i )
  { x->eraseNote( vn[i].getBar(), vn[i].getIndex() ); }
  
  setBarAsDirty( cb, true );
  setBarAsDirty( cb + 1, true );
  setCurrentNote( -1 );
  setCurrentBar( cb + 1 );
  updateUi();
  
  if( isVerbose() )
  { getLog().log( "PartitionViewer: commandAddBar." ); }
}
//-----------------------------------------------------------------------------
//void PartitionViewer::commandAddPreviousBar()
//{
//  /*Cette commande ne peut pas etre executée sur les barres speciales*/
//  if( getCurrentBar() < 0 ){ return; }
//  
//  if( x->isStartOfLine( getCurrentBar() ) )
//  {
//    int cl = x->findLine( getCurrentBar() );
//    QString t = x->getLineText( cl );
//    x->eraseLine( cl );
//    addBar( getCurrentBar() - 1 );
//    x->addLine( getCurrentBar(), t );
//  }
//  else
//  { addBar( getCurrentBar() - 1 ); }
//  
//  setCurrentNote( -1 );
//  setCurrentBar( getCurrentBar() );
//  clearSelection();
//  setBarAsDirty( getCurrentBar(), true );
//  updateUi();
//  
//  if( isVerbose() )
//  { getLog().log( "PartitionViewer: commandAddPreviousBar." ); }
//}
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
  
  commandAddBar();
  x->addLine( getCurrentBar() );
  setCurrentNote( -1 );
  setBarAsDirty( getCurrentBar(), true );
  updateUi();
  
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
  updateUi();
  
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
  updateUi();
  
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
  updateUi();
  
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
  updateUi();
  
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
  updateUi();
  
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
    if( getCurrentNote() != -1 )
    {
      x->eraseNote( cb, getCurrentNote() );
      setCurrentNote( getCurrentNote() - 1 );
    }
    else
    {
      /*On efface la ligne si la note courante est -1 et que la barre est
       un début de ligne. Par contre, on ne peut pas effacer la ligne 0.*/
      if( x->isStartOfLine( cb ) && x->findLine( cb ) != 0 )
      { x->eraseLine( x->findLine( cb ) ); }
      /*On ne peut pas effacer la derniere barre...*/
      if( x->getNumberOfBars() > 1 )
      {
        int i = x->getNumberOfNotesInBar( cb - 1 ) - 1;
        eraseBar( cb );
        setCurrentBar( cb - 1 );
        setCurrentNote( i );
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
  updateUi();
  
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
void PartitionViewer::commandRemoveStroke()
{
  /*Cette commande ne peut pas etre executée sur les barres speciales*/
  if( getCurrentBar() < 0 ){ return; }
  
  vector< pair<int, int> > l;
  if( hasSelection() )
  { l = mSelectedNotes; }
  else
  { l.push_back( make_pair( getCurrentBar(), getCurrentNote()) ); }
  
  for( int i = 0; i < l.size(); ++i )
  {
    int bar = l[i].first;
    int index = l[i].second;
    int strokeIndex = x->findStroke( bar, index );
    if( strokeIndex != -1 )
    {
      x->eraseStroke( bar, strokeIndex );
      setBarAsDirty( bar, true );
    }
  }
  updateUi();
  
  if( isVerbose() )
  { getLog().log( "PartitionViewer: commandRemoveStroke." ); }
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
  updateUi();
  
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
  
  //ligne pour le texte des barres
  mpBarTextEdit = new QLineEdit( this->parentWidget() );
  mpBarTextEdit->setFont( mBarTextFont );
  mpBarTextEdit->hide();
  connect( mpBarTextEdit, SIGNAL( editingFinished() ),
          this, SLOT( stopBarTextEdit() ) );
  connect( mpBarTextEdit, SIGNAL( textChanged(const QString&)),
          this, SLOT( resizeLineEditToContent() ) );
}
//-----------------------------------------------------------------------------
void PartitionViewer::draw( QPaintDevice* iPaintDevice ) const
{
  QElapsedTimer _timer;
  _timer.start();
  
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
  { drawBarContour( &p, mBarHoverIndex, getColor( cHover ) ); }
  
  //on dessine le contour de la barre courante
  drawBarContour( &p, getCurrentBar(), getColor( cSelection ) );
  
  //on dessine le bar text Hover.
  if( mBarTextHover != -1 )
  {
    p.save();
    QPen pen = p.pen();
    pen.setStyle( Qt::DashLine );
    pen.setColor( getColor( cHover ) );
    p.setPen(pen);
    p.drawRoundedRect( getBar( mBarTextHover ).mTextScreenLayout, 2, 2 );
    p.restore();
  }
  
  drawPageFooters( &p );
  
  if( hasLogTiming() )
  { getLog().log("PartitionViewer::draw: %.3f ms", _timer.nsecsElapsed() / 1000000.0 ); }
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
        ornementType ot = x->getOrnementType(i);
        switch (ot)
        {
          case otMeend:
            { iP->drawArc( r, 10 * 16, 170 * 16 ); } break;
          case otKrintan:
          {
            iP->drawLine( r.bottomLeft(), r.topLeft() );
            iP->drawLine( r.topLeft(), r.topRight() );
            iP->drawLine( r.topRight(), r.bottomRight() );
          } break;
          case otGamak:
          {
            iP->save();
            iP->translate( r.topLeft() );
            QFontMetrics fm(mBarFont);
            QPainterPath pp;
            //20 point par 2*Pi.
            //3*Pi par 2 notes -> 1.5Pi par notes
            //combien de pi à faire...
            const double numPointsPer2Pi = 20.0;
            const double pi = 3.1415629;
            const float nbPi = r.width() * 1.5 / fm.width("S ");
            const double numIter = nbPi * numPointsPer2Pi / 2.0;
            const double incPerIter = 2*pi / numPointsPer2Pi;
            double eval = 0;
            for( double i = 0; i < numIter; i ++, eval += incPerIter )
            { pp.lineTo( i / numIter * r.width(), -sin(eval)/2 * r.height() ); }
            iP->drawPath( pp );
            iP->restore();
          } break;
          default:break;
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
    
    //render text
    iP->setFont(mBarTextFont);
    {
      iP->drawText( b.mTextRect, Qt::AlignCenter, x->getBarText(iBar) );
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
  if( iBarIndex >= sbScale && hasFocus() )
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
    pen.setColor( getColor( cSelection ) );
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
/*Efface la barre iBar des donnees et des donnees d'affichage. Si la barre 
  contient des notes, ces notes sont transférées à la barre précédente*/
void PartitionViewer::eraseBar( int iBar )
{
  int moveToIndex = x->getNumberOfNotesInBar( iBar - 1 );
  //transfer des notes dans la barre précédente
  for( int i = 0; i < x->getNumberOfNotesInBar( iBar ); ++i )
  { x->addNote(iBar - 1, x->getNote( iBar, i ) ); }
  
  //transfert des matra
  moveMatraBackward(iBar, moveToIndex);
  //transfert des ornements
  moveOrnementBackward(iBar, moveToIndex);
  //transfert des strokes
  moveStrokeBackward(iBar, moveToIndex);
  //transfert des notes de grace
  moveGraceNoteBackward(iBar, moveToIndex);
  
  //efface des données d'affichage
  if( iBar >= 0 && iBar < x->getNumberOfBars() )
  {
    mBars.erase( mBars.begin() + iBar );
    if( x->getNumberOfBars() > 0 ){ setBarAsDirty(0, true); }
  }
  //efface les données
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
  const Bar* r = &mDummyBar;

  if( iBar >= sbScale && iBar < (int)mBars.size() )
  {
     switch( iBar )
     {
       case sbScale: r = &mScale; break;
       case sbTarabTuning: r = &mTarabTuning; break;
       default: r = &mBars[iBar]; break;
     }
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
QColor PartitionViewer::getColor( colors iC ) const
{
  QColor r;
  switch( iC )
  {
    case cHover: r = QColor( 125, 125, 125, 120 ); break;
    case cSelection: r = QColor( 0, 10, 210, 120 ); break;
    default: break;
  }
  
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
    case brOrnementY: r = getBarRegion(brGraceNoteTopY) - 5; break;
    case brMatraGroupY: r = getBarRegion(brNoteBottomY) - 4; break;
    case brGraceNoteTopY:
    {
      QFontMetrics fm(mGraceNotesFont);
      r = getBarRegion( brNoteTopY ) - fm.height() / 4;
    } break;
    case brTextX: r = getBarRegion(brNoteStartX); break;
    case brTextY: r = 2; break;
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
    r = 1;
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
int PartitionViewer::getNumberOfSelectedNote() const
{ return mSelectedNotes.size(); }
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
NoteLocator PartitionViewer::getSelectedNote( int i ) const
{ return NoteLocator( mSelectedNotes[i].first, mSelectedNotes[i].second ); }
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
bool PartitionViewer::hasBarTextEditionPending() const
{ return mEditingBarText != -1; }
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
    case Qt::Key_C: commandAddNote( nvChik ); break;
    case Qt::Key_B: //Bend - meend
      if( (ipE->modifiers() & Qt::ShiftModifier) )
      { commandBreakOrnementsFromSelection(); }
      else{ commandAddOrnement( otMeend ); }
      break;
    case Qt::Key_E:
      if( (ipE->modifiers() & Qt::ShiftModifier) )
      { commandRemoveStroke(); }
      else{ commandAddStroke( stDiri );}
      break;
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
    case Qt::Key_N: //gamak
      if( (ipE->modifiers() & Qt::ShiftModifier) )
      { commandBreakOrnementsFromSelection(); }
      else{ commandAddOrnement( otGamak ); }
      break;
    case Qt::Key_Q:
      if( (ipE->modifiers() & Qt::ShiftModifier) )
      { commandRemoveStroke(); }
      else{ commandAddStroke( stDa ); }
      break;
    case Qt::Key_R: commandAddNote( nvRest );break;
    case Qt::Key_S: commandShiftNote();break;
    case Qt::Key_W:
      if( (ipE->modifiers() & Qt::ShiftModifier) )
      { commandRemoveStroke(); }
      else{ commandAddStroke( stRa );}
      break;
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
    case Qt::Key_Space: commandAddBar(); break;
    case Qt::Key_Backspace: commandErase(); break;
    case Qt::Key_Return: commandAddLine(); break;
    case Qt::Key_Plus: commandIncreaseOctave(); break;
    case Qt::Key_Minus: commandDecreaseOctave(); break;
    case Qt::Key_Shift: break;
    default: break;
  }
  updateUi();
  emit ensureVisible( getCursorLine().p2() );
  emit interactionOccured();
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
  QElapsedTimer _timer;
  _timer.start();
  
  QPoint pos = ipE->pos();
  Qt::CursorShape cs = Qt::ArrowCursor;
  bool shouldUpdate = false;
  
  //intersection avec le titre
  if( mTitleScreenLayout.contains( pos ) )
  { cs = Qt::IBeamCursor; }
  
  //intersection avec le text de la barre courante
  const Bar& b = getBar( getCurrentBar() );
  if( b.mTextScreenLayout.contains( pos ) )
  {
    shouldUpdate |= mBarTextHover != getCurrentBar();
    mBarTextHover = getCurrentBar();
    if( x->hasBarText( getCurrentBar() ) )
    { cs = Qt::IBeamCursor; }
  }
  else if( mBarTextHover != -1 )
  {
    mBarTextHover = -1;
    shouldUpdate |= true;
  }

  //intersection avec les barres
  for( int i = sbScale; i < x->getNumberOfBars(); ++i )
  {
    const Bar& b = getBar(i);
    if( b.mScreenLayout.contains( pos ) )
    {
      shouldUpdate |= mBarHoverIndex != i;
      mBarHoverIndex = i;
    }
    else if ( mBarHoverIndex == i )
    {
      mBarHoverIndex = kNoBarIndex;
      shouldUpdate |= true; }
  }
  
  //intersection avec les lignes
  for( int i = 0; i < x->getNumberOfLines(); ++i )
  {
    if( mLines[i].mHotSpot.contains( pos ) )
    {
      shouldUpdate |= mAddLineTextHover != i;
      mAddLineTextHover = i;
    }
    else if(mAddLineTextHover == i)
    {
      mAddLineTextHover = -1;
      shouldUpdate |= true;
    }
      
    if( mLines[i].mTextScreenLayout.contains( pos ) )
    { cs = Qt::IBeamCursor; }
  }
  
  if( shouldUpdate ) {updateUi();}
  setCursor( cs );
  
  if( hasLogTiming() )
  {
    getLog().log("PartitionViewer::mouseMove: %.3f ms", _timer.nsecsElapsed() / 1000000.0 );
  }
}
//-----------------------------------------------------------------------------
void PartitionViewer::mouseReleaseEvent( QMouseEvent* ipE )
{
  QPoint pos = ipE->pos();
  
  //intersection avec le titre
  if( mTitleScreenLayout.contains( pos ) )
  { startTitleEdit(); }
  
  //intersection avec le text de la barre courante
  const Bar& b = getBar( getCurrentBar() );
  if( b.mTextScreenLayout.contains( pos ) )
  {
    mBarTextHover = -1;
    startBarTextEdit( getCurrentBar() );
  }
  
  //intersection avec les barres
  if( mBarHoverIndex != kNoBarIndex )
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
  
  emit interactionOccured();
}
//-----------------------------------------------------------------------------
void PartitionViewer::moveGraceNoteBackward(int iFromBar, int iToIndex)
{
  for( int i = 0; i < x->getNumberOfGraceNotesInBar(iFromBar); ++i )
  {
    int ni = x->getNoteIndexFromGraceNote(iFromBar, i) + iToIndex;
    { addNoteToSelection(iFromBar-1, ni); }
    commandAddGraceNotes();
    clearSelection();
  }
}
//-----------------------------------------------------------------------------
void PartitionViewer::moveGraceNoteForward(int iFromBar, int iFromIndex)
{
  for( int i = 0; i < x->getNumberOfGraceNotesInBar(iFromBar); ++i )
  {
    int ni = x->getNoteIndexFromGraceNote(iFromBar, i) - iFromIndex;
    if( ni >= 0 )
    { addNoteToSelection(iFromBar+1, ni); }
    commandAddGraceNotes();
    clearSelection();
  }
}
//-----------------------------------------------------------------------------
void PartitionViewer::moveMatraBackward(int iFromBar, int iToIndex)
{
  for( int i = 0; i < x->getNumberOfMatraInBar(iFromBar); ++i )
  {
    for( int j = 0; j < x->getNumberOfNotesInMatra(iFromBar, i); ++j )
    {
      int ni = x->getNoteIndexFromMatra(iFromBar, i, j) + iToIndex;
      { addNoteToSelection(iFromBar - 1, ni ); }
    }
    commandAddMatra();
    clearSelection();
  }
}
//-----------------------------------------------------------------------------
void PartitionViewer::moveMatraForward(int iFromBar, int iFromIndex)
{
  for( int i = 0; i < x->getNumberOfMatraInBar(iFromBar); ++i )
  {
    for( int j = 0; j < x->getNumberOfNotesInMatra(iFromBar, i); ++j )
    {
      int ni = x->getNoteIndexFromMatra(iFromBar, i, j) - iFromIndex;
      if( ni >= 0 )
      { addNoteToSelection(iFromBar+1, ni ); }
    }
    commandAddMatra();
    clearSelection();
  }
}
//-----------------------------------------------------------------------------
void PartitionViewer::moveOrnementBackward(int iFromBar, int iToIndex)
{
  /*ramasse toutes les notes de la bar iFromBar*/
  vector<NoteLocator> vn;
  for( int i = 0; i < x->getNumberOfNotesInBar(iFromBar); ++i )
  { vn.push_back( NoteLocator( iFromBar, i) ); }
  
  //ramasse tous les ornements à déplacer
  set<int> ornToMove;
  for( int i = 0; i < vn.size(); ++i )
  {
    int ornIndex = x->findOrnement( vn[i].getBar(), vn[i].getIndex() );
    if( ornIndex >= 0 ) { ornToMove.insert( ornIndex ); }
  }
  
  /*pour chaque ornement, si la barre du noteLocator est égale à iFromBar
    on décremente la barre puisqu'on veut déplacer la note dans la barre
   précédente et on ajoute ce NoteLocator à la selection. Il faut imaginer
   cette opération comme si on la faisait à bras. On commencerait par mettre
   les notes de la barre courante dans la barre précedente, ensuite on
   selectionnerait le note pour refaire le même ornement et ensuite on
   effacerait la barre voulue.*/
  set<int>::iterator it = ornToMove.begin();
  for( ; it != ornToMove.end(); ++it )
  {
    ornementType ot = x->getOrnementType( *it );
    for(int i = 0; i < x->getNumberOfNotesInOrnement( *it ); ++i )
    {
      NoteLocator nl = x->getNoteLocatorFromOrnement( *it, i);
      int bar = nl.getBar();
      int index = nl.getIndex();
      if( bar == iFromBar )
      { bar--; index = index + iToIndex; }
      addNoteToSelection(bar, index);
    }
    commandAddOrnement(ot);
    clearSelection();
  }
}
//-----------------------------------------------------------------------------
void PartitionViewer::moveOrnementForward(int iFromBar, int iFromIndex)
{
  vector<NoteLocator> vn;
  for( int i = iFromIndex; i < x->getNumberOfNotesInBar(iFromBar); ++i )
  { vn.push_back( NoteLocator( iFromBar, i) ); }
  
  set<int> ornToMove;
  for( int i = 0; i < vn.size(); ++i )
  {
    int ornIndex = x->findOrnement( vn[i].getBar(), vn[i].getIndex() );
    if( ornIndex >= 0 ) { ornToMove.insert( ornIndex ); }
  }
  set<int>::iterator it = ornToMove.begin();
  for( ; it != ornToMove.end(); ++it )
  {
    ornementType ot = x->getOrnementType( *it );
    for(int i = 0; i < x->getNumberOfNotesInOrnement( *it ); ++i )
    {
      NoteLocator nl = x->getNoteLocatorFromOrnement( *it, i);
      int bar = nl.getBar();
      int index = nl.getIndex();
      if( bar == iFromBar && index >= iFromIndex )
      {
        bar++;
        index -= iFromIndex;
      }
      addNoteToSelection(bar, index);
    }
    commandAddOrnement(ot);
    clearSelection();
  }
}
//-----------------------------------------------------------------------------
void PartitionViewer::moveStrokeBackward(int iFromBar, int iToIndex)
{
  for( int i = 0; i < x->getNumberOfStrokesInBar(iFromBar); ++i )
  {
    strokeType st = x->getStrokeType(iFromBar, i);
    for( int j = 0; j < x->getNumberOfNotesInStroke(iFromBar, i); ++j )
    {
      int ni = x->getNoteIndexFromStroke(iFromBar, i, j) + iToIndex;
      { addNoteToSelection(iFromBar - 1, ni ); }
    }
    if( hasSelection() )
    {
      commandAddStroke(st);
      clearSelection();
    }
  }
}
//-----------------------------------------------------------------------------
void PartitionViewer::moveStrokeForward(int iFromBar, int iFromIndex)
{
  for( int i = 0; i < x->getNumberOfStrokesInBar(iFromBar); ++i )
  {
    strokeType st = x->getStrokeType(iFromBar, i);
    for( int j = 0; j < x->getNumberOfNotesInStroke(iFromBar, i); ++j )
    {
      int ni = x->getNoteIndexFromStroke(iFromBar, i, j) - iFromIndex;
      if( ni >= 0 )
      { addNoteToSelection(iFromBar+1, ni ); }
    }
    if( hasSelection() )
    {
      commandAddStroke(st);
      clearSelection();
    }
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
    case nvChik: r = "\xE2\x9c\x93"; break; //check
    case nvRest: r = "\xE2\x80\x94"; break; //barre horizontale
    case nvComma: r = ","; break;
    default: break;
  }
  
  //ajout de la notation octave
  if( note != nvChik && note != nvRest && note != nvComma )
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
    p.setBackgroundMode( Qt::OpaqueMode );
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
      //layout du text
      p.drawRect( b.mTextScreenLayout );
      
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
void PartitionViewer::startBarTextEdit( int iBar )
{
  mpBarTextEdit->setFocus();
  mpBarTextEdit->setText( x->getBarText( iBar ) );
  mEditingBarText = iBar;
  updateUi();
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
void PartitionViewer::stopBarTextEdit()
{
  if( hasBarTextEditionPending() )
  {
    x->setBarText( mEditingBarText, mpBarTextEdit->text() );
    setBarAsDirty( mEditingBarText, true );
    mEditingBarText = -1;
    updateUi();
    setFocus();
  }
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
  int r = -1;
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
  
  //--- le text
  fm = QFontMetrics( mBarTextFont );
  QRect rect = fm.boundingRect( "+" );
  int w = max( rect.width(), rect.height() );
  rect.setTopLeft( QPoint(0, 0) );
  rect.setSize( QSize(w, w) );
  if( !x->getBarText(iBar).isEmpty() )
  { rect.setSize( fm.boundingRect( x->getBarText( iBar ) ).size() ); }
  rect.translate( getBarRegion( brTextX ), getBarRegion( brTextY ) );
  b.mTextRect = rect;
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
        
        /*Il faut mettre le -1 sur le layout.width() parce que width()
         retourne 1 de plus que la coordonnée bottomRight. Donc, un point
         qui peut etre a l'extérieur de pageBody...*/
        mLayoutCursor += QPoint( fullLayout.width() - 1, 0 );
        fullLayout.setWidth(0);
        b.mScreenLayout = pageLayout;
        
        //textScreenLayout.
        b.mTextScreenLayout = b.mTextRect;
        b.mTextScreenLayout.translate( b.mScreenLayout.topLeft() );
        
        mBarsPerPage[ pageIndex ].push_back( i );
      }
    }
  }
}
//-----------------------------------------------------------------------------
void PartitionViewer::updateLayout()
{
  QElapsedTimer _timer;
  _timer.start();
  
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
  
  if( hasLogTiming() )
  { getLog().log("PartitionViewer::updateLayout: %.3f ms", _timer.nsecsElapsed() / 1000000.0 ); }
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
    int height = 10;
    switch (x->getOrnementType(iOrn) )
    {
      case otMeend: height = kMeendHeight; break;
      case otKrintan: height = kKrintanHeight; break;
      case otGamak: height = kGamakHeight; break;
      default: break;
    }
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
  
  //place le line edit pour les barres
  if( hasBarTextEditionPending() )
  {
    QPoint p = getBar(mEditingBarText).mTextScreenLayout.topLeft();
    //voir la creation de mpLineTextEdit pour explication mapping.
    p = mapToParent(p);
    mpBarTextEdit->move( p );
    mpBarTextEdit->show();
  }else { mpBarTextEdit->hide(); }
  
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
