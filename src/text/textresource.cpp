#include "textresource.h"

#include "ui_textresource.h"

TextResource::TextResource(QString id, QWidget* parent, Qt::WindowFlags flags)
: Resource(id, parent, flags),
  m_ui(new Ui::TextResource)
{
  m_ui->setupUi(this);
}

TextResource::TextResource(const TextResource& res)
: Resource(res.id(), qobject_cast<QWidget*>(res.parent()), res.windowFlags()),
  m_ui(new Ui::TextResource)
{
  m_ui->setupUi(this);

  m_ui->textEdit->setPlainText(res.m_ui->textEdit->toPlainText());
}

TextResource::TextResource(QString id, QDataStream* in, QWidget* parent, Qt::WindowFlags flags)
: Resource(id, parent, flags),
  m_ui(new Ui::TextResource)
{
  m_ui->setupUi(this);

  parse(in);
}

TextResource::~TextResource()
{
  delete m_ui;
}

// Read NULL-terminated C-string from QDataStream.
void TextResource::parse(QDataStream* in)
{
  QString content;

  qint8 cur;
  *in >> cur;

  while (cur) {
    if (cur == ']') {
      cur = '\n';
    }

    content += (char)cur;
    *in >> cur;
  }

  checkError(in, tr("plain text data"));

  m_ui->textEdit->setPlainText(content);
}

// Write NULL-terminated C-string to QDataStream.
void TextResource::write(QDataStream* out) const
{
  QByteArray content = m_ui->textEdit->toPlainText().toLatin1();

  for (int i = 0; i < content.count(); i++) {
    if (content[i] == '\n') {
      content[i] = ']';
    }

    *out << (qint8)content[i];
  }

  *out << (qint8)0;

  checkError(out, tr("plain text data"), true);
}
