
#ifndef MATRIX_4_H
#define MATRIX_4_H

#include <limits>
#include "math/Vect.h"
#include "math/Quaternion.h"
#include "QString.h"

namespace realisim
{
namespace math
{

/*Classe qui représente un matrice 4x4 de la forme
		
   Représentation mathématique, tel que présenté par l'interface (voir methode
   	operator()(int, int)
   m = a00 a01 a02 a03    m = s r r t  -> r: rotation
       a10 a11 a12 a13        r s r t     s: scale
       a20 a21 a22 a23        r r s t     t: translation
       a30 a31 a32 a33        x x x 1
  
   Representation mémoire.
   m = 0  4  8  12  =>  a00 a10 a20 a30
       1  5  9  13      a01 a11 a21 a31
       2  6  10 14      a02 a12 a22 a32
       3  7  11 15      a03 a13 a23 a33
       
  La matrice est de type column-major (pour aller avec OpenGL. En effet, openGL
  s'attend a recevoir un pointeur sur une matrice de type column-major. On
  pourrait avoir une représentation row=major, mais il faudrait transposer la
  matrice avant de la passer à openGL).
  Les accesseurs présentent la matrice comme dans les livres de mathématique. 
  C'est à dire ligne-colonne (row-major). L'indice i,j de la matrice veut dire
  ieme ligne et jieme colonne, comme on s'y attendrait.
    
  Par défaut la matrice est initialisée comme une matrice identité.
  il est possible d'obtenir la valeur i, j de la matrice par l'opérateur ().
  
  
    notes consernant le row-major vs column-major:
  Pour plus d'explication sur la matrice row-major et column major voir
  https://www.opengl.org/discussion_boards/showthread.php/167648-Matrix-stacks-and-post-multiplication?p=1182985#post1182985
  
  Il faut être conscient qu'il se cache une subtilité dans le propos...
  
  AxB = C qu'on soit column-major ou row-major. La réponse sur le papier est
  toujour la même, par contre la représentation mémoire est différente. C'Est
  la que réside le problème.
  Par exemple
  A = | 1 2 | B = | 5 6 |
      | 3 4 |     | 7 8 |
      
  AxB = | 19 22 |
        | 43 50 |
        
	La représentation mémoire row-major =    | 19 22 43 50 |
  La représentation mémoire column-major = | 19 43 22 50 |
  
  On constate qu'il s'agit de la transposé. On pourrait s'arreté ici, mais les
  amis astucieux et inquiet du nombre de cycles d'horloge dépensés pour faire la
  transposé ont imaginé l'astuce suivante.
  
  La transposé de AxB = BtxAt
  (AxB)t = BtxAt = | 5 7 | | 1 3 | = | 19 43 |
                   | 6 8 | | 2 4 |   | 22 50 |
  
	La représentation mémoire row-major =    | 19 43 22 50 |
  La représentation mémoire column-major = | 19 22 43 50 |
  
  L'astuce est de cacher la transposition dans l'operateur * de la matrice...
  au lieur de faire 
  	a00 = a00*a00 + a10 * a01 on fait a00 = a00*a00 + a01 * a10 et ainsi de suite.
  En gros on cache la transposition dans l'opérateur *
  	
  Ainsi, on peut écrire
  
  (AxB)t = BxA = | 5 6 | | 1 2 | = | 19 43 |
                 | 7 8 | | 3 4 |   | 22 50 |
  
  C'est pour quoi le lien plus haut indique que P*V*M * v1 = v2 est strictement
  égale à v1 * M*V*P = v2... Ce qui est totalement faut, il s'agit d'une
  raccourcit entre matrice column-major et row-major.
  
  Ce qui serait exact est:
  P*V*M * v1 = v2 est strictement égale à Mt*Vt*Pt * v1 = v2

*/

class myMatrix4
{
public:
  myMatrix4();
  //myMatrix4( const myMatrix4& ); //explicitement absent
  //myMatrix4& operator= (const myMatrix4&); //explicitement absent
  myMatrix4( const double*, bool iRowMajor = true );
  myMatrix4( Vector3d ); //translation
  myMatrix4( Quaterniond ); //rotation
  //myMatrix4( Vector3d, Quaterniond, Vector3d = Vector3d(1.0) ); //translation - rotation - scaling
  myMatrix4( double, Vector3d ); //rotation (angle et axe)
  myMatrix4( Vector3d, Vector3d, Vector3d ); //specification de la base
  ~myMatrix4();

	double operator()(int, int) const;
  double& operator()(int, int);
  bool operator== (const myMatrix4&) const;
  bool operator!= (const myMatrix4&) const;
  myMatrix4 operator* (const myMatrix4&) const;
  myMatrix4& operator*= (const myMatrix4&);
  Point3d operator* (const Point3d&) const;
  Point2d operator* (const Point2d&) const;
  Vector3d operator* (const Vector3d&) const;
  Vector2d operator* (const Vector2d&) const;
//myMatrix4 operator+ (const myMatrix4&);
//myMatrix4& operator+= (const myMatrix4&);
//myMatrix4 operator- (const myMatrix4&);
//myMatrix4& operator-= (const myMatrix4&);

	const double* getDataPointer() const;
  Quaterniond getRotationAsQuaternion() const;
  Vector3d getTranslationAsVector() const;
  myMatrix4 inverse() const;
  myMatrix4& invert();
  bool isEqual( const myMatrix4&, 
  	double = std::numeric_limits<double>::epsilon() ) const;
  myMatrix4 transpose() const;
  QString toString() const;
  
protected:
	void identity();
	void import( const double*, bool iRowMajor = true );

  double m[4][4];
};

} //math
} // fin du namespace realisim

#endif // MATRIX_H
