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

namespace
{
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
  //mIsTechniqueGroupOpen( false ),
  //mIsBeatGroupOpen( false ),
  mOctave( 0 ),
  mIsSelectionOpen( false )
{
  setFocusPolicy( Qt::StrongFocus );
  srand( time(NULL) );
  mTitleFont = QFont( "Arial", 24 );
  mTitleFont.setBold( true );
  mBarFont = QFont( "Arial", 14 );

  addPage();
  addBar();
  setCurrentBar( 0 );
  //generateRandomPartition();
}

PartitionViewer::~PartitionViewer()
{}
//-----------------------------------------------------------------------------
void PartitionViewer::addBeatGroup()
{
  int bar = -1;
  vector< int > bg;
  for( int i = 0; i < mNotesSelected.size(); ++i )
  {
    bar = mNotesSelected[i].first;
    bg.push_back( mNotesSelected[i].second );
  }
  mBars[bar].mBeatGroups.push_back( bg );
  setBarAsDirty( bar , true );
}
//-----------------------------------------------------------------------------
void PartitionViewer::addTechniqueGroup( techniqueType iT )
{
  for( int i = 0; i < mNotesSelected.size(); ++i )
  {
    int bar = mNotesSelected[i].first;
    setBarAsDirty( bar , true );
  }
  Technique t;
  t.mTechniqueType = iT;
  t.mNotes = mNotesSelected;
  mTechniques.push_back( t );
}
//-----------------------------------------------------------------------------
void PartitionViewer::addNote( int iNote )
{ addNote( iNote, 0 ); }
//-----------------------------------------------------------------------------
void PartitionViewer::addNote( int iNote, int iOctave )
{ addNote( iNote, iOctave, mCurrentBar ); }
//-----------------------------------------------------------------------------
void PartitionViewer::addNote( int iNote, int iOctave, int iBar )
{
  if( iBar >= 0 && iBar < getNumberOfBars() )
  {
    mBars[iBar].mNotes.push_back( make_pair( iNote, iOctave ) );
    setBarAsDirty( iBar, true );
    
//    bool doesNoteClosesTechniqueGroup = iNote == nComma ||
//      iNote == nChick || iNote == nRest;
//    if( doesNoteClosesTechniqueGroup ){ closeTechniqueGroup(); }
//    if( isTechniqueGroupOpen() )
//    {
//      int noteIndex = mBars[iBar].mNotes.size() - 1;
//      Technique& m = mTechniques[ mTechniques.size() - 1 ]; //le dernier groupe
//      m.addNote( iBar, noteIndex );
//      //dirty sur toutes les barres affectées par cette technique
//      vector<int> bi = m.barsInvolved();
//      for( int i = 0; i < bi.size(); ++i )
//      { setBarAsDirty( bi[i], true ); }
//    }
  
    updateUi();
  }
}
//-----------------------------------------------------------------------------
void PartitionViewer::addNoteToSelection( int iBar, int iNote )
{
  mNotesSelected.push_back( make_pair( iBar, iNote ) );
  setBarAsDirty( iBar, true );
}
//-----------------------------------------------------------------------------
void PartitionViewer::addBar()
{
  mBars.push_back( Bar() );
  updateUi();
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
  mBars.clear();
  mTechniques.clear();
  mCurrentBar = -1;
  mCurrentNote = -1;
}
//-----------------------------------------------------------------------------
//void PartitionViewer::closeTechniqueGroup()
//{
//  mIsTechniqueGroupOpen = false;
//  validateAndCleanTechniqueGroup();
//}
//-----------------------------------------------------------------------------
void PartitionViewer::closeSelection()
{
  mIsSelectionOpen = false;
  //Une selection a 1 item n'est pas valide... la note courante fait ce travail
  if( mNotesSelected.size() < 2 )
  {
    mNotesSelected.clear();
    setBarAsDirty( getCurrentBar(), true );
  }
}
//-----------------------------------------------------------------------------
int PartitionViewer::cmToPixel( double iCm ) const
{ return logicalDpiX() * iCm / 2.54; }
//-----------------------------------------------------------------------------
void PartitionViewer::decreaseOctave()
{ mOctave = std::max( --mOctave, -1 ); }
//-----------------------------------------------------------------------------
void PartitionViewer::generateRandomPartition()
{
  clear();
  const int kNumBars = 52;
  mBars.resize( kNumBars );
  for( int i = 0; i < getNumberOfBars() - 1; ++i )
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
      mBars[i].mNotes.push_back( make_pair( note, octave ) );
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
    case brTechniqueY: r = getBarRegion(brNoteTopY) - 8; break;
    case brBeatGroupY: r = getBarRegion(brNoteBottomY) + 2; break;
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
std::pair<int, int> PartitionViewer::getNote( int iNote, int iBar ) const
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
      int titleHeight = iPage == 0 ? getRegion(rTitle).height() : 0;
      int margin = cmToPixel( kPageMarginInCm );
      int paperWidth = getPaperSizeInInch().width() * logicalDpiX();
      int paperHeight = getPaperSizeInInch().height() * logicalDpiY();
      int topOfPage = iPage * paperHeight + iPage * kInterPageGap;
      r.setLeft( margin );
      r.setWidth( paperWidth - 2*margin );
      r.setTop( topOfPage + titleHeight + margin );
      r.setHeight( paperHeight - titleHeight - 2*margin );
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
      int h = QFontMetrics( mTitleFont ).height();
      r.setTop( cmToPixel( kPageMarginInCm ) ); r.setWidth( w ); r.setHeight( h );
    }break;
  }
  return r;
}
//-----------------------------------------------------------------------------
bool PartitionViewer::isDebugging() const
{ return mIsDebugging; }
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
bool PartitionViewer::isSelectionOpen() const
{ return mIsSelectionOpen; }
//-----------------------------------------------------------------------------
//bool PartitionViewer::isTechniqueGroupOpen() const
//{ return mIsTechniqueGroupOpen; }
//-----------------------------------------------------------------------------
void PartitionViewer::increaseOctave()
{ mOctave = std::min( ++mOctave, 1 ); }
//-----------------------------------------------------------------------------
void PartitionViewer::keyPressEvent( QKeyEvent* ipE )
{
  int o = getOctave();
  switch (ipE->key())
  {
    case Qt::Key_1: addNote( nSa, o ); mCurrentNote++; ;break;
    case Qt::Key_2: addNote( nRe, o ); mCurrentNote++; break;
    case Qt::Key_3: addNote( nGa, o ); mCurrentNote++; break;
    case Qt::Key_4: addNote( nMa, o ); mCurrentNote++; break;
    case Qt::Key_5: addNote( nPa, o ); mCurrentNote++; break;
    case Qt::Key_6: addNote( nDha, o ); mCurrentNote++; break;
    case Qt::Key_7: addNote( nNi, o ); mCurrentNote++; break;
    case Qt::Key_Comma: addNote( nComma ); mCurrentNote++; break;
    case Qt::Key_C: addNote( nChick ); mCurrentNote++; break;
    case Qt::Key_R: addNote( nRest ); mCurrentNote++; break;
    case Qt::Key_G:
      addBeatGroup();
      break;
    case Qt::Key_M:
//      if( isTechniqueGroupOpen() ){ closeTechniqueGroup(); }
//      else{ openTechniqueGroup( ttMeend ); }
      addTechniqueGroup( ttMeend );
      break;
    case Qt::Key_K:
//      if( isTechniqueGroupOpen() ){ closeTechniqueGroup(); }
//      else{ openTechniqueGroup( ttKrintan ); }
      addTechniqueGroup( ttKrintan );
      break;
    case Qt::Key_Left:
      if( --mCurrentNote < 0 && getCurrentBar() > 0 )
      {
        setCurrentBar( getCurrentBar() - 1 );
        mCurrentNote = getNumberOfNotesInBar( getCurrentBar() ) - 1;
      }
      mCurrentNote = std::max( mCurrentNote, 0 );
      break;
    case Qt::Key_Right:
      if( ++mCurrentNote >= getNumberOfNotesInBar( getCurrentBar() )
         && getCurrentBar() < getNumberOfBars() - 1 )
      {
        setCurrentBar( getCurrentBar() + 1 );
        mCurrentNote = 0;
      }
      mCurrentNote = std::min(mCurrentNote, getNumberOfNotesInBar( getCurrentBar() ) - 1);
      
      if( ipE->modifiers() | Qt::ShiftModifier && isSelectionOpen() )
      { addNoteToSelection( getCurrentBar(), mCurrentNote ); }
      break;
    case Qt::Key_Space:
      addBar();
      setCurrentNote( -1 );
      setCurrentBar( getNumberOfBars() -1 );
      break;
    case Qt::Key_Backspace:
      removeNote( getCurrentBar(), getCurrentNote() );
      mCurrentNote--;
      break;
    case Qt::Key_Plus: increaseOctave(); break;
    case Qt::Key_Minus: decreaseOctave(); break;
    case Qt::Key_Shift:
      openSelection();
      addNoteToSelection( getCurrentBar(), getCurrentNote() );
      break;
    default: break;
  }
  updateUi();
}
//-----------------------------------------------------------------------------
void PartitionViewer::keyReleaseEvent( QKeyEvent* ipE )
{
  switch (ipE->key())
  {
    case Qt::Key_Shift: closeSelection(); break;
    default: break;
  }
}
//-----------------------------------------------------------------------------
QString PartitionViewer::noteToString( std::pair<int, int> iNote ) const
{
  int note = iNote.first;
  int octave = iNote.second;
  
  QString r;
  switch( note )
  {
    case nSa: r = "S"; break;
    case nRe: r = "R"; break;
    case nGa: r = "G"; break;
    case nMa: r = "M"; break;
    case nPa: r = "P"; break;
    case nDha: r = "D"; break;
    case nNi: r = "N"; break;
    case nChick: r = "\xE2\x9c\x93"; break; //check
    case nRest: r = "\xE2\x80\x94"; break; //barre horizontale
    case nComma: r = ","; break;
    default: break;
  }
  
  //ajout de la notation octave
  if( note != nChick && note != nRest && note != nComma )
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
////-----------------------------------------------------------------------------
//void PartitionViewer::openBeatGroup( int iBar )
//{
//  mIsBeatGroupOpen = true;
//  mBars[ iBar ].mBeatGroups.push_back( vector<int>() );
//}
//-----------------------------------------------------------------------------
//void PartitionViewer::openTechniqueGroup( techniqueType iT )
//{
//  mIsTechniqueGroupOpen = true;
//  mTechniques.push_back( Technique( iT ) );
//}
//-----------------------------------------------------------------------------
void PartitionViewer::openSelection()
{
  for( int i = 0; i < mNotesSelected.size(); ++i )
  { setBarAsDirty( mNotesSelected[i].first , true ); }
  mNotesSelected.clear();
  mIsSelectionOpen = true;
}
//-----------------------------------------------------------------------------
void PartitionViewer::paintEvent( QPaintEvent* ipE )
{
  QRect r = ipE->region().boundingRect();
  QPainter p(this);
  p.setBackgroundMode( Qt::OpaqueMode );
  QBrush b( Qt::white );
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
    //le titre
    if( i == 0 )
    {
      p.setFont( mTitleFont );
      p.drawText( getRegion( rTitle ), Qt::AlignCenter, "Title");
    }
    
    //on dessine le bas de page
    //l'indice de page commence à 0, mais on veut que montrer page 1.
    p.setFont(QFont("Arial", 10));
    p.drawText( getPageRegion( prPageFooter, i ), Qt::AlignCenter,
               "page " + QString::number(i) + 1 );
  }
  
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
      
      //le contoure de la barre selectionné
      p.setPen( QColor( 125, 125, 125, 120 ) );
      if( getCurrentBar() == i )
      { p.drawRoundedRect( b.mPageLayout[j], 2, 2 ); }
      p.setPen( Qt::black );
      
      //le contour de la note courante
      p.setPen( Qt::gray );
      if( getCurrentBar() == i && getCurrentNote() >= 0 )
      {
        QRect r = b.mNotesPageLayout[getCurrentNote()];
        if( !isNoteSelected( getCurrentBar(), getCurrentNote() ) )
        { r.adjust( -2, -2, 2, 2 ); }
        p.drawRoundedRect( r, 2, 2 );
      }
      p.setPen( Qt::black );
    }
  }
  
  //--- render debug infos
  if( isDebugging() )
  {
    p.setPen(Qt::blue);
    p.setBrush( Qt::NoBrush );
    for( int i = 0; i < getNumberOfPages(); ++i)
    {
      if( i == 0 )
      { p.drawRect( getRegion( rTitle ) ); }
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
        s.sprintf("number of notes: %d", getNumberOfNotesInBar( i ) );
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
void PartitionViewer::removeNote( int iBar, int iNote )
{
  if( iBar >= 0 && iBar < getNumberOfBars() &&
     iNote >= 0 && iNote < getNumberOfNotesInBar(iBar) )
  {
    Bar& b = mBars[iBar];
    b.mNotes.erase( b.mNotes.begin() + iNote );
    
    //on enleve la note des technique et des groupes...
    for( int i = 0; i < b.mBeatGroups.size(); ++i )
    {
      vector<int>& bg = b.mBeatGroups[i];
      for( int j = 0; j < bg.size(); ++j )
      {
        if( bg[j] == iNote )
        { bg.erase( bg.begin() + j ); }
      }
    }
    
    for( int i = 0; i < mTechniques.size(); ++i )
    {
      Technique& t = mTechniques[i];
      if( t.appliesToBar( iBar ) )
      { t.removeNote( iBar, iNote ); }
      
      //dirty sur toutes les barres affectées par cette technique
      vector<int> bi = t.barsInvolved();
      for( int i = 0; i < bi.size(); ++i )
      { setBarAsDirty( bi[i], true); }
      //{ mBars[bi[i]].mIsDirty = true; }
    }
    
    //b.mIsDirty = true;
    setBarAsDirty( iBar, true);
    updateUi();
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
    QString s = noteToString( getNote( j, iBar ) );
    bp.drawText( b.mNotesRect[j], s);
  }
  
  //la barre vertical
  int x1 = getBarRegion( brSeparatorX );
  int y1 = 0.15 * kBarHeight;
  int y2 = kBarHeight - 0.15 * kBarHeight;
  bp.drawLine( x1, y1, x1, y2 );
  
  //b.mPixmap.toImage().save( "bar" + QString::number(i) + ".png", "PNG" );
  
  //--- render beat groups
  for( int i = 0; i < b.mBeatGroupsRect.size(); ++i )
  {
    QRect r = b.mBeatGroupsRect[i];
    bp.drawArc( r, -10 * 16, -170 * 16 );
  }
  
  //--- render Techniques: meends and krintans
  for( int i = 0; i < mTechniques.size(); ++i )
  {
    Technique& m = mTechniques[i];
    if( m.appliesToBar( iBar ) )
    {
      QPixmap pix( m.mFullTechnique.width(), m.mFullTechnique.height() );
      pix.fill( QColor( 0, 0, 0, 0 ) );
      QPainter mp( &pix );
      mp.setRenderHints( mp.renderHints() | QPainter::Antialiasing );
      mp.setPen( Qt::black );
      mp.setBrush( Qt::NoBrush );
      if( m.mTechniqueType == ttMeend )
      { mp.drawArc( pix.rect(), 10 * 16, 170 * 16 ); }
      else //krintan
      {
        QRect r = pix.rect();
        mp.drawLine( r.bottomLeft(), r.topLeft() );
        mp.drawLine( r.topLeft(), r.topRight() );
        mp.drawLine( r.topRight(), r.bottomRight() );
      }
//pix.toImage().save( "Technique.png", "PNG" );
      //blit dans le rendu pixmap hors écran de la barre.
      QRect destination = m.getBlit( iBar );
      QRect source = m.getCut( iBar );
      destination.translate( 0, getBarRegion( brTechniqueY ) );
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
        { r = b.mNotesRect[noteIndex]; }
        else{ { r = r.united(b.mNotesRect[noteIndex]); } }
      }
    }
    
    bp.setPen( Qt::blue );
    if( !r.isNull() )
    {
      r.adjust( -2, -2, 2, 2 );
      bp.drawRoundedRect( r, 2, 2 );
    }
    bp.setPen( Qt::black );
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
  int separatorPos = getBarRegion( brNoteStartX );
  int barWidth = separatorPos + 2*kMargin;
  int cursorX = kMargin + separatorPos;
  for( int j = 0; j < getNumberOfNotesInBar( iBar ); ++j )
  {
    QString n = noteToString( getNote( j, iBar ) );
    int noteLenght = fm.width( n ) + 1;
    int noteHeight = fm.height();
    b.mNotesRect.push_back(
                           QRect( cursorX, getBarRegion( brNoteTopY ), noteLenght, noteHeight ) );
    cursorX += noteLenght + kInterNoteGap;
    barWidth += noteLenght;
  }
  barWidth += std::max( (getNumberOfNotesInBar( iBar ) - 1) * kInterNoteGap, 0 );
  
  //--- Rectangle contenant la bar
  QRect barRect( QPoint(0, 0), QSize(std::max( barWidth, kMinBarWidth), kBarHeight ) );
  b.mRect = barRect;
  
  //--- Rectangle contenant les beat groups.
  b.mBeatGroupsRect.clear();
  for( int i = 0; i < b.mBeatGroups.size(); ++i )
  {
    int left = numeric_limits<int>::max();
    int right = numeric_limits<int>::min();
    vector<int>& v = b.mBeatGroups[i];
    for( int j = 0; j < v.size(); ++j )
    {
      int noteIndex = v[j];
      left = std::min( left, mBars[iBar].mNotesRect[noteIndex].left() );
      right = std::max( right, mBars[iBar].mNotesRect[noteIndex].right() );
    }
    b.mBeatGroupsRect.push_back( QRect( QPoint( left, getBarRegion( brBeatGroupY ) ),
      QSize( right - left, kBeatGroupHeight ) ) );
  }
}
//-----------------------------------------------------------------------------
void PartitionViewer::updatePageLayouts()
{
  mLayoutCursor = getPageRegion( prBody, 0 ).topLeft();
  for( int i = 0; i < getNumberOfBars(); ++i )
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
          QRect n = b.mNotesRect[j];
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
          QRect n = b.mNotesRect[j];
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
void PartitionViewer::updateTechnique( Technique* ipM )
{
  ipM->mCuts.clear();
  ipM->mBlits.clear();
  ipM->mFullTechnique = QRect();
  
  //update rects...
  vector<int> bi = ipM->barsInvolved();
  ipM->mCuts.resize( bi.size() );
  ipM->mBlits.resize( bi.size() );
  int barIndex = -1;
  int widthAccum = 0;
  int height = ipM->mTechniqueType == ttMeend ? kMeendHeight : kKrintanHeight;
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
        left = std::min( left, mBars[barIndex].mNotesRect[noteIndex].left() );
        right = std::max( right, mBars[barIndex].mNotesRect[noteIndex].right() );
        
        //quand c'est la derniere note de la barre et que le Technique dépasse dnas la barre
        //suivante, on prend la fin de la barre...
        if( noteIndex == (getNumberOfNotesInBar(barIndex) - 1) && i < (bi.size() - 1) )
        { right = mBars[barIndex].mRect.right(); }
        
        //quand c'Est la premiere note de la barre et que le Technique vient de la barre
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
  ipM->mFullTechnique = QRect( QPoint(0, 0), QSize(widthAccum, height) );
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
void PartitionViewer::updateUi()
{
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
      for( int j = 0; j < mTechniques.size(); ++j )
      {
        if( mTechniques[j].appliesToBar( i ) )
        { updateTechnique( &mTechniques[j] ); }
      }
      renderBarOffscreen(i);
      setBarAsDirty( i, false );//b.mIsDirty = false;
      shouldUpdateLayout = true;
    }
  }
  
  if( shouldUpdateLayout ){ updatePageLayouts(); }
  
  update();
}
//-----------------------------------------------------------------------------
//void PartitionViewer::validateAndCleanBeatGroup( int iBar, int iGroup )
//{
//  //le group doit avoir au moins 2 notes...
//  if( mBars[iBar].mBeatGroups[iGroup].size() < 2 )
//  { mBars[iBar].mBeatGroups.erase( mBars[iBar].mBeatGroups.begin() + iGroup ); }
//}
////-----------------------------------------------------------------------------
//void PartitionViewer::validateAndCleanLastBeatGroup( int iBar )
//{
//  if( iBar >= 0 && iBar < mBars.size() )
//  {
//    int lastGroup = mBars[iBar].mBeatGroups.size() - 1;
//    if( lastGroup >=0 && lastGroup < mBars[iBar].mBeatGroups.size() )
//    { validateAndCleanBeatGroup( iBar, lastGroup ); }
//  }
//}
//-----------------------------------------------------------------------------
void PartitionViewer::validateAndCleanTechniqueGroup()
{
  //les meends doivent avoir au moins deux notes pour être valides
  int lastIndex = mTechniques.size() - 1;
  
  if( lastIndex >= 0 && mTechniques[lastIndex].mNotes.size() < 2 )
  { mTechniques.erase( mTechniques.begin() + lastIndex ); }
}
//-----------------------------------------------------------------------------
// --- PARTITIONVIEWER::Technique
//-----------------------------------------------------------------------------
void PartitionViewer::Technique::addNote( int iBar, int iNoteIndex )
{ mNotes.push_back( make_pair( iBar, iNoteIndex ) ); }
//-----------------------------------------------------------------------------
bool PartitionViewer::Technique::appliesToBar( int iBar ) const
{
  bool r = false;
  for( int i = 0; i < mNotes.size(); ++i )
  { if( mNotes[i].first == iBar ){ r = true; break; } }
  return r;
}
//-----------------------------------------------------------------------------
std::vector<int> PartitionViewer::Technique::barsInvolved() const
{
  vector<int> r;
  for( int i = 0; i < mNotes.size(); ++i )
  { r.push_back( mNotes[i].first ); }
  r.erase( std::unique( r.begin(), r.end() ), r.end() );
  return r;
}
//-----------------------------------------------------------------------------
QRect PartitionViewer::Technique::getBlit( int iBar ) const
{
  QRect r;
  for( int i = 0; i < mBlits.size(); ++i )
  { if( mBlits[i].first == iBar ) { r = mBlits[i].second; break; } }
  return r;
}
//-----------------------------------------------------------------------------
QRect PartitionViewer::Technique::getCut( int iBar ) const
{
  QRect r;
  for( int i = 0; i < mCuts.size(); ++i )
  { if( mCuts[i].first == iBar ) { r = mCuts[i].second; break; } }
  return r;
}
//-----------------------------------------------------------------------------
void PartitionViewer::Technique::removeNote( int iBar, int iNote )
{
  for( int i = 0; i < mNotes.size(); ++i )
  {
    if( mNotes[i].first == iBar && mNotes[i].second == iNote )
    { mNotes.erase( mNotes.begin() + i ); }
  }
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

