#SDI CMAKELISTS

PROJECT( SDI )

SET( SOURCES ${SOURCES} "./CMakeLists.txt" )

ADD_COMPONENT( "." "Application" )
ADD_COMPONENT( "../../reusables/3d/" "e3d" )


GENERATE_EXE( ${PROJECT_NAME} )