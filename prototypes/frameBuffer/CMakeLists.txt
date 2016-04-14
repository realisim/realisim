

SET( SOURCES ${SOURCES} "./CMakeLists.txt" )

ADD_COMPONENT( "." "Application" )
ADD_COMPONENT("../../reusables/3d" )
ADD_COMPONENT("../../reusables/math" )
ADD_COMPONENT("./shaders" )
ADD_COMPONENT("./images" )

ADD_QT_OPENGL()

GENERATE_EXE( FrameBuffer )