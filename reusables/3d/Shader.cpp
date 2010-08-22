
#include "Shader.h"

using namespace realisim;
using namespace math;
using namespace treeD;

Shader::Guts::Guts() : mRefCount(1),
  mFragmentId(0),
  mProgramId(0),
  mVertexId(0),
  mFragmentSource(),
  mVertexSource(),
  mIsValid(false)
{}

//Shader::Guts::Guts(QString iFragmentSource, QString iVertexSource) : mRefCount(1),
// mFragmentId(0),
// mProgramId(0),
// mVertexId(0),
// mFragmentSource(iFragmentSource),
// mVertexSource(iVertexSource)
//{
//	mProgramId = glCreateProgram();
//  
//  if(!mFragmentSource.isEmpty())
//  {
//    mFragmentId = glCreateShader(GL_FRAGMENT_SHADER);
//    //glShaderSource();
//    glCompileShader(mFragmentId);
//    glAttachShader(mProgramId, mFragmentId);
//  }
//  if(!mVertexSource.isEmpty())
//  {
//    mVertexId = glCreateShader(GL_VERTEX_SHADER);
//    //glShaderSource();
//    glCompileShader(mVertexId);
//    glAttachShader(mProgramId, mVertexId);
//  }
//  
//  glLinkProgram(mProgramId);
//}

//---
Shader::Shader() : mpGuts(0)
{makeGuts();}

Shader::Shader(QString iVertexSource, QString iFragmentSource) : mpGuts(0)
{ 
  makeGuts();
  addVertexShaderSource(iVertexSource);
  addFragmentShaderSource(iFragmentSource);
  link();
}

Shader::Shader(const Shader& iT) : mpGuts(0)
{ shareGuts(iT.mpGuts); }

Shader::~Shader()
{ deleteGuts(); }

Shader& Shader::operator=(const Shader& iT)
{
  if(mpGuts == iT.mpGuts)
    return *this;
  
  deleteGuts();
  shareGuts(iT.mpGuts);
  return *this;
}

//----------------------------------------------------------------------------
int Shader::addFragmentShaderSource(QString iSource)
{
  if(getProgramId() == 0)
    mpGuts->mProgramId = glCreateProgram();
    
  if(!iSource.isEmpty() && getFragmentId() == 0)
  {
    mpGuts->mFragmentSource = iSource;
    /*Puisque iSource peut venir d'une ressource (donc d'un QByteArray)
      il est important de passé par std string et ensuite const char*. 
      Par exemple, si on fait, .toAscii() et ensuite .data() il arrive
      fréquement que le shader ne compile pas parce qu'il y a un \0
      en plein milieu du QByteArray.*/
    std::string s = iSource.toStdString();
    const char* d = s.c_str();
    mpGuts->mFragmentId = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(getFragmentId(), 1, &d, NULL);
    glCompileShader(getFragmentId());
    glAttachShader(getProgramId(), getFragmentId());
  }

//Print the info log when in debug mode
#ifndef NDEBUG
  printShaderInfoLog(getFragmentId());
#endif
  return getFragmentId();
}

//----------------------------------------------------------------------------
int Shader::addVertexShaderSource(QString iSource)
{
  if(getProgramId() == 0)
    mpGuts->mProgramId = glCreateProgram();

  if(!iSource.isEmpty() && getVertexId() == 0)
  {
    mpGuts->mVertexSource = iSource;
    /*Puisque iSource peut venir d'une ressource (donc d'un QByteArray)
      il est important de passé par std string et ensuite const char*. 
      Par exemple, si on fait, .toAscii() et ensuite .data() il arrive
      fréquement que le shader ne compile pas parce qu'il y a un \0
      en plein milieu du QByteArray.*/
    std::string s = iSource.toStdString();
    const char* d = s.c_str();
    mpGuts->mVertexId = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(getVertexId(), 1, &d, NULL);
    glCompileShader(getVertexId());
    glAttachShader(getProgramId(), getVertexId());
  }
//Print the info log when in debug mode
#ifndef NDEBUG
  printShaderInfoLog(getVertexId());
#endif
  return getVertexId();
}

//----------------------------------------------------------------------------
/*Cette méthode sert a copier le shader et d'y allouer de nouvelle ressources
  opengl (fragment shader, vertex shader et program). On ne peut pas utilisé
  le partage implicite sur cette classe et créer une nouvelle instance de Shader
  qui aurait les meme id (fragment, vertex et program) qu'un autre program parce
  que lors de la destruction du vertex/fragment/program initiale, les ressources
  opengl serait detruites (par glDeleteShader et glDeleteProgram) causant ainsi
  de grâve problème a la copy. C'est pourquoi la méthode copy ré-alloue 
  completement un nouveau Shader!*/
Shader Shader::copy()
{
	Shader s;
  s.addVertexShaderSource(getVertexSource());
  s.addFragmentShaderSource(getFragmentSource());
  s.link();
  
  return s;
}

//----------------------------------------------------------------------------
void Shader::computeValidity()
{
  mpGuts->mIsValid = true;
  int status;
  //check if shaders are compiled
  glGetShaderiv(getFragmentId(), GL_COMPILE_STATUS, &status);
  mpGuts->mIsValid = mpGuts->mIsValid & (status == GL_TRUE);

  glGetShaderiv(getVertexId(), GL_COMPILE_STATUS, &status);
  mpGuts->mIsValid = mpGuts->mIsValid & (status == GL_TRUE);
  
  //check if program linked
  glGetProgramiv(getProgramId(), GL_LINK_STATUS, &status);
  mpGuts->mIsValid = mpGuts->mIsValid & (status == GL_TRUE);
}

//----------------------------------------------------------------------------
void Shader::deleteGuts()
{
  if(mpGuts && --mpGuts->mRefCount == 0)
  {
    glDetachShader(getProgramId(), getFragmentId());
    glDetachShader(getProgramId(), getVertexId());
    glDeleteShader(getFragmentId());
    glDeleteShader(getVertexId());
    glDeleteProgram(getProgramId());
    delete mpGuts;
    mpGuts = 0;
  }
}

//----------------------------------------------------------------------------
bool Shader::isValid() const
{ return mpGuts->mIsValid;}

//----------------------------------------------------------------------------
void Shader::link()
{
  glLinkProgram(getProgramId());
  computeValidity();
  //Print the info log when in debug mode
#ifndef NDEBUG
  printProgramInfoLog(getProgramId());
#endif
}

//----------------------------------------------------------------------------
void Shader::makeGuts()
{ mpGuts = new Guts(); }

//----------------------------------------------------------------------------
//void Shader::makeGuts(QString iFragmentSource, QString iVertexSource)
//{ mpGuts = new Guts(iFragmentSource, iVertexSource); }

//----------------------------------------------------------------------------
void Shader::printProgramInfoLog(GLuint iObj)
{
  int infologLength = 0;
  int charsWritten  = 0;
  char *infoLog;

  glGetProgramiv(iObj, GL_INFO_LOG_LENGTH,&infologLength);

  if (infologLength > 0)
  {
    infoLog = (char *)malloc(infologLength);
    glGetProgramInfoLog(iObj, infologLength, &charsWritten, infoLog);
    printf("%s\n",infoLog);
    free(infoLog);
  }
}


//----------------------------------------------------------------------------
void Shader::printShaderInfoLog(GLuint iObj)
{
  int infologLength = 0;
  int charsWritten  = 0;
  char *infoLog;

  glGetShaderiv(iObj, GL_INFO_LOG_LENGTH,&infologLength);

  if (infologLength > 0)
  {
    infoLog = (char *)malloc(infologLength);
    glGetShaderInfoLog(iObj, infologLength, &charsWritten, infoLog);
    printf("%s\n",infoLog);
    free(infoLog);
  }
}

//----------------------------------------------------------------------------
bool Shader::setUniform(const char* iName, int iValue)
{
	if(!isValid())
    return false;
    
  GLint loc = glGetUniformLocation(getProgramId(), iName);
  glUniform1i(loc, iValue);
  return loc >= 0 ? true : false;
}

//----------------------------------------------------------------------------
bool Shader::setUniform(const char* iName, float iValue)
{
	if(!isValid())
    return false;
    
  GLint loc = glGetUniformLocation(getProgramId(), iName);
  glUniform1f(loc, iValue);
  return loc >= 0 ? true : false;
}

//----------------------------------------------------------------------------
bool Shader::setUniform(const char* iName, double iValue)
{
	if(!isValid())
    return false;
    
  GLint loc = glGetUniformLocation(getProgramId(), iName);
  glUniform1f(loc, (float)iValue);
  return loc >= 0 ? true : false;
}

//----------------------------------------------------------------------------
bool Shader::setUniform(const char* iName, const Point3i& iValue)
{
	if(!isValid())
    return false;
    
  GLint loc = glGetUniformLocation(getProgramId(), iName);
  glUniform3i(loc, iValue.getX(), iValue.getY(), iValue.getZ());
  return loc >= 0 ? true : false;
}

//----------------------------------------------------------------------------
bool Shader::setUniform(const char* iName, const Point3d& iValue)
{
	if(!isValid())
    return false;
  
  Point3f t(iValue.getX(), iValue.getY(), iValue.getZ());
  GLint loc = glGetUniformLocation(getProgramId(), iName);
  glUniform3f(loc, t.getX(), t.getY(), t.getZ());
  return loc >= 0 ? true : false;
}

//----------------------------------------------------------------------------
bool Shader::setUniform(const char* iName, const Point3f& iValue)
{
	if(!isValid())
    return false;
    
  GLint loc = glGetUniformLocation(getProgramId(), iName);
  glUniform3f(loc, iValue.getX(), iValue.getY(), iValue.getZ());
  return loc >= 0 ? true : false;
}

//----------------------------------------------------------------------------
bool Shader::setUniform(const char* iName, const Vector3i& iValue)
{
	if(!isValid())
    return false;
    
  GLint loc = glGetUniformLocation(getProgramId(), iName);
  glUniform3i(loc, iValue.getX(), iValue.getY(), iValue.getZ());
  return loc >= 0 ? true : false;
}

//----------------------------------------------------------------------------
bool Shader::setUniform(const char* iName, const Vector3d& iValue)
{
	if(!isValid())
    return false;
    
  Point3f t(iValue.getX(), iValue.getY(), iValue.getZ());
  GLint loc = glGetUniformLocation(getProgramId(), iName);
  glUniform3f(loc, t.getX(), t.getY(), t.getZ());
  return loc >= 0 ? true : false;
}

//----------------------------------------------------------------------------
bool Shader::setUniform(const char* iName, const Vector3f& iValue)
{
	if(!isValid())
    return false;
    
  GLint loc = glGetUniformLocation(getProgramId(), iName);
  glUniform3f(loc, iValue.getX(), iValue.getY(), iValue.getZ());
  return loc >= 0 ? true : false;
}


//----------------------------------------------------------------------------
void Shader::shareGuts(Guts* g)
{
  mpGuts = g;
  ++mpGuts->mRefCount;
}

