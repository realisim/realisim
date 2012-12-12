
PROJECT( ParticuleSystem2 )
SET(QT_USE_QTOPENGL 1)
INCLUDE(${QT_USE_FILE})

SET( SOURCES ${SOURCES} "./CMakeLists.txt" )

ADD_COMPONENT( "." "Application" )
ADD_COMPONENT("../../reusables/3d" )
ADD_COMPONENT("../../reusables/math" )
ADD_COMPONENT("./images" )

ADD_OPENCL()

GENERATE_EXE( ${PROJECT_NAME} )