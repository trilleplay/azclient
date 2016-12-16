@ECHO ON
move ..\..\bin\azclient.exe ..\..\bin\AzireVPN.exe
%QTDIR%\bin\windeployqt.exe --compiler-runtime --angle ..\..\bin\AzireVPN.exe
SignTool.exe sign /v /tr http://timestamp.digicert.com /n "Netbouncer AB" /d "AzireVPN" /du "https://www.azirevpn.com/" /td sha256 /fd sha256 ..\..\bin\*
"%PROGRAMFILES(X86)%\NSIS\makensis.exe" installer.nsi
SignTool.exe sign /v /tr http://timestamp.digicert.com /n "Netbouncer AB" /d "AzireVPN Installer" /du "https://www.azirevpn.com/" /td sha256 /fd sha256 AzireVPN-%VERSION%.exe
