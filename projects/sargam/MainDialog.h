

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
  int getVersionRevision() const {return 3;}
  bool isToolBarVisible() const {return mIsToolBarVisible;}
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
  realisim::sargam::Composition mComposition;
  realisim::utils::Log mLog;
  bool mIsVerbose;
  bool mIsToolBarVisible;
  state mState;
};

//------------------------------------------------------------------------------
class PreferencesDialog : public QDialog
{
  Q_OBJECT
public:
  PreferencesDialog(const MainDialog*,
                    const realisim::sargam::PartitionViewer*,
                    QWidget* = 0);
  ~PreferencesDialog(){}
  
  int getFontSize() const;
  QPageLayout::Orientation getPageLayout() const;
  QPageSize::PageSizeId getPageSizeId() const;
  realisim::sargam::script getScript() const;
  bool isToolBarVisible() const;
  bool isVerbose() const;
  
protected slots:
  void updateUi();
  
protected:
  void fillPageSizeCombo();
  void initUi();
  
  //--- data
  const MainDialog* mpMainDialog;
  const realisim::sargam::PartitionViewer* mpPartViewer;
  realisim::sargam::Composition mPartPreviewData;
  realisim::sargam::PartitionViewer* mpPartPreview;
  std::vector<QPageSize::PageSizeId> mAvailablePageSizeIds;
  
  //--- Ui
  QSpinBox* mpFontSize;
  QComboBox* mpScriptCombo;
  QLabel* mpPreviewLabel;
  QComboBox* mpPageSizeCombo;
  QButtonGroup* mpOrientation;
  QCheckBox* mpPortrait;
  QCheckBox* mpLandscape;
  QCheckBox* mpVerboseChkBx;
  QCheckBox* mpShowToolBarChkBx;
};


#endif
