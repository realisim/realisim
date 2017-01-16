/*
 *  MainWindow.h
 */

#ifndef MainDialog_hh
#define MainDialog_hh

#include <map>
#include <QMainWindow>
#include "sound/AudioInterface.h"

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
};

#endif
