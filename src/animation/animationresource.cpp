#include "animationresource.h"
#include "ui_animationresource.h"

AnimationResource::AnimationResource(QString id, QWidget* parent, Qt::WindowFlags flags)
: Resource(id, parent, flags),
  m_ui(new Ui::AnimationResource)
{
  m_ui->setupUi(this);
}

AnimationResource::AnimationResource(const AnimationResource& res)
: Resource(res.id(), qobject_cast<QWidget*>(res.parent()), res.windowFlags()),
  m_ui(new Ui::AnimationResource)
{
  m_ui->setupUi(this);

  m_ui->framesEdit->setPlainText(res.m_ui->framesEdit->toPlainText());
}

AnimationResource::AnimationResource(QString id, QDataStream* in, QWidget* parent, Qt::WindowFlags flags)
: Resource(id, parent, flags),
  m_ui(new Ui::AnimationResource)
{
  m_ui->setupUi(this);

  parse(in);
}

AnimationResource::~AnimationResource()
{
  delete m_ui;
}

// Read NULL-terminated array from QDataStream.
void AnimationResource::parse(QDataStream* in)
{
  QString content;

  qint8 cur;
  *in >> cur;

  while (cur) {
    content.append(QString("%1\n").arg(cur));
    *in >> cur;
  }

  checkError(in, tr("animation sequence indices"));

  m_ui->framesEdit->setPlainText(content);
}

// Write NULL-terminated array to QDataStream.
void AnimationResource::write(QDataStream* out) const
{
  QStringList frames = m_ui->framesEdit->toPlainText().split('\n', Qt::SkipEmptyParts);

  foreach (const QString& f, frames) {
    quint8 i = f.toInt();
    *out << (quint8)(i ? i : 1);
  }

  *out << (qint8)0;

  checkError(out, tr("animation sequence indices"), true);
}
