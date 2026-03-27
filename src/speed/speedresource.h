#pragma once

#include "app/resource.h"

namespace Ui
{
  class SpeedResource;
}

class QSpinBox;

class SpeedResource : public Resource
{
  Q_OBJECT

public:
  SpeedResource(QString id, QWidget* parent = 0, Qt::WindowFlags flags = Qt::WindowFlags());
  SpeedResource(const SpeedResource& res);
  SpeedResource(QString id, QDataStream* in, QWidget* parent = 0, Qt::WindowFlags flags = Qt::WindowFlags());
  ~SpeedResource();

  QString            type() const  { return "speed"; }
  Resource*          clone() const { return new SpeedResource(*this); }

protected:
  void               parse(QDataStream* in);
  void               write(QDataStream* out) const;

private:
  void               setup();

  Ui::SpeedResource*  m_ui;

  static const int   NUM_VALUES = 16;
  QSpinBox*          m_spinBoxes[NUM_VALUES];
};
