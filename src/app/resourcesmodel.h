#pragma once

#include <QAbstractListModel>

class Resource;
class QItemSelectionModel;

typedef QList<Resource*> ResourcesList;

class ResourcesModel : public QAbstractListModel
{
  Q_OBJECT

public:
  ResourcesModel(QObject* parent = 0);
  ResourcesModel(const ResourcesList& resources, QObject* parent = 0);

  Qt::ItemFlags     flags(const QModelIndex& index) const;
  QVariant          data(const QModelIndex& index, int role) const;
  bool              setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole);

  void              insertRow(Resource* resource, int position = ROWS_MAX);
  bool              removeRows(int position, int rows, const QModelIndex& index = QModelIndex());
  void              removeRows(const QModelIndexList& rows);
  void              moveRows(QItemSelectionModel* selectionModel, int direction);
  void              duplicateRow(int position);
  void              clear();

  void              sort(int column = 0, Qt::SortOrder order = Qt::AscendingOrder);
  int               rowCount(const QModelIndex& /*parent*/ = QModelIndex()) const    { return m_resources.size(); }

  Resource*         at(int index) const                                              { return m_resources[index]; }
  Resource*         at(const QModelIndex& index) const;

  static const int  ROWS_MAX = 65536;

private:
  ResourcesList     m_resources;
};
