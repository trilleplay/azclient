# `azclient` -- Customizable VPN Client

[![Build Status](https://travis-ci.org/azirevpn/azclient.svg?branch=master)](https://travis-ci.org/azirevpn/azclient)

This is a customizable VPN client, meant to be simple and sleek. It was originally built for [AzireVPN](https://www.azirevpn.com/), but has been designed from the beginning for customization in other contexts. Without modifying anything, it will currently build with the AzireVPN branding.

### Linux Dependencies

* OpenVPN
* Qt 5 (specifically: QtCore, QtWidgets, QtGui, QtNetwork, QtSvg)
* PolicyKit
* Recent C++11 compiler

On Ubuntu, these may be installed via:

```
$ sudo apt install build-essential libqt5*5-dev qt*5-dev qttools5-dev-tools policykit-1 openvpn qtchooser
```

### Building and Installing

After acquiring the dependencies, you may build and install the application.

```
$ qtchooser -run-tool=qmake -qt=5  # Alternatively, just `qmake` might work
$ make
$ sudo make install
```

The `make install` target takes `DESTDIR` and `PREFIX` as standard environment variables.

### Other Platforms

This also works quite well on macOS and Windows. See the READMEs in [`dist/mac`](dist/mac) and [`dist/win`](dist/win) for an overview of the build systems and installer generation.

### Discussion

This project may be discussed in `#azclient` on Freenode.

### Customization

This is a customizable VPN client. To change the branding, adjust the following files:

* `customization.h` contains the various strings used to name the application.
* `resources/` contains all of the images and icons, as well as CSS theming info and CA pre-loads.
* `resources/ca.crt` may be optionally added, which will lock the program to only using the CAs specified.
* `dist/` contains installer scripts and code signing commands. You will likely need to change several strings in here.

### License

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License along
with this program; if not, write to the Free Software Foundation, Inc.,
51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

See `COPYING` for more information.
