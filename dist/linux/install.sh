#!/bin/sh

cd "$(dirname "$0")"

# https://www.gnu.org/prep/standards/html_node/Directory-Variables.html
DESTDIR="${DESTDIR:-}"
PREFIX="${PREFIX:-/usr}"
BINDIR="${BINDIR:-$PREFIX/bin}"
LIBDIR="${LIBDIR:-$PREFIX/lib}"
DATAROOTDIR="${DATAROOTDIR:-$PREFIX/share}"
ICONDIR="${ICONDIR:-$DATAROOTDIR/pixmaps}"
APPLICATIONSDIR="${APPLICATIONSDIR:-$DATAROOTDIR/applications}"

set -e

install -v -d -m 0755 "${DESTDIR}${APPLICATIONSDIR}"
install -v -d -m 0755 "${DESTDIR}${ICONDIR}"
install -v -d -m 0755 "${DESTDIR}${BINDIR}"

install -v -m 0755 ../../azclient "${DESTDIR}${BINDIR}/azirevpn"
install -v -m 0644 azirevpn.desktop "${DESTDIR}${APPLICATIONSDIR}/azirevpn.desktop"
install -v -m 0644 ../../resources/icons/app.svg "${DESTDIR}${ICONDIR}/azirevpn.svg"
