SetCompressor /SOLID /FINAL lzma
!addplugindir "nsis-plugins"

!define COMPANY_NAME "Netbouncer AB"
!define PRODUCT_NAME "AzireVPN"
!define EXE_NAME "AzireVPN.exe"
!define PRODUCT_DIR_REGKEY "Software\Microsoft\Windows\CurrentVersion\App Paths\${EXE_NAME}"
!define PRODUCT_UNINST_KEY "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME}"
!define PRODUCT_UNINST_ROOT_KEY "HKLM"

!include "MUI2.nsh"
!define MUI_ABORTWARNING
!define MUI_ICON "..\..\resources\icons\win\app.ico"
!define MUI_UNICON "..\..\resources\icons\win\uninstall.ico"

!insertmacro MUI_PAGE_WELCOME
!insertmacro MUI_PAGE_DIRECTORY
!insertmacro MUI_PAGE_INSTFILES
!define MUI_FINISHPAGE_RUN
!define MUI_FINISHPAGE_RUN_FUNCTION "LaunchApp"
!insertmacro MUI_PAGE_FINISH

!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES

!insertmacro MUI_LANGUAGE "English"

Name "${PRODUCT_NAME} v$%VERSION%"
OutFile "AzireVPN-$%VERSION%.exe"
InstallDir "$PROGRAMFILES64\${PRODUCT_NAME}\"
InstallDirRegKey HKLM "${PRODUCT_DIR_REGKEY}" ""
ShowInstDetails show
ShowUnInstDetails show
XPStyle on
RequestExecutionLevel admin

!include x64.nsh
Function .onInit
  ${IfNot} ${RunningX64}
    MessageBox MB_OK "This program only works on 64-bit Windows."
    Quit
  ${EndIf}:
FunctionEnd

Section "Uninstall Previous Version"
  ReadRegStr $R0 ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "UninstallString"
  StrCmp $R0 "" done
  nsExec::ExecToLog /OEM '"$R0" /S'
  Pop $R0
  Sleep 1000
  done:
SectionEnd

Section "Main Components"
  KillProcWMI::KillProc "${EXE_NAME}"
  KillProcWMI::KillProc "openvpn.exe"
  Sleep 300
  SetOverwrite on
  SetOutPath "$INSTDIR"
  File /r "..\..\bin\*"
SectionEnd

Section "Visual Studio Runtime"
  DetailPrint "Installing Visual Studio Runtime"
  nsExec::ExecToLog /OEM '"$INSTDIR\vcredist_x64.exe" /install /passive /norestart'
  Pop $R0
  Delete "$INSTDIR\vcredist_x64.exe"
SectionEnd

Section "TAP Driver"
  DetailPrint "Installing TAP Driver"
  nsExec::ExecToLog /OEM '"$INSTDIR\tap-windows.exe" /S /SELECT_UTILITIES=1'
  Pop $R0
  Delete "$INSTDIR\tap-windows.exe"
SectionEnd

Section Icons
  CreateShortCut "$SMPROGRAMS\${PRODUCT_NAME}.lnk" "$INSTDIR\${EXE_NAME}"
  CreateShortCut "$DESKTOP\${PRODUCT_NAME}.lnk" "$INSTDIR\${EXE_NAME}"
SectionEnd

Section Uninstaller
  WriteUninstaller "$INSTDIR\uninst.exe"
  WriteRegStr HKLM "${PRODUCT_DIR_REGKEY}" "" "$INSTDIR\${EXE_NAME}"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayName" "${PRODUCT_NAME}"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "UninstallString" "$INSTDIR\uninst.exe"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayIcon" "$INSTDIR\${EXE_NAME}"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayVersion" "$%VERSION%"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "Publisher" "${COMPANY_NAME}"
SectionEnd

Section Uninstall
  KillProcWMI::KillProc "${EXE_NAME}"
  KillProcWMI::KillProc "openvpn.exe"
  Sleep 300
  Delete "$SMPROGRAMS\${PRODUCT_NAME}.lnk"
  Delete "$DESKTOP\${PRODUCT_NAME}.lnk"
  RMDir /r "$INSTDIR"
  DeleteRegKey ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}"
  DeleteRegKey HKLM "${PRODUCT_DIR_REGKEY}"
SectionEnd

Function LaunchApp
  ;Exec '"$WINDIR\explorer.exe" "$INSTDIR\${EXE_NAME}"'
  ;Since we actually *DO* want to run as administrator, we execute it directly
  ;from the installer, rather than the explorer.exe trick.
  Exec "$INSTDIR\${EXE_NAME}"
FunctionEnd

BrandingText "AzireVPN"
