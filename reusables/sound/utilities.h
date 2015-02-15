/*  sound/utilities.h */
#include "sound/AudioInterface.h"

namespace realisim
{
namespace sound 
{

	QByteArray generateSoundBuffer( int, int, double, 
  	AudioInterface::format = AudioInterface::fMono8 );

}
}