// stressed - Stunts/4D [Sports] Driving resource editor
// Copyright (C) 2008 Daniel Stien <daniel@stien.org>
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.

#include <QStringList>

#include "settings.h"

const char Settings::DEFAULTS[] = ":/conf/defaults.conf";

StringMap Settings::getStringMap(const QString& path)
{
  StringMap map;

  beginGroup(path);

  QStringList keys = childKeys();

  if (keys.empty()) {
    endGroup();
    restoreStringMap(path);
    beginGroup(path);

    keys = childKeys();
  }

  foreach(QString key, keys) {
    map[key] = value(key).toString();
  }

  endGroup();

  return map;
}

void Settings::setStringMap(const QString& path, const StringMap& map)
{
  beginGroup(path);

  foreach(QString key, map.keys()) {
    setValue(key, map[key]);
  }

  endGroup();
}

void Settings::restoreStringMap(const QString& path)
{
  StringMap map;
  QSettings defaults(DEFAULTS, IniFormat);

  defaults.beginGroup(path);

  QStringList keys = defaults.childKeys();

  foreach(QString key, keys) {
    map[key] = defaults.value(key).toString();
  }

  defaults.endGroup();

  setStringMap(path, map);
}
