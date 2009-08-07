
PROJECT( CommandPrototype )

SET( SOURCES ${SOURCES} "./CMakeLists.txt" )

ADD_COMPONENT( "." "Application" )
ADD_COMPONENT("../../reusables/command" )

GENERATE_EXE( ${PROJECT_NAME} )