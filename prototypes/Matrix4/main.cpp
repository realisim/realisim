
#include <iostream>
#include "math/Matrix4.h"

using namespace realisim;
	using namespace math;

void printMatrix( const myMatrix4& m )
{ printf( "%s\n", m.toString().toStdString().c_str() ); }

int main(int argc, char** argv)
{
	
  {
  printf("---constructeur()\n");
  myMatrix4 m;
  printMatrix( m );
  }
  
  {
  printf("---constructeur( const double* ([][]), rowMajor = true )\n");
  double mat[4][4] = { {1,2,3,4}, {5,6,7,8}, {9,10,11,12}, {13,14,15,16} };
  myMatrix4 m( mat[0] );
  printMatrix( m );
  }
  
  {
  printf("---constructeur( const double* ([][]), rowMajor = false )\n");
  double mat[4][4] = { {1,2,3,4}, {5,6,7,8}, {9,10,11,12}, {13,14,15,16} };
  myMatrix4 m( mat[0], false );
  printMatrix( m );
  }
  
  {
  printf("---constructeur( const double* ([]), rowMajor = true )\n");
  double mat[16] = { 1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16 };
  myMatrix4 m( mat );
  printMatrix( m );
  }
  
  {
  printf("---constructeur( const double* ([]), rowMajor = false )\n");
  double mat[16] = { 1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16 };
  myMatrix4 m( mat, false );
  printMatrix( m );
  }
  
  {
  printf("---constructeur( const &myMatrix )\n");
  double mat[4][4] = { {1,2,3,4}, {5,6,7,8}, {9,10,11,12}, {13,14,15,16} };
  myMatrix4 m( mat[0] );
  myMatrix4 m2(m), m3;
  printMatrix( m2 );
  
  printf("---operateur =\n");
  m3 = m;
  printMatrix( m3 );
  }
  
  {
  	printf("---constructeur( Quaterniond ) \n");
  	Quaterniond q( 1, 1, 2, 3 );
    q.normalize();
    printf("quaternion normalisé \n");
    printf( "%.4f, %.4f, %.4f, %4f\n", q.w(), q.x(), q.y(), q.z() );
    printf("donne la matrice suivante: \n");
    myMatrix4 m( q );
    printMatrix( m );
    printf("getRotationAsQuaternion de la matrice donne le quat normalisé suivant:\n");
    Quaterniond q2 = m.getRotationAsQuaternion();
    printf( "%.4f, %.4f, %.4f, %4f\n", q2.w(), q2.x(), q2.y(), q2.z() );
  }
  
  {
  printf("---constructeur( vector3d )\n");
	Vector3d x( 1, 2, 3 );
  myMatrix4 m( x );
  printMatrix( m );
  }
  
  {
  printf("---constructeur( double iAngle, Vector3d iAxis )\n");
	Vector3d v( 0, 1, 0 );
  double angle = 3.14159265 / 4.0;
  myMatrix4 m( angle, v );
  printMatrix( m );
  }
  
  
  {
  printf("---constructeur( vector3d, vector3d, vector3d )\n");
	Vector3d x( 1, 2, 3 );
  Vector3d y( 4, 5, 6 );
  Vector3d z( 7, 8, 9 );
  myMatrix4 m( x, y, z );
  printMatrix( m );
  }
  
  {
  double mat[4][4] = { {1,2,3,4}, {5,6,7,8}, {9,10,11,12}, {13,14,15,16} };
  myMatrix4 m( mat[0] );
  printf("---transpose \n");
  m = m.transpose();
  printMatrix(m);
  }
  
  {
  double mat[4][4] = { {1,2,3,4}, 
  										 {5,6,7,8}, 
                       {9,10,11,12}, 
                       {13,14,15,16} };
  myMatrix4 m1( mat[0] ), m2( mat[0] ), m3;
  printf("---operator == \n");
	printf( "les matrices m1 et m2 sont égales: %s\n", m1 == m2 ? "oui":"non" );
  printf( "les matrices m1 et m3 sont égales: %s\n", m1 == m3 ? "oui":"non" );
  }
  
  {
  double mat[4][4] = { {1,2,3,4}, 
  										 {5,6,7,8}, 
                       {9,10,11,12}, 
                       {13,14,15,16} };
  myMatrix4 m1( mat[0] ), m2( mat[0] ), m3;
  printf("---operator != \n");
	printf( "les matrices m1 et m2 sont différentes: %s\n", m1 != m2 ? "oui":"non" );
  printf( "les matrices m1 et m3 sont édifférente: %s\n", m1 != m3 ? "oui":"non" );
  }
  
  {
  double mat[4][4] = { {1,2,3,4}, 
  										 {5,1,7,8}, 
                       {9,10,1,12}, 
                       {0,0,0,1} };
  myMatrix4 m( mat[0] );
  printf("---inverse \n");
  m = m.inverse();
  printMatrix(m);
  }
  
  {
  double mat[4][4] = { {1,2,3,4}, 
  										 {5,6,7,8}, 
                       {9,10,11,12}, 
                       {13,14,15,16} };
  myMatrix4 m( mat[0] );
  printf("---inverse d'une matrice non reversible \n");
  m = m.inverse();
  printMatrix(m);
  }
  
  {
  double mat[4][4] = { {1,2,3,4}, 
  										 {5,6,7,8}, 
                       {9,10,11,12}, 
                       {13,14,15,16} };
  Vector3d v3d(1, 2, 3);
  Vector2d v2d(1, 2);
  myMatrix4 m( mat[0] );
  printf("---operator* (const Vector3d&) const \n");
	Vector3d r3d = m * v3d;
  printf( "%.4f, %.4f, %.4f\n", r3d.x(), r3d.y(), r3d.z() );
  
  printf("---operator* (const Vectorsd&) const \n");
	Vector2d r2d = m * v2d;
  printf( "%.4f, %.4f \n", r2d.x(), r2d.y() );
  }
  
  {
  double mat[4][4] = { {1,2,3,4}, 
  										 {5,6,7,8}, 
                       {9,10,11,12}, 
                       {13,14,15,16} };
  double mat2[4][4] = { {1,2,3,4}, 
  										 {5,6,7,8}, 
                       {9,10,11,12}, 
                       {13,14,15,16} };
  myMatrix4 m( mat[0] ), m2( mat2[0] );
  printf("---operator* (const Matrix4&) const \n");
  myMatrix4 r = m * m2;
  printMatrix(r);
  }
  
  {
  double mat[4][4] = { {1,2,3,4}, 
  										 {5,6,7,8}, 
                       {9,10,11,12}, 
                       {13,14,15,16} };
  myMatrix4 m( mat[0] );
  printf("---getTranslationAsVector \n");
  Vector3d v = m.getTranslationAsVector();
  printf( "%.4f, %.4f, %.4f\n", v.x(), v.y(), v.z() );
  }
  
  
  return 0;
}
