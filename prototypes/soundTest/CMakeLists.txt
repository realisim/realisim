
PROJECT( Sound )
#INCLUDE(${QT_USE_FILE})

SET( SOURCES ${SOURCES} "./CMakeLists.txt" )

ADD_COMPONENT( "." "Application" )
ADD_COMPONENT("../../reusables/math" )
ADD_COMPONENT("../../reusables/sound" )

ADD_OPENAL()

GENERATE_EXE( ${PROJECT_NAME} )