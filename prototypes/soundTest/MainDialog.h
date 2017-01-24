/*
 *  MainWindow.h
 */

#ifndef MainDialog_hh
#define MainDialog_hh

#include <map>
#include <QLabel>
#include <QMainWindow>
#include <QPushButton>
#include <QTimerEvent>
#include "sound/AudioInterface.h"
#include "sound/Wave.h"

//------------------------------------------------------------------------------
class MainDialog : public QMainWindow
{
    Q_OBJECT
public:
    MainDialog();
    ~MainDialog(){};
    
    enum notes{ nC, nCSharp, nD, nEFlat, nE, nF, nFSharp, nG, nGSharp, nA, nBFlat,
        nB };
    
protected slots:
    void gainChanged(int);
    void loadWavClicked();
    void noteClicked( int );
    void playClicked();
    void pitchShiftChanged(int);
    void waveLoopClicked(int);
    void xPosChanged(int);
    void yPosChanged(int);
    
protected:
    void createUi();
    void generateNotes();
    virtual void timerEvent(QTimerEvent*) override;
    QString toString( notes ) const;
    void updateUi();
    
    realisim::sound::AudioInterface mAudio;
    int mSourcesId[3];
    int mSourceIndex;
    std::map< notes, int > mNotesToBuffer;
    int mFastUpdateTimerId;
    
    realisim::sound::Wave mWave0;
    int mWave0SourceId;
    int mWave0BufferId;

    //--- ui
    QWidget *mpWaveWidget;
    QPushButton *mpPlayStop;
    QLabel *mpWaveInfo;
    QLabel *mpGainValue;
    QLabel *mpPitchValue;
    QLabel *mpXPos;
    QLabel *mpYPos;
    QLabel *mpTimePlayed;
};

#endif
