Toutes les versions de QT peuvent être downloader a partir du ftp suivant:
ftp://ftp.trolltech.com/qt/source/

Pour windows:
installer Qt

- add qmake dir ([checkOutPath]\thirdParty\Qt\bin) to the envitonment variable PATH
- add QTDIR to environment variable... ([checkOutPath]\thirdParty\Qt)
- add QMAKESPEC to the environment variables... it must point to (%QTDIR%\mkspecs\win32-msvc2005)...

Pour mac:
compiler en static de préférence.