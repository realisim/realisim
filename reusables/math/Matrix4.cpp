
#include "cassert"
#include "math/Matrix4.h"
#include "math/MathUtils.h"

using namespace realisim;
	using namespace math;
  
namespace
{
	/*La matrice augmentée est row-major et on la remet en column-major puisque
    c'est la représentation interne choisie pour Matrix4*/
	void augmentedMatrixToMatrix4( const double* iAug, Matrix4& A, Matrix4 &B )
  {
  	double mA[4][4], mB[4][4];
    //double aug[4][8];
    //La matrice augmenté est sout la forme [ A | B ]
		//on transpose la matrice iAug a la volé
    int nbCols = 8;
    for( int i = 0; i < 4; ++i )
    	for( int j = 0; j < 4; ++j )
    	{ 
      	mA[i][j] = *(iAug + ( j*nbCols ) + i );
        mB[i][j] = *(iAug + ( j*nbCols ) + (i+4) );
      }
    A = Matrix4( mA[0], false );
    B = Matrix4( mB[0], false ); 
  }
  
  void printAugmentedMatrix( const double* iM )
  {
		QString s;
  	int nbRows = 4, nbCols = 8;
    for( int i = 0; i < nbRows; ++i )
    {
    	for( int j = 0; j < nbCols; ++j )
      {
      	if( j == 4 ) { s+= "| "; }
      	s += QString::number( *(iM + i*nbCols + j), 'f', 4 ) + " ";
      }
      s += "\n";
    }
    printf("%s\n", s.toStdString().c_str());
  }
}

//------------------------------------------------------------------------------
Matrix4::Matrix4()
{ identity(); }
//------------------------------------------------------------------------------
/*Constructeur qui recoit un pointeur sur un tableau de double. Le boolein 
	iRowMajor specifie l'agencement mémoire du tableau pointé par ipM. */
Matrix4::Matrix4( const double* ipM, bool iRowMajor /*= true*/ )
{ import( ipM, iRowMajor ); }
//------------------------------------------------------------------------------
Matrix4::Matrix4( Vector3d iV )
{
	identity();
  m[3][0] = iV.x(); m[3][1] = iV.y(); m[3][2] = iV.z();
}
//------------------------------------------------------------------------------
Matrix4::Matrix4( Quaterniond iQ )
{
	double x = iQ.x(), y = iQ.y(), z = iQ.z(), w = iQ.w();
  double mat[4][4] = {
    { 1-(2*y*y)-(2*z*z),
      (2*x*y)-(2*w*z),
      (2*x*z)+(2*w*y),
      0 },
    { (2*x*y)+(2*w*z),
      1-(2*x*x)-(2*z*z),
      (2*y*z)-(2*w*x),
      0 },
    { (2*x*z)-(2*w*y),
      (2*y*z)+(2*w*x),
      1-(2*x*x)-(2*y*y),
      0 },
    { 0, 0, 0, 1 }
  };
  import( mat[0] );
}
//------------------------------------------------------------------------------
/*angle en radian et axe de rotation */
Matrix4::Matrix4( double iAngle, Vector3d iAxis )
{
	Quaterniond q;
  iAxis.normalise();
  q.setRot( iAngle, iAxis );
  *this = Matrix4( q );
}
//------------------------------------------------------------------------------
/*Matrice de passage

http://fr.wikipedia.org/wiki/Matrice_de_passage
*/
Matrix4::Matrix4( Vector3d iX, Vector3d iY, Vector3d iZ )
{
	double mat[4][4] = {
  	{ iX.x(), iY.x(), iZ.x(), 0 },
    { iX.y(), iY.y(), iZ.y(), 0 },
    { iX.z(), iY.z(), iZ.z(), 0 },
    { 0, 0, 0, 1 }
  };
  import( mat[0] );
}
//------------------------------------------------------------------------------
Matrix4::~Matrix4()
{}
//------------------------------------------------------------------------------
const double* Matrix4::getDataPointer() const
{ return m[0]; }
//------------------------------------------------------------------------------
Quaterniond Matrix4::getRotationAsQuaternion() const
{
	double x, y, z, w;
 	double trace = m[0][0] + m[1][1] + m[2][2];
  if( trace > 0.0 ) 
  {
    double s = sqrt(trace+1.0) * 2.0;
    w = 0.25 * s;
    x = ( m[1][2] - m[2][1] ) / s;
    y = ( m[2][0] - m[0][2] ) / s;
    z = ( m[0][1] - m[1][0] ) / s;
  } 
  else if ( m[0][0] > m[1][1] && m[0][0] > m[2][2] ) 
  {
    double s = sqrt(1.0 + m[0][0] - m[1][1] - m[2][2]) * 2;
    w = ( m[1][2] - m[2][1] ) / s;
    x = 0.25 * s;
    y = ( m[0][1] + m[1][0] ) / s;
    z = ( m[2][0] + m[0][2] ) / s;
  } 
  else if ( m[1][1] > m[2][2] ) 
  {
    double s = sqrt(1.0 + m[1][1] - m[0][0] - m[2][2] ) * 2;
    w = ( m[2][0] - m[0][2] ) / s;
    x = ( m[0][1] + m[1][0] ) / s;
    y = 0.25 * s;
    z = ( m[1][2] + m[2][1] ) / s;
  } 
  else 
  {
    double s = sqrt(1.0 + m[2][2] - m[0][0] - m[1][1] ) * 2;
    w = ( m[0][1] - m[1][0] ) / s;
    x = ( m[2][0] + m[0][2] ) / s;
    y = ( m[1][2] + m[2][1] ) / s;
    z = 0.25 * s;
  }
  Quaterniond r( w, x, y, z );
  r.normalize();
  return r;
}
//------------------------------------------------------------------------------
/*La translation est dans la colonne 3*/
Vector3d Matrix4::getTranslationAsVector() const
{ return Vector3d( m[3][0], m[3][1], m[3][2] ); }
//------------------------------------------------------------------------------
void Matrix4::identity()
{
	memset( &m, 0, 16*sizeof(double) );
  m[0][0] = 1.0; m[1][1] = 1.0; m[2][2] = 1.0; m[3][3] = 1.0;
}
//------------------------------------------------------------------------------
void Matrix4::import( const double* ipM, bool iRowMajor /*=true*/ )
{
	memcpy( &m, ipM, 16*sizeof(double) );
	if( iRowMajor )
  { *this = transpose(); }
}
//------------------------------------------------------------------------------
/*L'élimination de Gauss-Jordan est utilisée pour effectuer l'inverse de la 
  matrice.
http://fr.wikipedia.org/wiki/%C3%89limination_de_Gauss-Jordan

Ne pas oublier que la representation interne est column-major. Ce qui veut
dire que A[colonne][ligne]. Pour simplifier, la matrice augmenter est en
row-major et les commentaires sont en notation mathématique (row-major)
A[ligne, colonne]. */
Matrix4 Matrix4::inverse() const
{  
  //la matrice augmenter est comme suis [A | I]
  double aug[4][8];  
  aug[0][0]=m[0][0];aug[1][0]=m[0][1];aug[2][0]=m[0][2];aug[3][0]=m[0][3]; //colonne 0 de matrice gauche (A)  
  aug[0][1]=m[1][0];aug[1][1]=m[1][1];aug[2][1]=m[1][2];aug[3][1]=m[1][3]; //colonne 1 de matrice gauche (A)
  aug[0][2]=m[2][0];aug[1][2]=m[2][1];aug[2][2]=m[2][2];aug[3][2]=m[2][3]; //colonne 2 de matrice gauche (A)
  aug[0][3]=m[3][0];aug[1][3]=m[3][1];aug[2][3]=m[3][2];aug[3][3]=m[3][3]; //colonne 3 de matrice gauche (A)
  //La partie droite de la matrice est une matrice identité.
  aug[0][4]=1;aug[1][4]=0;aug[2][4]=0;aug[3][4]=0; //colonne 4 de matrice droite (I)
  aug[0][5]=0;aug[1][5]=1;aug[2][5]=0;aug[3][5]=0; //colonne 5 de matrice droite (I)
  aug[0][6]=0;aug[1][6]=0;aug[2][6]=1;aug[3][6]=0; //colonne 6 de matrice droite (I)
  aug[0][7]=0;aug[1][7]=0;aug[2][7]=0;aug[3][7]=1; //colonne 7 de matrice droite (I)
  //printAugmentedMatrix( aug[0] );
  
  int r = -1, k = 0, nbRows = 4, nbCols = 8;
    
  //pour toutes les colonnes
  for( int j = 0; j < nbCols; ++j )
  {
  	double maxInColumn = 0.0;
	  //étape 2.1; on cherche le max en valeur abs dans la colonne j
  	for( int v = r+1; v < nbRows; ++v )
    {
    	//k égale l'indice de ligne où le max absolu à été trouvé.
    	if( maxInColumn < abs(aug[v][j]) )
    	{ k = v; maxInColumn = abs(aug[v][j]); }
    }
  
    //etape 2.2
    if( !math::isEqual( maxInColumn, 0.0 ) )
    {
      //etape 2.2.1
      r++;
      
      //etape 2.2.2
      if( k != r ) //on echange les ligne k et r
      {
      	double t;
      	for( int u = 0; u < nbCols; ++u )
        {
        	t = aug[k][u];
          aug[k][u] = aug[r][u];
          aug[r][u] = t;
        }
      }
      //printAugmentedMatrix( aug[0] );
      
      //etape 2.2.3 diviser ligne r par A[r,j]
      double augRj = aug[r][j];
      for( int u = 0; u < nbCols; ++u )
      { aug[r][u] /= augRj; }
      //printAugmentedMatrix( aug[0] );
      
      //etape 2.2.4 pour i variant de 1 à n, i différent de r
      //soustraire à la ligne i la ligne r multipliée par 
      //A[i,j] (de façon à annuler A[i,j])
      for( int i = 0; i < nbRows; ++i )
      {
      	if( i != r )
        {
        	double augIj = aug[i][j];
      		for( int u = 0; u < nbCols; ++u ) 
      		{ aug[i][u] -= augIj * aug[r][u] ; }
        	//printAugmentedMatrix( aug[0] );
        }  
      }
    }
  }
  
  Matrix4 identity, supposedlyIdentity, inverse;
	augmentedMatrixToMatrix4( aug[0], supposedlyIdentity, inverse );
  if( supposedlyIdentity != identity ) { inverse = identity; }
  return inverse;
}
//------------------------------------------------------------------------------
Matrix4& Matrix4::invert()
{
	*this = this->inverse();
	return *this;
}
//------------------------------------------------------------------------------
bool Matrix4::isEqual( const Matrix4& iM, double iEpsilon ) const
{
	return 
  	math::isEqual( m[0][0], iM.m[0][0], iEpsilon ) &&
  	math::isEqual( m[0][1], iM.m[0][1], iEpsilon ) &&
    math::isEqual( m[0][2], iM.m[0][2], iEpsilon ) &&
    math::isEqual( m[0][3], iM.m[0][3], iEpsilon ) &&
    math::isEqual( m[1][0], iM.m[1][0], iEpsilon ) &&
    math::isEqual( m[1][1], iM.m[1][1], iEpsilon ) &&
    math::isEqual( m[1][2], iM.m[1][2], iEpsilon ) &&
    math::isEqual( m[1][3], iM.m[1][3], iEpsilon ) &&
    math::isEqual( m[2][0], iM.m[2][0], iEpsilon ) &&
    math::isEqual( m[2][1], iM.m[2][1], iEpsilon ) &&	
    math::isEqual( m[2][2], iM.m[2][2], iEpsilon ) &&
    math::isEqual( m[2][3], iM.m[2][3], iEpsilon ) &&
    math::isEqual( m[3][0], iM.m[3][0], iEpsilon ) &&
    math::isEqual( m[3][1], iM.m[3][1], iEpsilon ) &&	
    math::isEqual( m[3][2], iM.m[3][2], iEpsilon ) &&
    math::isEqual( m[3][3], iM.m[3][3], iEpsilon );

}
//------------------------------------------------------------------------------
double Matrix4::operator()(int i, int j) const
{ return m[j][i]; }
//------------------------------------------------------------------------------
double& Matrix4::operator()(int i, int j)
{ return m[j][i]; }
//------------------------------------------------------------------------------
bool Matrix4::operator== (const Matrix4& iM) const
{ return isEqual( iM ); }
//------------------------------------------------------------------------------
bool Matrix4::operator!= (const Matrix4& iM) const
{ return !isEqual( iM ); }
//------------------------------------------------------------------------------
Matrix4 Matrix4::operator* (const Matrix4& iM) const
{
	Matrix4 r;
  for( int i = 0; i < 4; ++i ) //ligne
  {
  	for( int j = 0; j < 4; ++j ) //colonne
    {
    	double v = 0;
    	for( int u = 0; u < 4; ++u )
      { v += m[u][i] * iM.m[j][u]; } //somme de ligne * colone
    	r.m[j][i] = v; //on remplit la ligne i
    }
  }
  return r;
}
//------------------------------------------------------------------------------
Matrix4& Matrix4::operator*= (const Matrix4& iM)
{
	*this = *this * iM;
	return *this;
}
//------------------------------------------------------------------------------
Vector3d Matrix4::operator* (const Vector3d& iV) const
{
  double x,y,z;
  x = m[0][0]*iV.x() + m[1][0]*iV.y() + m[2][0]*iV.z();
  y = m[0][1]*iV.x() + m[1][1]*iV.y() + m[2][1]*iV.z();
  z = m[0][2]*iV.x() + m[1][2]*iV.y() + m[2][2]*iV.z();
//  w = m[0][3]*iV.x() + m[1][3]*iV.y() + m[2][3]*iV.z();
	return Vector3d( x, y, z );
}
//------------------------------------------------------------------------------
Vector2d Matrix4::operator* (const Vector2d& iV) const
{
  double x,y;
  x = m[0][0]*iV.x() + m[1][0]*iV.y() + m[2][0]*0;
  y = m[0][1]*iV.x() + m[1][1]*iV.y() + m[2][1]*0;
//  z = m[0][2]*iV.x() + m[1][2]*iV.y() + m[2][2]*0;
//  w = m[0][3]*iV.x() + m[1][3]*iV.y() + m[2][3]*0;
	return Vector2d( x, y ); 
}
//------------------------------------------------------------------------------
Point3d Matrix4::operator* (const Point3d& iV) const
{
  double x,y,z,w;
  x = m[0][0]*iV.x() + m[1][0]*iV.y() + m[2][0]*iV.z() + m[3][0]*1;
  y = m[0][1]*iV.x() + m[1][1]*iV.y() + m[2][1]*iV.z() + m[3][1]*1;
  z = m[0][2]*iV.x() + m[1][2]*iV.y() + m[2][2]*iV.z() + m[3][2]*1;
  w = m[0][3]*iV.x() + m[1][3]*iV.y() + m[2][3]*iV.z() + m[3][3]*1;
	return Point3d( x/w, y/w, z/w );
}
//------------------------------------------------------------------------------
Point2d Matrix4::operator* (const Point2d& iV) const
{
  double x,y,w;
  x = m[0][0]*iV.x() + m[1][0]*iV.y() + m[2][0]*0 + m[3][0]*1;
  y = m[0][1]*iV.x() + m[1][1]*iV.y() + m[2][1]*0 + m[3][1]*1;
//  z = m[0][2]*iV.x() + m[1][2]*iV.y() + m[2][2]*0 + m[3][2]*1;
  w = m[0][3]*iV.x() + m[1][3]*iV.y() + m[2][3]*0 + m[3][3]*1;
	return Point2d( x/w, y/w ); 
}
//------------------------------------------------------------------------------
void Matrix4::setTranslation( const Vector3d& iP )
{ m[3][0]=iP.x(); m[3][1] = iP.y(); m[3][2] = iP.z(); }
//------------------------------------------------------------------------------
/*Retourne un string formater qui presente la matrice sous forme row-major.*/
QString Matrix4::toString() const
{
	QString s;
  const Matrix4& mat = *this;
  for( int i = 0; i < 4; ++i)
  {
	  for( int j = 0; j < 4; ++j)
    { 
    	s += QString::number( mat(i, j), 'g', 4 );
      s += (j<3) ? ",":"";
    }
    s += "\n";
  }
  return s;
}
//------------------------------------------------------------------------------
Matrix4 Matrix4::transpose() const
{
	Matrix4 r; 
  r.m[0][0] = m[0][0]; r.m[0][1] = m[1][0]; r.m[0][2] = m[2][0]; r.m[0][3] = m[3][0];
  r.m[1][0] = m[0][1]; r.m[1][1] = m[1][1]; r.m[1][2] = m[2][1]; r.m[1][3] = m[3][1];
  r.m[2][0] = m[0][2]; r.m[2][1] = m[1][2]; r.m[2][2] = m[2][2]; r.m[2][3] = m[3][2];
  r.m[3][0] = m[0][3]; r.m[3][1] = m[1][3]; r.m[3][2] = m[2][3]; r.m[3][3] = m[3][3];
  return r;
}