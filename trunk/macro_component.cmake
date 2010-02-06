# Macro for adding the given component. This modifies the INCS, SRCS and
# PROJECT_INCLUDE_DIRS variables. The given folder is scanned for .cpp, .h and
# .png files. These files are then added to the corresponding variables.
# Specific build rules are also added for .png files to embed them in .h files.
# Client code should them #include these files and use qembed_findImage ().
# C_PATH: relative path of the component to add to the current project.
MACRO (ADD_COMPONENT C_PATH)
    GET_FILENAME_COMPONENT (C_NAME ${C_PATH} NAME)
    FILE (GLOB C_IMAGE_FILES ${C_PATH}/*.png)
    FILE (GLOB C_INCLUDE_FILES ${C_PATH}/*.h)
    FILE (GLOB C_SOURCE_FILES ${C_PATH}/*.cpp)
    
#    message( "C_NAME ${C_NAME} " )
#    message( "NAME ${NAME} " )
#    message( "C_IMAGE_FILES ${C_IMAGE_FILES}" )
#    MESSAGE( "C_INCLUDE_FILES ${C_INCLUDE_FILES}" )
#    message( "C_SOURCE_FILES ${C_SOURCE_FILES}" )
    
    IF ( ${ARGV1} MATCHES .+ )
       SET( C_NAME ${ARGV1} )
    ENDIF ( ${ARGV1} MATCHES .+ )
    
    IF( C_IMAGE_FILES )
       ADD_IMAGE_RESOURCES (${C_NAME} ${C_IMAGE_FILES})
    ENDIF( C_IMAGE_FILES )
    
    IF( C_INCLUDE_FILES )
      ADD_INCLUDE_FILES (${C_NAME} ${C_INCLUDE_FILES})
    ENDIF( C_INCLUDE_FILES )
    
    IF( C_SOURCE_FILES )
      ADD_SOURCE_FILES (${C_NAME} ${C_SOURCE_FILES})
    ENDIF( C_SOURCE_FILES )
    
    MESSAGE(${CMAKE_CURRENT_SOURCE_DIR}/${C_PATH}/../)
    SET (PROJECT_INCLUDE_DIRS ${PROJECT_INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR}/${C_PATH}/../)
ENDMACRO (ADD_COMPONENT)


# Macro for embedding images and adding them to the project in given group.
MACRO (ADD_IMAGE_RESOURCES TO_GROUP IMAGES)
  #create a source group for compiled embeded resources
  
   SET ( IMAGES_TMP ${IMAGES} ${ARGN} )   
  
   SOURCE_GROUP( "MOC files" REGULAR_EXPRESSION .*\\.cxx ) 
   SOURCE_GROUP ("Resources png" FILES ${IMAGES_TMP})
   
   QT4_ADD_RESOURCES( IMAGES_TMP ${PROJECT_SOURCE_DIR}/images.qrc )
   SET( RESOURCES ${IMAGES_TMP} )
ENDMACRO (ADD_IMAGE_RESOURCES)


# Macro for adding headers to the project in given group.
MACRO (ADD_INCLUDE_FILES TO_GROUP INCLUDE_FILES)

    #We Need a tmp value to store all the args of the macro
    SET ( INCLUDES_TMP ${INCLUDE_FILES} ${ARGN} )

	#we need at least one file to try to add the moc header files
    IF ( ${ARGV0} MATCHES .+ )
       ADD_MOC_HEADER_FILES( ${INCLUDES_TMP} )
    ENDIF ()

    SET (INCLUDES ${INCLUDES} ${INCLUDES_TMP})
    
#    MESSAGE( "INCLUDES ${INCLUDES}" )
    
    SOURCE_GROUP (${TO_GROUP} FILES ${INCLUDES_TMP})
ENDMACRO (ADD_INCLUDE_FILES)

#Macro for adding moc head to the project
MACRO (ADD_MOC_HEADER_FILES MOC_HEADER_FILES)
   SET( MOC_HEADER_FILES_TMP ${MOC_HEADER_FILES} ${ARGN} )
   
   FOREACH (MOC_HEADER ${MOC_HEADER_FILES_TMP})
      
      FILE (READ ${MOC_HEADER} fileValue)
      
      string(REGEX MATCH .*Q_OBJECT.* isMocable ${fileValue} )
     
#        MESSAGE( "isMocable: ${isMocable}" )
      IF ( ${isMocable} MATCHES .* )      
        SET( MOC_HEADERS ${MOC_HEADERS} ${MOC_HEADER} )
     ENDIF ( ${isMocable} MATCHES .* )
           
         
   ENDFOREACH (MOC_HEADER ${MOC_HEADER_FILES_TMP})
   
ENDMACRO (ADD_MOC_HEADER_FILES MOCABLES)

# Macro for adding source files to the project in given group.
MACRO(ADD_SOURCE_FILES TO_GROUP SOURCE_FILES)

    #We Need a tmp value to store all the args of the macro
    SET( SOURCE_FILES_TMP ${SOURCE_FILES} ${ARGN} )
    
    SET (SOURCES ${SOURCES} ${SOURCE_FILES_TMP})
    SOURCE_GROUP (${TO_GROUP} FILES ${SOURCE_FILES_TMP})
ENDMACRO(ADD_SOURCE_FILES)