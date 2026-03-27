#pragma once

#include <QItemDelegate>

class QComboBox;

typedef QList<QPixmap> Icons;

class MaterialDelegate : public QItemDelegate
{
  Q_OBJECT

public:
  MaterialDelegate(QObject *parent = 0);

  QWidget*              createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const;
  void                  setEditorData(QWidget* editor, const QModelIndex& index) const;
  void                  setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const;
  void                  updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option, const QModelIndex& index) const;

  static QComboBox*     createComboBox(QWidget* parent);
  static const QPixmap& getIcon(unsigned int index);

private:
  static void           setup();

  static Icons          m_icons;
  static bool           m_initialized;

  static const unsigned int NUM_MATERIALS = 130;
};
