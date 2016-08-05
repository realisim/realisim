
#include "Image.h"

using namespace engine3d;
  using namespace data;


//-------------------------------------------------------------------
//--- Image::Guts
//-------------------------------------------------------------------
Image::Guts::Guts() : mRefCount(1),
	mAbsoluteFilePath(),
	mType(tDds),
	mBitDepth(8),
	mNumChannels(4),
	mWidth(0),
	mHeight(0),
	mpPayload(nullptr),
	mPayloadSize(0)
{}

//-------------------------------------------------------------------
//--- Image
//-------------------------------------------------------------------
Image::Image() : mpGuts(nullptr)
{
	makeGuts(); 
}

//-------------------------------------------------------------------
Image::Image(const Image& i) : mpGuts(nullptr)
{
	shareGuts(i.mpGuts);
}

//-------------------------------------------------------------------
Image& Image::operator=(const Image& i)
{
	shareGuts(i.mpGuts);
	return *this;
}

//-------------------------------------------------------------------
Image::~Image()
{
	deleteGuts();
}

//-------------------------------------------------------------------
void Image::deleteGuts()
{
	//!!! Not thread safe
	if (mpGuts && --mpGuts->mRefCount == 0)
	{
		delete mpGuts;
		mpGuts = nullptr;
	}
}

//-------------------------------------------------------------------
int data::Image::getBitDepth() const
{
	return mpGuts->mBitDepth;
}

//-------------------------------------------------------------------
QString data::Image::getFilePath() const
{
	return mpGuts->mAbsoluteFilePath;
}

//-------------------------------------------------------------------
int data::Image::getNumberOfChannels() const
{
	return mpGuts->mNumChannels;
}

//-------------------------------------------------------------------
int data::Image::getHeight() const
{
	return mpGuts->mHeight;
}

//-------------------------------------------------------------------
void * data::Image::getPayload() const
{
	return mpGuts->mpPayload;
}

//-------------------------------------------------------------------
unsigned int data::Image::getPayloadSize() const
{
	return mpGuts->mPayloadSize;
}

//-------------------------------------------------------------------
Image::type data::Image::getType() const
{
	return mpGuts->mType;
}

//-------------------------------------------------------------------
int data::Image::getWidth() const
{
	return mpGuts->mWidth;
}

//-------------------------------------------------------------------
bool data::Image::isLoaded() const
{
	return mpGuts->mIsLoaded;
}

//-------------------------------------------------------------------
void data::Image::load()
{
	//open file via loaders?!?
}


//-------------------------------------------------------------------
void Image::makeGuts()
{
	mpGuts = new Guts();
}

//-------------------------------------------------------------------
void Image::shareGuts(Guts *ipGuts)
{
	//!!! Not thread safe
	if (mpGuts != ipGuts)
	{
		deleteGuts();
		mpGuts = ipGuts;
		++mpGuts->mRefCount;
	}
}

//-------------------------------------------------------------------
void data::Image::setBitDepth(int iDepth)
{
	mpGuts->mBitDepth = iDepth;
}

//-------------------------------------------------------------------
void data::Image::setFilePath(QString iPath)
{
	mpGuts->mAbsoluteFilePath = iPath;
}

//-------------------------------------------------------------------
void data::Image::setNumberOfChannels(int iN)
{
	mpGuts->mNumChannels = iN;
}

//-------------------------------------------------------------------
void data::Image::setType(type iType)
{
	mpGuts->mType = iType;
}