/* utilities.cpp */

#include "sound/utilities.h"
namespace realisim 
{
namespace sound 
{
  
//------------------------------------------------------------------------------
/*
	f: frequence a génerer en hertz  
  s: sampling rate en Hertz
  d: durée en seconde
  af: format audio - voir AudioInterface.h
  
  Par exemple, pour faire un "la" de 1 sec avec un sampling rate de 44.1KHz: 
  generateSoundBuffer( 440, 1.0, 44100 )
  
  8 bits: 0 à 255
  16 bits: -32768 à 32767
*/
QByteArray generateSoundBuffer( int f, int s, double d, AudioInterface::format 
	af )
{
	QByteArray ba( d*s, 0 );
  const double twoPi = 2*3.1415926;
  for( int i = 0; i < d * s; ++i )
  {
  	const double sineSample = ((i/(double)s) * f * twoPi);
    ba[i] = (int)(sinf( fmodf( sineSample, twoPi ) ) * 127) + 127;
  }
  
  return ba;
}


} //fin de namespace
}