

#ifndef MainDialog_hh
#define MainDialog_hh

#include "data.h"
#include <QProxyStyle>
#include <QPrinter>
#include <QtWidgets>
#include <QSettings>
#include "utils/Log.h"
#include "Updater.h"
#include <vector>
#include <map>

namespace realisim { namespace sargam { class PartitionViewer; } }


class CustomProxyStyle : public QProxyStyle
{
public:
  CustomProxyStyle() : QProxyStyle() {}
  
  virtual void drawPrimitive(PrimitiveElement,
    const QStyleOption*, QPainter*, const QWidget* = 0) const;
protected:
};

//------------------------------------------------------------------------------
class MainDialog : public QMainWindow
{
	Q_OBJECT
public:
	MainDialog();
	~MainDialog(){};

  const realisim::utils::Log& getLog() const {return mLog;}
  QString getVersionAsQString() const;  
  int getVersionMajor() const {return 0;}
  int getVersionMinor() const {return 5;}
  int getVersionRevision() const {return 2;}
  bool isVerbose() const;
  void setAsVerbose( bool );
  
protected slots:
  void about();
  void ensureVisible( QPoint );
  void generatePrintPreview(QPrinter*);
  void generateRandomPartition();
  void handleUpdateAvailability();
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
  enum state{ sNormal, sUpdatesAreAvailable };
  enum action{ aAddBar, aLineJump, aAddMatra, aRemoveMatra, aAddKrintan,
    aAddMeend, aAddGamak, aAddAndolan, aRemoveOrnement, aAddGraceNote, aRemoveGraceNote,
    aAddParenthesis, aRemoveParenthesis,
    aDecreaseOctave, aIncreaseOctave, aRest, aChik, aPhrasing, aTivra, aShuddh, aKomal,
    aDa, aRa, aDiri, aRemoveStroke, aUnknown };
  
  void applyPrinterOptions( QPrinter* );
  void createUi();
  void createToolBar();
  action findAction( QAction* ) const;
  state getState() const;
  bool isToolBarVisible() const {return mIsToolBarVisible;}
  void loadSettings();
  void fillPageSizeCombo( QComboBox* );
  void saveSettings();
  void setState(state);
  void setToolBarVisible( bool i ) { mIsToolBarVisible = i; }
  void showUpdateDialog();
  void updateActions();
  
  QScrollArea* mpScrollArea;
  realisim::sargam::PartitionViewer* mpPartitionViewer;
  QToolBar* mpToolBar;
  std::map< action, QAction* > mActions;
  realisim::sargam::Updater* mpUpdater;
  
  QSettings mSettings;
  QString mSaveFileName;
  QString mLastSavePath;
  std::vector<QPageSize::PageSizeId> mAvailablePageSizeIds;
  realisim::sargam::Composition mComposition;
  realisim::utils::Log mLog;
  bool mIsVerbose;
  bool mIsToolBarVisible;
  state mState;
};

#endif
