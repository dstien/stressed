#pragma once

#include <QDialog>

#include "settings.h"
#include "ui_optionsdialog.h"

class OptionsDialog : public QDialog
{
  Q_OBJECT

public:
  OptionsDialog(QWidget* parent = 0);

  static CompressionFormat compressionFormat();
  CompressionFormat selectedFormat() const;

private:
  Ui::OptionsDialog m_ui;
};
