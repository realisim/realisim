
// expliquer ou mettre gl/glew.h
#ifdef WIN32
    #define  _CRT_SECURE_NO_WARNINGS //pour qu'on puisse utiliser le C (strcpy, strtok, vnsprintf...)!!!
    #define NOMINMAX //avant d'inclure windows, on s'assure de ne pas briser std::numeric_limits<>::min/max
    #include <Windows.h>
    #include <GL/glew.h> //n�cessaire pour l'utilisation de GL_BGRA
    #include <gl/GLU.h>
#endif

#ifdef __APPLE__
    #include <OpenGL/glu.h>
#endif

#include <QtOpenGL/qgl.h>