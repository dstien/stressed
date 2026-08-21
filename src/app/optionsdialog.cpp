#include "optionsdialog.h"
#include "settings.h"

OptionsDialog::OptionsDialog(QWidget* parent)
: QDialog(parent)
{
  m_ui.setupUi(this);

  CompressionFormat format = Settings::getCompressionFormat();

  if (format == CompressionFormat::GrandPrix) {
    m_ui.radioGrandPrix->setChecked(true);
  } else {
    m_ui.radioStunts->setChecked(true);
  }
}

CompressionFormat OptionsDialog::compressionFormat()
{
  return Settings::getCompressionFormat();
}

CompressionFormat OptionsDialog::selectedFormat() const
{
  if (m_ui.radioGrandPrix->isChecked()) {
    return CompressionFormat::GrandPrix;
  }
  return CompressionFormat::Stunts;
}
