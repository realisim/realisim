
#include "MainDialog.h"
#include <QFrame>
#include <QLayout>
#include <QButtonGroup>
#include <qpushbutton>
#include "sound/utilities.h"

using namespace realisim;
using namespace sound;

MainDialog::MainDialog() : QMainWindow(),
mSourcesId(),
mSourceIndex(0)
{
    createUi();
    generateNotes();
    
    mSourcesId[0] = mAudio.addSource();
    mSourcesId[1] = mAudio.addSource();
    mSourcesId[2] = mAudio.addSource();
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
    
    pLyt->addLayout( pNotesLyt );
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
void MainDialog::noteClicked( int iNote )
{
    notes n = (notes)iNote;
    int bufId = mNotesToBuffer[n];
    
    mSourceIndex = (mSourceIndex + 1) % 3;
    
    mAudio.attachBufferToSource( bufId, mSourcesId[mSourceIndex] );
    mAudio.playSource( mSourcesId[mSourceIndex] );
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
{}
