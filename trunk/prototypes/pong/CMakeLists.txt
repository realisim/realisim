
PROJECT( Pong )

SET( SOURCES ${SOURCES} "./CMakeLists.txt" )

ADD_COMPONENT( "." "Pong" )
ADD_COMPONENT("../../reusables/3d" )
ADD_COMPONENT("../../reusables/math" )
ADD_COMPONENT("./images" "images")

ADD_CHIPMUNK()

GENERATE_EXE( ${PROJECT_NAME} )