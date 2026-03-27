#include <QComboBox>

#include "shapemodel.h"
#include "typedelegate.h"

TypeDelegate::TypeDelegate(QObject *parent)
: QItemDelegate(parent)
{
}

QWidget* TypeDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem& /*option*/, const QModelIndex& /*index*/) const
{
  QComboBox* editor = new QComboBox(parent);
  editor->addItems(ShapeModel::TYPES());

  return editor;
}

void TypeDelegate::setEditorData(QWidget* editor, const QModelIndex& index) const
{
  QComboBox* typeComboBox = static_cast<QComboBox*>(editor);
  typeComboBox->setCurrentIndex(index.model()->data(index, Qt::EditRole).toInt() - 1);
}

void TypeDelegate::setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const
{
  QComboBox* typeComboBox = static_cast<QComboBox*>(editor);
  model->setData(index, typeComboBox->currentIndex() + 1);
}

void TypeDelegate::updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option, const QModelIndex& /*index*/) const
{
  editor->setGeometry(option.rect);
}
