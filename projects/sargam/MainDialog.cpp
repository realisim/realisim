/* */

#include "MainDialog.h"
#include "PartitionViewer.h"
#include "utils/utilities.h"
#include <QPrintDialog>
#include <QPrintPreviewDialog>
#include <QPrinter>
#include <QPrinterInfo>
#include <set>
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */

using namespace std;
using namespace realisim;
  using namespace utils;
  using namespace sargam;

namespace
{
  const int kSettingVersion = 2;
  const QString kLogFoldername("sargamLogs");
}

//-----------------------------------------------------------------------------
// --- customProxyStyle
//-----------------------------------------------------------------------------
void CustomProxyStyle::drawPrimitive(PrimitiveElement pe,
  const QStyleOption* ipSo, QPainter* ipP, const QWidget* ipW) const
{
  const ThinLineEdit* tle = dynamic_cast<const ThinLineEdit*>(ipW);
  if(tle)
  {
    switch(pe)
    {
      case PE_PanelLineEdit:
      {
        ipP->setRenderHints( ipP->renderHints() | QPainter::Antialiasing );
        QPen p(QColor( 0, 10, 210, 120 ));
        ipP->setPen(p);
        QRect r = tle->rect();
        r.adjust(1,1,-1,-1);
        ipP->drawRoundRect(r, 2, 2);
      } break;
      default: QProxyStyle::drawPrimitive(pe, ipSo, ipP, ipW); break;
    }
  }
  else
  { QProxyStyle::drawPrimitive(pe, ipSo, ipP, ipW); }
}
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

//-----------------------------------------------------------------------------
// --- MAIN WINDOW
//-----------------------------------------------------------------------------
MainDialog::MainDialog() : QMainWindow(),
  mpScrollArea(0),
  mpPartitionViewer(0),
  mpUpdater(0),
  mSettings( QSettings::UserScope, "Realisim", "Sargam" ),
  mLog(),
  mIsVerbose( false ),
  mIsToolBarVisible(true),
  mState( sNormal )
{
  createUi();
  
  if(isVerbose()) 
  {
    mLog.log( "Sargam started. version: %s", getVersionAsQString().
      toStdString().c_str() );
  }
  mpPartitionViewer->setLog( &mLog );
  
  //--- init viewer
  loadSettings();
  newFile();
  
  resize( mpPartitionViewer->width() + mpToolBar->width() + 35, 800 );
  
  //check pour les updates...
  mpUpdater = new Updater(this);
  connect(mpUpdater, SIGNAL(updateInformationAvailable()),
          this, SLOT(handleUpdateAvailability()));
  mpUpdater->checkForUpdate();
}
//-----------------------------------------------------------------------------
void MainDialog::applyPrinterOptions( QPrinter* iP )
{
  //appliquer les configurations de visualisation a l'imprimante
  QPageSize ps = QPageSize( mpPartitionViewer->getPageSizeId() );
  bool pageSizeAccepted = iP->setPageSize( ps );
  
  QPageLayout::Orientation o = mpPartitionViewer->getLayoutOrientation();
  /*la marge ici sert a definir la region imprimable. 12 points correspond
   environs a 5 mm. Il est nécessaire de mettre une marge valide pour que
   setPageLayout fonctionne. Cette marge n'est pas très importante car le
   widget PartitionViewer s'occupe de mettre une marge.*/
  QMarginsF margins( 12, 12, 12, 12 );
  QPageLayout pl( ps, o, margins, QPageLayout::Point );
  bool pageLayoutAccepted = iP->setPageLayout( pl );
  
  if( !pageSizeAccepted )
  { getLog().log( "Page size could not be accepted by current printer." ); }
  if( !pageLayoutAccepted )
  { getLog().log( "Page layout could not be accepted by current printer."); }
}
//-----------------------------------------------------------------------------
void MainDialog::createUi()
{
  QWidget* pMainWidget = new QWidget( this );
  setCentralWidget(pMainWidget);
  
  //--- la barre de menu
  //--- file
  QMenuBar* pMenuBar = new QMenuBar(this);
  setMenuBar( pMenuBar );
  QMenu* pFile = pMenuBar->addMenu("File");
  pFile->addAction( QString("&New"), this, SLOT( newFile() ),
                   QKeySequence::New );
  
  pFile->addAction( QString("&Open..."), this, SLOT( openFile() ),
                   QKeySequence::Open );
  pFile->addAction( QString("&Save"), this, SLOT( save() ),
                   QKeySequence::Save );
  pFile->addAction( QString("Save As..."), this, SLOT( saveAs() ),
                   QKeySequence::SaveAs );
  pFile->addSeparator();
  pFile->addAction( QString("Print..."), this, SLOT( print() ),
                   QKeySequence::Print );
  pFile->addAction( QString("Print preview..."), this, SLOT( printPreview() ) );
  pFile->addAction( QString("About..."), this, SLOT( about() ) );
  
  //--- tool bar
  createToolBar();
  
  //--- preferences
  QMenu* pPreferences = pMenuBar->addMenu("Preferences");
  pPreferences->addAction( "Options...", this, SLOT( preferences() ) );
  
  //debug action
  QShortcut* pRandomPart = new QShortcut( QKeySequence( Qt::CTRL + Qt::SHIFT + Qt::Key_G), this );
  connect( pRandomPart, SIGNAL(activated()), this, SLOT(generateRandomPartition()) );
  
  QShortcut* pDebugD = new QShortcut( QKeySequence( Qt::CTRL + Qt::SHIFT + Qt::Key_D), this );
  connect( pDebugD, SIGNAL(activated()), this, SLOT(toggleDebugging()) );
  QShortcut* pDebugT = new QShortcut( QKeySequence( Qt::CTRL + Qt::SHIFT + Qt::Key_T), this );
  connect( pDebugT, SIGNAL(activated()), this, SLOT(toggleLogTiming()) );

  
  //--- le reste du Ui
  QHBoxLayout* pLyt = new QHBoxLayout(pMainWidget);
  pLyt->setMargin(0);
  pLyt->setSpacing(0);
  
  mpScrollArea = new QScrollArea( pMainWidget );
  mpScrollArea->setAlignment( Qt::AlignHCenter );
  {
    mpPartitionViewer = new PartitionViewer( pMainWidget );
    mpPartitionViewer->setFocus();
    connect( mpPartitionViewer, SIGNAL( ensureVisible(QPoint) ),
            this, SLOT( ensureVisible(QPoint) ) );
    connect( mpPartitionViewer, SIGNAL( interactionOccured() ),
            this, SLOT( updateUi() ) );
    
    mpScrollArea->setWidget( mpPartitionViewer );
  }
  pLyt->addWidget( mpScrollArea );
}
//-----------------------------------------------------------------------------
void MainDialog::about()
{
  QDialog d( this );
  
  QVBoxLayout* pMainLyt = new QVBoxLayout( &d );
  pMainLyt->setMargin(2); pMainLyt->setSpacing(2);
  {
    //Verbatim
    QString s;
    s.sprintf(
      "<div align='center'> <b>Sargam</b> </div>"
      "Version: %s"
      "<p>The intention behind this product is to ease learning and sharing of the<br>"
      "sitar. Please share your compositions/transcriptions.</p>"
      "<p>Original idea and design by Pierre-Olivier Beaudoin (no, I am not the<br>"
      "male model you will find googling!).</p>"
      "Many thanks to the following contributors:"
      "<ul>"
        "<li>Lars Jacobsen from http://raincitymusic.com/</li>"
        "<li>Shivraj Sawant and his poject http://vishwamohini.com/</li>"
        "<li>www.reddit.com/r/sitar</li>"
      "</ul>"
      "A special thanks to generous donators who make this project possible:"
      "<ul>"
        "<li>Wallace Thompson</li>"
      "</ul>"
      "For any information regarding the software contact us at "
      "sargam.software@gmail.com\n\n", getVersionAsQString().toStdString().c_str() );
    
    QLabel* pVerbatim = new QLabel(this);
    pVerbatim->setText(s);
    
    //--- close
    QHBoxLayout* pBottomButLyt = new QHBoxLayout();
    {
      QPushButton* pClose = new QPushButton( "Close", &d );
      connect( pClose, SIGNAL( clicked() ), &d, SLOT( accept() ) );
      
      pBottomButLyt->addStretch(1);
      pBottomButLyt->addWidget(pClose);
    }
    
    pMainLyt->addWidget(pVerbatim);
    pMainLyt->addStretch(1);
    pMainLyt->addLayout( pBottomButLyt );
  }
  
  d.exec();
}
//-----------------------------------------------------------------------------
void MainDialog::createToolBar()
{
  mpToolBar = new QToolBar( "Tools", this );
  connect( mpToolBar, SIGNAL( actionTriggered(QAction*) ),
          this, SLOT( toolActionTriggered(QAction*) ) );
  
  mpToolBar->setMovable( false );
  addToolBar(Qt::LeftToolBarArea, mpToolBar);
  mpToolBar->setOrientation( Qt::Vertical );
  
  {
    QAction* a = new QAction( "add bar", this );
    a->setToolTip("Adds a bar after the current bar.<br>"
                  "<B>(spacebar)</B>");
    mActions[ aAddBar ] = a;
    mpToolBar->addAction( mActions[aAddBar] );
  }
  {
    QAction* a = new QAction( "line jump", this );
    a->setToolTip("Current bar goes to next line.<br>"
                  "<B>(enter)</B>");
    mActions[ aLineJump ] = a;
    mpToolBar->addAction( mActions[aLineJump] );
  }
  {
    QAction* a = new QAction( "matra", this );
    a->setToolTip("Makes a matra with the current selection.<br>"
                  "<B>(M)</B>");
    mActions[ aAddMatra ] = a;
    mpToolBar->addAction( mActions[aAddMatra] );
  }
  {
    QAction* a = new QAction( "remove matra", this );
    a->setToolTip("Removes matra on the selection<br>"
                  "<B>(shift+M)</B>");
    mActions[ aRemoveMatra ] = a;
    mpToolBar->addAction( mActions[aRemoveMatra] );
  }
  {
    QAction* a = new QAction( "krintan", this );
    a->setToolTip("Adds a krintan over the selection.<br>"
                  "<B>(K)</B>");
    mActions[ aAddKrintan ] = a;
    mpToolBar->addAction( mActions[aAddKrintan] );
  }
  {
    QAction* a = new QAction( "meend", this );
    a->setToolTip("Adds a meend over the selection.<br>"
                  "<B>(B)</B>");
    mActions[ aAddMeend ] = a;
    mpToolBar->addAction( mActions[aAddMeend] );
  }
  {
    QAction* a = new QAction( "gamak", this );
    a->setToolTip("Adds a gamak over the selection.<br>"
                  "<B>(N)</B>");
    mActions[ aAddGamak ] = a;
    mpToolBar->addAction( mActions[aAddGamak] );
  }
  {
    QAction* a = new QAction( "andolan", this );
    a->setToolTip("Adds an andolan over the selection.<br>"
                  "<B>(A)</B>");
    mActions[ aAddAndolan ] = a;
    mpToolBar->addAction( mActions[aAddAndolan] );
  }
  {
    QAction* a = new QAction( "remove ornement", this );
    a->setToolTip("Removes krintan|meend|etc.. from the selection.<br>"
                  "<B>(shift+K or shift+M)</B>");
    mActions[ aRemoveOrnement ] = a;
    mpToolBar->addAction( mActions[aRemoveOrnement] );
  }
  {
    QAction* a = new QAction( "grace note", this );
    a->setToolTip("Selected notes become grace notes.<br>"
                  "<B>(G)</B>");
    mActions[ aAddGraceNote ] = a;
    mpToolBar->addAction( mActions[aAddGraceNote] );
  }
  {
    QAction* a = new QAction( "remove grace note", this );
    a->setToolTip("Selected notes become normal notes.<br>"
                  "<B>(shift+G)</B>");
    mActions[ aRemoveGraceNote ] = a;
    mpToolBar->addAction( mActions[aRemoveGraceNote] );
  }
  {
    QAction* a = new QAction( "add parenthesis", this );
    a->setToolTip("Embraces selected notes in parenthesis.<br>"
                  "<B>(P)</B>");
    mActions[ aAddParenthesis ] = a;
    mpToolBar->addAction( mActions[aAddParenthesis] );
  }
  {
    QAction* a = new QAction( "remove parenthesis", this );
    a->setToolTip("Removes parenthesis around current notes.<br>"
                  "<B>(shift+P)</B>");
    mActions[ aRemoveParenthesis ] = a;
    mpToolBar->addAction( mActions[aRemoveParenthesis] );
  }
  {
    QAction* a = new QAction( "increase octave", this );
    a->setToolTip("Increase octave on current note.<br>"
                  "<B>(+)</B>");
    mActions[ aIncreaseOctave ] = a;
    mpToolBar->addAction( mActions[aIncreaseOctave] );
  }
  {
    QAction* a = new QAction( "decrease octave", this );
    a->setToolTip("Decrease octave on current note.<br>"
                  "<B>(-)</B>");
    mActions[ aDecreaseOctave ] = a;
    mpToolBar->addAction( mActions[aDecreaseOctave] );
  }
  {
    QAction* a = new QAction( "rest", this );
    a->setToolTip("Inserts a rest.<br>"
                  "<B>(R)</B>");
    mActions[ aRest ] = a;
    mpToolBar->addAction( mActions[aRest] );
  }
  {
    QAction* a = new QAction( "chik", this );
    a->setToolTip("Inserts a chik.<br>"
                  "<B>(C)</B>");
    mActions[ aChik ] = a;
    mpToolBar->addAction( mActions[aChik] );
  }
  {
    QAction* a = new QAction( "phrasing", this );
    a->setToolTip("Inserts a comma for phrasing.<br>"
                  "<B>(,)</B>");
    mActions[ aPhrasing ] = a;
    mpToolBar->addAction( mActions[aPhrasing] );
  }
  {
    QAction* a = new QAction( "komal", this );
    a->setToolTip("Makes current note komal.<br>"
                  "<B>(S)</B>");
    mActions[ aKomal ] = a;
    mpToolBar->addAction( mActions[aKomal] );
  }
  {
    QAction* a = new QAction( "shuddh", this );
    a->setToolTip("Makes current note shuddh.<br>"
                  "<B>(S)</B>");
    mActions[ aShuddh ] = a;
    mpToolBar->addAction( mActions[aShuddh] );
  }
  {
    QAction* a = new QAction( "tivra", this );
    a->setToolTip("Makes current note tivra.<br>"
                  "<B>(S)</B>");
    mActions[ aTivra ] = a;
    mpToolBar->addAction( mActions[aTivra] );
  }
  {
    QAction* a = new QAction( "da", this );
    a->setToolTip("Adds a Da stroke to selection.<br>"
                  "<B>(Q)</B>");
    mActions[ aDa ] = a;
    mpToolBar->addAction( mActions[aDa] );
  }
  {
    QAction* a = new QAction( "ra", this );
    a->setToolTip("Adds a Ra stroke to selection.<br>"
                  "<B>(W)</B>");
    mActions[ aRa ] = a;
    mpToolBar->addAction( mActions[aRa] );
  }
  {
    QAction* a = new QAction( "diri", this );
    a->setToolTip("Adds a Diri stroke to selection.<br>"
                  "<B>(E)</B>");
    mActions[ aDiri ] = a;
    mpToolBar->addAction( mActions[aDiri] );
  }
  {
    QAction* a = new QAction( "remove stroke", this );
    a->setToolTip("Removes stroke from the selection.<br>"
                  "<B>(shift+Q or shift+W or shift+E)</B>");
    mActions[ aRemoveStroke ] = a;
    mpToolBar->addAction( mActions[aRemoveStroke] );
  }
  
}
//-----------------------------------------------------------------------------
void MainDialog::ensureVisible( QPoint p )
{ mpScrollArea->ensureVisible( p.x(), p.y() ); }
//-----------------------------------------------------------------------------
MainDialog::action MainDialog::findAction( QAction* ipA ) const
{
  action r = aUnknown;
  map<action, QAction*>::const_iterator it = mActions.begin();
  for( ; it != mActions.end(); ++it )
  { if( it->second == ipA ){ r = it->first; break; } }
  return r;
}
//-----------------------------------------------------------------------------
void MainDialog::generatePrintPreview( QPrinter* iP )
{ mpPartitionViewer->print( iP ); }
//-----------------------------------------------------------------------------
void MainDialog::generateRandomPartition()
{
  mpPartitionViewer->generateRandomPartition();
  
  if( isVerbose() )
  { getLog().log( "MainDialog: random partition generated." ); }
}
//-----------------------------------------------------------------------------
QString MainDialog::getVersionAsQString() const
{
   return QString::number( getVersionMajor() ) + "." +
     QString::number( getVersionMinor() ) + "." +
     QString::number( getVersionRevision() );
}
//-----------------------------------------------------------------------------
MainDialog::state MainDialog::getState() const
{ return mState; }
//-----------------------------------------------------------------------------
void MainDialog::handleUpdateAvailability()
{
  const QString currentVersion = getVersionAsQString();
  for( int i = 0; i < mpUpdater->getNumberOfVersions(); i++ )
  {
    if( currentVersion < mpUpdater->getVersionAsQString(i) )
    { setState( sUpdatesAreAvailable ); break; }
  }
}
//-----------------------------------------------------------------------------
bool MainDialog::isVerbose() const
{ return mIsVerbose; }
//-----------------------------------------------------------------------------
void MainDialog::loadSettings()
{
  int version = mSettings.value( "version", 1 ).toInt();
	mLastSavePath = mSettings.value( "lastSavePath" ).toString();
  
  int defaultFontSize = 14;
#ifdef _WIN32
  defaultFontSize = 11;
#endif
  //---view
  int psi = mSettings.value( "view/pageSizeId", QPageSize::Letter ).toInt();
  int plo = mSettings.value( "view/pageLayoutOrientation", QPageLayout::Portrait ).toInt();
  int pScript = mSettings.value( "view/script", (int)sLatin ).toInt();
  int fontSize = mSettings.value( "view/fontSize", defaultFontSize ).toInt();
  mpPartitionViewer->setPageSize( (QPageSize::PageSizeId)psi );
  mpPartitionViewer->setLayoutOrientation( (QPageLayout::Orientation)plo );
  mpPartitionViewer->setScript( (script)pScript );
  mpPartitionViewer->setFontSize(fontSize);
  
  //--- log
  bool v = mSettings.value( "verboseLog", false ).toBool();
  setAsVerbose( v );
  
  //--- toolBar visibility
  bool tbv = mSettings.value( "isToolBarVisible", true ).toBool();
  setToolBarVisible( tbv );
  
  if( isVerbose() )
  { getLog().log( "MainDialog: settings loaded." ); }
}
//-----------------------------------------------------------------------------
void MainDialog::newFile()
{
  //check modif and save...
  mSaveFileName = QString();
  
  mComposition = Composition();
//!!! les 3 lignes suivantes devraient etre dans le constructeur de Composition
  mComposition.setTitle( "Untitled" );
  mComposition.addBar();
  mComposition.addLine(0);
  mpPartitionViewer->setComposition( &mComposition );
  
  updateUi();
  
  if( isVerbose() )
  { getLog().log( "MainDialog: new file." ); }
}
//-----------------------------------------------------------------------------
void MainDialog::openFile()
{
 	QString s;
  s = QFileDialog::getOpenFileName(
    this, tr("Open Composition"),
    mLastSavePath,
    tr("Sargam file (*.srg)"));
  
  if(!s.isEmpty())
  {
    mSaveFileName = s;
    mLastSavePath = QFileInfo(s).absolutePath();
    mComposition.fromBinary( utils::fromFile( s ) );
    saveSettings();
    
    if( !mComposition.hasError() )
    {
      mpPartitionViewer->setComposition( &mComposition );
      
      if( isVerbose() )
      { getLog().log( "MainDialog: file %s opened.", s.toStdString().c_str() ); }
    }
    else
    {
      getLog().log( "Errors while opening file %s: %s.", s.toStdString().c_str(),
        mComposition.getAndClearLastErrors().toStdString().c_str() );
      newFile();
    }
  }
}
//-----------------------------------------------------------------------------
void MainDialog::preferences()
{
  PreferencesDialog pd(this, mpPartitionViewer, this);
  
  if (pd.exec() == QDialog::Accepted)
  {
    //font size
    mpPartitionViewer->setFontSize( pd.getFontSize() );
    
    //script
    mpPartitionViewer->setScript( pd.getScript() );
    
    //pageSize
    mpPartitionViewer->setPageSize( pd.getPageSizeId() );
    
    //layout orientation
    mpPartitionViewer->setLayoutOrientation( pd.getPageLayout() );
    
    //log
    setAsVerbose( pd.isVerbose() );
    
    //tool bar visibility
    setToolBarVisible( pd.isToolBarVisible() );
    
    saveSettings();
    updateUi();
  }
  
  if( isVerbose() )
  { getLog().log( "MainDialog: view options closed." ); }
}
//-----------------------------------------------------------------------------
void MainDialog::print()
{
  QPrinter p;
  applyPrinterOptions( &p );
  
  QPrintDialog d( &p, this );
  if (d.exec() == QDialog::Accepted)
  { mpPartitionViewer->print( &p ); }
  
  if( isVerbose() )
  { getLog().log( "MainDialog: print."); }
}
//-----------------------------------------------------------------------------
void MainDialog::printPreview()
{
  QPrinter p;
  applyPrinterOptions( &p );
  
  QPrintPreviewDialog d( &p, this );
  connect( &d, SIGNAL( paintRequested(QPrinter*) ),
          this, SLOT( generatePrintPreview(QPrinter*) ) );
  if (d.exec() == QDialog::Accepted)
  { mpPartitionViewer->print( &p ); }
  disconnect(&d, SIGNAL( paintRequested(QPrinter*) ),
             this, SLOT( generatePrintPreview(QPrinter*) ) );
  
  if( isVerbose() )
  { getLog().log( "MainDialog: print preview."); }
}
//-----------------------------------------------------------------------------
void MainDialog::save()
{
	if( mSaveFileName.isEmpty() )
  { saveAs(); }
  else
  {
    utils::toFile( mSaveFileName,
      mpPartitionViewer->getComposition().toBinary() );
  }
  updateUi();
  
  if( isVerbose() )
  { getLog().log( "MainDialog: saved." ); }
}

//-----------------------------------------------------------------------------
void MainDialog::saveAs()
{
	QString s;
  s = QFileDialog::getSaveFileName(
    this, tr("Save Composition"),
    mLastSavePath + "/untitled.srg",
    tr("Sargam file (*.srg)"));
      
  if(!s.isEmpty())
  {
  	mSaveFileName = s;
    mLastSavePath = QFileInfo(s).absolutePath();
    saveSettings();
    save();
  }
  updateUi();
  
  if( isVerbose() )
  { getLog().log( "MainDialog: save as %s.", mSaveFileName.toStdString().c_str() ); }
}
//-----------------------------------------------------------------------------
void MainDialog::saveSettings()
{
  mSettings.setValue( "version", kSettingVersion );
	mSettings.setValue( "lastSavePath", mLastSavePath );
  
  //---view options
	mSettings.setValue( "view/pageSizeId", mpPartitionViewer->getPageSizeId() );
  mSettings.setValue( "view/pageLayoutOrientation", mpPartitionViewer->getLayoutOrientation() );
  mSettings.setValue( "view/script", mpPartitionViewer->getScript() );
  mSettings.setValue( "view/fontSize", mpPartitionViewer->getFontSize() );

  //--- log
  mSettings.setValue( "verboseLog", isVerbose() );
  
  //---tool bar visibility
  mSettings.setValue( "isToolBarVisible", isToolBarVisible() );
  
  if( isVerbose() )
  { getLog().log( "MainDialog: save settings." ); }
}
//-----------------------------------------------------------------------------
void MainDialog::setAsVerbose( bool iV )
{
  mIsVerbose = iV;
  mpPartitionViewer->setAsVerbose( iV );
  
  if(isVerbose())
  {
     //--- init log  
     QDir logDir( "./"+kLogFoldername );
     bool canLogToFile = true;
     if( !logDir.exists() )
     { canLogToFile = QDir(".").mkdir( kLogFoldername ); }

     if(canLogToFile)
     {
        mLog.logToFile( true, kLogFoldername+"/" + QDateTime::currentDateTime().toString(
           "yyyy-MM-dd_hh_mm_ss" ) + ".txt" );
     }
     else
     {
        getLog().log( "Impossible to log to file. Could not create directory "
           "'sargamLogs'. Try creating it manually beside the executables." );
     }
    
  }

  getLog().log( "verbose set to: %s", iV?"true":"false" );
}
//-----------------------------------------------------------------------------
void MainDialog::setState(state s)
{ if(s != getState()) { mState = s; updateUi(); } }
//-----------------------------------------------------------------------------
void MainDialog::showUpdateDialog()
{
  QDialog d(this, Qt::WindowTitleHint);
  d.resize(540, 240);
  d.setWindowTitle("New version is available");
  d.setWindowModality(Qt::ApplicationModal);
  QVBoxLayout *pVlyt = new QVBoxLayout(&d);
  pVlyt->setMargin(0); pVlyt->setSpacing(2);
  {
    QTextEdit *pTextEdit = new QTextEdit(&d);
    pTextEdit->setReadOnly(true);
    
    //populate the text edit
    QString t;
    t += "You are currently using version: " + getVersionAsQString() + "<br>";
    t += "Click on 'Visit web site' to access the download links from your "
      "favorite browser.";
    for( int i = 0; i < mpUpdater->getNumberOfVersions(); ++i )
    {
      if( getVersionAsQString() < mpUpdater->getVersionAsQString(i) )
      {
        t += "<p>";
        t += "<b>Version: " + mpUpdater->getVersionAsQString(i)+"</b><br>";
        t += mpUpdater->getReleaseNotes(i);
        t += "</p>";
      }
    }
    pTextEdit->setText(t);
    
    //add cancel and visit web site button
    QHBoxLayout *pButLyt = new QHBoxLayout();
    {
      QPushButton *pCancel = new QPushButton("Cancel", &d);
      connect(pCancel, SIGNAL(clicked()), &d, SLOT(reject()) );
              
      QPushButton *pVisitWebSite = new QPushButton("Visit web site", &d);
      connect(pVisitWebSite, SIGNAL(clicked()), &d, SLOT(accept()) );
      
      pButLyt->addStretch(1);
      pButLyt->addWidget(pCancel);
      pButLyt->addWidget(pVisitWebSite);
    }
    
    pVlyt->addWidget(pTextEdit);
    pVlyt->addLayout(pButLyt);
  }

  if( d.exec() == QDialog::Accepted )
  {
    QDesktopServices::openUrl(QUrl( mpUpdater->getDownloadPage() ));
  }
  setState(sNormal);
}
//-----------------------------------------------------------------------------
void MainDialog::toggleDebugging()
{
  mpPartitionViewer->setAsDebugging( !mpPartitionViewer->isDebugging() );
  getLog().log( "MainDialog: debugging toggled to %s.", mpPartitionViewer->isDebugging()?"true":"false" );
}
//-----------------------------------------------------------------------------
void MainDialog::toggleLogTiming()
{
  mpPartitionViewer->setLogTiming( !mpPartitionViewer->hasLogTiming() );
  getLog().log( "MainDialog: logTiming toggled to %s.", mpPartitionViewer->hasLogTiming()?"true":"false" );
}
//-----------------------------------------------------------------------------
void MainDialog::toolActionTriggered(QAction* ipA)
{
  action a = findAction( ipA );
  switch (a)
  {
    case aAddBar: mpPartitionViewer->commandAddBar(); break;
    case aLineJump: mpPartitionViewer->commandAddLine(); break;
    case aAddMatra: mpPartitionViewer->commandAddMatra(); break;
    case aRemoveMatra: mpPartitionViewer->commandBreakMatrasFromSelection(); break;
    case aAddKrintan: mpPartitionViewer->commandAddOrnement( otKrintan ); break;
    case aAddMeend: mpPartitionViewer->commandAddOrnement( otMeend ); break;
    case aAddGamak: mpPartitionViewer->commandAddOrnement( otGamak ); break;
    case aAddAndolan: mpPartitionViewer->commandAddOrnement( otAndolan ); break;
    case aRemoveOrnement: mpPartitionViewer->commandBreakOrnementsFromSelection(); break;
    case aAddGraceNote: mpPartitionViewer->commandAddGraceNotes(); break;
    case aRemoveGraceNote: mpPartitionViewer->commandRemoveSelectionFromGraceNotes(); break;
    case aAddParenthesis: mpPartitionViewer->commandAddParenthesis(2); break;
    case aRemoveParenthesis: mpPartitionViewer->commandRemoveParenthesis(); break;
    case aDecreaseOctave: mpPartitionViewer->commandDecreaseOctave(); break;
    case aIncreaseOctave: mpPartitionViewer->commandIncreaseOctave(); break;
    case aRest: mpPartitionViewer->commandAddNote( nvRest ); break;
    case aChik: mpPartitionViewer->commandAddNote( nvChik ); break;
    case aPhrasing: mpPartitionViewer->commandAddNote( nvComma ); break;
    case aTivra: mpPartitionViewer->commandShiftNote(); break;
    case aShuddh: mpPartitionViewer->commandShiftNote(); break;
    case aKomal: mpPartitionViewer->commandShiftNote(); break;
    case aDa: mpPartitionViewer->commandAddStroke( stDa ); break;
    case aRa: mpPartitionViewer->commandAddStroke( stRa ); break;
    case aDiri: mpPartitionViewer->commandAddStroke( stDiri ); break;
    case aRemoveStroke: mpPartitionViewer->commandRemoveStroke(); break;
    default: getLog().log( "Unknown tool action triggered." ); break;
  }
  updateUi();
}
//-----------------------------------------------------------------------------
void MainDialog::updateActions()
{
  PartitionViewer* x = mpPartitionViewer;
  
  /*Les barres sous zéro sont les barres speciales et le ui est plus limité*/
  if( x->getCurrentBar() >= 0 )
  {
    mActions[aAddBar]->setEnabled(true);
    mActions[aLineJump]->setEnabled(true);
    mActions[aRest]->setEnabled(true);
    mActions[aChik]->setEnabled(true);
    mActions[aPhrasing]->setEnabled(true);
    if( x->hasSelection() )
    {
      bool canBreakMatra = true;
      bool canBreakOrnement = true;
      bool canRemoveGraceNote = true;
      bool canRemoveParenthesis = true;
      bool canRemoveStroke = true;
      for( int i = 0; i < x->getNumberOfSelectedNote(); ++i )
      {
        NoteLocator nl = x->getSelectedNote( i );
        canBreakMatra &= mComposition.isNoteInMatra( nl.getBar(), nl.getIndex() );
        canBreakOrnement &= mComposition.isNoteInOrnement( nl.getBar(), nl.getIndex() );
        canRemoveGraceNote &= mComposition.isGraceNote( nl.getBar(), nl.getIndex() );
        canRemoveParenthesis &= mComposition.isNoteInParenthesis( nl.getBar(), nl.getIndex() );
        canRemoveStroke &= mComposition.hasStroke( nl.getBar(), nl.getIndex() );
      }
      //matras
      mActions[aRemoveMatra]->setEnabled(canBreakMatra);
      mActions[aAddMatra]->setEnabled(!canBreakMatra);
      
      //ornements
      mActions[aRemoveOrnement]->setEnabled(canBreakOrnement);
      mActions[aAddKrintan]->setEnabled(!canBreakOrnement);
      mActions[aAddMeend]->setEnabled(!canBreakOrnement);
      mActions[aAddGamak]->setEnabled(!canBreakOrnement);
      mActions[aAddAndolan]->setEnabled(!canBreakOrnement);
      
      //graceNote
      mActions[aRemoveGraceNote]->setEnabled(canRemoveGraceNote);
      mActions[aAddGraceNote]->setEnabled(!canRemoveGraceNote);
      
      //parenthesis
      mActions[aRemoveParenthesis]->setEnabled(canRemoveParenthesis);
      mActions[aAddParenthesis]->setEnabled(!canRemoveParenthesis);
      
      //strokes
      mActions[aDa]->setEnabled(true);
      mActions[aRa]->setEnabled(true);
      mActions[aDiri]->setEnabled(true);
      mActions[ aRemoveStroke ]->setEnabled(canRemoveStroke);
    }
    else
    {
      Note n = mComposition.getNote( x->getCurrentBar(), x->getCurrentNote() );
      
      mActions[aIncreaseOctave]->setEnabled( n.getOctave() < 1 );
      mActions[aDecreaseOctave]->setEnabled( n.getOctave() > -1 );
      
      mActions[aKomal]->setEnabled( n.canBeKomal() );
      mActions[aShuddh]->setEnabled( n.getModification() != nmShuddh );
      mActions[aTivra]->setEnabled( n.canBeTivra() );
      
      //strokes
      mActions[aDa]->setEnabled(true);
      mActions[aRa]->setEnabled(true);
      mActions[aDiri]->setEnabled(true);
      
      bool canRemoveStroke = mComposition.hasStroke( x->getCurrentBar() , x->getCurrentNote() );
      mActions[ aRemoveStroke ]->setEnabled(canRemoveStroke);
    }
  }
  else //Ui reduit pour barres speciales
  {
    Note n = mComposition.getNote( x->getCurrentBar(), x->getCurrentNote() );
    
    mActions[aIncreaseOctave]->setEnabled( n.getOctave() < 1 );
    mActions[aDecreaseOctave]->setEnabled( n.getOctave() > -1 );
    
    mActions[aKomal]->setEnabled( n.canBeKomal() );
    mActions[aShuddh]->setEnabled( n.getModification() != nmShuddh );
    mActions[aTivra]->setEnabled( n.canBeTivra() );
  }
}
//-----------------------------------------------------------------------------
void MainDialog::updateUi()
{
  mpToolBar->setVisible( isToolBarVisible() );
  
  //disable everything...
  for( int i = 0; i < aUnknown; ++i )
  { mActions[ (action)i ]->setEnabled(false); }
  
  
  switch (getState())
  {
    case sNormal: updateActions(); break;
    case sUpdatesAreAvailable: showUpdateDialog(); break;
    default: break;
  }
}

