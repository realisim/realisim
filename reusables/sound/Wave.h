#pragma once

#include <fstream>
#include <string>
#include <stdint.h>

namespace realisim
{
namespace sound
{

    class Wave
    {
    public:
        Wave();
        Wave(const Wave&) = delete;
        Wave& operator=(const Wave&) = delete;
        ~Wave();

        enum AudioFormat {afUndefined = 0, afPcm = 1, afAlaw = 6, afUlaw = 7, afIma4adpcm = 17 };

        void clear();
        AudioFormat getAudioFormat() const;
        int getBitsPerSample() const;
        int getByteRate() const;
        const std::string& getFilenamePath() const;
        int getNumberOfChannels() const;
        int getSamplingFrequency() const;
        unsigned char* getSoundData() const;
        int getSoundDataSize() const;
        unsigned char* giveOwnershipOfSoundData();
        bool hasSoundData() const;
        bool isValid() const;
        void load();
        void loadHeader();
        void setFilenamePath(const std::string&);

    private:
        bool parseHeader(std::ifstream& ifs);
        bool parsePayload(std::ifstream& ifs);
        
        bool mIsValid;
        bool mHasSoundDataOwnership;
        std::string mFilenamePath;

        uint16_t mAudioFormat;
        uint16_t mNumberOfChannels;
        uint32_t mSamplingFrequency;
        uint32_t mByteRate; //== mSamplingFrequency * mNumberOfChannels * BitsPerSample/8
        uint16_t mBytesPerBlock;    //== NumChannels * BitsPerSample/8
                                    //The number of bytes for one sample including
                                    //all channels. I wonder what happens when
                                    //this number isn't an integer?
        uint16_t mBitsPerSample;
        uint32_t mSoundDataSize; //in bytes
        unsigned char *mpSoundData;
    };

}
}