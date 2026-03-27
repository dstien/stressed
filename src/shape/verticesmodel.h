#pragma once

#include <QAbstractTableModel>

#include "types.h"

class ShapeModel;

inline bool operator==(const Vertex& v1, const Vertex& v2)
{
  return v1.x == v2.x && v1.y == v2.y && v1.z == v2.z;
}

class VerticesModel : public QAbstractTableModel
{
  Q_OBJECT

public:
  VerticesModel(const VerticesList& vertices, ShapeModel* parent = 0);
  VerticesModel(int type, ShapeModel* parent = 0);

  Qt::ItemFlags     flags(const QModelIndex& index) const;
  QVariant          data(const QModelIndex& index, int role) const;
  bool              setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole);
  QVariant          headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

  bool              insertRows(int position, int rows, const QModelIndex& index = QModelIndex());
  bool              removeRows(int position, int rows, const QModelIndex& index = QModelIndex());

  int               rowCount(const QModelIndex& /*parent*/ = QModelIndex()) const    { return m_vertices.size(); }
  int               columnCount(const QModelIndex& /*parent*/ = QModelIndex()) const { return 3; }

  void              flip();
  void              invertX(bool flip = true);

  void              replace(const Vertex& curVert, const Vertex& newVert, Primitive& primitive);
  void              resize(int type);
  VerticesList*     verticesList()                                                   { return &m_vertices; }
  VerticesFList*    verticesFList()                                                  { return &m_verticesF; }

  static void       toggleWeld(bool enable)                                          { m_weld = enable; }
  static bool       verticesNeeded(int type, int& verticesNeeded);
  static VertexF    toInternal(const Vertex& vertex);

  static const float Y_RATIO;

private:
  void              setup();

  VerticesList      m_vertices;
  VerticesFList     m_verticesF;

  static bool       m_weld;

  static const int  VAL_MIN = -32768;
  static const int  VAL_MAX =  32767;
};
