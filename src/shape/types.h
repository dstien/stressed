#pragma once

#include <QList>
#include <QVector3D>

typedef struct {
  qint16 x;
  qint16 y;
  qint16 z;

  inline QVector3D toQ() const { return QVector3D(x, y, z); }
} Vertex;

typedef QList<Vertex> VerticesList;

typedef struct {
  float x;
  float y;
  float z;

  inline QVector3D toQ() const { return QVector3D(x, y, z); }
} VertexF;

typedef QList<VertexF> VerticesFList;

class VerticesModel;

typedef QList<quint8> MaterialsList;

class MaterialsModel;

typedef struct {
  quint8            type;
  bool              twoSided;
  bool              zBias;
  VerticesModel*    verticesModel;
  MaterialsModel*   materialsModel;
  quint32           cull1;
  quint32           cull2;
} Primitive;

typedef QList<Primitive> PrimitivesList;
