
SET( SOURCES ${SOURCES} "./CMakeLists.txt" )

ADD_COMPONENT( "." "Application" )
ADD_COMPONENT("../../reusables/3d" )
ADD_COMPONENT("../../reusables/math" )

ADD_QT_OPENGL()

GENERATE_EXE( Camera )