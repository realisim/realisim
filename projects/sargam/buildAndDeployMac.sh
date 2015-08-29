#!/bin/bash

#configuration cmake

#— build
#build clean
xcodebuild -project ../../../cmake/Realisim.xcodeproj clean -configuration Release -sdk macosx10.10

#build in release 64 bits with SDK 10.10 and deployment target 10.7
xcodebuild -project ../../../cmake/Realisim.xcodeproj build -configuration Release -sdk macosx10.10 MACOSX_DEPLOYMENT_TARGET=10.7

#qtmacdeploy

#modification du dmg pour ajouter le raccourci application
