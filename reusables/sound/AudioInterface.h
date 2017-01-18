/* AudioInterface.h */

#pragma once

#include "math/Point.h"
#include "math/Vect.h"

#ifdef  __APPLE__
#include "OpenAL/al.h"
#include "OpenAL/alc.h"
#endif //  __APPLE__

#ifdef _WIN32
#include "al.h"
#include "alc.h"
#endif // _WIN32


#include <string>
#include <vector>

namespace realisim
{
namespace sound
{
    
    class AudioInterface
    {
    public:
        AudioInterface();
        virtual ~AudioInterface();
        
        enum sourceState { ssInitial, ssPlaying, ssPaused, ssStopped };
        enum sourceType { stUndetermined, stStatic, stStreaming };
        enum supportedFileType{ sftWav };
        enum format{ fMono8, fMono16, fStereo8, fStereo16 };
        
        int addBuffer();
        int addSource();
        void attachBufferToSource( int, int );
        void detachBufferFromSource(int iSourceId);
        std::string getAndClearLastErrors() const;
        int getBufferBitsPerSample( int iBufferId ) const;
        int getBufferFrequency( int iBufferId ) const;
        int getBufferId( int iBufferId ) const;
        double getBufferLengthInSeconds( int iBufferId ) const;
        int getBufferNumberOfChannels( int iBufferId ) const;
        int getBufferSize( int iBufferId ) const; //in bytes
        double getListenerGain() const;
        math::Point3d getListenerPosition() const;
        math::Vector3d getListenerVelocity() const;
        math::Vector3d getListenerLookDirection() const;
        math::Vector3d getListenerUp() const;
        int getNumberOfBuffers() const;
        int getNumberOfQueuedBuffersAtSource( int ) const;
        int getNumberOfSources() const;
        double getSourceConeInnerAngle(int) const;
        double getSourceConeOutterAngle(int) const;
        double getSourceConeOutterGain(int) const;
        int getSourceCurrentBuffer( int ) const;
        math::Vector3d getSourceDirection( int ) const;
        double getSourceGain( int ) const;
        int getSourceId( int iIndex );
        double getSourceMaximumGain( int ) const;
        double getSourceMinimumGain( int ) const;
        double getSourceOffsetInBytes( int ) const;
        double getSourceOffsetInSamples( int ) const;
        double getSourceOffsetInSeconds( int ) const;
        double getSourcePitchShift( int ) const;
        math::Point3d getSourcePosition( int ) const;
        math::Vector3d getSourceVelocity( int ) const;
        sourceState getSourceState( int ) const;
        sourceType getSourceType( int ) const;
        bool hasError() const;
        bool isSourceDirectional( int ) const; //AL_DIRECTION != 0.0
        bool isSourceLooping( int ) const;
        bool isSourceRelative( int ) const;
        bool isSourceReadyToStream( int ) const;
        bool isSourceReadyToPlayStaticBuffer( int ) const;
        bool isVerbose() const;
        void pauseSource( int );
        void pauseSources( std::vector<int> );
        void playSource( int );
        void playSources( std::vector<int> );
        void removeBuffer( int );
        void removeSource( int );
        void rewindSource( int );
        void rewindSources( std::vector<int> );
        //void queueBufferToSource( int, int );
        //void queueBuffersToSource( vector<int>, int );
        void setBufferData( int iBufferId, const unsigned char* iData, int iDataSize, format iFormat, int iFreq );
        void setBufferData( int iBufferId, const std::string& iData, format iFormat, int iFreq );
        void setListenerGain( double );
        void setListenerPosition( math::Point3d );
        void setListenerVelocity( math::Vector3d );
        void setListenerOrientation( math::Vector3d, math::Vector3d );
        void setSourceAsLooping( int, bool );
        void setSourceAsRelative( int, bool );
        void setAsVerbose( bool );
        void setSourceConeInnerAngle( int, double );
        void setSourceConeOuterAngle( int, double );
        void setSourceConeOuterGain( int, double );
        void setSourceDirection( int, math::Vector3d );
        void setSourceGain( int, double );
        void setSourceGainBounds( int, double, double );
        void setSourceOffsetInBytes( int, double );
        void setSourceOffsetInSamples( int, double );
        void setSourceOffsetInSeconds( int, double );
        void setSourcePitchShift( int, double );
        void setSourcePosition( int, math::Point3d );
        void setSourceVelocity( int, math::Vector3d );
        void stopSource( int );
        void stopSources( std::vector<int> );
        //void unqueueBufferFromSource( int, int );
        //void unqueueBuffersFromSource( vector<int>, int );
        
    protected:
        void addError( const std::string& ) const;
        void checkForAlError(const std::string& iM = std::string() ) const;
        void checkForAlcError(ALCdevice *ipD, const std::string& iM = std::string()) const;
        
        //std::string getVersion() const;
        //int getMajorVersion() const;
        //int getMinorVersion();
        //std::string getRenderer();
        
        ALCdevice* mpDevice;
        ALCcontext* mpContext;
        mutable std::string mErrors;
        bool mIsVerbose;
        std::vector<int> mSources;
        std::vector<int> mBuffers;
    };
    
}
}
