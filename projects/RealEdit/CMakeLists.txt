#realEdit CMAKELISTS

PROJECT( RealEdit )

SET( SOURCES ${SOURCES} "CMakeLists.txt" )
SET( SOURCES ${SOURCES} "../../macro_component.cmake" )

ADD_COMPONENT( "." "Application" )
ADD_COMPONENT( "Commands" "Commands")
ADD_COMPONENT( "DataModel" "Data Model")
ADD_COMPONENT( "images" )
ADD_COMPONENT( "../../reusables/math" )
ADD_COMPONENT( "UserInterface" "UI")
ADD_COMPONENT( "../../reusables/utils" )
ADD_COMPONENT( "../../reusables/3d" )

GENERATE_EXE( ${PROJECT_NAME} )