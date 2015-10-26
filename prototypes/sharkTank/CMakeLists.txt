START_PROJECT (sharkTank)


# This defines the ADD_COMPONENT () macro.
INCLUDE ("../../dryden_applications/component macros.cmake")

# Components to compile into the application.
ADD_COMPONENT ("." "application" )
ADD_COMPONENT ("../../dryden_tools/utilities" "utilities" )
ADD_COMPONENT ("../../dryden_tools/dicom" "dicom" )
ADD_COMPONENT ("../../dryden_library/" "coreLib" )


# External components linked to the application.
ADD_QT ()
ADD_QTMAIN()
ADD_DRYDEN_LIBRARY()
ADD_WIN32_LIBS()
ADD_ITK()

# Create the actual project inside the solution.
INCLUDE_DIRECTORIES( ${PROJECT_INCLUDE_DIRS} )
LINK_DIRECTORIES( ${PROJECT_LINK_DIRS} )
ADD_EXECUTABLE (
	${PROJECT_NAME}
	WIN32
	${SRCS}
	${NOTIFICATION}
	${TRANSLATION_FILES}
	${QT_MOC_SRCS}
	${QT_UIC_SRCS}
	${QT_UIC_INCS}
	${UI_QT_DESIGNER_FILES}
	${INCS})
TARGET_LINK_LIBRARIES (${PROJECT_NAME} ${PROJECT_TARGET_LIBS})
DIST_EXE( ${PROJECT_NAME} )

