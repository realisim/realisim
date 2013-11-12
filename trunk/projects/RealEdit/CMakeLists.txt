#realEdit CMAKELISTS

PROJECT( RealEdit )

SET(QT_USE_QTOPENGL 1)
SET(QT_USE_QTNETWORK 1)
INCLUDE(${QT_USE_FILE})

SET( SOURCES ${SOURCES} "CMakeLists.txt" )
SET( SOURCES ${SOURCES} "../../macro_component.cmake" )

ADD_COMPONENT( "." "Application" )
ADD_COMPONENT( "commands" "Commands")
ADD_COMPONENT( "DataModel" "Data Model")
ADD_COMPONENT( "images" )
ADD_COMPONENT( "../../reusables/math" )
ADD_COMPONENT( "UserInterface" "UI")
ADD_COMPONENT( "UserInterface/Palettes" "UI/Palettes")
ADD_COMPONENT( "../../reusables/utils" )
ADD_COMPONENT( "../../reusables/3d" )

GENERATE_EXE( ${PROJECT_NAME} )