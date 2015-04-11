

#ifndef MainDialog_hh
#define MainDialog_hh

#include "data.h"
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
  
  void clear();
  void generateRandomPartition();
  Composition getComposition() const;
  int getOctave() const;
  QSizeF getPaperSizeInInch() const;
  bool isDebugging() const;
  void setAsDebugging( bool );
  //void setPaperSize( QSizeF );
  void setComposition( const Composition& );
  
protected slots:
  void resizeLineEditToContent();
  void restoreFocus();
  void stopLineTextEdit();
  
protected:
  enum bars{ bScale = -1, /*bAscendingScale, bDescendingScale, bTarabTuning*/ };
  enum region { rPartition, rTitle, rSargamScaleLabel, rSargamScale };
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
    //--- data
    std::vector< sargam::Note > mNotes;
    std::vector< Stroke > mStrokes;
    std::vector< std::vector<int> > mMatraGroups;
    std::vector< int > mGraceNotes;
    bool mIsDirty;
  };
  
  struct Ornement
  {
    Ornement() : mOrnementType( otMeend ){}
    Ornement( ornementType iTt ) : mOrnementType( iTt ){}
    Ornement( ornementType, const std::vector<NoteLocator>& );
    
    //void addNote( int, int );
    bool appliesToBar( int ) const;
    std::vector<int> barsInvolved() const;
    QRect getBlit( int ) const;
    QRect getCut( int ) const;
    
    //--- cache d'Affichage
    QRect mFullOrnement;
    std::vector< std::pair< int, QRect > > mCuts; //barIndex, rect to form full meend
    std::vector< std::pair< int, QRect > > mBlits; //barIndex, rect to cut from mFullMeend
    //and blit to bar.mPixmap;
    
    //--- data
    std::vector< std::pair<int, int> > mNotes; //barIndex, noteIndex
    ornementType mOrnementType;
  };
  
  struct Line
  {
    Line();
    Line( int, QString);
    
    int getFirstBar() const {return mFirstBar;}
    QString getText() const {return mText;}
    bool hasText() const {return !mText.isEmpty(); }
    void setBarIndex( int i ) { mFirstBar = i; }
    void setText( QString s ) { mText = s; }
    
    //--- cache d'affichage
    QRect mLineNumberRect;
    QRect mTextRect;
    QRect mHotSpot;
    
    //--- data
    int mFirstBar; //index de la premiere barre de cette ligne.
    QString mText;
  };

void addBar();
void addBar( int );
void addGraceNotes( int, int );
void addLine( int, QString = QString() );
void addMatra( std::vector< std::pair<int, int> > );
void addNote( Note );
void addNote( Note, int, int );
void addOrnement( ornementType, std::vector< std::pair<int, int> > );
void addStroke( strokeType, std::vector< std::pair<int, int> > );
void addNoteToMatra( int, int, int );
void addNoteToOrnement( int, int, int );
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
  void clearSelection();
  void createUi();
  int cmToPixel( double ) const;
  void drawBarContour( QPainter&, int, QColor );
void eraseBar(int);
void eraseGraceNote( int, int );
void eraseLine( int );
void eraseMatra( int, int );
void eraseNote( int, int );
void eraseNoteFromMatra( int, int );
void eraseNoteFromOrnement( int, int );
void eraseOrnement( int );
void eraseStroke( int, int );
int findLine( int ) const;
int findMatra( int, int ) const;
int findOrnement( int, int ) const;
int findStroke( int, int ) const;
  Bar& getBar(int);
  const Bar& getBar(int) const;
  int getBarRegion( barRegion ) const;
  int getCurrentBar() const;
  int getCurrentNote() const;
  int getInterNoteSpacing(int, int, int) const;
int getLineFirstBar( int ) const;
QString getLineText( int ) const;
const Note& getNote( int, int ) const;
Note& getNote( int, int );
int getNumberOfBars() const;
int getNumberOfLines() const;
  int getNumberOfPages() const;
int getNumberOfNotesInBar( int ) const;
  QRect getPageRegion( pageRegion, int ) const;
  QRect getRegion( region ) const;
std::vector<Note> getScale() const;
QString getTitle() const;
  bool hasLineEditionPending() const;
  bool hasSelection() const;
bool hasStroke( int, int ) const;
bool isGraceNote( int, int) const;
bool isNoteInMatra( int, int ) const;
bool isNoteInOrnement( int, int ) const;
  bool isNoteSelected( int, int ) const;
bool isStartOfLine( int ) const;
  virtual void keyPressEvent( QKeyEvent* );
  virtual void keyReleaseEvent( QKeyEvent* );
  Note makeNoteFromScale( noteValue ) const;
  virtual void mouseMoveEvent( QMouseEvent* );
  virtual void mouseReleaseEvent( QMouseEvent* );
  QString noteToString( Note ) const;
  virtual void paintEvent(QPaintEvent*);
  void renderBarOffscreen( int );
  void setBarAsDirty( int, bool );
  void setCurrentBar(int);
  void setCurrentNote(int);
void shiftGraceNotes( int, int, int );
void shiftLines( int, int );
void shiftMatras(int, int, int);
void shiftOrnements(int, int);
void shiftOrnements(int, int, int);
void shiftStrokes( int, int, int );
  std::map< int, std::vector< int > > splitPerBar( std::vector< std::pair<int, int> > );
  void startLineTextEdit( int );
  QString strokeToString( strokeType );
  int toPageIndex( QPoint ) const;
  void updateBar( int );
  void updateOrnement( Ornement* );
  void updatePageLayouts();
  void updateUi();
  
  //--- ui
  QLineEdit* mpTitleLe;
  QString mSargamScaleLabel;
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
};

#endif
