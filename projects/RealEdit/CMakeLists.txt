#REALEDIT CMAKELISTS

PROJECT( RealEdit )

SET( SOURCES ${SOURCES} "../../macro_component.cmake" )

ADD_COMPONENT( "." "Application" )
ADD_COMPONENT( "UserInterface" )

ADD_COMPONENT( "images" )
ADD_COMPONENT( "../../reusables/3d" )


GENERATE_EXE( ${PROJECT_NAME} )