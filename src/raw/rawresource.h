#pragma once

#include "app/resource.h"

namespace Ui
{
  class RawResource;
}

class QLineEdit;

class RawResource : public Resource
{
  Q_OBJECT

public:
  RawResource(QString id, QString type, unsigned int length, QWidget* parent = 0, Qt::WindowFlags flags = Qt::WindowFlags());
  RawResource(const RawResource& res);
  RawResource(QString id, QString type, unsigned int length, QDataStream* in, QWidget* parent = 0, Qt::WindowFlags flags = Qt::WindowFlags());
  ~RawResource();

  QString           type() const  { return m_type; }
  Resource*         clone() const { return new RawResource(*this); }

  static const int  LENGTH_PATH   = 186;
  static const int  LENGTH_TUNING = 776;

protected:
  void              parse(QDataStream* in);
  void              write(QDataStream* out) const;

private slots:
  void              exportFile();
  void              importFile();

private:
  void              setup();
  void              cleanup();

  Ui::RawResource*   m_ui;

  QString           m_type;
  unsigned int      m_length;
  quint8*           m_data;
  QLineEdit**       m_lineEdits;

  static QString    m_currentFilePath;

  static const char FILE_SETTINGS_PATH[];
  static const char FILE_FILTERS[];
};
