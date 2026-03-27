#pragma once

#include <QAbstractTableModel>

#include "types.h"

class MaterialsModel : public QAbstractTableModel
{
  Q_OBJECT

public:
  MaterialsModel(const MaterialsList& materials, QObject* parent = 0);
  MaterialsModel(quint8 material, QObject* parent = 0);
  MaterialsModel(int num, QObject* parent = 0);

  Qt::ItemFlags     flags(const QModelIndex& index) const;
  QVariant          data(const QModelIndex& index, int role) const;
  bool              setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole);
  QVariant          headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

  bool              insertRows(int position, int rows, const QModelIndex& index = QModelIndex());
  bool              removeRows(int position, int rows, const QModelIndex& index = QModelIndex());

  void              moveMaterialTo(int row, int newPosition);

  int               rowCount(const QModelIndex& /*parent*/ = QModelIndex()) const    { return m_materials.size(); }
  int               columnCount(const QModelIndex& /*parent*/ = QModelIndex()) const { return 1; }

  void              resize(int num);
  MaterialsList*    materialsList()                                                  { return &m_materials; }

  static const int  VAL_MIN = 0;
  static const int  VAL_MAX = 255;

private:
  void              setup();

  MaterialsList     m_materials;
};
