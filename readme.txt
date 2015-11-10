Toutes les versions de QT peuvent être downloader a partir du ftp suivant:
ftp://ftp.qt-project.org/qt/source/

Pour windows:
installer Qt 4.7.0 ( qt-win-opensource-4.7.0.vs2008.exe )
- mettre le path de qmake ( [disque]/Qt/4.7.0/bin/qmake.exe ) dans la variable QT_QMAKE_EXECUTABLE de CMake


Qt Pour mac:
-compiler en dynamique (/usr/local/TrollTech/Qt-4.7.0/)
-compiler en static de préférence dans un répertoire different de la compile dynamique (/usr/local/TrollTech/Qt-4.7.0-static/).
  -Pour utiliser Qt en static, il faut ajouter les lib suivantes au link (le cmakelist principale le fait quand on choisi l'option de compilation 'Qt-Static': Carbon, ApplicationServices, AppKit. 
  -Il faut aussi ahouter les plugins de qt (qt-libjpeg, qt-libpng, qt-libsvg)
    - Pour utiliser les plugins de qt (qt-libjpeg, qt-libpng, qt-libsvg) lorsqu'on utilise Qt en statique:
      http://doc.trolltech.com/4.4/plugins-howto.html#static-plugins
  (voir cmakelist du root à la révision 80 pour un exemple)

les lib devraient être compiler en 64 bits
Enssuite, l'option cmake OSX_CMAKE_ARCHITECTURES devrait être x86_64 et le CMAKE_OSX_SYSROOT: /Developer/SDKs/MacOSX10.6.sdk


Notes:
Pour connaitre le nombre de download des releases sur github
curl -i https://api.github.com/repos/realisim/realisim/releases -H "Accept: application/vnd.github.manifold-preview+json"