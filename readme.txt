Toutes les versions de QT peuvent être downloader a partir du ftp suivant:
ftp://ftp.qt-project.org/qt/source/

Pour windows:
installer Qt 5.4
- mettre le path de qmake ( [disque]/Qt/4.7.0/bin/qmake.exe ) dans la variable QT_QMAKE_EXECUTABLE de CMake
- mettre tous les paths nÈcessaires de Qt pour cmake (QT5Core_Dir et les autres...)
	- sur mac qt_install_dir/Qt5.4.0/5.4/clang_64/lib/cmake/Qt5Core
	- sur windows ???

Notes:
Pour connaitre le nombre de download des releases sur github
curl -i https://api.github.com/repos/realisim/realisim/releases -H "Accept: application/vnd.github.manifold-preview+json"