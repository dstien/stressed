#pragma once

#include <QItemDelegate>

class FlagDelegate : public QItemDelegate
{
  Q_OBJECT

public:
  FlagDelegate(QObject *parent = 0);

  void              drawCheck(QPainter* painter, const QStyleOptionViewItem& option, const QRect& rect, Qt::CheckState state) const;
  void              drawDisplay(QPainter* /*painter*/, const QStyleOptionViewItem& /*option*/, const QRect& /*rect*/, const QString& /*text*/) const { }
  void              drawFocus(QPainter* painter, const QStyleOptionViewItem& option, const QRect& rect) const;
  bool              editorEvent(QEvent* event, QAbstractItemModel* model, const QStyleOptionViewItem& option, const QModelIndex& index);
};
