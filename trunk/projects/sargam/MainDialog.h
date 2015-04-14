

#ifndef MainDialog_hh
#define MainDialog_hh

#include "data.h"
#include <QPrinter>
#include <QtWidgets>
#include <QSettings>
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
  QSizeF getPaperSizeInInch() const;
  bool isDebugging() const;
  void print( QPrinter* );
  void setAsDebugging( bool );
  void setPaperSize( QSizeF );
  void setComposition( Composition* );
  
protected slots:
  void resizeLineEditToContent();
  void restoreFocus();
  void stopLineTextEdit();
  
protected:
  enum region { rPartition, rTitle, rSargamScaleLabel, rSargamScale,
    rTarabTuningLabel, rTarabTuning };
  enum pageRegion { prPage, prBody, prPageFooter };
  enum barRegion { brSeparatorX, brNoteStartX, brNoteTopY, brNoteBottomY, brStrokeY,
    brOrnementY, brMatraGroupY, brGraceNoteTopY };
  
  struct Bar
  {
    Bar() : mIsDirty(true){;}
    QRect getNoteRect( int ) const;
    
    //--- cache d'Affichage
    QRect mRect;
    std::vector< QRect > mNotesRect;
    std::vector< QRect > mMatraGroupsRect;
    QPixmap mPixmap;
    std::vector< QRect > mPageLayout;
    std::vector< QRect > mNotesPageLayout;

    bool mIsDirty;
  };
  
  struct Ornement
  {
    Ornement() {;}

    QRect getBlit( int ) const;
    QRect getCut( int ) const;
    
    //--- cache d'Affichage
    QRect mFullOrnement;
    std::vector< std::pair< int, QRect > > mCuts; //barIndex, rect to form full meend
    std::vector< std::pair< int, QRect > > mBlits; //barIndex, rect to cut from mFullMeend
    //and blit to bar.mPixmap;
  };
  
  struct Line
  {
    Line() {;}
    
    //--- cache d'affichage
    QRect mLineNumberRect;
    QRect mTextRect;
    QRect mHotSpot;
  };

  void addBar( int );
  void addNoteToSelection( int, int );
  void addPage();
  void commandAddBar();
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
  void drawBarContour( QPainter&, int, QColor ) const;
  void eraseBar(int);
  void eraseOrnement( int );
  Bar& getBar(int);
  const Bar& getBar(int) const;
  int getBarRegion( barRegion ) const;
  int getCurrentBar() const;
  int getCurrentNote() const;
  int getInterNoteSpacing(int, int, int) const;
  int getNumberOfPages() const;
  QRect getPageRegion( pageRegion, int ) const;
  QRect getRegion( region ) const;
  QString getTitle() const;
  bool hasLineEditionPending() const;
  bool hasSelection() const;
  bool isNoteSelected( int, int ) const;
  virtual void keyPressEvent( QKeyEvent* );
  virtual void keyReleaseEvent( QKeyEvent* );
  Note makeNoteFromScale( noteValue ) const;
  virtual void mouseMoveEvent( QMouseEvent* );
  virtual void mouseReleaseEvent( QMouseEvent* );
  QString noteToString( Note ) const;
  virtual void paintEvent(QPaintEvent*);
  void renderBarOffscreen( int );
  void setBarAsDirty( int, bool );
  void setBarAsDirty( std::vector<int>, bool );
  void setCurrentBar(int);
  void setCurrentNote(int);
  std::map< int, std::vector< int > > splitPerBar( std::vector< std::pair<int, int> > );
  void startLineTextEdit( int );
  QString strokeToString( strokeType );
  int toPageIndex( QPoint ) const;
  std::vector<NoteLocator> toNoteLocator( const std::vector< std::pair<int, int> > ) const;
  void updateBar( int );
  void updateBarsLayout();
  void updateOrnement( int );
  void updateLayout();
  void updateLinesLayout();
  void updateSpecialBarLayout( specialBar );
  void updateUi();
  
  //--- ui
  QLineEdit* mpTitleLe;
  QLineEdit* mpLineTextEdit;
  
  //--- data
  bool mIsDebugging;
  QSizeF mPaperSize; //inches
  int mNumberOfPages;
  QFont mTitleFont;
  QFont mBarFont;
  QFont mGraceNotesFont;
  QFont mLineFont;
  QFont mStrokeFont;
  Bar mScale;
  Bar mTarabTuning;
  std::vector< Bar > mBars;
  std::vector< Ornement > mOrnements;
  std::vector< Line > mLines;
  int mCurrentBar;
  int mCurrentNote;
  QPoint mLayoutCursor;
  int mOctave;
  std::vector< std::pair<int, int> > mSelectedNotes; //bar, index
  int mEditingLineIndex;
  int mAddLineTextHover;
  int mBarHoverIndex;
  static Composition mDummyComposition;
  Composition* x; //jamais null...
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

protected slots:
  void newFile();
  void openFile();
  void print();
  void save();
  void saveAs();
  void generateRandomPartition();
  void toggleDebugging();
  
protected:
  void loadSettings();
  void saveSettings();
  void updateUi();
  
  realisim::sargam::PartitionViewer* mpPartitionViewer;
  QSettings mSettings;
  QString mSaveFileName;
  QString mLastSavePath;
  
  realisim::sargam::Composition mComposition;
};

#endif
