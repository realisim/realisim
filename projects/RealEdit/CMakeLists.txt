#realEdit CMAKELISTS

PROJECT( RealEdit )

SET( SOURCES ${SOURCES} "CMakeLists.txt" )
SET( SOURCES ${SOURCES} "../../macro_component.cmake" )

ADD_COMPONENT( "." "Application" )
ADD_COMPONENT( "UserInterface" "User Interface")
ADD_COMPONENT( "DataModel" "Data Model")


ADD_COMPONENT( "images" )
ADD_COMPONENT( "../../reusables/3d" )
ADD_COMPONENT( "../../reusables/command" )
ADD_COMPONENT( "../../reusables/math" )


GENERATE_EXE( ${PROJECT_NAME} )