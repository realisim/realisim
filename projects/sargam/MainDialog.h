

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
  
  //void addMatra( std::vector< std::pair<int, int> > );
  //void addGraceNotes( std::vector< std::pair<int, int> > )
  void addMatraFromSelection();
  void addNote( Note );
  void addNote( Note, int, int );
  //void addOrnement( std::vector< std::pair<int, int> > );
  void addOrnementFromSelection( ornementType );
  void addSelectionToGraceNotes();
  void clear();
  void decreaseOctave();
  void eraseNote( int, int );
  void generateRandomPartition();
  int getCurrentBar() const;
  int getCurrentNote() const;
  int getLineStart( int ) const;
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
  void increaseOctave();
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
  void titleChanged( const QString& );
  
protected:
  enum bars{ bScale = 0, /*bAscendingScale, bDescendingScale, bTarabTuning,*/
    bFirstSargamBar };
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
    //void removeNote( int, int );
    
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
    void setBarIndex( int i ) { mFirstBar = i; }
    void setText( QString s ) { mText = s; }
    
    //--- cache d'affichage
    QRect mLineNumberRect;
    QRect mTextRect;
    
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
void commandBreakMatrasFromSelection();
void commandBreakOrnementsFromSelection();
  void commandErase();
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
  int findLine( int ) const;
  int findMatra( int, int ) const;
  int findOrnement( int, int ) const;
//Bar& getBar( int, int );
  int getBarRegion( barRegion ) const;
  int getInterNoteSpacing(int, int, int) const;
  Note& getNote( int, int );
  QRect getPageRegion( pageRegion, int ) const;
  QRect getRegion( region ) const;
  bool isStartOfLine( int ) const;
  virtual void keyPressEvent( QKeyEvent* );
  virtual void keyReleaseEvent( QKeyEvent* );
  Note makeNoteFromScale( noteValue ) const;
  QString noteToString( Note ) const;
  virtual void paintEvent(QPaintEvent*);
  void renderBarOffscreen( int );
  void setBarAsDirty( int, bool );
  void shiftGraceNotes( int, int, int );
  void shiftMatra(int, int, int);
  void shiftOrnement(int, int, int);
  int toPageIndex( QPoint ) const;
  void updateBar( int );
  void updateOrnement( Ornement* );
  void updatePageLayouts();
  void updateUi();
  
  //--- ui
  QLineEdit* mpTitleLe;
  QString mSargamScaleLabel;
  
  //--- data
  bool mIsDebugging;
  QSizeF mPaperSize; //inches
  int mNumberOfPages;
  QFont mTitleFont;
  QFont mBarFont;
  QFont mGraceNotesFont;
  QFont mLineNumberFont;
  std::vector< Bar > mBars;
  std::vector< Ornement > mOrnements;
  std::vector< Line > mLines;
  int mCurrentBar;
  int mCurrentNote;
  QPoint mLayoutCursor;
  
  int mOctave;
  std::vector< std::pair<int, int> > mNotesSelected; //bar, index
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
