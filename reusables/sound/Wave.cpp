#include <limits>
#include <sstream>
#include "Utils/StreamUtility.h"
#include "Wave.h"


using namespace std;
using namespace realisim;
    using namespace sound;

//-----------------------------------------------------------------------------
Wave::Wave() : mIsValid(false),
mHasSoundDataOwnership(true),
mFilenamePath(),
mAudioFormat(afUndefined),
mNumberOfChannels(0),
mSamplingFrequency(0),
mByteRate(0),
mBytesPerBlock(0),
mBitsPerSample(0),
mSoundDataSize(0),
mpSoundData(nullptr)
{}

//-----------------------------------------------------------------------------
Wave::~Wave()
{
    clear();
}

//-----------------------------------------------------------------------------
void Wave::clear()
{
    if(mHasSoundDataOwnership && mpSoundData)
    { 
        delete mpSoundData;
        mpSoundData = nullptr;
    }

    mIsValid = false;
    mHasSoundDataOwnership = true;
    mAudioFormat = afUndefined;
    mNumberOfChannels = 0;
    mSamplingFrequency = 0;
    mByteRate = 0;
    mBytesPerBlock = 0;
    mBitsPerSample = 0;
    mSoundDataSize = 0;
}

//-----------------------------------------------------------------------------
Wave::AudioFormat Wave::getAudioFormat() const
{
    AudioFormat r;
    switch(mAudioFormat)
    {
        case 1: r = afPcm; break;
        case 6: r = afAlaw; break;
        case 7: r = afUlaw; break;
        case 17: r = afIma4adpcm; break;
        default: r = afUndefined; break;
    }
    return r;
}

//-----------------------------------------------------------------------------
int Wave::getBitsPerSample() const
{
    return mBitsPerSample;
}

//-----------------------------------------------------------------------------
int Wave::getByteRate() const
{
    return mByteRate;
}

//-----------------------------------------------------------------------------
double Wave::getDurationInSeconds() const
{
    double r = std::numeric_limits<double>::quiet_NaN();
    if (isValid())
    {
        const int lenghtInSamples = mSoundDataSize * 8 / (mNumberOfChannels * mBitsPerSample);
        r = lenghtInSamples / (double)mSamplingFrequency;
    }
    return r;
}

//-----------------------------------------------------------------------------
const string& Wave::getFilenamePath() const
{
    return mFilenamePath;
}

//-----------------------------------------------------------------------------
int Wave::getNumberOfChannels() const
{
    return mNumberOfChannels;
}

//-----------------------------------------------------------------------------
int Wave::getSamplingFrequency() const
{
    return mSamplingFrequency;
}

//-----------------------------------------------------------------------------
unsigned char* Wave::getSoundData() const
{
    return mpSoundData;
}

//-----------------------------------------------------------------------------
int Wave::getSoundDataSize() const
{
    return mSoundDataSize;
}

//-----------------------------------------------------------------------------
unsigned char* Wave::giveOwnershipOfSoundData()
{
    mHasSoundDataOwnership = false;
    return mpSoundData;
}

//-----------------------------------------------------------------------------
bool Wave::hasSoundData() const
{
    return mpSoundData != nullptr;
}

//-----------------------------------------------------------------------------
bool Wave::isValid() const
{
    return mIsValid;
}

//-----------------------------------------------------------------------------
void Wave::load()
{
    clear();
    
    // Parse the file
    ifstream ifs;
    ifs.open(getFilenamePath(), ifstream::in | ios_base::binary);
    if(!ifs.fail())
    {
        mIsValid = parseHeader(ifs);
        
        if(isValid())
        {
            mIsValid = parsePayload(ifs);
        }
    }
    
    ifs.close();
}

//-----------------------------------------------------------------------------
void Wave::loadHeader()
{
    clear();
    
    // Parse the file
    ifstream ifs;
    ifs.open(getFilenamePath(), ifstream::in | ios_base::binary);
    if(!ifs.fail())
    {
        mIsValid = parseHeader(ifs);
    }
    ifs.close();
}

//-----------------------------------------------------------------------------
bool Wave::parseHeader(ifstream& ifs)
{
    // see specification
    utils::StreamUtility su;
    su.setStreamFormat(utils::StreamUtility::eLittleEndian);
    
    bool ok = true;
    
    string riff;
    ok &= su.readBytes(ifs, 4, &riff);
    ok &= riff == "RIFF";

    int32_t restOfFileSizeInByte; //file size minus 8 bytes.
    ok &= su.readInt32(ifs, &restOfFileSizeInByte);
    
    string format;
    ok &= su.readBytes(ifs, 4, &format);
    ok &= format == "WAVE";
    
    //read the "fmt " chunk
    string fmt;
    ok &= su.readBytes(ifs, 4, &fmt);
    ok &= fmt == "fmt ";
    
    if(ok)
    {
        int32_t fmtChunkSize;
        ok &= su.readInt32(ifs, &fmtChunkSize);
        
        ok &= su.readUint16(ifs, &mAudioFormat);
        ok &= su.readUint16(ifs, &mNumberOfChannels);
        ok &= su.readUint32(ifs, &mSamplingFrequency);
        ok &= su.readUint32(ifs, &mByteRate);
        ok &= su.readUint16(ifs, &mBytesPerBlock);
        ok &= su.readUint16(ifs, &mBitsPerSample);       

        // here we read 16 bytes of data, if fmt chunksize is greater than 16, 
        // make sure we read them into the bit bucket!
        //
        // Technically, there can be extra info there, but it does not concern
        // PCM data, which is the only one supported right now.
        //
        const int kPcmChunkSize = 16;
        if(fmtChunkSize > kPcmChunkSize)
        { 
            string bitBucket;
            ok &= su.readBytes(ifs, fmtChunkSize - kPcmChunkSize, &bitBucket);
        }

        //the "data" chunk is read by parsepayload
    }
    else
    {
        ostringstream oss;
        oss << "Error while parsing header for Wave: " << getFilenamePath();
        printf("%s\n", oss.str().c_str());
    }
    
    return ok;
}

//-----------------------------------------------------------------------------
bool Wave::parsePayload(ifstream& ifs)
{
    utils::StreamUtility su;
    su.setStreamFormat(utils::StreamUtility::eLittleEndian);
    
    bool ok = true;
    bool done = false;
    
    while(!done)
    {
        //read the chunk tag
        string tag;
        ok &= su.readBytes(ifs, 4, &tag);
        
        //read the remaining size of the chunk
        uint32_t chunkSize = 0;
        ok &= su.readUint32(ifs, &chunkSize);
        
        if(tag == "data")
        {
            char *data = new char[chunkSize];
            ok &= su.readBytes(ifs, chunkSize, data);
            mpSoundData = (unsigned char*)(data);
            mSoundDataSize = chunkSize;
            done = true;
        }
        else
        {
            printf("Wave::parsePayload skipped tag: %s\n", tag.c_str());
            //skip unknow tags
            ifs.seekg( chunkSize, ios_base::cur );
        }
    }

    return ok;
}

//-----------------------------------------------------------------------------
void Wave::setFilenamePath(const string& iFilenamePath)
{
    mFilenamePath = iFilenamePath;
}


