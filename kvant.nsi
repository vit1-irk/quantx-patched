# http://sourceforge.net/project/memberlist.php?group_id=355577
!include "Version.nsh"
Name "Kvant ${VERSION}"
OutFile "Kvant-${VERSION}.exe"
InstallDir $PROGRAMFILES32\Kvant
RequestExecutionLevel user

XPStyle on

; First is default
LoadLanguageFile "${NSISDIR}\Contrib\Language files\English.nlf"
LoadLanguageFile "${NSISDIR}\Contrib\Language files\Russian.nlf"
LicenseLangString license ${LANG_ENGLISH} license-en.txt
LicenseLangString license ${LANG_RUSSIAN} license-ru.txt
LicenseData $(license)
LicenseForceSelection checkbox
LangString note ${LANG_ENGLISH} "This will install educational software Kvant. Do you wish to continue?"
LangString note ${LANG_RUSSIAN} "Вы устанавливаете учебную программу Квант. Продолжить?"

VIProductVersion "${VERSIONA}"
VIAddVersionKey /LANG=${LANG_ENGLISH} "ProductName" "Kvant"
VIAddVersionKey /LANG=${LANG_ENGLISH} "Comments" "Software for teaching quantum mechanics"
VIAddVersionKey /LANG=${LANG_ENGLISH} "FileDescription" "Kvant installation program"
VIAddVersionKey /LANG=${LANG_ENGLISH} "FileVersion" "${VERSION}"
VIAddVersionKey /LANG=${LANG_ENGLISH} "LegalCopyright" "Olga Tkachenko"

VIAddVersionKey /LANG=${LANG_RUSSIAN} "ProductName" "Kvant"
VIAddVersionKey /LANG=${LANG_RUSSIAN} "Comments" "Программа для изучения квантовой механики"
VIAddVersionKey /LANG=${LANG_RUSSIAN} "FileDescription" "Инсталляционная программа"
VIAddVersionKey /LANG=${LANG_RUSSIAN} "FileVersion" "${VERSION}"
VIAddVersionKey /LANG=${LANG_RUSSIAN} "LegalCopyright" "Ольга Ткаченко"

Page license
Page directory
Page instfiles
UninstPage uninstConfirm
UninstPage instfiles

Section "Installer Section"
  SetOutPath $INSTDIR
  File "release\kvant.exe"
  File "C:\Qt\4.4.0\bin\QtCore4.dll"
  File "C:\Qt\4.4.0\bin\QtGui4.dll"
  File "C:\Program Files\Microsoft Visual Studio 9.0\VC\redist\x86\Microsoft.VC90.CRT\msvcr90.dll"
  File "C:\Program Files\Microsoft Visual Studio 9.0\VC\redist\x86\Microsoft.VC90.CRT\msvcp90.dll"
  File "documentation\kvant.pdf"
  File "documentation\tasks.pdf"
SectionEnd

Section -post
  SetOutPath $INSTDIR
  WriteRegStr HKLM "Software\Kvant" "" $INSTDIR

  WriteRegExpandStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Kvant" "UninstallString" '"$INSTDIR\uninstall.exe"'
  WriteRegExpandStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Kvant" "InstallLocation" "$INSTDIR"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Kvant" "DisplayName" "Kvant Software"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Kvant" "DisplayVersion" "${VERSION}"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Kvant" "URLInfoAbout" "http://sourceforge.net/projects/quantx/"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Kvant" "HelpLink" "http://sourceforge.net/projects/quantx/support"
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Kvant" "NoModify" "1"
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Kvant" "NoRepair" "1"
  WriteUninstaller $INSTDIR\uninstall.exe
SectionEnd

; Optional section (can be disabled by the user)
Section "Start Menu Shortcuts"
  CreateDirectory "$SMPROGRAMS\Kvant"
  CreateShortCut "$SMPROGRAMS\Kvant\Uninstall.lnk" "$INSTDIR\uninstall.exe" "" "$INSTDIR\uninstall.exe" 0
  CreateShortCut "$SMPROGRAMS\Kvant\Kvant.lnk" "$INSTDIR\kvant.exe" "" "$INSTDIR\kvant.exe" 0
  CreateShortCut "$SMPROGRAMS\Kvant\Kvant.pdf.lnk" "$INSTDIR\Kvant.pdf" "" "$INSTDIR\Kvant.pdf" 0
  CreateShortCut "$SMPROGRAMS\Kvant\Tasks.pdf.lnk" "$INSTDIR\Tasks.pdf" "" "$INSTDIR\Tasks.pdf" 0
SectionEnd

Section "un.Installer Section"
  DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Kvant"
  DeleteRegKey HKLM "Software\Kvant"
  Delete $INSTDIR\kvant.exe
  Delete $INSTDIR\uninstall.exe
  RMDir /r "$SMPROGRAMS\Kvant"
  RMDir /r $INSTDIR
SectionEnd


Function .onInit
  MessageBox MB_YESNO $(note) IDYES cont
      Abort
  cont:
FunctionEnd


; TODOs
; $APPDATA for saving files, or $PROFILE ?
; Check if the build is from clean checkout, and check version.
;
