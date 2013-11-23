
#include "Shader.h"

using namespace realisim;
  using namespace math;
  using namespace treeD;
using namespace std;

Shader::Guts::Guts() : mRefCount(1),
  mFragmentIds(),
  mProgramId(),
  mVertexIds(),
  mFragmentSources(),
  mVertexSources(),
  mPreviousShaders(),
  mIsValid(false)
{}

//---
Shader::Shader() : mpGuts(0)
{makeGuts();}

Shader::Shader(QString iVertexSource, QString iFragmentSource) : mpGuts(0)
{ 
  makeGuts();
  addVertexSource(iVertexSource);
  addFragmentSource(iFragmentSource);
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
void Shader::addFragmentSource(QString iSource)
{
  if(getProgramId() == 0)
    mpGuts->mProgramId = glCreateProgram();
    
  if(!iSource.isEmpty())
  {
    mpGuts->mFragmentSources.push_back(iSource);
    int fragmentIndex = mpGuts->mFragmentSources.size() - 1;
    /*Puisque iSource peut venir d'une ressource (donc d'un QByteArray)
      il est important de passé par std string et ensuite const char*. 
      Par exemple, si on fait, .toAscii() et ensuite .data() il arrive
      fréquement que le shader ne compile pas parce qu'il y a un \0
      en plein milieu du QByteArray.*/
    std::string s = iSource.toStdString();
    const char* d = s.c_str();
    mpGuts->mFragmentIds.push_back(glCreateShader(GL_FRAGMENT_SHADER));
    glShaderSource(getFragmentId(fragmentIndex), 1, &d, NULL);
    glCompileShader(getFragmentId(fragmentIndex));
    glAttachShader(getProgramId(), getFragmentId(fragmentIndex));
  }
  link();

//Print the info log when in debug mode
#ifndef NDEBUG
  int fragmentIndex = mpGuts->mFragmentSources.size() - 1;
  printf("Log for fragment shader number: %d\n", fragmentIndex);
  printShaderInfoLog(getFragmentId(fragmentIndex));
#endif
}

//----------------------------------------------------------------------------
void Shader::addVertexSource(QString iSource)
{
  if(getProgramId() == 0)
    mpGuts->mProgramId = glCreateProgram();

  if(!iSource.isEmpty())
  {
    mpGuts->mVertexSources.push_back(iSource);
    int vertexIndex = mpGuts->mVertexSources.size() - 1;
    /*Puisque iSource peut venir d'une ressource (donc d'un QByteArray)
      il est important de passé par std string et ensuite const char*. 
      Par exemple, si on fait, .toAscii() et ensuite .data() il arrive
      fréquement que le shader ne compile pas parce qu'il y a un \0
      en plein milieu du QByteArray.*/
    std::string s = iSource.toStdString();
    const char* d = s.c_str();
    mpGuts->mVertexIds.push_back(glCreateShader(GL_VERTEX_SHADER));
    glShaderSource(getVertexId(vertexIndex), 1, &d, NULL);
    glCompileShader(getVertexId(vertexIndex));
    glAttachShader(getProgramId(), getVertexId(vertexIndex));
  }
  
//Print the info log when in debug mode
#ifndef NDEBUG
  int vertexIndex = mpGuts->mVertexSources.size() - 1;
  printf("Log for vertex shader number: %d\n", vertexIndex);
  printShaderInfoLog(getVertexId(vertexIndex));
#endif
}

//----------------------------------------------------------------------------
void Shader::begin()
{
  GLint p = 0;
  glGetIntegerv(GL_CURRENT_PROGRAM, &p);
  mpGuts->mPreviousShaders.push_back( p );
  if( isValid() ) glUseProgram( getProgramId() );
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
  for(int i = 0; i < getVertexSourcesSize(); ++i)
    s.addVertexSource(getVertexSource(i));
  for(int i = 0; i < getFragmentSourcesSize(); ++i)
    s.addFragmentSource(getFragmentSource(i));
  return s;
}

//----------------------------------------------------------------------------
void Shader::end()
{
	GLuint p = 0;
  if( mpGuts->mPreviousShaders.size() > 0 )
  {
  	p = mpGuts->mPreviousShaders.back();
    mpGuts->mPreviousShaders.pop_back();
  }
  glUseProgram( p );
}

//----------------------------------------------------------------------------
void Shader::validate() const
{
  mpGuts->mIsValid = true;
  int status;
  //check if shaders are compiled
  for(int i = 0; i < getFragmentSourcesSize(); ++i)
  {
    glGetShaderiv(getFragmentId(i), GL_COMPILE_STATUS, &status);
    mpGuts->mIsValid = mpGuts->mIsValid & (status == GL_TRUE);
  }

  for(int i = 0; i < getVertexSourcesSize(); ++i)
  {
    glGetShaderiv(getVertexId(i), GL_COMPILE_STATUS, &status);
    mpGuts->mIsValid = mpGuts->mIsValid & (status == GL_TRUE);
  }
  
  //check if program linked
  glGetProgramiv(getProgramId(), GL_LINK_STATUS, &status);
  mpGuts->mIsValid = mpGuts->mIsValid & (status == GL_TRUE);
}

//----------------------------------------------------------------------------
void Shader::deleteGuts()
{
  if(mpGuts && --mpGuts->mRefCount == 0)
  {
    for(int i = 0; i < getVertexSourcesSize(); ++i)
    {
      glDetachShader(getProgramId(), getVertexId(i));
      glDeleteShader(getVertexId(i));
    }
    
    for(int i = 0; i < getFragmentSourcesSize(); ++i)
    {
    	glDetachShader(getProgramId(), getFragmentId(i));
	    glDeleteShader(getFragmentId(i));
    }

    glDeleteProgram(getProgramId());
    delete mpGuts;
    mpGuts = 0;
  }
}

//----------------------------------------------------------------------------
int Shader::getFragmentId(int i) const
{
	int r = 0;
  if(i >= 0 && (uint)i < mpGuts->mFragmentIds.size())
  	r = mpGuts->mFragmentIds[i];
  return r;
}

//----------------------------------------------------------------------------
int Shader::getVertexId(int i) const
{
  int r = 0;
  if(i >= 0 && (uint)i < mpGuts->mVertexIds.size())
  	r = mpGuts->mVertexIds[i];
  return r;
}

//----------------------------------------------------------------------------
bool Shader::isValid() const
{
	if( !mpGuts->mIsValid ) link();
  return mpGuts->mIsValid;
}

//----------------------------------------------------------------------------
void Shader::link() const
{
  glLinkProgram(getProgramId());
  validate();
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
void Shader::printProgramInfoLog(GLuint iObj) const
{
  int infologLength = 0;
  int charsWritten  = 0;
  char *infoLog;

  glGetProgramiv(iObj, GL_INFO_LOG_LENGTH,&infologLength);

  if (infologLength > 0)
  {
  	printf("link log for program %d\n", iObj);
    infoLog = (char *)malloc(infologLength);
    glGetProgramInfoLog(iObj, infologLength, &charsWritten, infoLog);
    printf("%s\n",infoLog);
    free(infoLog);
  }
}


//----------------------------------------------------------------------------
void Shader::printShaderInfoLog(GLuint iObj) const
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
/*Permet de passer un tableau de float au shader*/
bool Shader::setUniform(const char* iName, int iSize, const float* iData)
{
  if(!isValid())
    return false;
    
  GLint loc = glGetUniformLocation(getProgramId(), iName);
  glUniform1fv(loc, iSize, iData);
  return loc >= 0 ? true : false;
}

//----------------------------------------------------------------------------
/*Permet de passer un tableau de double au shader*/
bool Shader::setUniform(const char* iName, int iSize, const double* iData)
{
  if(!isValid())
    return false;
  
  float d[iSize];
  for(int i = 0; i < iSize; ++i)
  { d[i] = (float)iData[i]; }
  
  GLint loc = glGetUniformLocation(getProgramId(), iName);
  glUniform1fv(loc, iSize, d);
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
bool Shader::setUniform(const char* iName, const math::Point2i& iV)
{
	if(!isValid())
    return false;
    
  GLint loc = glGetUniformLocation(getProgramId(), iName);
  glUniform2i( loc, iV.x(), iV.y() );
  return loc >= 0 ? true : false;
}

//----------------------------------------------------------------------------
bool Shader::setUniform(const char* iName, const math::Point2d& iV)
{
	if(!isValid())
    return false;
  
  Point2f v( iV );
  GLint loc = glGetUniformLocation(getProgramId(), iName);
  glUniform2f( loc, v.x(), v.y() );
  return loc >= 0 ? true : false;
}

//----------------------------------------------------------------------------
bool Shader::setUniform(const char* iName, const math::Vector2i& iV)
{
  if(!isValid())
    return false;
    
  GLint loc = glGetUniformLocation(getProgramId(), iName);
  glUniform2i( loc, iV.x(), iV.y() );
  return loc >= 0 ? true : false;
}

//----------------------------------------------------------------------------
bool Shader::setUniform(const char* iName, const math::Vector2d& iV)
{
	if(!isValid())
    return false;
  
  Vector2f v( iV );
  GLint loc = glGetUniformLocation(getProgramId(), iName);
  glUniform2f( loc, v.x(), v.y() );
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
/*Permet de passer un tableau de vecteur3i au shader*/
bool Shader::setUniform(const char* iName, int iSize, const math::Vector3i* iData)
{
  if(!isValid())
    return false;
    
  GLint loc = glGetUniformLocation(getProgramId(), iName);
  glUniform3iv(loc, iSize, iData->getPtr());
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
/*Permet de passer un tableau de vecteur3f au shader*/
bool Shader::setUniform(const char* iName, int iSize, const math::Vector3f* iData)
{
  if(!isValid())
    return false;
    
  GLint loc = glGetUniformLocation(getProgramId(), iName);
  glUniform3fv(loc, iSize, iData->getPtr());
  return loc >= 0 ? true : false;
}

//----------------------------------------------------------------------------
bool Shader::setUniform(const char* iName, const Matrix4f& iValue)
{
	if(!isValid())
    return false;
    
  GLint loc = glGetUniformLocation(getProgramId(), iName);
  /*pas besoin de faire le transpose, nos matrice son column-major comme
    openGL.*/
  glUniformMatrix4fv(loc, 1, false, iValue.getPtr());
  return loc >= 0 ? true : false;
}

//----------------------------------------------------------------------------
bool Shader::setUniform(const char* iName, const Matrix4d& iValue)
{
	if(!isValid())
    return false;
    
  Matrix4f m( iValue );
  GLint loc = glGetUniformLocation(getProgramId(), iName);
  glUniformMatrix4fv(loc, 1, false, m.getPtr());
  return loc >= 0 ? true : false;
}

//----------------------------------------------------------------------------
void Shader::shareGuts(Guts* g)
{
  mpGuts = g;
  ++mpGuts->mRefCount;
}

