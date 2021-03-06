# Macro for adding the given component. This modifies the INCS, SRCS and
# PROJECT_INCLUDE_DIRS variables. The given folder is scanned for .cpp, .h and
# .png files. These files are then added to the corresponding variables.
# Specific build rules are also added for .png files to embed them in .h files.
# Client code should them #include these files and use qembed_findImage ().
# C_PATH: relative path of the component to add to the current project.
MACRO (ADD_COMPONENT C_PATH)
    GET_FILENAME_COMPONENT (C_NAME ${C_PATH} NAME)
    #Glober les images png et jpg
    FILE (GLOB C_IMAGE_FILE_PNG RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}/ ${C_PATH}/*.png)
    FILE (GLOB C_IMAGE_FILE_JPG RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}/ ${C_PATH}/*.jpg)
    SET(C_IMAGE_FILES ${C_IMAGE_FILE_PNG} ${C_IMAGE_FILE_JPG})

    FILE (GLOB C_INCLUDE_FILES ${C_PATH}/*.h)
    FILE (GLOB C_SOURCE_FILES ${C_PATH}/*.cpp)

    #Glober les shaker .vert et .frag
    FILE (GLOB C_VERT_SHADER_FILES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}/ ${C_PATH}/*.vert )
    FILE (GLOB C_FRAG_SHADER_FILES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}/ ${C_PATH}/*.frag )
    SET(C_SHADER_FILES ${C_VERT_SHADER_FILES} ${C_FRAG_SHADER_FILES})

   #Glober les program openCL
   FILE (GLOB C_OPENCL_PROGRAM_FILES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}/ ${C_PATH}/*.cl )

   #Glober les binaires catalogues
   FILE (GLOB C_BINARY_FILES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}/ ${C_PATH}/*.cat )

#    message( "C_NAME ${C_NAME} " )
#    message( "NAME ${NAME} " )
#    message( "C_IMAGE_FILES ${C_IMAGE_FILES}" )
#    MESSAGE( "C_INCLUDE_FILES ${C_INCLUDE_FILES}" )
#    message( "C_SOURCE_FILES ${C_SOURCE_FILES}" )
#    message( "C_SHADER_FILES ${C_SHADER_FILES}" )
#    message( "C_BINARY_FILES ${C_BINARY_FILES}" )

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

    IF( C_SHADER_FILES )
      ADD_SHADER_RESOURCES (${C_NAME} ${C_SHADER_FILES})
    ENDIF( C_SHADER_FILES )

    IF( C_OPENCL_PROGRAM_FILES )
      ADD_OPENCL_RESOURCES (${C_NAME} ${C_OPENCL_PROGRAM_FILES})
    ENDIF( C_OPENCL_PROGRAM_FILES )

    IF( C_BINARY_FILES )
      ADD_BINARY_RESOURCES (${C_NAME} ${C_BINARY_FILES})
    ENDIF( C_BINARY_FILES )

    
#    MESSAGE(${CMAKE_CURRENT_SOURCE_DIR}/${C_PATH}/../)
    SET (PROJECT_INCLUDE_DIRS ${PROJECT_INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR}/${C_PATH}/../)
ENDMACRO (ADD_COMPONENT)


# Macro for embedding images and adding them to the project in given group.
MACRO (ADD_IMAGE_RESOURCES TO_GROUP IMAGES)
  #create a source group for compiled embeded resources
  
   SET ( IMAGES_TMP ${IMAGES} ${ARGN} )   
  
   SOURCE_GROUP( "MOC files" REGULAR_EXPRESSION .*\\.cxx ) 
   SOURCE_GROUP ("Resources image" FILES ${IMAGES_TMP})
   
   CREATE_QRC_FILE(images.qrc ${IMAGES_TMP})
   QT5_ADD_RESOURCES( IMAGES_TMP  ${CMAKE_CURRENT_SOURCE_DIR}/images.qrc )
   SET( RESOURCES ${RESOURCES} ${IMAGES_TMP} )
ENDMACRO (ADD_IMAGE_RESOURCES)


# Macro for adding headers to the project in given group.
MACRO (ADD_INCLUDE_FILES TO_GROUP INCLUDE_FILES)

    #We Need a tmp value to store all the args of the macro
    SET ( INCLUDES_TMP ${INCLUDE_FILES} ${ARGN} )

    SET (INCLUDES ${INCLUDES} ${INCLUDES_TMP})
    
#    MESSAGE( "INCLUDES ${INCLUDES}" )
    
    SOURCE_GROUP (${TO_GROUP} FILES ${INCLUDES_TMP})
ENDMACRO (ADD_INCLUDE_FILES)

# Macro for adding source files to the project in given group.
MACRO(ADD_SOURCE_FILES TO_GROUP SOURCE_FILES)

    #We Need a tmp value to store all the args of the macro
    SET( SOURCE_FILES_TMP ${SOURCE_FILES} ${ARGN} )
    
    SET (SOURCES ${SOURCES} ${SOURCE_FILES_TMP})
    SOURCE_GROUP (${TO_GROUP} FILES ${SOURCE_FILES_TMP})
ENDMACRO(ADD_SOURCE_FILES)

# Macro for embedding shader and adding them to the project in given group.
MACRO (ADD_SHADER_RESOURCES TO_GROUP SHADERS)
  #create a source group for compiled embeded resources
  
   SET ( SHADER_TMP ${SHADERS} ${ARGN} )   
  
   SOURCE_GROUP( "MOC files" REGULAR_EXPRESSION .*\\.cxx ) 
   SOURCE_GROUP ("Resources Shader" FILES ${SHADER_TMP})
   
   CREATE_QRC_FILE(shaders.qrc ${SHADER_TMP})
   QT5_ADD_RESOURCES( SHADER_TMP  ${CMAKE_CURRENT_SOURCE_DIR}/shaders.qrc )
   SET( RESOURCES ${RESOURCES} ${SHADER_TMP} )
ENDMACRO (ADD_SHADER_RESOURCES)

# Macro for embedding openCL program and adding them to the project in given group.
MACRO (ADD_OPENCL_RESOURCES TO_GROUP PROGRAMS)
  #create a source group for compiled embeded resources
   SET ( PROGRAMS_TMP ${PROGRAMS} ${ARGN} )   
  
   SOURCE_GROUP( "MOC files" REGULAR_EXPRESSION .*\\.cxx ) 
   SOURCE_GROUP ("Resources image" FILES ${PROGRAMS_TMP})
   
   CREATE_QRC_FILE(openCL.qrc ${PROGRAMS_TMP})
   QT5_ADD_RESOURCES( PROGRAMS_TMP  ${CMAKE_CURRENT_SOURCE_DIR}/openCL.qrc )
   SET( RESOURCES ${RESOURCES} ${PROGRAMS_TMP} )
ENDMACRO (ADD_OPENCL_RESOURCES)

# Macro for embedding binary resources and adding them to the project in given group.
MACRO (ADD_BINARY_RESOURCES TO_GROUP PROGRAMS)
  #create a source group for compiled embeded resources
   SET ( PROGRAMS_TMP ${PROGRAMS} ${ARGN} )   
  
   SOURCE_GROUP( "MOC files" REGULAR_EXPRESSION .*\\.cxx ) 
   SOURCE_GROUP ("binary resources" FILES ${PROGRAMS_TMP})
   
   CREATE_QRC_FILE(binaries.qrc ${PROGRAMS_TMP})
   QT5_ADD_RESOURCES( PROGRAMS_TMP  ${CMAKE_CURRENT_SOURCE_DIR}/binaries.qrc )
   SET( RESOURCES ${RESOURCES} ${PROGRAMS_TMP} )
ENDMACRO (ADD_BINARY_RESOURCES)

#macro to generate the qrc resource file needed by QT. It is a 
#simple xml file listing files names
MACRO (CREATE_QRC_FILE FILE_NAME FILES)
  SET ( FILES_TMP ${FILES} ${ARGN} )
  #create qrc file
   SET (QRC_FILE "<!DOCTYPE RCC><RCC version=\"1.0\"> \n <qresource>\n")

   FOREACH(FILE ${FILES_TMP})
     SET (QRC_FILE ${QRC_FILE} "  <file>${FILE}</file>\n")
   ENDFOREACH(FILE ${FILES_TMP})

   SET (QRC_FILE ${QRC_FILE} " </qresource>\n</RCC>")
   file(WRITE ${FILE_NAME} ${QRC_FILE})
   #qrc file created
ENDMACRO (CREATE_QRC_FILE FILE_NAME FILES)