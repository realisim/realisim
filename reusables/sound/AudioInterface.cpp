/* AudioInterface.h */

#include <cassert>
#include "math/MathUtils.h"
#include "sound/AudioInterface.h"
#include <sstream>

using namespace realisim;
using namespace math;
using namespace sound;
using namespace std;

AudioInterface::AudioInterface() :
mpDevice( 0 ),
mpContext( 0 ),
mIsVerbose( false )
{
    //--- creation du device
    mpDevice = alcOpenDevice(NULL);
    if( mpDevice )
    {
        //mettre les attributs... ALC_FREQUENCY, ALC_REFRESH etc...
        mpContext = alcCreateContext( mpDevice, NULL);
    }
    
    //--- creation du contexte
    if( mpContext )
    {
        alcMakeContextCurrent( mpContext );
        alcProcessContext( mpContext );
    }
    
    //-- parametres  par default du listener
    const Point3d p(0.0);
    const Vector3d v(0.0);
    const Vector3d l(0.0, 0.0, -1.0);
    const Vector3d u(0.0, 1.0, 0.0);
    setListenerGain( 1.0 );
    setListenerPosition( p );
    setListenerVelocity( v );
    setListenerOrientation( l, u );
    
    
    if( !mpDevice || !mpContext || alcGetError( mpDevice ) != ALC_NO_ERROR )
    { addError( getOpenAlcError() ); }
}

//------------------------------------------------------------------------------
AudioInterface::~AudioInterface()
{
    //détacher tous les buffers, détruire les sources
    for( int i = 0; i < getNumberOfSources(); ++i )
    {
        ALuint id = getSourceId( i );
        attachBufferToSource( 0, id );
        alDeleteSources(1, &id);
    }
    mSources.clear();
    
    //détruire tous les buffers
    for(int i = 0; i < getNumberOfBuffers(); ++i)
    {
        ALuint id = getBufferId(i);
        alDeleteBuffers(1, &id);
    }
    
    //--- détruire le context et le relacher le device.
    alcMakeContextCurrent(NULL);
    alcDestroyContext(mpContext);
    alcCloseDevice(mpDevice);
    
    if( alcGetError( mpDevice ) != ALC_NO_ERROR )
    { addError( getOpenAlcError() ); }
}

//------------------------------------------------------------------------------
int AudioInterface::addBuffer()
{
    ALuint id = 0;
    alGenBuffers( 1, &id );
    
    if( alGetError() != AL_NO_ERROR )
    {
        addError( getOpenAlError() );
        id = 0;
    }
    
    if( !hasError() )
    { mBuffers.push_back( id ); }
    return (int)id;
}

//------------------------------------------------------------------------------
int AudioInterface::addSource()
{
    ALuint id = 0;
    alGenSources(1, &id);
    
    setSourceGain( id, 1.0 );
    setSourceAsLooping( id, false );
    setSourceAsRelative( id, true );
    setSourcePosition( id, Point3d(0.0) );
    setSourceVelocity( id, Vector3d(0.0) );
    
    if( alGetError() != AL_NO_ERROR )
    {
        addError( getOpenAlError() );
        id = 0;
    }
    
    if( !hasError() )
    { mSources.push_back( id ); }
    return (int)id;
}

//------------------------------------------------------------------------------
void AudioInterface::addError(const std::string& s ) const
{ mErrors += mErrors.empty() ? s : "\n" + s;}

//------------------------------------------------------------------------------
void AudioInterface::attachBufferToSource( int iBufferId, int iSourceId )
{
    alSourcei( iSourceId, AL_BUFFER, iBufferId );
    if( alGetError() != AL_NO_ERROR )
    { addError( "attachBufferToSource: " + getOpenAlError() ); }
}

//------------------------------------------------------------------------------
std::string AudioInterface::getAndClearLastErrors() const
{
    std::string r = mErrors;
    mErrors = std::string();
    return r;
}
//------------------------------------------------------------------------------
int AudioInterface::getBufferId( int iIndex ) const
{
    int r = 0;
    assert( iIndex >= 0 && iIndex < (int)mBuffers.size() );
    if( ( iIndex >= 0 && iIndex < (int)mSources.size() ) )
    { r = mSources[iIndex]; }
    return r;
    
}
//------------------------------------------------------------------------------
double AudioInterface::getListenerGain() const
{
    float r;
    alGetListenerf( AL_GAIN, &r );
    return (double)r;
}
//------------------------------------------------------------------------------
Point3d AudioInterface::getListenerPosition() const
{
    float x, y, z;
    alGetListener3f( AL_POSITION, &x, &y, &z );
    return Point3d( x, y, z );
}
//------------------------------------------------------------------------------
Vector3d AudioInterface::getListenerVelocity() const
{
    float x, y, z;
    alGetListener3f( AL_VELOCITY, &x, &y, &z );
    return Vector3d( x, y, z );
}
//------------------------------------------------------------------------------
Vector3d AudioInterface::getListenerLookDirection() const
{
    float o[6];
    alGetListenerfv( AL_ORIENTATION, o );
    return Vector3d( o[0], o[1], o[2] );
}
//------------------------------------------------------------------------------
Vector3d AudioInterface::getListenerUp() const
{
    float o[6];
    alGetListenerfv( AL_ORIENTATION, o );
    return Vector3d( o[3], o[4], o[5] );
}
//------------------------------------------------------------------------------
int AudioInterface::getNumberOfBuffers() const
{ return mBuffers.size(); }
//------------------------------------------------------------------------------
int AudioInterface::getNumberOfSources() const
{ return mSources.size(); }
//------------------------------------------------------------------------------
std::string AudioInterface::getOpenAlError() const
{
    ALenum e = alGetError();
    const ALchar* c = alGetString(e);
    std::string s( (const char*)c );
    return s;
}
//------------------------------------------------------------------------------
std::string AudioInterface::getOpenAlcError() const
{
    ALCenum e = alcGetError(mpDevice);
    const ALCchar* c = alcGetString(mpDevice, e);
    std::string s( (const char*)c );
    return s;
}
//------------------------------------------------------------------------------
double AudioInterface::getSourceConeInnerAngle(int iId) const
{
    float r;
    alGetSourcef( iId, AL_CONE_INNER_ANGLE, &r );
    return (double)r;
}
//------------------------------------------------------------------------------
double AudioInterface::getSourceConeOutterAngle(int iId) const
{
    float r;
    alGetSourcef( iId, AL_CONE_OUTER_ANGLE, &r );
    return (double)r;
}
//------------------------------------------------------------------------------
double AudioInterface::getSourceConeOutterGain(int iId) const
{
    float r;
    alGetSourcef( iId, AL_CONE_OUTER_GAIN, &r );
    return (double)r;
}
//------------------------------------------------------------------------------
int AudioInterface::getSourceCurrentBuffer( int iId ) const
{
    ALint r;
    alGetSourcei( iId, AL_BUFFER, &r );
    return (int)r;
}
//------------------------------------------------------------------------------
math::Vector3d AudioInterface::getSourceDirection( int iId ) const
{
    float x, y, z;
    alGetSource3f( iId, AL_DIRECTION, &x, &y, &z );
    return Vector3d( x, y, z );
}
//------------------------------------------------------------------------------
double AudioInterface::getSourceGain( int iId ) const
{
    ALfloat r;
    alGetSourcef( iId, AL_GAIN, &r );
    return (double)r;
}
//------------------------------------------------------------------------------
int AudioInterface::getSourceId( int iIndex )
{
    int r = 0;
    assert( iIndex >= 0 && iIndex < (int)mSources.size() );
    if( ( iIndex >= 0 && iIndex < (int)mSources.size() ) )
    { r = mSources[iIndex]; }
    return r;
}
//------------------------------------------------------------------------------
double AudioInterface::getSourceMaximumGain( int iId ) const
{
    ALfloat r;
    alGetSourcef( iId, AL_MAX_GAIN, &r );
    return (double)r;
}
//------------------------------------------------------------------------------
double AudioInterface::getSourceMinimumGain( int iId ) const
{
    ALfloat r;
    alGetSourcef( iId, AL_MIN_GAIN, &r );
    return (double)r;
}
//------------------------------------------------------------------------------
double AudioInterface::getSourceOffsetInBytes( int iId ) const
{
    ALfloat r;
    alGetSourcef( iId, AL_BYTE_OFFSET, &r );
    return (double)r;
}
//------------------------------------------------------------------------------
double AudioInterface::getSourceOffsetInSamples( int iId ) const
{
    ALfloat r;
    alGetSourcef( iId, AL_SAMPLE_OFFSET, &r );
    return (double)r;
}
//------------------------------------------------------------------------------
double AudioInterface::getSourceOffsetInSeconds( int iId ) const
{
    ALfloat r;
    alGetSourcef( iId, AL_SEC_OFFSET, &r );
    return (double)r;
}
//------------------------------------------------------------------------------
double AudioInterface::getSourcePitchShift( int iId ) const
{
    ALfloat r;
    alGetSourcef( iId, AL_PITCH, &r );
    return (double)r;
}
//------------------------------------------------------------------------------
Point3d AudioInterface::getSourcePosition( int iId ) const
{
    float x, y, z;
    alGetSource3f( iId, AL_POSITION, &x, &y, &z );
    return Point3d( x, y, z );
}
//------------------------------------------------------------------------------
Vector3d AudioInterface::getSourceVelocity( int iId ) const
{
    float x, y, z;
    alGetSource3f( iId, AL_VELOCITY, &x, &y, &z );
    return Vector3d( x, y, z );
}
//------------------------------------------------------------------------------
AudioInterface::sourceState AudioInterface::getSourceState( int iId ) const
{
    ALint r;
    enum sourceState s;
    alGetSourcei( iId, AL_SOURCE_STATE, &r );
    switch (r)
    {
        case AL_INITIAL: s = ssInitial; break;
        case AL_PLAYING: s = ssPlaying; break;
        case AL_STOPPED: s = ssStopped; break;
        case AL_PAUSED: s = ssPaused; break;
        default: s = ssInitial; break;
    }
    return s;
}
//------------------------------------------------------------------------------
AudioInterface::sourceType AudioInterface::getSourceType( int iId ) const
{
    ALint r;
    enum sourceType s;
    alGetSourcei( iId, AL_SOURCE_TYPE, &r );
    switch (r)
    {
        case AL_UNDETERMINED: s = stUndetermined; break;
        case AL_STATIC: s = stStatic; break;
        case AL_STREAMING: s = stStreaming; break;
        default: s = stUndetermined; break;
    }
    return s;
}
//------------------------------------------------------------------------------
bool AudioInterface::hasError() const
{	return !mErrors.empty();	}
//------------------------------------------------------------------------------
bool AudioInterface::isSourceDirectional( int iId ) const
{
    float x, y, z;
    alGetSource3f( iId, AL_DIRECTION, &x, &y, &z );
    return isEqual( x, 0.0f ) && isEqual( y, 0.0f ) && isEqual( z, 0.0f );
}
//------------------------------------------------------------------------------
bool AudioInterface::isSourceLooping( int iId ) const
{
    ALint r;
    alGetSourcei( iId, AL_LOOPING, &r );
    return r == AL_TRUE;
}
//------------------------------------------------------------------------------
bool AudioInterface::isSourceRelative( int iId ) const
{
    ALint r;
    alGetSourcei( iId, AL_SOURCE_RELATIVE, &r );
    return r == AL_TRUE;
}
//------------------------------------------------------------------------------
bool AudioInterface::isSourceReadyToStream( int iId ) const
{ return getSourceType( iId ) == stStreaming; }
//------------------------------------------------------------------------------
bool AudioInterface::isSourceReadyToPlayStaticBuffer( int iId ) const
{ return getSourceType( iId ) == stStatic; }
//------------------------------------------------------------------------------
bool AudioInterface::isVerbose() const
{ return mIsVerbose; }
//------------------------------------------------------------------------------
void AudioInterface::pauseSource( int iId )
{ alSourcePause( iId ); }
//------------------------------------------------------------------------------
void AudioInterface::pauseSources( vector<int> iS )
{
    if( iS.size() )
    {
        ALuint* s = new ALuint[ iS.size() ];
        for( uint i = 0; i < iS.size(); ++i )
        { s[i] = (ALuint)iS[i]; }
        alSourcePausev( iS.size(), s );
        delete[] s;
    }
}
//------------------------------------------------------------------------------
void AudioInterface::playSource( int iId )
{ alSourcePlay( iId ); }
//------------------------------------------------------------------------------
void AudioInterface::playSources( vector<int> iS )
{
    if( iS.size() )
    {
        ALuint* s = new ALuint[ iS.size() ];
        for( uint i = 0; i < iS.size(); ++i )
        { s[i] = (ALuint)iS[i]; }
        alSourcePlayv( iS.size(), s );
        delete[] s;
    }
}
//------------------------------------------------------------------------------
void AudioInterface::removeBuffer( int iId )
{
    vector<int>::iterator it = find( mBuffers.begin(), mBuffers.end(), iId );
    if( it != mBuffers.end() && alIsBuffer( iId ) )
    {
        ALuint id = (ALuint)iId;
        alDeleteBuffers(1, &id);
        mBuffers.erase( it );
    }
    else
    {
        ostringstream oss;
        oss << "Impossible to remove buffer (" << iId << "). as it is not present in the internal data.";
        addError( oss.str());
    }
}
//------------------------------------------------------------------------------
void AudioInterface::removeSource( int iId )
{
    vector<int>::iterator it = find( mSources.begin(), mSources.end(), iId );
    if( it != mSources.end() && alIsSource( iId ) )
    {
        ALuint id = (ALuint)iId;
        alDeleteSources(1, &id);
        mSources.erase( it );
    }
    else
    {
        ostringstream oss;
        oss << "Impossible to remove source (" << iId << "). as it is not present in the internal data.";
        addError( oss.str());

    }
}
//------------------------------------------------------------------------------
void AudioInterface::rewindSource( int iId )
{ alSourceRewind( iId); }
//------------------------------------------------------------------------------
void AudioInterface::rewindSources( vector<int> iS )
{
    if( iS.size() )
    {
        ALuint* s = new ALuint[ iS.size() ];
        for( uint i = 0; i < iS.size(); ++i )
        { s[i] = (ALuint)iS[i]; }
        alSourceRewindv( iS.size(), s );
        delete[] s;
    }
}
//------------------------------------------------------------------------------
void AudioInterface::setBufferData( int iId, const string& iB, format iF, int iFreq )
{
    int sizeInBytes = 0;
    ALenum alFormat;
    switch (iF)
    {
        case fMono8:
            alFormat= AL_FORMAT_MONO8;
            sizeInBytes = iB.size();
            break;
        case fMono16:
            alFormat= AL_FORMAT_MONO16;
            sizeInBytes = iB.size() / 2;
            break;
        case fStereo8:
            alFormat= AL_FORMAT_STEREO8;
            sizeInBytes = iB.size();
            break;
        case fStereo16:
            alFormat= AL_FORMAT_STEREO16;
            sizeInBytes = iB.size() / 2;
            break;
        default: alFormat = AL_FORMAT_MONO8; break;
    }
    alBufferData( iId, alFormat, (void*)(iB.c_str()), sizeInBytes, iFreq );
    if( alGetError() != AL_NO_ERROR )
    { addError( "setBufferData: " + getOpenAlError() ); }
}

//------------------------------------------------------------------------------
void AudioInterface::setSourceAsLooping( int iId, bool iB )
{ alSourcei( iId, AL_LOOPING, iB ? AL_TRUE : AL_FALSE ); }

//------------------------------------------------------------------------------
void AudioInterface::setSourceAsRelative( int iId, bool iB )
{ alSourcei( iId, AL_SOURCE_RELATIVE, iB ? AL_TRUE : AL_FALSE ); }

//------------------------------------------------------------------------------
void AudioInterface::setAsVerbose( bool iV )
{ mIsVerbose = iV; }

//------------------------------------------------------------------------------
void AudioInterface::setSourceConeInnerAngle( int iId, double iV )
{ alSourcef( iId, AL_CONE_INNER_ANGLE, iV ); }

//------------------------------------------------------------------------------
void AudioInterface::setSourceConeOuterAngle( int iId, double iV )
{ alSourcef( iId, AL_CONE_OUTER_ANGLE, iV ); }

//------------------------------------------------------------------------------
void AudioInterface::setSourceConeOuterGain( int iId, double iV )
{ alSourcef( iId, AL_CONE_OUTER_GAIN, iV ); }

//------------------------------------------------------------------------------
void AudioInterface::setSourceDirection( int iId, math::Vector3d iV )
{
    ALfloat d[3];
    d[0] = iV.x(); d[1] = iV.y(); d[2] = iV.z();
    alSourcefv( iId, AL_DIRECTION, d );
}

//------------------------------------------------------------------------------
void AudioInterface::setSourceGain( int iId, double iV )
{ alSourcef( iId, AL_GAIN, iV ); }

//------------------------------------------------------------------------------
void AudioInterface::setSourceGainBounds( int iId, double iMin, double iMax )
{ alSourcef( iId, AL_MIN_GAIN, iMin ); alSourcef( iId, AL_MAX_GAIN, iMax ); }

//------------------------------------------------------------------------------
void AudioInterface::setSourceOffsetInBytes( int iId, double iV )
{ alSourcef( iId, AL_BYTE_OFFSET, iV ); }

//------------------------------------------------------------------------------
void AudioInterface::setSourceOffsetInSamples( int iId, double iV )
{ alSourcef( iId, AL_SAMPLE_OFFSET, iV ); }

//------------------------------------------------------------------------------
void AudioInterface::setSourceOffsetInSeconds( int iId, double iV )
{ alSourcef( iId, AL_SEC_OFFSET, iV ); }

//------------------------------------------------------------------------------
void AudioInterface::setSourcePitchShift( int iId, double iV )
{ alSourcef( iId, AL_PITCH, iV ); }

//------------------------------------------------------------------------------
void AudioInterface::setSourcePosition( int iId, math::Point3d iV )
{
    ALfloat p[3];
    p[0] = iV.x(); p[1] = iV.y(); p[2] = iV.z();
    alSourcefv( iId, AL_POSITION, p );
}

//------------------------------------------------------------------------------
void AudioInterface::setSourceVelocity( int iId, math::Vector3d iV )
{
    ALfloat v[3];
    v[0] = iV.x(); v[1] = iV.y(); v[2] = iV.z();
    alSourcefv( iId, AL_VELOCITY, v );
}

//------------------------------------------------------------------------------
void AudioInterface::stopSource( int iId )
{ alSourceStop( iId ); }

//------------------------------------------------------------------------------
void AudioInterface::stopSources( vector<int> iS )
{
    if( iS.size() )
    {
        ALuint* s = new ALuint[ iS.size() ];
        for( uint i = 0; i < iS.size(); ++i )
        { s[i] = (ALuint)iS[i]; }
        alSourceStopv( iS.size(), s );
        delete[] s;
    }
}

//------------------------------------------------------------------------------
void AudioInterface::setListenerGain( double iG )
{ alListenerf( AL_GAIN, iG ); }

//------------------------------------------------------------------------------
void AudioInterface::setListenerPosition( Point3d iP )
{ alListener3f( AL_POSITION, iP.x(), iP.y(), iP.z() ); }

//------------------------------------------------------------------------------
void AudioInterface::setListenerVelocity( Vector3d iV )
{ alListener3f( AL_VELOCITY, iV.x(), iV.y(), iV.z() );}

//------------------------------------------------------------------------------
void AudioInterface::setListenerOrientation( Vector3d iL, Vector3d iU )
{ 
    float o[6];
    o[0] = iL.x(); o[1] = iL.y(); o[2] = iL.z();
    o[3] = iU.x(); o[4] = iU.y(); o[5] = iU.z();
    alListenerfv( AL_ORIENTATION, o );
}
