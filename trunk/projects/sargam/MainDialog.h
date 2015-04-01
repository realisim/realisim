

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
  
  enum ornementType{ otMeend, otKrintan, otAndolan, otGamak }; //a bouger dans data
  
  void addMatra( std::vector< std::pair<int, int> > );
  //void addGraceNotes( std::vector< std::pair<int, int> > )
  void addNote( Note );
  void addNote( Note, int, int );
  void addOrnement( ornementType, std::vector< std::pair<int, int> > );
  void addStroke( int, strokeType, std::vector< int > );
  void addSelectionToGraceNotes();
  void clear();
  void eraseNote( int, int );
  void generateRandomPartition();
  int getCurrentBar() const;
  int getCurrentNote() const;
  int getLineFirstBar( int ) const;
  QString getLineText( int ) const;
  const Note& getNote( int, int ) const;
  int getNumberOfBars() const;
  int getNumberOfLines() const;
  int getNumberOfPages() const;
  int getNumberOfNotesInBar( int ) const;
  //getNumberOfMatrasInBar( int ) const;
  //getNumberOfNoteInMatra( int iBar, int iMatra ) const;
  //getMatraNoteIndex( int iBar, int iMatra, int iIndex ) const;
  //getNumberOfGraceNotesInBar(int iBar) const;
  //getGraceNoteIndex( int iBar, int iIndex ) const;
  //getNumberOfOrnements() const;
  //getNumberOfNotesInOrnement( int iOrnementIndex ) const;
  //getOrnmentNoteIndex( int iOrnementIndex, int iIndex ) const;
int getOctave() const;
  QSizeF getPaperSizeInInch() const;
  std::vector<Note> getScale() const;
  bool hasSelection() const;
  bool isDebugging() const;
  bool isGraceNote( int, int) const;
  bool isNoteInMatra( int, int ) const;
  bool isNoteInOrnement( int, int ) const;
  bool isNoteSelected( int, int ) const;
  void setAsDebugging( bool );
  void setCurrentBar(int);
  void setCurrentNote(int);
  //void setPaperSize( QSizeF );
  
protected slots:
  void resizeLineEditToContent();
  void restoreFocus();
  void stopLineTextEdit();
  
protected:
  enum bars{ bScale = 0, /*bAscendingScale, bDescendingScale, bTarabTuning,*/
    bFirstSargamBar };
  enum region { rPartition, rTitle, rSargamScaleLabel, rSargamScale };
  enum pageRegion { prPage, prBody, prPageFooter };
  enum barRegion { brSeparatorX, brNoteStartX, brNoteTopY, brNoteBottomY, brStrokeY,
    brOrnementY, brMatraGroupY, brGraceNoteTopY };
  
  struct Stroke
  {
    Stroke( strokeType, int );
    Stroke( strokeType, std::vector<int> );
    
    strokeType mStrokeType;
    std::vector<int> mSpan;
  };
  
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
  
  struct Ornement //ornoment...
  {
    Ornement() : mOrnementType( otMeend ){}
    Ornement( ornementType iTt ) : mOrnementType( iTt ){}
    
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
void addLine( int, QString = QString() );
  void addPage();
  void addNoteToMatra( int, int, int );
  void addNoteToOrnement( int, int, int );
  void addNoteToSelection( int, int );
  void addToGraceNotes( int, int );
  void commandAddBar();
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
  void eraseGraceNote( int, int );
  void eraseMatra( int, int );
  void eraseNoteFromMatra( int, int );
  void eraseNoteFromOrnement( int, int );
  void eraseOrnement( int );
  void eraseStroke( int, int );
  int findLine( int ) const;
  int findMatra( int, int ) const;
  int findOrnement( int, int ) const;
  int findStroke( int, int ) const;
//Bar& getBar( int, int );
  int getBarRegion( barRegion ) const;
  int getInterNoteSpacing(int, int, int) const;
  Note& getNote( int, int );
  QRect getPageRegion( pageRegion, int ) const;
  QRect getRegion( region ) const;
  bool hasLineEditionPending() const;
  bool hasStroke( int, int ) const;
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
  void shiftGraceNotes( int, int, int );
  void shiftMatras(int, int, int);
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
  static Note mDummyNote;
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
};

#endif
