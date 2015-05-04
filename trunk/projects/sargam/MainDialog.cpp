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
  const int kSettingVersion = 1;
}

//-----------------------------------------------------------------------------
// --- MAIN WINDOW
//-----------------------------------------------------------------------------
MainDialog::MainDialog() : QMainWindow(),
  mpScrollArea(0),
  mpPartitionViewer(0),
  mSettings( QSettings::UserScope, "Realisim", "Sargam" ),
  mLog(),
  mIsVerbose( false ),
  mIsToolBarVisible(true)
{
  createUi();
  
  //--- init log
  QDir logDir( "./logs" );
  bool canLogToFile = true;
  if( !logDir.exists() )
  {
    canLogToFile = QDir(".").mkdir( "logs" );
  }
  if( canLogToFile )
  {
    mLog.logToFile( true, "logs/" + QDateTime::currentDateTime().toString(
      "yyyy-MM-dd_hh_mm_ss" ) + ".txt" );
  }
  else
  {
    getLog().log( "Impossible to log to file. Could not create directory "
                     "'logs'. Try creating it manually beside the executables." );
  }
  mLog.log( "Sargam started. version %d", getVersion() );
  mpPartitionViewer->setLog( &mLog );
  
  //--- init viewer
  loadSettings();
  newFile();
  
  //!!! pour le beta, on met a verbose...
  setAsVerbose( true );
  
  resize( mpPartitionViewer->width() + mpToolBar->width() + 35, 800 );
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
  QMenu* pPreferences = pMenuBar->addMenu("pPreferences");
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
      "Version: %d"
      "<p>The intention behind this product is to ease learning and sharing of the<br>"
      "sitar. Please share your compositions/transcriptions.</p>"
      "<p>Original idea and design by Pierre-Olivier Beaudoin (no, I am not the<br>"
      "male model you will find googling!).</p>"
      "Many thanks to the following contributors:"
      "<ul>"
        "<li>Lars Jacobsen</li>"
        "<li>www.reddit.com/r/sitar</li>"
      "</ul>"
      "For any information regarding the software contact us at "
      "sargam.software@gmail.com\n\n", getVersion() );
    
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
                  "<B>(K)</B>");
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
    QAction* a = new QAction( "remove ornement", this );
    a->setToolTip("Removes krintan|meend from the selection.<br>"
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
void MainDialog::fillPageSizeCombo( QComboBox* ipC )
{
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
  { ipC->insertItem(i, QPageSize::name( mAvailablePageSizeIds[i] ) ); }
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
bool MainDialog::isVerbose() const
{ return mIsVerbose; }
//-----------------------------------------------------------------------------
void MainDialog::loadSettings()
{
  int version = mSettings.value( "version", 1 ).toInt();
	mLastSavePath = mSettings.value( "lastSavePath" ).toString();
  
  //---view
  int psi = mSettings.value( "view/pageSizeId", QPageSize::Letter ).toInt();
  int plo = mSettings.value( "view/pageLayoutOrientation", QPageLayout::Portrait ).toInt();
  mpPartitionViewer->setPageSize( (QPageSize::PageSizeId)psi );
  mpPartitionViewer->setLayoutOrientation( (QPageLayout::Orientation)plo );
  
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
    this, tr("Opem Composition"),
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
  QDialog d( this );
  QComboBox* pPageSizeCombo;
  QButtonGroup* pOrientation;
  QCheckBox* pVerboseChkBx;
  QCheckBox* pShowToolBarChkBx;
  
  QVBoxLayout* pMainLyt = new QVBoxLayout( &d );
  {
    QGroupBox* pVisualizationGrp = new QGroupBox( "Visualization", &d );
    {
      QVBoxLayout* pVLyt = new QVBoxLayout();
      {
        //--- taille du papier
        QHBoxLayout* pPaperLyt = new QHBoxLayout();
        {
          QLabel* pLabel = new QLabel( "Page size: ", &d );
          
          pPageSizeCombo = new QComboBox(&d);
          fillPageSizeCombo( pPageSizeCombo );
          int currrentIndex = distance( mAvailablePageSizeIds.begin(),
                                       std::find( mAvailablePageSizeIds.begin(), mAvailablePageSizeIds.end(),
                                                 mpPartitionViewer->getPageSizeId() ) );
          pPageSizeCombo->setCurrentIndex( currrentIndex );
          
          pPaperLyt->addWidget(pLabel);
          pPaperLyt->addWidget(pPageSizeCombo);
        }
        
        //--- Portrait, landscape
        QHBoxLayout* pOrientationLyt = new QHBoxLayout();
        {
          pOrientation = new QButtonGroup( &d );
          QCheckBox* pPortrait = new QCheckBox( "Portrait", &d );
          QCheckBox* pLandscape = new QCheckBox( "Landscape", &d );
          pOrientation->addButton( pPortrait, 0 );
          pOrientation->addButton( pLandscape, 1 );
          
          if( mpPartitionViewer->getLayoutOrientation() == QPageLayout::Portrait )
          { pPortrait->setCheckState( Qt::Checked ); }
          else{ pLandscape->setCheckState( Qt::Checked ); }
          
          pOrientationLyt->addStretch(1);
          pOrientationLyt->addWidget(pPortrait);
          pOrientationLyt->addWidget(pLandscape);
        }
        
        pVLyt->addLayout( pPaperLyt );
        pVLyt->addLayout( pOrientationLyt );
      } //vLyt

      pVisualizationGrp->setLayout(pVLyt);
    } //groupbox
    
    //--- log
    pVerboseChkBx = new QCheckBox( "Verbose log", &d);
    pVerboseChkBx->setChecked( isVerbose() );
    
    //--- show tool bar
    pShowToolBarChkBx = new QCheckBox( "Show toolbar", &d );
    pShowToolBarChkBx->setChecked( isToolBarVisible() );
    
    //--- ok, Cancel
    QHBoxLayout* pBottomButLyt = new QHBoxLayout();
    {
      QPushButton* pOk = new QPushButton( "Ok", &d );
      connect( pOk, SIGNAL( clicked() ), &d, SLOT( accept() ) );
      
      QPushButton* pCancel = new QPushButton( "Cancel", &d );
      connect( pCancel, SIGNAL( clicked() ), &d, SLOT( reject() ) );
      
      pBottomButLyt->addStretch(1);
      pBottomButLyt->addWidget(pOk);
      pBottomButLyt->addWidget(pCancel);
    }
    
    pMainLyt->addWidget( pVisualizationGrp );
    pMainLyt->addWidget( pVerboseChkBx );
    pMainLyt->addWidget( pShowToolBarChkBx );
    pMainLyt->addStretch(1);
    pMainLyt->addLayout( pBottomButLyt );
  }
  
  if (d.exec() == QDialog::Accepted)
  {
    //pageSize
    mpPartitionViewer->setPageSize( mAvailablePageSizeIds[ pPageSizeCombo->currentIndex() ] );
    
    //layout orientation
    QPageLayout::Orientation o;
    if( pOrientation->checkedId() == 0 ){ o = QPageLayout::Portrait; }
    else{ o = QPageLayout::Landscape; }
    mpPartitionViewer->setLayoutOrientation( o );
    
    //log
    setAsVerbose( pVerboseChkBx->isChecked() );
    
    //tool bar visibility
    setToolBarVisible( pShowToolBarChkBx->isChecked() );
    
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
  
  getLog().log( "verbose set to: %s", iV?"true":"false" );
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
    case aRemoveOrnement: mpPartitionViewer->commandBreakOrnementsFromSelection(); break;
    case aAddGraceNote: mpPartitionViewer->commandAddGraceNotes(); break;
    case aRemoveGraceNote: mpPartitionViewer->commandRemoveSelectionFromGraceNotes(); break;
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
void MainDialog::updateUi()
{
  //on commence par caché tous les item du tool bar...
  PartitionViewer* x = mpPartitionViewer;
  
  mpToolBar->setVisible( isToolBarVisible() );
  
  //disable everything...
  for( int i = 0; i < aUnknown; ++i )
  { mActions[ (action)i ]->setEnabled(false); }
  
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
      //matras
      bool canBreakMatra = true;
      for( int i = 0; i < x->getNumberOfSelectedNote(); ++i )
      {
        NoteLocator nl = x->getSelectedNote( i );
        canBreakMatra &= mComposition.isNoteInMatra( nl.getBar(), nl.getIndex() );
      }
      mActions[aRemoveMatra]->setEnabled(canBreakMatra);
      mActions[aAddMatra]->setEnabled(!canBreakMatra);
      
      //ornements
      bool canBreakOrnement = true;
      for( int i = 0; i < x->getNumberOfSelectedNote(); ++i )
      {
        NoteLocator nl = x->getSelectedNote( i );
        canBreakOrnement &= mComposition.isNoteInOrnement( nl.getBar(), nl.getIndex() );
      }
      mActions[aRemoveOrnement]->setEnabled(canBreakOrnement);
      mActions[aAddKrintan]->setEnabled(!canBreakOrnement);
      mActions[aAddMeend]->setEnabled(!canBreakOrnement);
      mActions[aAddGamak]->setEnabled(!canBreakOrnement);
      
      //graceNote
      bool canRemoveGraceNote = true;
      for( int i = 0; i < x->getNumberOfSelectedNote(); ++i )
      {
        NoteLocator nl = x->getSelectedNote( i );
        canRemoveGraceNote &= mComposition.isGraceNote( nl.getBar(), nl.getIndex() );
      }
      mActions[aRemoveGraceNote]->setEnabled(canRemoveGraceNote);
      mActions[aAddGraceNote]->setEnabled(!canRemoveGraceNote);
      
      //strokes
      mActions[aDa]->setEnabled(true);
      mActions[aRa]->setEnabled(true);
      mActions[aDiri]->setEnabled(true);
      
      bool canRemoveStroke = true;
      for( int i = 0; i < x->getNumberOfSelectedNote(); ++i )
      {
        NoteLocator nl = x->getSelectedNote( i );
        canRemoveStroke &= mComposition.hasStroke( nl.getBar(), nl.getIndex() );
      }
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

