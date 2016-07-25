
PROJECT( Engine3d )

SET( SOURCES ${SOURCES} "./CMakeLists.txt" )

ADD_COMPONENT( "." "Application" )
ADD_COMPONENT("../../reusables/3d" )
ADD_COMPONENT("../../reusables/math" )
ADD_COMPONENT("../../reusables/utils" )
ADD_COMPONENT("./render" )
ADD_COMPONENT("./data" )

ADD_QT_OPENGL()

GENERATE_EXE( ${PROJECT_NAME} )