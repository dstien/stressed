#pragma once

#include <QList>
#include <QVector3D>

struct Vertex{
  qint16 x;
  qint16 y;
  qint16 z;

  inline QVector3D toQ() const { return QVector3D(x, y, z); }
};

typedef QList<Vertex> VerticesList;

struct VertexF{
  float x;
  float y;
  float z;

  inline QVector3D toQ() const { return QVector3D(x, y, z); }
};

typedef QList<VertexF> VerticesFList;

class VerticesModel;

typedef QList<quint8> MaterialsList;

class MaterialsModel;

struct Primitive{
  quint8            type;
  bool              twoSided;
  bool              zBias;
  VerticesModel*    verticesModel;
  MaterialsModel*   materialsModel;
  quint32           cull1;
  quint32           cull2;
};

typedef QList<Primitive> PrimitivesList;
