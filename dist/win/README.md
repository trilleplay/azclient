## Windows NSIS Installer

### Requirements

* [Qt5](https://www.qt.io/download-open-source/) (Choose "msvc2015 64-bit" from the installer.)
* [Visual Studio 2015](https://www.visualstudio.com/downloads/) (Community edition is fine; make sure "Visual C++" is selected.)
* [NSIS](http://nsis.sourceforge.net/Download) (Install to the default path.)

### Instructions

From the start menu, select "Qt" > "Qt 5.x 64-bit for Desktop (MSVC 2015)", which will open a terminal window:

    > cd \path\to\azclient
    > qmake

Next, from the start menu, select "Visual Studio 2015" > "Visual Studio Tools" > "WIndows Desktop Command Prompts" > "VS2015 x64 Native Tools Command Prompt", which will open a terminal window:

    > cd \path\to\azclient
    > nmake
    > nmake installer

Then find the installer in `dist\win\AzireVPN-x.y.exe`.

### Upkeep

Be sure to keep the binaries in `dist/win/bin` up to date. They're taken from [the OpenVPN download server](http://build.openvpn.net/downloads/releases/latest/). `tap-windows-latest-stable.exe` corresponds to `tap-windows.exe`, and the rest are extracted out of an install of `openvpn-install-latest-stable.exe`.
