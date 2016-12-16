## Translations

If you plan to maintain translations for this application, follow the steps listed below. All language files must use ISO 639-1 language codes.

### Updating an existing translation

Open `locale/{language}.ts` in [Qt Linguist](http://doc.qt.io/qt-5/qtlinguist-index.html). Make changes. Save the file. Recompile the application.

Be sure to always respect the use of ampersands and `%1`, `%2`, `%3`, etc. Failure to do so will result in odd program glitches.

### Adding a new translation language

1. Add the `locale/{language}.ts` entry to the `TRANSLATIONS +=` directive in `azclient.pro`, which is space-separated.
2. Add the corresponding XML entry to `locale/locales.qrc`.
3. Run `lupdate azclient.pro`.
4. See "Updating an existing translation" section above.

### Adding new translation strings from source code to translation files

Run `lupdate azclient.pro`.
