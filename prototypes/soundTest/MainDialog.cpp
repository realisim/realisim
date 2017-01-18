
#include "MainDialog.h"
#include <QButtonGroup>
#include <QCheckBox>
#include <QFileDialog>
#include <QFrame>
#include <QLayout>
#include <QGroupBox>
#include <QSlider>
#include <QPushButton>
#include "sound/utilities.h"
#include <sstream>
#include "utils/utilities.h"

using namespace std;
using namespace realisim;
using namespace sound;

MainDialog::MainDialog() : QMainWindow(),
mSourcesId(),
mSourceIndex(0),
mWave0BufferId(0),
mpWaveWidget(nullptr)
{
    createUi();
    generateNotes();
    
    mSourcesId[0] = mAudio.addSource();
    mSourcesId[1] = mAudio.addSource();
    mSourcesId[2] = mAudio.addSource();
    
    mWave0SourceId = mAudio.addSource();
    mWave0BufferId = mAudio.addBuffer();

    updateUi();
}

//-----------------------------------------------------------------------------
void MainDialog::createUi()
{
    resize(640, 480);
    
    QFrame* pMainFrame = new QFrame(this);
    setCentralWidget(pMainFrame);
    
    QVBoxLayout* pLyt = new QVBoxLayout(pMainFrame);
    pLyt->setMargin(2); pLyt->setSpacing(5);
    
    QHBoxLayout* pNotesLyt = new QHBoxLayout();
    pNotesLyt->setMargin(2); pNotesLyt->setSpacing(5);
    {
        QButtonGroup* pButGroup = new QButtonGroup( pMainFrame );
        connect( pButGroup, SIGNAL( buttonClicked(int) ),
                this, SLOT( noteClicked(int) ) );
        
        for( int i = nC; i < nB; ++i )
        {
            QPushButton* pBut = new QPushButton( toString( (notes)i ), pMainFrame );
            pButGroup->addButton( pBut, i );
            pNotesLyt->addWidget( pBut );
        }
    }
    
    QGroupBox *pWavGrp = new QGroupBox("Wave", pMainFrame);
    {
        QVBoxLayout *pWavLyt = new QVBoxLayout(pWavGrp);
        pWavLyt->setMargin(2); pWavLyt->setSpacing(5);
        {
            QPushButton *pLoad = new QPushButton( "load wav...", pMainFrame);
            connect(pLoad, SIGNAL(clicked()), this, SLOT(loadWavClicked()));

            //--- wave widget - 
            // info on wave and controls
            mpWaveWidget = new QWidget(pWavGrp);
            {
                QVBoxLayout *pWaveVLyt = new QVBoxLayout(mpWaveWidget);
                {
                    QHBoxLayout *pInfos = new QHBoxLayout(mpWaveWidget);
                    {
                        //infos
                        mpWaveInfo = new QLabel(mpWaveWidget);
                        mpWaveInfo->setText("no infos...");

                        //options
                        QVBoxLayout *pOptions = new QVBoxLayout();
                        {
                            QCheckBox *pLoop = new QCheckBox("Loop", mpWaveWidget);
                            connect(pLoop, SIGNAL(stateChanged(int)), this, SLOT(waveLoopClicked(int)) );

                            //-- pitch
                            QHBoxLayout *pPitchLyt = new QHBoxLayout();
                            {
                                QLabel *pL = new QLabel("Pitch:", mpWaveWidget);

                                QSlider *pPitchShift = new QSlider(mpWaveWidget);
                                pPitchShift->setOrientation(Qt::Horizontal);
                                pPitchShift->setMinimum(0);
                                pPitchShift->setMaximum(100);
                                pPitchShift->setTickInterval(1);
                                pPitchShift->setValue(20);
                                connect(pPitchShift, SIGNAL(valueChanged(int)), this, SLOT(pitchShiftChanged(int)));                                

                                mpPitchValue = new QLabel("1.0", mpWaveWidget);

                                pPitchLyt->addWidget(pL);
                                pPitchLyt->addWidget(pPitchShift);
                                pPitchLyt->addWidget(mpPitchValue);
                            }

                            //-- x pos
                            QHBoxLayout *pXPosLyt = new QHBoxLayout();
                            {
                                QLabel *pL = new QLabel("x pos:", mpWaveWidget);

                                QSlider *pSlider = new QSlider(mpWaveWidget);
                                pSlider->setOrientation(Qt::Horizontal);
                                pSlider->setMinimum(-100);
                                pSlider->setMaximum(100);
                                pSlider->setTickInterval(1);
                                pSlider->setValue(0);
                                connect(pSlider, SIGNAL(valueChanged(int)), this, SLOT(xPosChanged(int)));                                

                                mpXPos = new QLabel("0.0", mpWaveWidget);

                                pXPosLyt->addWidget(pL);
                                pXPosLyt->addWidget(pSlider);
                                pXPosLyt->addWidget(mpXPos);
                            }

                            //-- y pos
                            QHBoxLayout *pYPosLyt = new QHBoxLayout();
                            {
                                QLabel *pL = new QLabel("y pos:", mpWaveWidget);

                                QSlider *pSlider = new QSlider(mpWaveWidget);
                                pSlider->setOrientation(Qt::Horizontal);
                                pSlider->setMinimum(-100);
                                pSlider->setMaximum(100);
                                pSlider->setTickInterval(1);
                                pSlider->setValue(0);
                                connect(pSlider, SIGNAL(valueChanged(int)), this, SLOT(yPosChanged(int)));                                

                                mpYPos = new QLabel("0.0", mpWaveWidget);

                                pYPosLyt->addWidget(pL);
                                pYPosLyt->addWidget(pSlider);
                                pYPosLyt->addWidget(mpYPos);
                            }
                            
                            pOptions->addWidget(pLoop);
                            pOptions->addLayout(pPitchLyt);
                            pOptions->addLayout(pXPosLyt);
                            pOptions->addLayout(pYPosLyt);
                            pOptions->addStretch(1);
                        }

                        pInfos->addWidget(mpWaveInfo);
                        pInfos->addLayout(pOptions);
                    }

                    QHBoxLayout *pControls = new QHBoxLayout();
                    {
                        mpTimePlayed = new QLabel("- / -", mpWaveWidget);

                        mpPlayStop = new QPushButton("Play", mpWaveWidget);
                        connect(mpPlayStop, SIGNAL(clicked()), this, SLOT(playClicked()));

                        pControls->addStretch(1);
                        pControls->addWidget(mpTimePlayed);
                        pControls->addWidget(mpPlayStop);
                    }

                    pWaveVLyt->addLayout(pInfos);
                    pWaveVLyt->addLayout(pControls);
                }                
            }

            pWavLyt->addWidget(pLoad);
            pWavLyt->addWidget(mpWaveWidget);
            pWavLyt->addStretch(1);
        }
    }
    
    pLyt->addLayout( pNotesLyt );
    pLyt->addWidget( pWavGrp );
    pLyt->addStretch( 1 );
}
//-----------------------------------------------------------------------------
void MainDialog::generateNotes()
{
    //C			C#		D			Eb		E			F			F#		G			G#		A			Bb		B
    //261.6	277.2	293.7	311.1	329.6	349.2	370.0	392.0	415.3	440.0	466.2	493.9
    double noteFreq[12];
    noteFreq[nC] = 261.6; noteFreq[nCSharp] = 277.2; noteFreq[nD] = 293.7;
    noteFreq[nEFlat] = 311.1; noteFreq[nE] = 329.6; noteFreq[nF] = 349.2;
    noteFreq[nFSharp] = 370.0; noteFreq[nG] = 392.0; noteFreq[nGSharp] = 415.3;
    noteFreq[nA] = 440; noteFreq[nBFlat] = 466.2; noteFreq[nB] = 493.9;
    
    for( int i = nC; i < nB; ++i )
    {
        int id = mAudio.addBuffer();
        if( !mAudio.hasError() )
        {
            std::string data = generateSoundBuffer( noteFreq[i], 44100, 1.0 );
            mAudio.setBufferData( id, data, AudioInterface::fMono8, 44100 );
            mNotesToBuffer[ (notes)i ] = id;
        }
    }
}

//-----------------------------------------------------------------------------
void MainDialog::loadWavClicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open Wave"),
                                                    realisim::utils::getAssetFolder(),
                                                    tr("Wave (*.wav)"));
    mWave0.setFilenamePath(fileName.toStdString());
    mWave0.load();
    
    mAudio.stopSource(mWave0SourceId);
    mAudio.detachBufferFromSource(mWave0SourceId);

    if(mWave0.isValid() && mWave0.getBitsPerSample() <= 16) //24 bits per sample is unsupported
    {
        AudioInterface::format f = AudioInterface::fMono8;
        switch (mWave0.getNumberOfChannels())
        {
            case 1: //mono
                if(mWave0.getBitsPerSample() == 8) {f = AudioInterface::fMono8;}
                if(mWave0.getBitsPerSample() == 16) {f = AudioInterface::fMono16;}
                break;
            case 2: //stereo
                if(mWave0.getBitsPerSample() == 8) {f = AudioInterface::fStereo8;}
                if(mWave0.getBitsPerSample() == 16) {f = AudioInterface::fStereo16;}
                break;
            default: f = AudioInterface::fMono8; break;
        }
                
        mAudio.setBufferData(mWave0BufferId, mWave0.getSoundData(), mWave0.getSoundDataSize(), f, mWave0.getSamplingFrequency());
        mAudio.attachBufferToSource(mWave0BufferId, mWave0SourceId);
    }
    else //unsupported, flush the buffer
    {
        mAudio.removeBuffer(mWave0BufferId);
        mWave0BufferId = mAudio.addBuffer();
    }

    if(mAudio.hasError())
    {
        printf("audio error: %s\n", mAudio.getAndClearLastErrors().c_str() );
    }

    updateUi();
}

//-----------------------------------------------------------------------------
void MainDialog::pitchShiftChanged(int iV)
{
    const double v = 5.0;
    double pitch = iV/100.0 * v;

    mAudio.setSourcePitchShift(mWave0SourceId, pitch);

    mpPitchValue->setText(QString::number(pitch, 'g', 4));
    updateUi();
}

//-----------------------------------------------------------------------------
void MainDialog::playClicked()
{
    AudioInterface::sourceState ss = mAudio.getSourceState(mWave0SourceId);

    if(ss == AudioInterface::ssInitial || ss == AudioInterface::ssStopped)
    { 
        mFastUpdateTimerId = startTimer(15);
        mAudio.playSource(mWave0SourceId);
    }

    if(ss == AudioInterface::ssPlaying)
    { 
        killTimer(mFastUpdateTimerId);
        mFastUpdateTimerId = 0;

        mAudio.stopSource(mWave0SourceId);
    }

    
    updateUi();
}

//-----------------------------------------------------------------------------
void MainDialog::noteClicked( int iNote )
{
    notes n = (notes)iNote;
    int bufId = mNotesToBuffer[n];
    
    mSourceIndex = (mSourceIndex + 1) % 3;
    
    mAudio.attachBufferToSource( bufId, mSourcesId[mSourceIndex] );
    mAudio.playSource( mSourcesId[mSourceIndex] );
}

//-----------------------------------------------------------------------------
void MainDialog::timerEvent(QTimerEvent* ipE)
{
    if (ipE->timerId() == mFastUpdateTimerId)
    {
        ostringstream oss;
        oss << fixed << setprecision(2);
        oss << mAudio.getSourceOffsetInSeconds(mWave0SourceId) << "(sec) / " << mAudio.getBufferLengthInSeconds(mWave0BufferId) << " (sec).";
        mpTimePlayed->setText( QString::fromStdString(oss.str()) );
    }
}

//-----------------------------------------------------------------------------
QString MainDialog::toString( notes iN ) const
{
    QString r;
    switch (iN)
    {
        case nC: r = "C"; break;
        case nCSharp: r = "C#"; break;
        case nD: r = "D"; break;
        case nEFlat: r = "Eb"; break;
        case nE: r = "E"; break;
        case nF: r = "F"; break;
        case nFSharp: r = "F#"; break;
        case nG: r = "G"; break;
        case nGSharp: r = "G#"; break;
        case nA: r = "A"; break;
        case nBFlat: r = "Bb"; break;
        case nB: r = "B"; break;
        default: break;
    }
    return r;
}
//-----------------------------------------------------------------------------
void MainDialog::updateUi()
{
    mpWaveWidget->setVisible( mWave0.hasSoundData() );

    //gather wave infos
    {
        ostringstream oss;
        string audioFormat("PCM");
        if(mWave0.getAudioFormat() != Wave::afPcm) { audioFormat = "unsupported"; }
        oss << "Wave filename: " << mWave0.getFilenamePath() << "\n"
            << "Audio format: " << audioFormat << "\n"
            << "Number of channels: " << mWave0.getNumberOfChannels() << "\n"
            << "Sampling rate: " << mWave0.getSamplingFrequency() << "\n"
            << "Byte rate: " << mWave0.getByteRate() << "\n"
            << "Bits per sample: " << mWave0.getBitsPerSample() << "\n"
            << "Sound buffer size (bytes): " << mWave0.getSoundDataSize() << "\n"
            << "Duration in seconds: " << mWave0.getDurationInSeconds();
        mpWaveInfo->setText( QString::fromStdString( oss.str() ) );
    }
    
    //time played
    {
        ostringstream oss;
        oss << fixed << setprecision(2);
        oss << 0 << " / " << mAudio.getBufferLengthInSeconds(mWave0BufferId) << " (sec).";
        mpTimePlayed->setText( QString::fromStdString(oss.str()) );
    }
    

    //play stop
    AudioInterface::sourceState ss = mAudio.getSourceState(mWave0SourceId);
    QString playStop("Play");
    switch (ss)
    {
    case realisim::sound::AudioInterface::ssInitial: playStop = "Play"; break;
    case realisim::sound::AudioInterface::ssPlaying: playStop = "Stop"; break;
    case realisim::sound::AudioInterface::ssPaused: break;
    case realisim::sound::AudioInterface::ssStopped: playStop = "Play"; break;
    default: break;
    }
    mpPlayStop->setText( playStop );
}

//-----------------------------------------------------------------------------
void MainDialog::waveLoopClicked(int iState)
{
    mAudio.setSourceAsLooping(mWave0SourceId, iState == Qt::Checked);
}

//-----------------------------------------------------------------------------
void MainDialog::xPosChanged(int iV)
{
    const double v = 5.0;
    double xPos = iV/100.0 * v;
    const double yPos = mAudio.getSourcePosition(mWave0SourceId).y();

    mAudio.setSourcePosition(mWave0SourceId, math::Point3d(xPos, yPos, 0.0) );

    mpXPos->setText(QString::number(xPos, 'g', 4));
    updateUi();
}

//-----------------------------------------------------------------------------
void MainDialog::yPosChanged(int iV)
{
    const double v = 5.0;
    double yPos = iV/100.0 * v;
    const double xPos = mAudio.getSourcePosition(mWave0SourceId).x();

    mAudio.setSourcePosition(mWave0SourceId, math::Point3d(xPos, yPos, 0.0) );

    mpYPos->setText(QString::number(yPos, 'g', 4));
    updateUi();
}