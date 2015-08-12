

#ifndef MainDialog_hh
#define MainDialog_hh

#include "data.h"
#include <QPrinter>
#include <QtWidgets>
#include <QSettings>
#include "utils/Log.h"
#include <vector>
#include <map>

namespace realisim { namespace sargam { class PartitionViewer; } }

//------------------------------------------------------------------------------
class MainDialog : public QMainWindow
{
	Q_OBJECT
public:
	MainDialog();
	~MainDialog(){};

  const realisim::utils::Log& getLog() const {return mLog;}
  int getVersion() const { return 179; } //svn revision
  bool isVerbose() const;
  void setAsVerbose( bool );
  
protected slots:
  void about();
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
  void toolActionTriggered(QAction*);
  void updateUi();
  
protected:
  enum action{ aAddBar, aLineJump, aAddMatra, aRemoveMatra, aAddKrintan,
    aAddMeend, aAddGamak, aAddAndolan, aRemoveOrnement, aAddGraceNote, aRemoveGraceNote,
    aAddParenthesis, aRemoveParenthesis,
    aDecreaseOctave, aIncreaseOctave, aRest, aChik, aPhrasing, aTivra, aShuddh, aKomal,
    aDa, aRa, aDiri, aRemoveStroke, aUnknown };
  
  void applyPrinterOptions( QPrinter* );
  void createUi();
  void createToolBar();
  action findAction( QAction* ) const;
  bool isToolBarVisible() const {return mIsToolBarVisible;}
  void loadSettings();
  void fillPageSizeCombo( QComboBox* );
  void saveSettings();
  void setToolBarVisible( bool i ) { mIsToolBarVisible = i; }
  
  QScrollArea* mpScrollArea;
  realisim::sargam::PartitionViewer* mpPartitionViewer;
  QToolBar* mpToolBar;
  std::map< action, QAction* > mActions;
  
  QSettings mSettings;
  QString mSaveFileName;
  QString mLastSavePath;
  std::vector<QPageSize::PageSizeId> mAvailablePageSizeIds;
  realisim::sargam::Composition mComposition;
  realisim::utils::Log mLog;
  bool mIsVerbose;
  bool mIsToolBarVisible;
};

#endif
