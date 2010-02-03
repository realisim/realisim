
#include <iostream>
#include "OpenGLInfo.h"
#include <sstream>

using namespace realisim;
using namespace treeD;
using namespace std;

OpenGLInfo::OpenGLInfo() : mMaxTextureSize(0), mMax3dTextureSize(0),
  mIsValid(false)
{  refresh(); }

//------------------------------------------------------------------------------
//get all relevant OpenGLInfo
void OpenGLInfo::refresh()
{
   mExtensions.clear();
   char* str = 0;
   char* tok = 0;
   mIsValid = true;

   str = (char*)glGetString(GL_VENDOR);
   if(str) mVendor = str;
   else mIsValid = false;

   str = (char*)glGetString(GL_RENDERER);
   if(str) mRenderer = str;
   else mIsValid = false;

   str = (char*)glGetString(GL_VERSION);
   if(str) mVersion = str;
   else mIsValid = false;

   str = (char*)glGetString(GL_SHADING_LANGUAGE_VERSION);
   if(str) mGlSlVersion = str;
   else mIsValid = false;

   /*make a copy of the extension string because it will be modified by strtok
     and we would like to keep that pointer intact for subsequent call of
     OpenGLInfo.*/
   str = (char*)glGetString(GL_EXTENSIONS);
   char* extStringCopy = new char[strlen(str) + 1];
   strcpy(extStringCopy, str);
   // split extensions and insert them in a the string vector.
   if(extStringCopy)
   {
      tok = strtok((char*)extStringCopy, " ");
      while(tok)
      {
         mExtensions.push_back(tok);    // put a extension into struct
         tok = strtok(0, " ");               // next token
      }
   }
   else
     mIsValid = false;
   delete[] extStringCopy;
   // sort extension by alphabetical order
   std::sort(mExtensions.begin(), mExtensions.end());

   glGetIntegerv(GL_MAX_TEXTURE_SIZE, &mMaxTextureSize);
   glGetIntegerv(GL_MAX_3D_TEXTURE_SIZE, &mMax3dTextureSize);
}

//------------------------------------------------------------------------------
bool OpenGLInfo::isExtensionSupported(const char* ext) const
{
  if(!isValid())
    return false;

  // search corresponding extension
  vector< string >::const_iterator iter = mExtensions.begin();
  vector< string >::const_iterator endIter = mExtensions.end();
  while(iter != endIter)
  {
    if(ext == *iter)
        return true;
    else
        ++iter;
  }
  return false;
}

//------------------------------------------------------------------------------
void OpenGLInfo::print()
{
   if(!isValid())
   {
     cout << "OpenGL info invalide. Assurez vous qu'il y a au moins un \n" \
     "contexte openGL valide." << endl;
     return;
   }
   
   stringstream ss;

   ss << endl; // blank line
   ss << "OpenGL Driver Info" << endl;
   ss << "==================" << endl;
   ss << "Vendor: " << mVendor << endl;
   ss << "Renderer: " << mRenderer << endl;
   ss << "Version: " << mVersion << endl;
   ss << "Shader Version: " << mGlSlVersion << endl;

   ss << endl;
   ss << "Max Texture Size: " << mMaxTextureSize << " x " << 
      mMaxTextureSize<< endl;
   ss << "Max 3D Texture Size: " << mMax3dTextureSize << " x " << 
      mMax3dTextureSize << " x " << mMax3dTextureSize << endl;

   ss << endl;
//print the extensions only in debug
#ifndef NDEBUG
   ss << "Total Number of Extensions: " << mExtensions.size() << endl;
   ss << "==============================" << endl;
   for(unsigned int i = 0; i < mExtensions.size(); ++i)
      ss << mExtensions.at(i) << endl;

   ss << "======================================================================" << endl;
#endif

   cout << ss.str() << endl;
}
