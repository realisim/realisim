#include <math/Vect.h>

using namespace realisim;
using namespace math;

//-----------------------------------------------------------------------------
//--- Vector4d
//-----------------------------------------------------------------------------
Vector4d::Vector4d()
{ mData[0] = mData[1] = mData[2] = mData[3] = 0.0; }

//-----------------------------------------------------------------------------
Vector4d::Vector4d(double x, double y, double z, double w)
{ set(x, y, z, w); }

//-----------------------------------------------------------------------------
Vector4d::Vector4d(const Vector3<double>& v3, double w)
{
	mData[0] = v3.x();
	mData[1] = v3.y();
	mData[2] = v3.z();
	mData[3] = w;
}

//-----------------------------------------------------------------------------
Vector4d::Vector4d(double v)
{
	mData[0] = mData[1] = mData[2] = mData[3] = v;
}

//-----------------------------------------------------------------------------
Vector4d::Vector4d(const Vector4d& iV)
{
	mData[0] = iV.x();
	mData[1] = iV.y();
	mData[2] = iV.z();
	mData[3] = iV.w();
}

//-----------------------------------------------------------------------------
Vector4d& Vector4d::operator=(const Vector4d& iV)
{
	mData[0] = iV.x();
	mData[1] = iV.y();
	mData[2] = iV.z();
	mData[3] = iV.w();
	return *this;
}

//-----------------------------------------------------------------------------
Vector4d::~Vector4d()
{}

//-----------------------------------------------------------------------------
Vector4d Vector4d::operator+(const Vector4d& iV)
{
	return Vector4d(mData[0] + iV.mData[0],
		mData[1] + iV.mData[1],
		mData[2] + iV.mData[2],
		mData[3] + iV.mData[3]);
}

//-----------------------------------------------------------------------------
Vector4d& Vector4d::operator+=(const Vector4d& iV)
{
	mData[0] = mData[0] + iV.mData[0];
	mData[1] = mData[1] + iV.mData[1];
	mData[2] = mData[2] + iV.mData[2];
	mData[3] = mData[3] + iV.mData[3];
	return *this;
}


//-----------------------------------------------------------------------------
Vector4d Vector4d::operator-(const Vector4d& iV)
{
	return Vector4d(mData[0] - iV.mData[0],
		mData[1] - iV.mData[1],
		mData[2] - iV.mData[2],
		mData[3] - iV.mData[3]);
}

//-----------------------------------------------------------------------------
Vector4d& Vector4d::operator-=(const Vector4d& iV)
{
	mData[0] = mData[0] - iV.mData[0];
	mData[1] = mData[1] - iV.mData[1];
	mData[2] = mData[2] - iV.mData[2];
	mData[3] = mData[3] - iV.mData[3];
	return *this;
}

//-----------------------------------------------------------------------------
void Vector4d::set(double x, double y, double z, double w)
{
	mData[0] = x;
	mData[1] = y;
	mData[2] = z;
	mData[3] = w;
}

//-----------------------------------------------------------------------------
void Vector4d::setX(double x)
{ mData[0] = x; }

//-----------------------------------------------------------------------------
void Vector4d::setY(double y)
{ mData[1] = y; }

//-----------------------------------------------------------------------------
void Vector4d::setZ(double z)
{ mData[2] = z; }

//-----------------------------------------------------------------------------
void Vector4d::setW(double w)
{ mData[3] = w; }

//-----------------------------------------------------------------------------
std::string Vector4d::toString(int iPrecision /*=3*/) const
{
	std::stringstream iss;
	iss << std::fixed << std::setprecision(iPrecision);
	iss << "(" << x() << ", " << y() << ", " << z() << ", " << w() << ")";
	return iss.str();
}

//-----------------------------------------------------------------------------
double Vector4d::x() const
{ return mData[0]; }

//-----------------------------------------------------------------------------
double Vector4d::y() const
{ return mData[1]; }

//-----------------------------------------------------------------------------
double Vector4d::z() const
{ return mData[2]; }

//-----------------------------------------------------------------------------
double Vector4d::w() const
{ return mData[3]; }