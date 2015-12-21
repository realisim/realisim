
#pragma once

#include "data.h"
#include <QDialog>
#include <QPageLayout>

class MainDialog;
class QButtonGroup;
class QCheckBox;
class QComboBox;
class QLabel;
class QSpinBox;

namespace realisim
{
namespace sargam
{

//------------------------------------------------------------------------------
//--- PreferencesDialog
//------------------------------------------------------------------------------
class PartitionViewer;
class PreferencesDialog : public QDialog
{
  Q_OBJECT
public:
  PreferencesDialog(const MainDialog*,
                    const realisim::sargam::PartitionViewer*,
                    QWidget* = 0);
  ~PreferencesDialog();
  
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

//------------------------------------------------------------------------------
//--- SaveDialog
//------------------------------------------------------------------------------
class SaveDialog : public QDialog
{
  Q_OBJECT
public:
  SaveDialog(QWidget *, QString);
  virtual ~SaveDialog(){}
  
  enum answer{aDontSave, aSave, aCancel};
  
  answer getAnswer() const;
  
protected slots:
  void cancel();
  void dontSave();
  void save();
  
protected:
  void createUi();
  
  answer mAnswer;
  QString mFileName;
};

  
}
}