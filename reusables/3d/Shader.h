
#ifndef Realisim_TreeD_Shader_hh
#define Realisim_TreeD_Shader_hh

#include <QString>
#include <qgl.h>
#include <math/Point.h>
#include <math/Vect.h>
#include <math/Matrix4x4.h>

/* Cette classe sert à créer et interacgir avec un shader OpenGL. Les
   ressources nécessaires pour le(les) shaders (geometry, vertex et fragment)
   ainsi que pour le program sont prises en chargent par la classe Shader. De 
   plus, afin de gérer efficacement les ressources openGL, la classe Shader
   utilise le partage explicite (voir méthode copy pour comprendre pourquoi
   on ne peut pas utiliser le partage implicite). La copie d'un Shader doit être
   faite par un appel à la méthode copy(). Le constructeur copie et l'opérateur=
   ne font qu'incrémenter le compte de référence sur les guts (les ressources
   openGL).
   
   L'utilisation classique de la classe Shader est la suivante.
   Shader s;
   s.addVertexShaderSource(...);
   s.addFragmentShaderSource(...);
   s.link();
   glUseProgram(s.getProgramId());

   Il est possible d'ajouter plusieurs vertex shader ainsi que plusieurs
   fragment shader comme spécifié par la norme.

	membres:
  	mpGuts: pointeur sur les guts qui contiennent toutes les ressources openGL
            propre au shaders et program.
  
  membre de Guts:
    mRefCount: compte de référence utilisé à l'interne pour déclencher la 
               destruction des ressources openGL.
    mFragmentId: id de la ressource openGL représentant le fragment shader.
    mProgramId id de la ressource openGL représentant le program.
    mVertexId; id de la ressource openGL représentant le vertex shader.
    mFragmentSource; le code source du fragment shader.
    mVertexSource; le code source du vertex shader. 
    mIsValid; Booléen qui indique si les shaders ont compilés et si le
              program a linké.
  
  Note: Usually, the program must be in use (glUseProgram) for setUniform call
  to work.
*/
namespace realisim
{
namespace treeD
{

class Shader
{
public:
  Shader();
  Shader(QString, QString);
  Shader(const Shader&);
  virtual ~Shader();
  virtual Shader& operator=(const Shader&);

  int addFragmentShaderSource(QString);
  int addVertexShaderSource(QString);
  Shader copy();
  int getProgramId() const {return mpGuts->mProgramId;} 
  QString getFragmentSource() const {return mpGuts->mFragmentSource;} 
  QString getVertexSource() const {return mpGuts->mVertexSource;}
  bool isValid() const;
  void link();
  
  bool setUniform(const char*, int);
  bool setUniform(const char*, float);
  bool setUniform(const char*, double);
  bool setUniform(const char*, const math::Point3i&);
  bool setUniform(const char*, const math::Point3d&);
  bool setUniform(const char*, const math::Point3f&);
  bool setUniform(const char*, const math::Vector3i&);
  bool setUniform(const char*, const math::Vector3d&);
  bool setUniform(const char*, const math::Vector3f&);
//bindVextexAttrib et setVertexAttrib?
  
protected:
  void computeValidity();
	int getFragmentId() const {return mpGuts->mFragmentId;}
	int getVertexId() const {return mpGuts->mVertexId;}
  void printProgramInfoLog(GLuint);
  void printShaderInfoLog(GLuint);

  struct Guts
  {
    explicit Guts();
    unsigned int mRefCount;
    
    int mFragmentId; //vector<int> mFragmentIds
    int mProgramId;  
    int mVertexId; //vector<int> mVertexIds
    QString mFragmentSource; //vector<QString> mFragmentSources
    QString mVertexSource; //vector<QString> mVertexSources
    bool mIsValid;
  };
  
  virtual void deleteGuts();
  virtual void makeGuts();
  virtual void shareGuts(Guts*);
  
  Guts* mpGuts;
};

}//treeD
}//realisim

#endif