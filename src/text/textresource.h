#pragma once

#include "app/resource.h"

namespace Ui
{
  class TextResource;
}

class TextResource : public Resource
{
  Q_OBJECT

public:
  TextResource(QString id, QWidget* parent = 0, Qt::WindowFlags flags = Qt::WindowFlags());
  TextResource(const TextResource& res);
  TextResource(QString id, QDataStream* in, QWidget* parent = 0, Qt::WindowFlags flags = Qt::WindowFlags());
  ~TextResource();

  QString           type() const  { return "text"; }
  Resource*         clone() const { return new TextResource(*this); }

protected:
  void              parse(QDataStream* in);
  void              write(QDataStream* out) const;

private:
  Ui::TextResource*  m_ui;
};
