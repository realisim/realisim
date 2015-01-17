
#include "MathUtils.h"

using namespace realisim;
	using namespace math;
using namespace std;

//----------------------------------------------------------------------------
/* applique l'équation d'une distribution normale.
voir 
https://en.wikipedia.org/wiki/Normal_distribution#Standard_normal_distribution
*/
double realisim::math::normalDistribution( double x, double s, double u /*= 0.0*/ )
{
  double e = 2.718281828459045235360287471352662497757247093;
  double sigmaSquare = s*s;
  double r = ( 1.0 / sqrt( 2.0 * 3.1415629f * sigmaSquare) ) * 
    pow(e, -( (x * x - u * u) / (2.0 * sigmaSquare) ) );
  return r;
}
  
//----------------------------------------------------------------------------
vector<double> realisim::math::gaussianKernel1D( int iKernelSize, double s )
{
  vector<double> r; r.resize( iKernelSize );
  int i, j = 0;
  for(i = -iKernelSize / 2; i <= iKernelSize / 2; ++i)
  {
    r[j] = normalDistribution( i, s);
    ++j;
  }
  return r;
}
  
//----------------------------------------------------------------------------
/*retourne un filtre gaussien 2d avec l'arrangement mémoire suivant (pour
  un kernel size de 3):
  a00, a01, a02,
  a10, a11, a12,
  a20, a21, a22
  */
vector< double > realisim::math::gaussianKernel2D( int iKernelSize, double s )
{
  iKernelSize = iKernelSize % 2 == 0 ? iKernelSize - 1 : iKernelSize;
  vector< double > r; r.resize( iKernelSize * iKernelSize );
      
  int i = 0, j = 0, ki, kj;
  for( j = 0, kj = -iKernelSize / 2; kj <= iKernelSize / 2; kj++, j++ )
  {
    for( i = 0, ki = -iKernelSize / 2; ki <= iKernelSize / 2; ki++, i++ )
    { r[ j * iKernelSize + i ] = normalDistribution( ki, s ) * normalDistribution( kj, s ); }
  }
  return r;
}

//----------------------------------------------------------------------------
/*retourne un filtre moyen 2d voir gaussianKernel2D*/
vector< double > realisim::math::meanKernel2D( int iKernelSize )
{
  vector< double > r; r.resize( iKernelSize * iKernelSize );
      
  int i, j;
  for( j = 0; j < iKernelSize; j++ )
    for( i = 0; i < iKernelSize ; i++ )
    { r[ j * iKernelSize + i ] = 1.0 / (iKernelSize * iKernelSize); }
  return r;
}
