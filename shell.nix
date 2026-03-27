{ pkgs ? import <nixpkgs> {} }:

let
  qt = pkgs.qt5;
in
pkgs.mkShell {
  packages = [
    qt.qtbase
    qt.qtsvg
    pkgs.cmake
    pkgs.libGLU
  ];

  QT_PLUGIN_PATH="${qt.qtbase.bin}/lib/qt-${qt.qtbase.version}/plugins:${qt.qtsvg.bin}/lib/qt-${qt.qtsvg.version}/plugins";
}
