Toutes les versions de QT peuvent être downloader a partir du ftp suivant:
ftp://ftp.trolltech.com/qt/source/

Pour windows:
installer Qt

- add qmake dir ([checkOutPath]\thirdParty\Qt\bin) to the envitonment variable PATH
- add QTDIR to environment variable... ([checkOutPath]\thirdParty\Qt)
- add QMAKESPEC to the environment variables... it must point to (%QTDIR%\mkspecs\win32-msvc2005)...

Qt Pour mac:
-compiler en dynamique (/usr/local/TrollTech/Qt-4.7.0/)
-compiler en static de préférence dans un répertoire different de la compile dynamique (/usr/local/TrollTech/Qt-4.7.0-static/).
  -Pour utiliser Qt en static, il faut ajouter les lib suivantes au link (le cmakelist principale le fait quand on choisi l'option de compilation 'Qt-Static': Carbon, ApplicationServices, AppKit. 
  -Il faut aussi ahouter les plugins de qt (qt-libjpeg, qt-libpng, qt-libsvg)
    - Pour utiliser les plugins de qt (qt-libjpeg, qt-libpng, qt-libsvg) lorsqu'on utilise Qt en statique:
      http://doc.trolltech.com/4.4/plugins-howto.html#static-plugins