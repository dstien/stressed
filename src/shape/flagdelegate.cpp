#include <QAbstractItemModel>
#include <QApplication>
#include <QMouseEvent>

#include "flagdelegate.h"

FlagDelegate::FlagDelegate(QObject *parent)
: QItemDelegate(parent)
{
}

void FlagDelegate::drawCheck(QPainter* painter, const QStyleOptionViewItem& option, const QRect& /*rect*/, Qt::CheckState state) const
{
  const int textMargin = QApplication::style()->pixelMetric(QStyle::PM_FocusFrameHMargin) + 1;
  QRect checkRect = QStyle::alignedRect(option.direction, Qt::AlignCenter, doCheck(option, option.rect, Qt::Checked).size(),
      QRect(option.rect.x() + textMargin, option.rect.y(), option.rect.width() - (textMargin * 2), option.rect.height()));

  QItemDelegate::drawCheck(painter, option, checkRect, state);
}

void FlagDelegate::drawFocus(QPainter* painter, const QStyleOptionViewItem& option, const QRect& /*rect*/) const
{
  QItemDelegate::drawFocus(painter, option, option.rect);
}

bool FlagDelegate::editorEvent(QEvent* event, QAbstractItemModel* model, const QStyleOptionViewItem& option, const QModelIndex& index)
{
  Qt::ItemFlags flags = model->flags(index);
  if (!(flags & Qt::ItemIsUserCheckable) || !(flags & Qt::ItemIsEnabled)) {
    return false;
  }

  QVariant value = index.data(Qt::CheckStateRole);
  if (!value.isValid()) {
    return false;
  }

  if (event->type() == QEvent::MouseButtonRelease) {
    const int textMargin = QApplication::style()->pixelMetric(QStyle::PM_FocusFrameHMargin) + 1;
    QRect checkRect = QStyle::alignedRect(option.direction, Qt::AlignCenter, doCheck(option, option.rect, Qt::Checked).size(),
        QRect(option.rect.x() + textMargin, option.rect.y(), option.rect.width() - (textMargin * 2), option.rect.height()));

    if (!checkRect.contains(static_cast<QMouseEvent*>(event)->pos())) {
      return false;
    }
  }
  else if (event->type() == QEvent::KeyPress) {
    if (static_cast<QKeyEvent*>(event)->key() != Qt::Key_Space && static_cast<QKeyEvent*>(event)->key() != Qt::Key_Select) {
      return false;
    }
  }
  else {
    return false;
  }

  Qt::CheckState state = (static_cast<Qt::CheckState>(value.toInt()) == Qt::Checked ? Qt::Unchecked : Qt::Checked);
  return model->setData(index, state, Qt::CheckStateRole);
}
