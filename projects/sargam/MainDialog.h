

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
  enum techniqueType{ ttMeend, ttKrintan };
  
  void addNote( int );
  void addNote( int, int );
  void addNote( int, int, int );
  void clear();
//  void closeBeatGroup( int );
//  void closeTechniqueGroup();
  void decreaseOctave();
  void generateRandomPartition();
  int getCurrentBar() const;
  int getCurrentNote() const;
  std::pair<int, int> getNote( int, int ) const;
  int getNumberOfBars() const;
  int getNumberOfPages() const;
  int getNumberOfNotesInBar( int ) const;
  int getOctave() const;
  QSizeF getPaperSizeInInch() const;
  void increaseOctave();
  bool isDebugging() const;
//  bool isBeatGroupOpen() const;
//bool isTechniqueGroupOpen() const;
//  void openBeatGroup( int );
//  void openTechniqueGroup( techniqueType );
  void removeNote( int, int );
  void setAsDebugging( bool );
  void setCurrentBar(int);
  void setCurrentNote(int);
//void setPaperSize( QSizeF );
  
protected:
  enum region { rPartition, rTitle };
  enum pageRegion { prPage, prBody, prPageFooter };
  enum barRegion { brSeparatorX, brNoteStartX, brNoteTopY, brNoteBottomY, brStrokeY,
    brTechniqueY, brBeatGroupY };
  
  struct Bar
  {
    Bar() : mIsDirty(true){;}
    //--- cache d'Affichage
    QRect mRect;
    std::vector< QRect > mNotesRect;
    std::vector< QRect > mBeatGroupsRect;
    QPixmap mPixmap;
    std::vector< QRect > mPageLayout;
    std::vector< QRect > mNotesPageLayout;
    //--- data
    std::vector< std::pair<int, int> > mNotes;
    std::vector< std::vector<int> > mBeatGroups;
    bool mIsDirty;
  };
  
  struct Technique //technique...
  {
    Technique() : mTechniqueType( ttMeend ){}
    Technique( techniqueType iTt ) : mTechniqueType( iTt ){}
    
    void addNote( int, int );
    bool appliesToBar( int ) const;
    std::vector<int> barsInvolved() const;
    QRect getBlit( int ) const;
    QRect getCut( int ) const;
    void removeNote( int, int );
    
    QRect mFullTechnique;
    std::vector< std::pair< int, QRect > > mCuts; //barIndex, rect to form full meend
    std::vector< std::pair< int, QRect > > mBlits; //barIndex, rect to cut from mFullMeend
      //and blit to bar.mPixmap;
    std::vector< std::pair<int, int> > mNotes; //barIndex, noteIndex
    techniqueType mTechniqueType;
  };
  
  void addBar();
  void addBeatGroup();
  void addTechniqueGroup( techniqueType );
  void addPage();
  void addNoteToSelection( int, int );
  void closeSelection();
  int cmToPixel( double ) const;
  int getBarRegion( barRegion ) const;
  QRect getPageRegion( pageRegion, int ) const;
  QRect getRegion( region ) const;
  bool isNoteSelected( int, int ) const;
  bool isSelectionOpen() const;
  virtual void keyPressEvent( QKeyEvent* );
  virtual void keyReleaseEvent( QKeyEvent* );
  QString noteToString( std::pair<int, int> ) const;
  void openSelection();
  virtual void paintEvent(QPaintEvent*);
  void renderBarOffscreen( int );
  void setBarAsDirty( int, bool );
  int toPageIndex( QPoint ) const;
  void updateBar( int );
  void updateTechnique( Technique* );
  void updatePageLayouts();
  void updateUi();
//  void validateAndCleanBeatGroup( int, int );
//  void validateAndCleanLastBeatGroup( int );
  void validateAndCleanTechniqueGroup();
  
  //--- data
  bool mIsDebugging;
  QSizeF mPaperSize; //inches
  int mNumberOfPages;
  QFont mTitleFont;
  QFont mBarFont;
  std::vector< Bar > mBars;
  Technique mCurrentTechnique;
  std::vector< Technique > mTechniques;
  int mCurrentBar;
  int mCurrentNote;
  QPoint mLayoutCursor;
//  bool mIsTechniqueGroupOpen;
//  bool mIsBeatGroupOpen;
  int mOctave;
  bool mIsSelectionOpen;
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
