
PROJECT( Matrix4 )
#INCLUDE(${QT_USE_FILE})

SET( SOURCES ${SOURCES} "./CMakeLists.txt" )

ADD_COMPONENT( "." "Application" )
ADD_COMPONENT("../../reusables/math" )

GENERATE_EXE( ${PROJECT_NAME} )