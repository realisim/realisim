/**/

#pragma once
#include <QString>

namespace engine3d
{
namespace data
{
	class Image
	{
	public:
		Image();
		Image(const Image&);
		Image& operator=(const Image&);
		~Image();

		enum type {tDds, tHeightMap, tLightMap};

		int getBitDepth() const;
		QString getFilePath() const;
		int getNumberOfChannels() const;
		int getHeight() const;
		void* getPayload() const;
		unsigned int getPayloadSize() const;
		type getType() const;
		int getWidth() const;
		bool isLoaded() const;
		void load();
		void setBitDepth(int);
		void setFilePath(const QString);
		void setNumberOfChannels(int);
		void setType(type);

	private:
		struct Guts
		{
			Guts();

			int mRefCount;
			QString mAbsoluteFilePath;
			type mType;
			int mBitDepth; //bits per channel
			int mNumChannels; //RGBA = 4 channel
			int mWidth; //pixel
			int mHeight; //pixel
			void* mpPayload;
			unsigned int mPayloadSize;
			bool mIsLoaded;
		};

		void makeGuts();
		void shareGuts(Guts*);
		void deleteGuts();

		Guts *mpGuts;
  };
  
}
}
