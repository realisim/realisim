
PROJECT( sargam )

SET( SOURCES ${SOURCES} "./CMakeLists.txt" )

ADD_COMPONENT( "." "Application" )
ADD_COMPONENT("../../reusables/utils" )

ADD_QT_PRINTSUPPORT()
GENERATE_EXE( ${PROJECT_NAME} )