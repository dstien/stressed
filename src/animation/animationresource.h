#pragma once

#include "app/resource.h"

namespace Ui {
  class AnimationResource;
}

class AnimationResource : public Resource
{
  Q_OBJECT

public:
  AnimationResource(QString id, QWidget* parent = 0, Qt::WindowFlags flags = Qt::WindowFlags());
  AnimationResource(const AnimationResource& res);
  AnimationResource(QString id, QDataStream* in, QWidget* parent = 0, Qt::WindowFlags flags = Qt::WindowFlags());
  ~AnimationResource();

  QString                type() const  { return "animation"; }
  Resource*              clone() const { return new AnimationResource(*this); }

protected:
  void                   parse(QDataStream* in);
  void                   write(QDataStream* out) const;

private:
  Ui::AnimationResource*  m_ui;
};
