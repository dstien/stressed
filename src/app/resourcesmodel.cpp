#include <QItemSelectionModel>

#include "resource.h"
#include "resourcesmodel.h"

const int ResourcesModel::ROWS_MAX;

ResourcesModel::ResourcesModel(QObject* parent)
: QAbstractListModel(parent)
{
}

ResourcesModel::ResourcesModel(const ResourcesList& resources, QObject* parent)
: QAbstractListModel(parent),
  m_resources(resources)
{
}

Qt::ItemFlags ResourcesModel::flags(const QModelIndex& index) const
{
  if (!index.isValid()) {
    return Qt::ItemFlags();
  }

  return QAbstractItemModel::flags(index) | Qt::ItemIsEditable;
}

QVariant ResourcesModel::data(const QModelIndex& index, int role) const
{
  int row = index.row(), col = index.column();

  if (!index.isValid() || row >= rowCount() || col != 0) {
    return QVariant();
  }

  switch (role) {
    case Qt::DisplayRole:
    case Qt::EditRole:
      return m_resources[row]->id();

    default:
      return QVariant();
  }
}

bool ResourcesModel::setData(const QModelIndex &index, const QVariant& value, int role)
{
  int row = index.row(), col = index.column();

  if (!index.isValid() || role != Qt::EditRole ||
      row >= rowCount() || col != 0) {
    return false;
  }

  QString id = value.toString();

  if (id != m_resources[row]->id() && id.size() == 4) {
    m_resources[row]->setId(id);
    emit dataChanged(index, index);
    return true;
  }
  else {
    return false;
  }
}

Resource* ResourcesModel::at(const QModelIndex& index) const
{
  if (!index.isValid() || index.row() >= rowCount()) {
    return NULL;
  }

  return m_resources[index.row()];
}

void ResourcesModel::insertRow(Resource* resource, int position)
{
  position = position == ROWS_MAX ? m_resources.size() : position;
  beginInsertRows(QModelIndex(), position, position);
  m_resources.insert(position, resource);
  endInsertRows();
}

bool ResourcesModel::removeRows(int position, int rows, const QModelIndex& index)
{
  beginRemoveRows(index, position, position + rows - 1);

  for (int row = 0; row < rows; row++) {
    m_resources.removeAt(position);
  }

  endRemoveRows();

  return true;
}

void ResourcesModel::removeRows(const QModelIndexList& rows)
{
  // Using persistent indices since row removal will invalidate current selection.
  QList<QPersistentModelIndex> persistentRows;
  foreach (QModelIndex row, rows) {
    persistentRows.append(QPersistentModelIndex(row));
  }

  foreach (QPersistentModelIndex row, persistentRows) {
    removeRows(row.row(), 1);
  }
}

void ResourcesModel::moveRows(QItemSelectionModel* selectionModel, int direction)
{
  // Using persistent indices since row removal/insertion will invalidate current selection.
  QList<QPersistentModelIndex> newPersistentRows;
  QList<QPersistentModelIndex> curPersistentRows;
  foreach (QModelIndex row, selectionModel->selectedRows()) {
    curPersistentRows.append(QPersistentModelIndex(row));
  }

  // Sort by direction to prevent overwriting.
  if (direction < 0) {
    std::sort(curPersistentRows.begin(), curPersistentRows.end());
  }
  else {
    std::reverse(curPersistentRows.begin(), curPersistentRows.end());
  }

  QPersistentModelIndex persistentCurrent = selectionModel->currentIndex();
  QModelIndex parent, current = persistentCurrent;

  foreach (QPersistentModelIndex row, curPersistentRows) {
    int curRow = row.row();
    int newRow = qBound(0, curRow + direction, rowCount() - 1);

    if (curRow != newRow) {
      if (persistentCurrent.row() == curRow) {
        current = index(newRow < ROWS_MAX ? newRow : rowCount() - 1);
      }

      Resource* resource = m_resources[curRow];

      beginRemoveRows(parent, curRow, curRow);
      m_resources.removeAt(curRow);
      endRemoveRows();

      beginInsertRows(parent, newRow, newRow);
      m_resources.insert(newRow, resource);
      endInsertRows();
    }

    newPersistentRows.append(QPersistentModelIndex(index((newRow < ROWS_MAX ? newRow : rowCount() - 1))));
  }

  selectionModel->reset();
  selectionModel->setCurrentIndex(current, QItemSelectionModel::Current);

  foreach (QPersistentModelIndex row, newPersistentRows) {
    selectionModel->select(row, QItemSelectionModel::Select);
  }
}

void ResourcesModel::duplicateRow(int position)
{
  if (position >= 0 && position < m_resources.size()) {
    beginInsertRows(QModelIndex(), position, position);
    m_resources.insert(position, m_resources[position]->clone());
    endInsertRows();
  }
}

void ResourcesModel::clear()
{
  beginResetModel();

  foreach(Resource* resource, m_resources) {
    delete resource;
  }

  m_resources.clear();

  endResetModel();
}

void ResourcesModel::sort(int /*column*/, Qt::SortOrder order)
{
  beginResetModel();

  std::sort(m_resources.begin(), m_resources.end(), (order == Qt::AscendingOrder ? Resource::lessThan : Resource::greaterThan));

  endResetModel();
}
