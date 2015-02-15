
#ifndef Realisim_TreeD_Shader_hh
#define Realisim_TreeD_Shader_hh

#include <QString>
#include <QtOpenGL/qgl.h>
#include <math/Matrix4.h>
#include <math/Point.h>
#include <math/Vect.h>
#include <vector>

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
  
  Widget3d possède les méthodes pushShader(Shader)/popShader qui permettent
  d'empiler/dépiler correctement les shaders.
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

//void addFragmentShader(QString); a partir d'un fichier
//void addVertexShader(QString); a partir d'un fichier
  virtual void addFragmentSource(QString);
  virtual void addVertexSource(QString);
  virtual void begin();
  virtual Shader copy();
  virtual void end();
  virtual int getProgramId() const {return mpGuts->mProgramId;} 
  virtual QString getFragmentSource(int i) const {return mpGuts->mFragmentSources[i];}
  virtual int getFragmentSourcesSize() const {return mpGuts->mFragmentSources.size();}
  virtual QString getVertexSource(int i) const {return mpGuts->mVertexSources[i];}
  virtual int getVertexSourcesSize() const {return mpGuts->mVertexSources.size();}
  virtual bool isValid() const;
  virtual void link() const;
  
  virtual bool setUniform(const char*, int);
  virtual bool setUniform(const char*, float);
  virtual bool setUniform(const char*, int, const float*);
  virtual bool setUniform(const char*, int, const double*);
  virtual bool setUniform(const char*, double);
  virtual bool setUniform(const char*, const math::Point2i&);
  virtual bool setUniform(const char*, const math::Point2d&);
  virtual bool setUniform(const char*, const math::Vector2i&);
  virtual bool setUniform(const char*, const math::Vector2d&);
  virtual bool setUniform(const char*, const math::Point3i&);
  virtual bool setUniform(const char*, const math::Point3d&);
  virtual bool setUniform(const char*, const math::Point3f&);
  virtual bool setUniform(const char*, const math::Vector3i&);
  virtual bool setUniform(const char*, int, const math::Vector3i*);
  virtual bool setUniform(const char*, const math::Vector3d&);
  virtual bool setUniform(const char*, const math::Vector3f&);
  virtual bool setUniform(const char*, int, const math::Vector3f*);
  virtual bool setUniform(const char*, const math::Matrix4&);

//bindVextexAttrib et setVertexAttrib?
  
protected:
  virtual void validate() const;
	virtual int getFragmentId(int i) const;
	virtual int getVertexId(int i) const;
  virtual void printProgramInfoLog(GLuint) const;
  virtual void printShaderInfoLog(GLuint) const;

  struct Guts
  {
    explicit Guts();
    unsigned int mRefCount;
    
    std::vector<int> mFragmentIds;
    int mProgramId;  
    std::vector<int> mVertexIds;
    std::vector<QString> mFragmentSources;
    std::vector<QString> mVertexSources;
    std::vector<GLuint> mPreviousShaders;
    mutable bool mIsValid;
  };
  
  virtual void deleteGuts();
  virtual void makeGuts();
  virtual void shareGuts(Guts*);
  
  Guts* mpGuts;
};

}//treeD
}//realisim

#endif