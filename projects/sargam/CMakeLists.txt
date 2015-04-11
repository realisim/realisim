
PROJECT( sargam )

SET( SOURCES ${SOURCES} "./CMakeLists.txt" )

ADD_COMPONENT( "." "Application" )
ADD_COMPONENT("../../reusables/utils" )

GENERATE_EXE( ${PROJECT_NAME} )