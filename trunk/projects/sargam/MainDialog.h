

#ifndef MainDialog_hh
#define MainDialog_hh

#include <QtWidgets>
#include <QSettings>
#include <vector>

//------------------------------------------------------------------------------
class PartitionViewer : public QWidget
{
  Q_OBJECT
public:
  PartitionViewer( QWidget* );
  ~PartitionViewer();

enum notes{ nSa = 1, nRe, nGa, nMa, nPa, nDha, nNi, nComma, nChick, nRest };
//  enum phrasing{ sComa, sAccent };
//  enum picking{ pDa, pRa, pDiri };
  enum ornementType{ otMeend, otKrintan, otAndolan, otGamak };
  
  //void addMatra( std::vector< std::pair<int, int> > );
  void addMatraFromSelection();
  void addNote( int );
  void addNote( int, int );
  void addNote( int, int, int, int );
  //void addOrnement( std::vector< std::pair<int, int> > );
  void addOrnementFromSelection( ornementType );
  void clear();
  void decreaseOctave();
  void eraseNote( int, int );
  void generateRandomPartition();
  int getCurrentBar() const;
  int getCurrentNote() const;
  std::pair<int, int> getNote( int, int ) const;
  int getNumberOfBars() const;
  int getNumberOfPages() const;
  int getNumberOfNotesInBar( int ) const;
  int getOctave() const;
  QSizeF getPaperSizeInInch() const;
  bool hasSelection() const;
  void increaseOctave();
  bool isDebugging() const;
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
  enum region { rPartition, rTitle };
  enum pageRegion { prPage, prBody, prPageFooter };
  enum barRegion { brSeparatorX, brNoteStartX, brNoteTopY, brNoteBottomY, brStrokeY,
    brOrnementY, brMatraGroupY };
  
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
    std::vector< std::pair<int, int> > mNotes;
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
  
  void addBar();
  void addPage();
  void addNoteToMatra( int, int, int );
  void addNoteToOrnement( int, int, int );
  void addNoteToSelection( int, int );
  void clearSelection();
  void createUi();
  int cmToPixel( double ) const;
void eraseNoteFromMatraGroup( int, int );
void eraseNoteFromOrnementGroup( int, int );
  int findMatra( int, int ) const;
  int findOrnement( int, int ) const;
  int getBarRegion( barRegion ) const;
  QRect getPageRegion( pageRegion, int ) const;
  QRect getRegion( region ) const;
  bool isSelectionOpen() const;
  virtual void keyPressEvent( QKeyEvent* );
  virtual void keyReleaseEvent( QKeyEvent* );
  QString noteToString( std::pair<int, int> ) const;
  virtual void paintEvent(QPaintEvent*);
  void renderBarOffscreen( int );
  void setBarAsDirty( int, bool );
  void shiftMatra(int, int, int);
  void shiftOrnement(int, int, int);
  int toPageIndex( QPoint ) const;
  void updateBar( int );
  void updateOrnement( Ornement* );
  void updatePageLayouts();
  void updateUi();
  
  //--- ui
  QLineEdit* mpTitleLe;
  
  //--- data
  bool mIsDebugging;
  QSizeF mPaperSize; //inches
  int mNumberOfPages;
  QFont mTitleFont;
  QFont mBarFont;
  std::vector< Bar > mBars;
  std::vector< Ornement > mOrnements;
  int mCurrentBar;
  int mCurrentNote;
  QPoint mLayoutCursor;

  int mOctave;
  std::vector< std::pair<int, int> > mNotesSelected; //bar, index
};

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
  
  PartitionViewer* mpPartitionViewer;
  QSettings mSettings;
};

#endif
