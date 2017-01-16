#PROTOTYPES CMAKELISTS

IF( PROTOTYPES_BUILDOPTION )
   #ADD_SUBDIRECTORY( bezierCurve )
   #ADD_SUBDIRECTORY( Bloom)
   #ADD_SUBDIRECTORY( camera )
   #ADD_SUBDIRECTORY( command )
   #ADD_SUBDIRECTORY( frameBuffer)
   #ADD_SUBDIRECTORY( genese) #
   #ADD_SUBDIRECTORY( imageFilter) #ne fonctionne plus
   #ADD_SUBDIRECTORY( jeuDePouvoir )
   #ADD_SUBDIRECTORY( Matrix4 )
   #ADD_SUBDIRECTORY( network)
   #ADD_SUBDIRECTORY( OpenFlightViewer )
   #ADD_SUBDIRECTORY( particuleSystem) #utilise openCL... un test pas très concluant
   #ADD_SUBDIRECTORY( particuleSystem2)
   #ADD_SUBDIRECTORY( platform )
   #ADD_SUBDIRECTORY( polygones)
   #ADD_SUBDIRECTORY( pong) #ne compile plus a cause de chipmunk 
   #ADD_SUBDIRECTORY( qtableTest ) # ne compile plus
   #ADD_SUBDIRECTORY( rayCasting) # un projet de David en C#
   #ADD_SUBDIRECTORY( remoteDesktop )
   #ADD_SUBDIRECTORY( SDI ) #ne compile plus
   #ADD_SUBDIRECTORY( shader)
   #ADD_SUBDIRECTORY( shaderEditor )
   #ADD_SUBDIRECTORY( SMAA )
   ADD_SUBDIRECTORY( soundTest )
   #ADD_SUBDIRECTORY( spaceTime ) # pas maintenu
   #ADD_SUBDIRECTORY( statistics )
   #ADD_SUBDIRECTORY( texte_et_texture )
   #ADD_SUBDIRECTORY( VertexBufferObject )
ENDIF( PROTOTYPES_BUILDOPTION )