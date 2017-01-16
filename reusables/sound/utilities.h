/*  sound/utilities.h */
#include "sound/AudioInterface.h"
#include <string>

namespace realisim
{
namespace sound 
{

    std::string generateSoundBuffer( int iFreq, int iSampling, double iDuration,
                                   AudioInterface::format = AudioInterface::fMono8 );

}
}