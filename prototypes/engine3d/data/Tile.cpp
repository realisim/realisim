
#include "Tile.h"

using namespace engine3d;
using namespace data;

//-------------------------------------------------------------------
//--- Tile::Guts
//-------------------------------------------------------------------
Tile::Guts::Guts() : mRefCount(1),
	mLatitude(0),
	mLongitude(0),
	mAlbedoHighResolution(),
	mAlbedoLowResolution(),
	mHeightMap(),
	mNormalMap(),
	mLightPoint()
{}

//-------------------------------------------------------------------
//--- Tile
//-------------------------------------------------------------------
Tile::Tile() : mpGuts(nullptr)
{
	makeGuts();
}

//-------------------------------------------------------------------
Tile::Tile(const Tile& iT) : mpGuts(nullptr)
{
	shareGuts(iT.mpGuts);
}

//-------------------------------------------------------------------
Tile& Tile::operator=(const Tile& iT)
{
	shareGuts(iT.mpGuts);
	return *this;
}

//-------------------------------------------------------------------
Tile::~Tile()
{
	deleteGuts();
}

//-------------------------------------------------------------------
void Tile::deleteGuts()
{
	//!!! Not thread safe
	if (mpGuts && --mpGuts->mRefCount == 0)
	{
		delete mpGuts;
		mpGuts = nullptr;
	}
}

//-------------------------------------------------------------------
Image Tile::getAlbedoHighResolution()
{
	return mpGuts->mAlbedoHighResolution;
}

//-------------------------------------------------------------------
Image Tile::getAlbedoLowResolution()
{
	return mpGuts->mAlbedoLowResolution;
}

//-------------------------------------------------------------------
void Tile::makeGuts()
{
	mpGuts = new Guts();
}

//-------------------------------------------------------------------
void Tile::setLatitude(double iLat)
{
	mpGuts->mLatitude = iLat;
}

//-------------------------------------------------------------------
void Tile::setLongitude(double iLong)
{
	mpGuts->mLongitude = iLong;
}

//-------------------------------------------------------------------
void Tile::shareGuts(Guts *ipGuts)
{
	//!!! Not thread safe
	if (mpGuts != ipGuts)
	{
		deleteGuts();
		mpGuts = ipGuts;
		++mpGuts->mRefCount;
	}
}