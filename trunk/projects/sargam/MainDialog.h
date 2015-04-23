

#ifndef MainDialog_hh
#define MainDialog_hh

#include "data.h"
#include <QPrinter>
#include <QtWidgets>
#include <QSettings>
#include "utils/Log.h"
#include <vector>

namespace realisim
{
namespace sargam
{

//------------------------------------------------------------------------------
class PartitionViewer : public QWidget
{
  Q_OBJECT
public:
  PartitionViewer( QWidget* );
  ~PartitionViewer();
  
  void generateRandomPartition();
  Composition getComposition() const;
  int getOctave() const;
  QPageLayout::Orientation getLayoutOrientation() const;
  const utils::Log& getLog() const;
  QPageSize::PageSizeId getPageSizeId() const;
  bool hasLogTiming() const { return mHasLogTiming; }
  bool isDebugging() const;
  bool isVerbose() const {return mIsVerbose;}
  void print( QPrinter* );
  void setAsDebugging( bool );
  void setLayoutOrientation( QPageLayout::Orientation );
  void setPageSize( QPageSize::PageSizeId );
  void setAsVerbose( bool );
  void setComposition( Composition* );
  void setLog( utils::Log* );
  void setLogTiming( bool iL ) {mHasLogTiming = iL;}
  
signals:
  void ensureVisible( QPoint );
  
protected slots:
  void resizeLineEditToContent();
  void stopBarTextEdit();
  void stopLineTextEdit();
  void stopTitleEdit();
  
protected:
  enum region { rPartition, rTitle, rSargamScaleLabel, rSargamScale,
    rTarabTuningLabel, rTarabTuning };
  enum pageRegion { prPage, prBody, prPageFooter };
  enum barRegion { brSeparatorX, brNoteStartX, brNoteTopY, brNoteBottomY, brStrokeY,
    brOrnementY, brMatraGroupY, brGraceNoteTopY, brTextX, brTextY };
  enum colors{ cHover, cSelection };
  
  struct Bar
  {
    Bar() : mIsDirty(true), mIsWayTooLong(false){;}
    QRect getNoteRect( int ) const;
    
    /*--- cache d'Affichage 
     mRect: coin à (0, 0). C'est les rect qui contient toute la barre
     mNotesRect: les rect qui contiennent chaque note en coordonnées barre. Donc,
       par rapport à (0, 0).
     mMatraGroupRect: les rects qui contiennent les matras. En coordonnées barre.
     mScreenLayout: mRect, mais en coordonnées écran.
     mNoteScreenLayouts: comme mScreenLayout mais pour les notes.
     */
    QRect mRect;
    std::vector< QRect > mNotesRect;
    std::vector< QRect > mMatraGroupsRect;
    QRect mScreenLayout;
    std::vector< QRect > mNoteScreenLayouts;
    QRect mTextRect; //bar coord.
    QRect mTextScreenLayout;
    

    bool mIsDirty;
    bool mIsWayTooLong;
  };
  
  struct Ornement
  {
    Ornement() {;}

    int getDestination( int ) const;
    int getOffset( int ) const;
    
    /*--- cache d'Affichage 
     Coordonées barre:
     mFullOrnement est le rectangle avec coin à (0, 0) qui contient
       l'ornement complet, même s'il est sur plusieurs barres.
     mOffsets: l'offset X à appliquer à l'ornement
     mDestination: la coordonnée X de destination de l'ornement dans la barre
     */
    QRect mFullOrnement;
    std::vector< std::pair< int, int > > mOffsets;
    std::vector< std::pair< int, int > > mDestinations;
  };
  
  struct Line
  {
    Line() {;}
    
    /*--- cache d'affichage 
     Coordonnées écran. */
    QRect mLineNumberRect;
    QRect mTextScreenLayout;
    QRect mHotSpot;
  };

  void addBar( int );
  void addNoteToSelection( int, int );
  void addPage();
  void commandAddBar();
  void commandAddBarBefore();
  void commandAddGraceNotes();
  void commandAddLine();
  void commandAddNote( noteValue );
  void commandAddMatra();
  void commandAddOrnement( ornementType );
  void commandAddStroke( strokeType );
  void commandBreakMatrasFromSelection();
  void commandBreakOrnementsFromSelection();
  void commandDecreaseOctave();
  void commandErase();
  void commandIncreaseOctave();
  void commandRemoveSelectionFromGraceNotes();
  void commandShiftNote();
  void clear();
  void clearSelection();
  void createUi();
  int cmToPixel( double ) const;
  void draw( QPaintDevice* iPaintDevice ) const;
  void drawBar( QPainter*, int ) const;
  void drawBarContour( QPainter*, int, QColor ) const;
  void drawCursor( QPainter* ) const;
  void drawLine( QPainter*, int ) const;
  void drawPages( QPainter* ) const;
  void drawPageFooter( QPainter*, int ) const;
  void drawPageFooters( QPainter* iP ) const;
  void drawSelectedNotes( QPainter* ) const;
  void drawSpecialBars( QPainter* ) const;
  void drawTitle( QPainter* ) const;
  void eraseBar(int);
  void eraseOrnement( int );
  Bar& getBar(int);
  const Bar& getBar(int) const;
  int getBarRegion( barRegion ) const;
  std::vector<int> getBarsFromPage( int ) const;
  QColor getColor( colors ) const;
  int getCurrentBar() const;
  int getCurrentNote() const;
  QLine getCursorLine() const;
  int getInterNoteSpacing(int, int, int) const;
  utils::Log& getLog();
  int getNumberOfPages() const;
  QRect getPageRegion( pageRegion ) const;
  QRect getPageRegion( pageRegion, int ) const;
  QSizeF getPageSizeInInch() const;
  QRect getRegion( region ) const;
  bool hasBarTextEditionPending() const;
  bool hasLineEditionPending() const;
  bool hasSelection() const;
  bool hasTitleEditionPending() const {return mEditingTitle;}
  bool isNoteSelected( int, int ) const;
  virtual void keyPressEvent( QKeyEvent* );
  virtual void keyReleaseEvent( QKeyEvent* );
  Note makeNoteFromScale( noteValue ) const;
  virtual void mouseMoveEvent( QMouseEvent* );
  virtual void mouseReleaseEvent( QMouseEvent* );
  QString noteToString( Note ) const;
  virtual void paintEvent(QPaintEvent*);
  void setBarAsDirty( int, bool );
  void setBarAsDirty( std::vector<int>, bool );
  void setCurrentBar(int);
  void setCurrentNote(int);
  void setNumberOfPage(int);
  std::map< int, std::vector< int > > splitPerBar( std::vector< std::pair<int, int> > ) const;
  void startBarTextEdit( int );
  void startLineTextEdit( int );
  void startTitleEdit();
  QString strokeToString( strokeType ) const;
  int toPageIndex( QPoint ) const;
  std::vector<NoteLocator> toNoteLocator( const std::vector< std::pair<int, int> > ) const;
  void updateBar( int );
  void updateBarLayout();
  void updateOrnementLayout();
  void updateLayout();
  void updateLineLayout();
  void updateSpecialBarLayout( specialBar );
  void updateUi();
  
  //--- ui
  QLineEdit* mpTitleEdit;
  QLineEdit* mpLineTextEdit;
  QLineEdit* mpBarTextEdit;
  
  //--- data
  bool mIsDebugging;
  QRect mTitleScreenLayout;
  QPageSize::PageSizeId mPageSizeId;
  QPageLayout::Orientation mLayoutOrientation;
  int mNumberOfPages;
  QFont mTitleFont;
  QFont mBarFont;
  QFont mBarTextFont;
  QFont mGraceNotesFont;
  QFont mLineFont;
  QFont mStrokeFont;
  Bar mScale;
  Bar mTarabTuning;
  std::vector< Bar > mBars;
  std::vector< Ornement > mOrnements;
  std::vector< Line > mLines;
  std::map< int, std::vector<int> > mBarsPerPage;
  int mCurrentBar;
  int mCurrentNote;
  QPoint mLayoutCursor;
  int mOctave;
  std::vector< std::pair<int, int> > mSelectedNotes; //bar, index
  int mEditingBarText;
  int mEditingLineIndex;
  bool mEditingTitle;
  int mAddLineTextHover;
  int mBarHoverIndex;
  int mBarTextHover;
  static Composition mDummyComposition;
  Composition* x; //jamais null...
  utils::Log mDefaultLog;
  utils::Log *mpLog; //jamais null
  bool mIsVerbose;
  bool mHasLogTiming;
};

  
}
}

//------------------------------------------------------------------------------
class MainDialog : public QMainWindow
{
	Q_OBJECT
public:
	MainDialog();
	~MainDialog(){};

  const realisim::utils::Log& getLog() const {return mLog;}
  int getVersion() const { return 1; }
  bool isVerbose() const;
  void setAsVerbose( bool );
  
protected slots:
  void ensureVisible( QPoint );
  void generatePrintPreview(QPrinter*);
  void generateRandomPartition();
  void newFile();
  void openFile();
  void preferences();
  void print();
  void printPreview();
  void save();
  void saveAs();
  void toggleDebugging();
  void toggleLogTiming();
  
protected:
  void applyPrinterOptions( QPrinter* );
  void createUi();
  void loadSettings();
  void fillPageSizeCombo( QComboBox* );
  void saveSettings();
  void updateUi();
  
  QScrollArea* mpScrollArea;
  realisim::sargam::PartitionViewer* mpPartitionViewer;
  QSettings mSettings;
  QString mSaveFileName;
  QString mLastSavePath;
  std::vector<QPageSize::PageSizeId> mAvailablePageSizeIds;
  
  realisim::sargam::Composition mComposition;
  realisim::utils::Log mLog;
  bool mIsVerbose;
};

#endif
