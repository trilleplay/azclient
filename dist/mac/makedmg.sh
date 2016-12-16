#!/bin/sh
set -ex
output="AzireVPN-${VERSION}.dmg"
cd "$(dirname "$0")"
rm -rf *dmg
mkdir dmg
cp -R ../../azclient.app dmg/AzireVPN.app
"${QTDIR}/bin/macdeployqt" dmg/AzireVPN.app
codesign -s "Netbouncer AB" --deep dmg/AzireVPN.app || echo -e "WARNING WARNING WARNING WARNING\nCode signing failed!\nWARNING WARNING WARNING WARNING"
hdiutil create -megabytes 120 -format UDZO -volname "AzireVPN" -srcfolder dmg "$output"
rm -rf dmg
