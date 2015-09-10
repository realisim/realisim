#!/bin/bash

if [[ $# < 1 ]]; then
	echo "too few parameters..."
	echo "./buildAdnDeployMac.sh [version string]"
	echo "for example: ./buildAdnDeployMac.sh v0.5.1 will create a dmg called Sargam v0.5.1.dmg"
else
    VERSION_STRING=$1
    VOLUME_NAME="Sargam ${VERSION_STRING}"
	APP_NAME="sargam.app"
	RW_DMG_NAME="${VOLUME_NAME}_tmp.dmg"
	FINAL_DMG_NAME="${VOLUME_NAME}.dmg"
	 
	#configuration cmake

	#——— build ———
	#build clean
	xcodebuild -project ../../../cmake/Realisim.xcodeproj clean -configuration Release -sdk macosx10.10

	#build in release 64 bits with SDK 10.10 and deployment target 10.7
	xcodebuild -project ../../../cmake/Realisim.xcodeproj build -configuration Release -sdk macosx10.10 MACOSX_DEPLOYMENT_TARGET=10.7

	#——— ajout de frameworks et plugin QT au app avec qtmacdeploy
	/Users/po/Qt5.4.0/5.4/clang_64/bin/macdeployqt ../../../cmake/projects/sargam/Release/sargam.app

	#--- creation du DMG de distribution
	pushd ../../../cmake/projects/sargam/Release/

	# taille du DMG doit etre aussi grande que la taille du app
	# figure out how big our DMG needs to be
	#  assumes our contents are at least 1M!
	SIZE=`du -sh "${APP_NAME}" | sed 's/\([0-9]*\)M\(.*\)/\1/'`
	SIZE=`echo "${SIZE} + 1.0" | bc | awk '{print int($1+0.5)}'`

	hdiutil create -srcfolder "${APP_NAME}" -volname "${VOLUME_NAME}" -fs HFS+ -fsargs "-c c=64,a=16,e=16" -format UDRW -size ${SIZE}M "${RW_DMG_NAME}"

	#mounter le dmg pour ajouter background et application
	DEVICE=$(hdiutil attach -readwrite -noverify "${RW_DMG_NAME}" | egrep '^/dev/' | sed 1q | awk '{print $1}')

	#ajout du lien application et du background
	pushd /Volumes/"${VOLUME_NAME}"
	ln -s /Applications
	popd

	hdiutil detach "${DEVICE}"

	#convertion en image compressée
	hdiutil convert "${RW_DMG_NAME}" -format UDZO -o "${FINAL_DMG_NAME}"

	#nettoyage des dmg temporaires
	rm -rf "${RW_DMG_NAME}"

	popd
fi
	



