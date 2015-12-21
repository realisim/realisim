
#include "Dialogs.h"
#include "MainDialog.h"
#include "PartitionViewer.h"
#include <QButtonGroup>
#include <QCheckBox>
#include <QComboBox>
#include <QFont>
#include <QLabel>
#include <qlayout.h>
#include <QPrinterInfo>
#include <QPushButton>
#include <QSpinBox>

using namespace realisim;
  using namespace sargam;
using namespace std;

//-----------------------------------------------------------------------------
// --- PreferencesDialog
//-----------------------------------------------------------------------------
PreferencesDialog::PreferencesDialog(const MainDialog* ipMd,
                                     const PartitionViewer* ipPv,
                                     QWidget* ipParent) :
QDialog(ipParent),
mpMainDialog(ipMd),
mpPartViewer(ipPv),
mPartPreviewData(),
mpPartPreview(0)
{
  
  initUi();
  
  mPartPreviewData.setTitle( "Untitled" );
  mPartPreviewData.addBar();
  mPartPreviewData.addLine(0);
  
  //--- 16 notes
  mPartPreviewData.addNote(0, Note(nvMa, -1));
  mPartPreviewData.addNote(0, Note(nvPa, -1));
  mPartPreviewData.addNote(0, Note(nvDha, -1));
  mPartPreviewData.addNote(0, Note(nvNi, -1));
  mPartPreviewData.addNote(0, Note(nvSa, 0));
  mPartPreviewData.addNote(0, Note(nvRe, 0));
  mPartPreviewData.addNote(0, Note(nvGa, 0));
  mPartPreviewData.addNote(0, Note(nvMa, 0));
  mPartPreviewData.addNote(0, Note(nvPa, 0));
  mPartPreviewData.addNote(0, Note(nvDha, 0));
  mPartPreviewData.addNote(0, Note(nvNi, 0));
  mPartPreviewData.addNote(0, Note(nvSa, 1));
  mPartPreviewData.addNote(0, Note(nvRe, 1));
  mPartPreviewData.addNote(0, Note(nvGa, 1));
  mPartPreviewData.addNote(0, Note(nvMa, 1));
  mPartPreviewData.addNote(0, Note(nvPa, 1));
  
  // 4 matras a 4 notes par matra
  vector<int> m1(4, 0); m1[0] = 0; m1[1] = 1; m1[2] = 2; m1[3] = 3;
  vector<int> m2(4, 0); m2[0] = 4; m2[1] = 5; m2[2] = 6; m2[3] = 7;
  vector<int> m3(4, 0); m3[0] = 8; m3[1] = 9; m3[2] = 10; m3[3] = 11;
  vector<int> m4(4, 0); m4[0] = 12; m4[1] = 13; m4[2] = 14; m4[3] = 15;
  mPartPreviewData.addMatra(0, m1);
  mPartPreviewData.addMatra(0, m2);
  mPartPreviewData.addMatra(0, m3);
  mPartPreviewData.addMatra(0, m4);
  
  // quelques notes de grace sur les 2iem et 3ieme matra
  mPartPreviewData.addGraceNote(0, 4);
  mPartPreviewData.addGraceNote(0, 5);
  mPartPreviewData.addGraceNote(0, 6);
  mPartPreviewData.addGraceNote(0, 8);
  mPartPreviewData.addGraceNote(0, 9);
  mPartPreviewData.addGraceNote(0, 10);
  
  //krintan et meend sur 2ieme et 3ieme matra
  vector<NoteLocator> o1; o1.resize(4);
  o1[0] = NoteLocator(0, 4); o1[1] = NoteLocator(0, 5);
  o1[2] = NoteLocator(0, 6); o1[3] = NoteLocator(0, 7);
  vector<NoteLocator> o2; o2.resize(4);
  o2[0] = NoteLocator(0, 8); o2[1] = NoteLocator(0, 9);
  o2[2] = NoteLocator(0, 10); o2[3] = NoteLocator(0, 11);
  mPartPreviewData.addOrnement(otKrintan, o1);
  mPartPreviewData.addOrnement(otMeend, o2);
  
  //des strokes
  mPartPreviewData.addStroke(0, stDa, 0);
  mPartPreviewData.addStroke(0, stRa, 1);
  mPartPreviewData.addStroke(0, stDa, 2);
  mPartPreviewData.addStroke(0, stRa, 3);
  
  mPartPreviewData.addStroke(0, stDa, 4);
  mPartPreviewData.addStroke(0, stDa, 8);
  
  mPartPreviewData.addStroke(0, stDa, 12);
  mPartPreviewData.addStroke(0, stRa, 13);
  mPartPreviewData.addStroke(0, stDa, 14);
  mPartPreviewData.addStroke(0, stRa, 15);
  
  //un peu de texte
  mPartPreviewData.setBarText(0, "Test string");
  
  mpPartPreview->setComposition( &mPartPreviewData );
  updateUi();
}
//-----------------------------------------------------------------------------
PreferencesDialog::~PreferencesDialog()
{
  if(mpPartPreview)
  {
    delete mpPartPreview;
    mpPartPreview = nullptr;
  }
}
//-----------------------------------------------------------------------------
void PreferencesDialog::fillPageSizeCombo()
{
  mpPageSizeCombo->clear();
  mAvailablePageSizeIds.clear();
  //ajout de letter par defaut...
  mAvailablePageSizeIds.push_back( QPageSize::Letter );
  
  QList<QPrinterInfo> pis = QPrinterInfo::availablePrinters();
  for( int i = 0; i < pis.size(); ++i )
  {
    QList<QPageSize> lps = pis.at(i).supportedPageSizes();
    for( int j = 0; j < lps.size(); ++j )
    { mAvailablePageSizeIds.push_back( lps.at(j).id() ); }
  }
  
  sort( mAvailablePageSizeIds.begin(), mAvailablePageSizeIds.end() );
  mAvailablePageSizeIds.erase( unique( mAvailablePageSizeIds.begin(),
                                      mAvailablePageSizeIds.end() ), mAvailablePageSizeIds.end() );
  
  for( int i = 0; i < mAvailablePageSizeIds.size(); ++i )
  { mpPageSizeCombo->insertItem(i, QPageSize::name( mAvailablePageSizeIds[i] ) ); }
}
//-----------------------------------------------------------------------------
int PreferencesDialog::getFontSize() const
{ return mpFontSize->value(); }
//-----------------------------------------------------------------------------
QPageLayout::Orientation PreferencesDialog::getPageLayout() const
{
  QPageLayout::Orientation o;
  if( mpOrientation->checkedId() == 0 ){ o = QPageLayout::Portrait; }
  else{ o = QPageLayout::Landscape; }
  return o;
}
//-----------------------------------------------------------------------------
QPageSize::PageSizeId PreferencesDialog::getPageSizeId() const
{ return mAvailablePageSizeIds[ mpPageSizeCombo->currentIndex() ]; }
//-----------------------------------------------------------------------------
realisim::sargam::script PreferencesDialog::getScript() const
{ return (realisim::sargam::script)mpScriptCombo->currentIndex(); }
//-----------------------------------------------------------------------------
void PreferencesDialog::initUi()
{
  mpPartPreview = new PartitionViewer( this );
  mpPartPreview->hide();
  
  QVBoxLayout* pMainLyt = new QVBoxLayout( this );
  {
    QGroupBox* pVisualizationGrp = new QGroupBox( "Visualization", this );
    {
      QVBoxLayout* pVLyt = new QVBoxLayout();
      {
        //--- font size
        QHBoxLayout *pFontSizeLyt = new QHBoxLayout();
        {
          QLabel *pName = new QLabel("Font size:", this);
          mpFontSize = new QSpinBox(this);
          mpFontSize->setMinimum(10);
          mpFontSize->setMaximum(60);
          
          pFontSizeLyt->addWidget(pName);
          pFontSizeLyt->addWidget(mpFontSize);
          
          mpFontSize->setValue( mpPartViewer->getFontSize() );
        }
        
        //--- script
        QHBoxLayout *pScriptLyt = new QHBoxLayout();
        {
          QLabel *pName = new QLabel("Script:", this);
          mpScriptCombo = new QComboBox(this);
          mpScriptCombo->insertItem(sLatin, "Latin");
          mpScriptCombo->insertItem(sDevanagari, "देवनागरी");
          
          pScriptLyt->addWidget(pName);
          pScriptLyt->addWidget(mpScriptCombo);
          
          mpScriptCombo->setCurrentIndex( mpPartViewer->getScript() );
        }
        
        //--- part preview
        QHBoxLayout *pPartPreviewLyt = new QHBoxLayout();
        {
          mpPreviewLabel = new QLabel("n/a", this);
          
          pPartPreviewLyt->addStretch(1);
          pPartPreviewLyt->addWidget(mpPreviewLabel, Qt::AlignHCenter);
          pPartPreviewLyt->addStretch(1);
        }
        
        //--- taille du papier
        QHBoxLayout* pPaperLyt = new QHBoxLayout();
        {
          QLabel* pLabel = new QLabel( "Page size: ", this );
          
          mpPageSizeCombo = new QComboBox(this);
          
          pPaperLyt->addWidget(pLabel);
          pPaperLyt->addWidget(mpPageSizeCombo);
          
          fillPageSizeCombo();
          int currrentIndex = distance( mAvailablePageSizeIds.begin(),
                                       std::find( mAvailablePageSizeIds.begin(), mAvailablePageSizeIds.end(),
                                                 mpPartViewer->getPageSizeId() ) );
          mpPageSizeCombo->setCurrentIndex( currrentIndex );
        }
        
        //--- Portrait, landscape
        QHBoxLayout* pOrientationLyt = new QHBoxLayout();
        {
          mpOrientation = new QButtonGroup( this );
          mpPortrait = new QCheckBox( "Portrait", this );
          mpLandscape = new QCheckBox( "Landscape", this );
          mpOrientation->addButton( mpPortrait, 0 );
          mpOrientation->addButton( mpLandscape, 1 );
          
          pOrientationLyt->addStretch(1);
          pOrientationLyt->addWidget(mpPortrait);
          pOrientationLyt->addWidget(mpLandscape);
          
          QPageLayout::Orientation o = mpPartViewer->getLayoutOrientation();
          if( o == QPageLayout::Portrait )
          { mpPortrait->setCheckState( Qt::Checked ); }
          else{ mpLandscape->setCheckState( Qt::Checked ); }
        }
        
        connect(mpFontSize, SIGNAL(valueChanged(int)),
                this, SLOT(updateUi()));
        connect(mpScriptCombo, SIGNAL(activated(int)),
                this, SLOT(updateUi()));
        
        pVLyt->addLayout(pFontSizeLyt);
        pVLyt->addLayout(pScriptLyt);
        pVLyt->addLayout(pPartPreviewLyt);
        pVLyt->addLayout( pPaperLyt );
        pVLyt->addLayout( pOrientationLyt );
      } //vLyt
      
      pVisualizationGrp->setLayout(pVLyt);
    } //groupbox
    
    //--- log
    mpVerboseChkBx = new QCheckBox( "Verbose log", this);
    mpVerboseChkBx->setChecked(mpMainDialog->isVerbose());
    
    //--- show tool bar
    mpShowToolBarChkBx = new QCheckBox( "Show toolbar", this );
    mpShowToolBarChkBx->setChecked(mpMainDialog->isToolBarVisible());
    
    //--- ok, Cancel
    QHBoxLayout* pBottomButLyt = new QHBoxLayout();
    {
      QPushButton* pOk = new QPushButton( "Ok", this );
      connect( pOk, SIGNAL( clicked() ), this, SLOT( accept() ) );
      
      QPushButton* pCancel = new QPushButton( "Cancel", this );
      connect( pCancel, SIGNAL( clicked() ), this, SLOT( reject() ) );
      
      pBottomButLyt->addStretch(1);
      pBottomButLyt->addWidget(pOk);
      pBottomButLyt->addWidget(pCancel);
    }
    
    pMainLyt->addWidget( pVisualizationGrp );
    pMainLyt->addWidget( mpVerboseChkBx );
    pMainLyt->addWidget( mpShowToolBarChkBx );
    pMainLyt->addStretch(1);
    pMainLyt->addLayout( pBottomButLyt );
  }
}
//-----------------------------------------------------------------------------
bool PreferencesDialog::isToolBarVisible() const
{ return mpShowToolBarChkBx->isChecked(); }
//-----------------------------------------------------------------------------
bool PreferencesDialog::isVerbose() const
{ return mpVerboseChkBx->isChecked(); }
//-----------------------------------------------------------------------------
void PreferencesDialog::updateUi()
{
  mpPartPreview->setFontSize( mpFontSize->value() );
  mpPartPreview->setScript( (script)mpScriptCombo->currentIndex() );
  QPixmap pix = QPixmap::fromImage(mpPartPreview->getBarAsImage(0));
  mpPreviewLabel->setPixmap(pix);
}

//------------------------------------------------------------------------------
//--- SaveDialog
//------------------------------------------------------------------------------
SaveDialog::SaveDialog(QWidget *ipParent, QString iFileName) :
QDialog(ipParent),
mAnswer(aCancel),
mFileName(iFileName)
{ createUi(); }
//------------------------------------------------------------------------------
void SaveDialog::cancel()
{
  mAnswer = aCancel;
  reject();
}
//------------------------------------------------------------------------------
void SaveDialog::createUi()
{
  QVBoxLayout *pVlyt = new QVBoxLayout(this);
  {
    QLabel *pLabel = new QLabel(this);
    QString t;
    t.sprintf("Do you want to save the changes you made to %s?",
              mFileName.toStdString().c_str() );
    pLabel->setText(t);
    QFont f = pLabel->font();
    f.setBold(true);
    pLabel->setFont( f );
    
    QLabel *pLabel2 = new QLabel(this);
    pLabel2->setText("Your changes will be lost if you don't save them.");
    
    //-- buttons
    QHBoxLayout *pButtonsLyt = new QHBoxLayout();
    {
      QPushButton *pDontSave = new QPushButton("Don't save", this);
      connect( pDontSave, SIGNAL(clicked()), this, SLOT(dontSave()) );
      
      QPushButton *pSave = new QPushButton("Save", this);
      connect( pSave, SIGNAL(clicked()), this, SLOT(save()) );
      
      QPushButton *pCancel = new QPushButton("Cancel", this);
      connect( pCancel, SIGNAL(clicked()), this, SLOT(cancel()) );
      
      pButtonsLyt->addWidget(pDontSave);
      pButtonsLyt->addStretch(1);
      pButtonsLyt->addWidget(pCancel);
      pButtonsLyt->addWidget(pSave);
    }
    
    pVlyt->addWidget(pLabel);
    pVlyt->addWidget(pLabel2);
    pVlyt->addLayout(pButtonsLyt);
  }
}
//------------------------------------------------------------------------------
void SaveDialog::dontSave()
{
  mAnswer = aDontSave;
  accept();
}
//------------------------------------------------------------------------------
void SaveDialog::save()
{
  mAnswer = aSave;
  accept();
}
//------------------------------------------------------------------------------
SaveDialog::answer SaveDialog::getAnswer() const
{ return mAnswer; }