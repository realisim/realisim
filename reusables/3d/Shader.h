
#ifndef Realisim_TreeD_Shader_hh
#define Realisim_TreeD_Shader_hh

#include "openGlHeaders.h"
#include <QString>
#include <map>
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
  Shader(const Shader&);
  virtual ~Shader();
  virtual Shader& operator=(const Shader&);  

  virtual void addDefineToFragmentSource(int iSourceId, QString iName, QString iValue);
  virtual void addDefineToVertexSource(int iSourceId, QString iName, QString iValue);
  virtual int addFragmentSource(QString);
  virtual int addVertexSource(QString);
  virtual void begin();
  virtual void clear();
  virtual Shader copy();
  virtual void end();
  //virtual ShaderDefine getFragmentDefine(int) const;
  //virtual ShaderDefine getVertexDefine(int) const;  
  virtual int getProgramId() const {return mpGuts->mProgramId;} 
  virtual QString getName() const {return mpGuts->mName;}
  virtual int getNumberOfDefineForFragment(int) const {return (int)mpGuts->mFragmentDefineMap.size();}
  virtual int getNumberOfDefineForVertex(int) const { return (int)mpGuts->mVertexDefineMap.size(); }
  virtual bool isValid() const;
  virtual void link();
  virtual void setName(QString iName) { mpGuts->mName = iName; }
  
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
  virtual bool setUniform(const char*, const math::Vector4d&);
  virtual bool setUniform(const char*, const math::Matrix4&);

//bindVextexAttrib et setVertexAttrib?
  
protected:
    struct ShaderDefine
    {
        QString mName;
        QString mValue;
    };

  virtual void addDefine(QString iName, QString iValue, std::vector<ShaderDefine>&);
  virtual void detachAndDeleteGlResources();
  virtual int getFragmentId(int i) const;
  virtual QString getFragmentSource(int i) const { return mpGuts->mFragmentSources[i]; }
  virtual int getFragmentSourcesSize() const { return (int)mpGuts->mFragmentSources.size(); }
  virtual int getVertexId(int i) const;
  virtual QString getVertexSource(int i) const {return mpGuts->mVertexSources[i];}
  virtual int getVertexSourcesSize() const {return (int)mpGuts->mVertexSources.size();}
  virtual void injectDefines(QString &iSouce, const std::vector<ShaderDefine> &iShaderDefines);
  virtual void injectFragmentDefines();
  virtual void injectVertexDefines();
  virtual void printProgramInfoLog(GLuint) const;
  virtual void printShaderInfoLog(GLuint) const;
  virtual void validate() const;

  struct Guts
  {
    explicit Guts();
    unsigned int mRefCount;
    
    QString mName;
    std::vector<int> mFragmentIds;
    int mProgramId;  
    std::vector<int> mVertexIds;
    std::vector<QString> mFragmentSources;
    std::vector<QString> mVertexSources;
    std::vector<GLuint> mPreviousShaders;
    std::map<int, std::vector<ShaderDefine> > mFragmentDefineMap;
    std::map<int, std::vector<ShaderDefine> > mVertexDefineMap;
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