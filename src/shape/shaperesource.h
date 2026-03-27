#pragma once

#include "app/resource.h"
#include "verticesmodel.h"

namespace Ui
{
  class ShapeResource;
}

class ShapeModel;

class ShapeResource : public Resource
{
  Q_OBJECT

public:
  ShapeResource(QString id, QWidget* parent = 0, Qt::WindowFlags flags = Qt::WindowFlags());
  ShapeResource(const ShapeResource& res);
  ShapeResource(QString id, QDataStream* in, QWidget* parent = 0, Qt::WindowFlags flags = Qt::WindowFlags());
  ~ShapeResource();

  QString           type() const       { return "shape"; }
  Resource*         clone() const      { return new ShapeResource(*this); }
  Primitive*        currentPrimitive() { return m_currentPrimitive; }

signals:
  void              paintJobMoved(int oldPosition, int newPosition);

protected:
  void              parse(QDataStream* in);
  void              write(QDataStream* out) const;

private slots:
  void              deselectAll();
  void              setModels(const QModelIndex& index);
  void              setNumPaintJobs();
  void              toggleWeld(bool enable);

  void              movePrimitives(int direction);
  void              moveFirstPrimitives();
  void              moveUpPrimitives();
  void              moveToPrimitives();
  void              moveDownPrimitives();
  void              moveLastPrimitives();

  void              insertPrimitive();
  void              duplicatePrimitive();
  void              mirrorXPrimitive();
  void              computeCullPrimitives();
  void              removePrimitives();
  void              primitivesContextMenu(const QPoint& pos);

  void              flipVertices();
  void              invertXVertices();
  void              verticesContextMenu(const QPoint& pos);

  void              replaceMaterials();
  void              movePaintJobs(int direction);
  void              moveFirstPaintJobs();
  void              moveUpPaintJobs();
  void              moveDownPaintJobs();
  void              moveLastPaintJobs();
  void              moveToPaintJobs();
  void              materialsContextMenu(const QPoint& pos);

  void              exportFile();
  void              importFile();

  void              isModified();

private:
  void              setup();
  void              showEvent(QShowEvent* event);
  VerticesList      buildVerticesList(bool boundBox = false) const;

  Ui::ShapeResource* m_ui;

  ShapeModel*       m_shapeModel;
  Primitive*        m_currentPrimitive;

  static QString    m_currentFilePath;
  static QString    m_currentFileFilter;

  static const int  MAX_VERTICES = 256;

  static const char FILE_SETTINGS_PATH[];
  static const char FILE_FILTERS[];
  static const char MTL_SRC[];
  static const char MTL_DST[];

  static const QRegExp OBJ_REGEXP_WHITESPACE;
  static const QRegExp OBJ_REGEXP_VERTEX;
  static const QRegExp OBJ_REGEXP_FACE;
};
