/*
 *  MainWindow.h
 */

#ifndef MainDialog_hh
#define MainDialog_hh

#include <map>
#include <QMainWindow>
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
    void loadWavClicked();
    void noteClicked( int );
    
protected:
    void createUi();
    void generateNotes();
    QString toString( notes ) const;
    void updateUi();
    
    realisim::sound::AudioInterface mAudio;
    int mSourcesId[3];
    int mSourceIndex;
    std::map< notes, int > mNotesToBuffer;
    
    realisim::sound::Wave mWave0;
    int mWave0SourceId;
    int mWave0BufferId;
};

#endif
