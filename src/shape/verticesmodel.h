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

#ifndef VERTICESMODEL_H
#define VERTICESMODEL_H

#include <QAbstractTableModel>

typedef struct {
  qint16 x;
  qint16 y;
  qint16 z;
} Vertex;

inline bool operator==(const Vertex& v1, const Vertex& v2)
{
  return v1.x == v2.x && v1.y == v2.y && v1.z == v2.z;
}

typedef QList<Vertex> VerticesList;

class VerticesModel : public QAbstractTableModel
{
  Q_OBJECT

public:
  VerticesModel(const VerticesList& vertices, QObject* parent = 0);

  Qt::ItemFlags     flags(const QModelIndex& index) const;
  QVariant          data(const QModelIndex& index, int role) const;
  QVariant          headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

  int               rowCount(const QModelIndex& /*parent*/ = QModelIndex()) const    { return vertices.size(); }
  int               columnCount(const QModelIndex& /*parent*/ = QModelIndex()) const { return 3; }

  VerticesList*     verticesList()                                                   { return &vertices; }

private:
  VerticesList      vertices;
};

#endif
