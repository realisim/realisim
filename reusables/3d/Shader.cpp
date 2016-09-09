
#include "Shader.h"

using namespace realisim;
  using namespace math;
  using namespace treeD;
using namespace std;

#define WARN_WHEN_UNIFORM_NOT_FOUND(iLoc, iName) if (iLoc < 0) { printf("uniform %s not found\n", iName); }

Shader::Guts::Guts() : mRefCount(1),
  mName("no name"),
  mFragmentIds(),
  mProgramId(0),
  mVertexIds(),
  mFragmentSources(),
  mVertexSources(),
  mPreviousShaders(),
  mIsValid(false)
{}

//---
Shader::Shader() : mpGuts(0)
{makeGuts();}

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
void Shader::addDefine(QString iName, QString iValue, vector<ShaderDefine> &iV)
{
    ShaderDefine sd;
    sd.mName = iName;
    sd.mValue = iValue;
    iV.push_back(sd);

    /*This code checks if the define is already present and replaces the value... 
    Not needed at the moment*/
    //bool alreadyPresent = false;
    ////check if there is already a define with that name and replace value if it is the case
    //for(size_t i = 0; i < iV.size() && !alreadyPresent; ++i)
    //{
    //    if (iV[i].mName == iName)
    //    {
    //        iV[i].mValue = iValue;
    //        alreadyPresent = true;
    //    }
    //}

    //if(!alreadyPresent)
    //{ iV.push_back(sd); }
}

//----------------------------------------------------------------------------
//this will inject a define in the fragment source located at iSourceId. This
//will only have effect if the program is not yet linked (via ::link())
//The injection will happen right before compile see link() method.
void Shader::addDefineToFragmentSource(int iSourceId, QString iName, QString iValue)
{    
    if(iSourceId >= 0 && iSourceId < (int)mpGuts->mFragmentSources.size())
    {
        auto it = mpGuts->mFragmentDefineMap.find(iSourceId);
        if(it == mpGuts->mFragmentDefineMap.end())
        { it = mpGuts->mFragmentDefineMap.insert( make_pair(iSourceId, vector<ShaderDefine>()) ).first; }

        addDefine(iName, iValue, it->second);        
    }
}

//----------------------------------------------------------------------------
//see Shader::addDefineToFragmentSource
void Shader::addDefineToVertexSource(int iSourceId, QString iName, QString iValue)
{
    if (iSourceId >= 0 && iSourceId < (int)mpGuts->mVertexSources.size())
    {
        auto it = mpGuts->mVertexDefineMap.find(iSourceId);
        if (it == mpGuts->mVertexDefineMap.end())
        {
            it = mpGuts->mVertexDefineMap.insert(make_pair(iSourceId, vector<ShaderDefine>())).first;
        }

        addDefine(iName, iValue, it->second);
    }
}
//----------------------------------------------------------------------------
//adds a fragment source to be compiled when method link() will be called.
int Shader::addFragmentSource(QString iSource)
{  
    int r = -1;
    if (!iSource.isEmpty())
    {
        mpGuts->mFragmentSources.push_back(iSource);
        r = (int)mpGuts->mFragmentSources.size() - 1;
    }
    return r;
}

//----------------------------------------------------------------------------
int Shader::addVertexSource(QString iSource)
{
    int r = -1;
    if (!iSource.isEmpty())
    {
        mpGuts->mVertexSources.push_back(iSource);
        r = (int)mpGuts->mVertexSources.size() - 1;
    }
    return r;
}

//----------------------------------------------------------------------------
void Shader::begin()
{
  GLint p = 0;
  glGetIntegerv(GL_CURRENT_PROGRAM, &p);
  mpGuts->mPreviousShaders.push_back( p );
  if( isValid() ) glUseProgram( getProgramId() );
  else
  { printf("Shader %s is not valid. Either it did not link properly or was never linked...\n", mpGuts->mName.toStdString().c_str()); }
}

//----------------------------------------------------------------------------
void Shader::clear()
{
    detachAndDeleteGlResources();
    mpGuts->mVertexSources.clear();
    mpGuts->mVertexDefineMap.clear();
    mpGuts->mFragmentSources.clear();
    mpGuts->mFragmentDefineMap.clear();
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
void Shader::detachAndDeleteGlResources()
{
    for (int i = 0; i < getVertexSourcesSize(); ++i)
    {
        glDetachShader(getProgramId(), getVertexId(i));
        glDeleteShader(getVertexId(i));
    }

    for (int i = 0; i < getFragmentSourcesSize(); ++i)
    {
        glDetachShader(getProgramId(), getFragmentId(i));
        glDeleteShader(getFragmentId(i));
    }

    glDeleteProgram(getProgramId());

    mpGuts->mFragmentIds.clear();
    mpGuts->mVertexIds.clear();
    mpGuts->mProgramId = 0;

    mpGuts->mIsValid = false;
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
    detachAndDeleteGlResources();
    
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
    return mpGuts->mIsValid;
}

//----------------------------------------------------------------------------
void Shader::injectDefines(QString &iSouce, const vector<ShaderDefine> &iShaderDefines )
{
    //bake the #define string for the current fragment source
    QString defineString = "\n";
    for (size_t i = 0; i < iShaderDefines.size(); ++i)
    {
        ShaderDefine sd = iShaderDefines[i];
        defineString += "#define " + sd.mName + " " + sd.mValue + "\n";
    }

    //find occurence of #version and insert the define string right under.
    QRegExp regExp("#version[ \\w]+");
    int p = iSouce.lastIndexOf(regExp);
    if (p == -1)
    {
        printf("Shader::injectDefines did not find #version tag. Define string "
            " will be added at top of shader code.\n");
        p = 0;
    }
    iSouce.insert(p + max(regExp.matchedLength(), 0), defineString);
}

//----------------------------------------------------------------------------
void Shader::injectFragmentDefines()
{
    auto it = mpGuts->mFragmentDefineMap.begin();
    for(; it != mpGuts->mFragmentDefineMap.end(); ++it)
    {
        QString& s = mpGuts->mFragmentSources[ it->first ];
        injectDefines(s, it->second);        
    }
}

//----------------------------------------------------------------------------
void Shader::injectVertexDefines()
{
    auto it = mpGuts->mVertexDefineMap.begin();
    for (; it != mpGuts->mVertexDefineMap.end(); ++it)
    {
        QString& s = mpGuts->mVertexSources[it->first];
        injectDefines(s, it->second);
    }
}

//----------------------------------------------------------------------------
void Shader::link()
{
    if (getProgramId() == 0)
        mpGuts->mProgramId = glCreateProgram();

    //inject all user define for fragment sources
    injectVertexDefines();
    //compile all vertex
    for (int i = 0; i < (int)mpGuts->mVertexSources.size(); ++i)
    {
        mpGuts->mVertexIds.push_back(glCreateShader(GL_VERTEX_SHADER));
        /*Puisque iSource peut venir d'une ressource (donc d'un QByteArray)
        il est important de passé par std string et ensuite const char*.
        Par exemple, si on fait, .toAscii() et ensuite .data() il arrive
        fréquement que le shader ne compile pas parce qu'il y a un \0
        en plein milieu du QByteArray.*/
        std::string s = mpGuts->mVertexSources[i].toStdString();
        const char* d = s.c_str();
        glShaderSource(getVertexId(i), 1, &d, NULL);
        glCompileShader(getVertexId(i));
        glAttachShader(getProgramId(), getVertexId(i));       
    }
    mpGuts->mVertexSources.clear();

    //inject all user define for fragment sources
    injectFragmentDefines();
    //compile all fragment
    for(int i = 0; i < (int)mpGuts->mFragmentSources.size(); ++i )
    {
        mpGuts->mFragmentIds.push_back(glCreateShader(GL_FRAGMENT_SHADER));
        
        std::string s = mpGuts->mFragmentSources[i].toStdString();
        const char* d = s.c_str();
        glShaderSource(getFragmentId(i), 1, &d, NULL);
        glCompileShader(getFragmentId(i));
        glAttachShader(getProgramId(), getFragmentId(i));
    }
    mpGuts->mFragmentSources.clear();

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
    printf("link log for program %s id: %d\n", mpGuts->mName.toStdString().c_str(), iObj);
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
WARN_WHEN_UNIFORM_NOT_FOUND(loc, iName);
  return loc >= 0 ? true : false;
}

//----------------------------------------------------------------------------
bool Shader::setUniform(const char* iName, float iValue)
{
    if(!isValid())
    return false;
    
  GLint loc = glGetUniformLocation(getProgramId(), iName);
  glUniform1f(loc, iValue);
  WARN_WHEN_UNIFORM_NOT_FOUND(loc, iName);
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
  WARN_WHEN_UNIFORM_NOT_FOUND(loc, iName);
  return loc >= 0 ? true : false;
}

//----------------------------------------------------------------------------
/*Permet de passer un tableau de double au shader*/
bool Shader::setUniform(const char* iName, int iSize, const double* iData)
{
  if(!isValid())
    return false;
  
  float *d = new float[iSize];
  for(int i = 0; i < iSize; ++i)
  { d[i] = (float)iData[i]; }
    
  GLint loc = glGetUniformLocation(getProgramId(), iName);
  glUniform1fv(loc, iSize, d);
  WARN_WHEN_UNIFORM_NOT_FOUND(loc, iName);
  delete[] d;
  return loc >= 0 ? true : false;
}

//----------------------------------------------------------------------------
bool Shader::setUniform(const char* iName, double iValue)
{
    if(!isValid())
    return false;
    
  GLint loc = glGetUniformLocation(getProgramId(), iName);
  glUniform1f(loc, (float)iValue);
  WARN_WHEN_UNIFORM_NOT_FOUND(loc, iName);
  return loc >= 0 ? true : false;
}

//----------------------------------------------------------------------------
bool Shader::setUniform(const char* iName, const math::Point2i& iV)
{
    if(!isValid())
    return false;
    
  GLint loc = glGetUniformLocation(getProgramId(), iName);
  glUniform2i( loc, iV.x(), iV.y() );
  WARN_WHEN_UNIFORM_NOT_FOUND(loc, iName);
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
  WARN_WHEN_UNIFORM_NOT_FOUND(loc, iName);
  return loc >= 0 ? true : false;
}

//----------------------------------------------------------------------------
bool Shader::setUniform(const char* iName, const math::Vector2i& iV)
{
  if(!isValid())
    return false;
    
  GLint loc = glGetUniformLocation(getProgramId(), iName);
  glUniform2i( loc, iV.x(), iV.y() );
  WARN_WHEN_UNIFORM_NOT_FOUND(loc, iName);
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
  WARN_WHEN_UNIFORM_NOT_FOUND(loc, iName);
  return loc >= 0 ? true : false;
}

//----------------------------------------------------------------------------
bool Shader::setUniform(const char* iName, const Point3i& iValue)
{
    if(!isValid())
    return false;
    
  GLint loc = glGetUniformLocation(getProgramId(), iName);
  glUniform3i(loc, iValue.x(), iValue.y(), iValue.z());
  WARN_WHEN_UNIFORM_NOT_FOUND(loc, iName);
  return loc >= 0 ? true : false;
}

//----------------------------------------------------------------------------
bool Shader::setUniform(const char* iName, const Point3d& iValue)
{
    if(!isValid())
    return false;
  
  Point3f t(iValue.x(), iValue.y(), iValue.z());
  GLint loc = glGetUniformLocation(getProgramId(), iName);
  glUniform3f(loc, t.x(), t.y(), t.z());
  WARN_WHEN_UNIFORM_NOT_FOUND(loc, iName);
  return loc >= 0 ? true : false;
}

//----------------------------------------------------------------------------
bool Shader::setUniform(const char* iName, const Point3f& iValue)
{
    if(!isValid())
    return false;
    
  GLint loc = glGetUniformLocation(getProgramId(), iName);
  glUniform3f(loc, iValue.x(), iValue.y(), iValue.z());
  WARN_WHEN_UNIFORM_NOT_FOUND(loc, iName);
  return loc >= 0 ? true : false;
}

//----------------------------------------------------------------------------
bool Shader::setUniform(const char* iName, const Vector3i& iValue)
{
    if(!isValid())
    return false;
    
  GLint loc = glGetUniformLocation(getProgramId(), iName);
  glUniform3i(loc, iValue.x(), iValue.y(), iValue.z());
  WARN_WHEN_UNIFORM_NOT_FOUND(loc, iName);
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
  WARN_WHEN_UNIFORM_NOT_FOUND(loc, iName);
  return loc >= 0 ? true : false;
}


//----------------------------------------------------------------------------
bool Shader::setUniform(const char* iName, const Vector3d& iValue)
{
    if(!isValid())
    return false;
    
  Point3f t(iValue.x(), iValue.y(), iValue.z());
  GLint loc = glGetUniformLocation(getProgramId(), iName);
  glUniform3f(loc, t.x(), t.y(), t.z());
  WARN_WHEN_UNIFORM_NOT_FOUND(loc, iName);
  return loc >= 0 ? true : false;
}

//----------------------------------------------------------------------------
bool Shader::setUniform(const char* iName, const Vector3f& iValue)
{
	if (!isValid())
		return false;

	GLint loc = glGetUniformLocation(getProgramId(), iName);
	glUniform3f(loc, iValue.x(), iValue.y(), iValue.z());
	WARN_WHEN_UNIFORM_NOT_FOUND(loc, iName);
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
  WARN_WHEN_UNIFORM_NOT_FOUND(loc, iName);
  return loc >= 0 ? true : false;
}

//----------------------------------------------------------------------------
bool Shader::setUniform(const char* iName, const math::Vector4d& iValue)
{
	if (!isValid())
		return false;

	GLint loc = glGetUniformLocation(getProgramId(), iName);
	glUniform4f(loc, iValue.x(), iValue.y(), iValue.z(), iValue.w());
	WARN_WHEN_UNIFORM_NOT_FOUND(loc, iName);
	return loc >= 0 ? true : false;
}

//----------------------------------------------------------------------------
bool Shader::setUniform(const char* iName, const Matrix4& iValue)
{
    if(!isValid())
    return false;

  float f[16];
  for( int i = 0; i < 4; ++i )
      for( int j = 0; j < 4; ++j )
    { f[ i*4 + j] = (float)iValue(j, i); }
  GLint loc = glGetUniformLocation(getProgramId(), iName);
  glUniformMatrix4fv(loc, 1, false, f);
  WARN_WHEN_UNIFORM_NOT_FOUND(loc, iName);
  return loc >= 0 ? true : false;
}

//----------------------------------------------------------------------------
void Shader::shareGuts(Guts* g)
{
  mpGuts = g;
  ++mpGuts->mRefCount;
}

