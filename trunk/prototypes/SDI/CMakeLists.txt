#SDI CMAKELISTS

PROJECT( SDI )

SET( SOURCES ${SOURCES} "./CMakeLists.txt" )

ADD_COMPONENT( "." "Application" )
ADD_COMPONENT( "../../reusables/3d/" "e3d" )
ADD_COMPONENT( "../../reusables/math/" "math" )


GENERATE_EXE( ${PROJECT_NAME} )