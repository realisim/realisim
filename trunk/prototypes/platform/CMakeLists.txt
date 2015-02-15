
PROJECT( platform )
ADD_QT_OPENGL()
#INCLUDE(${QT_USE_FILE})

SET( SOURCES ${SOURCES} "./CMakeLists.txt" )

ADD_COMPONENT( "." "Application" )
ADD_COMPONENT("../../reusables/3d" )
ADD_COMPONENT("../../reusables/math" )
ADD_COMPONENT("../../reusables/utils" )
#ADD_COMPONENT("../../reusables/network" )
ADD_COMPONENT("./images" )

GENERATE_EXE( ${PROJECT_NAME} )