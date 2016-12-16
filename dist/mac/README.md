## macOS DMG Builder

### Requirements

* [Qt5](https://www.qt.io/download-open-source/) (Choose "macOS" from the installer.)
* [XCode](https://itunes.apple.com/us/app/xcode/id497799835?ls=1&mt=12)

### Instructions

    $ path/to/Qt/.../bin/qmake
    $ make
    $ make installer

Then find the DMG in `dist/mac`.

### Upkeep

Be sure to keep the `openvpn` binary in `dist/mac/bin` up to date. It is taken from inside the app folder of [Tunnelblick](https://www.tunnelblick.net/).
