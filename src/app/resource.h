#pragma once

#include <QWidget>

class QDataStream;
class QListWidget;
class ResourcesModel;

typedef struct {
  int      pos;
  QString id;
  quint32 offset;
  quint32 size;
} TocEntry;

typedef QList<TocEntry> TocList;

class Resource : public QWidget
{
  Q_OBJECT

public:
  Resource(QString id, QWidget* parent = 0, Qt::WindowFlags flags = Qt::WindowFlags());
  virtual ~Resource() {};

  static bool       parse(const QString& fileName, ResourcesModel* resourcesModel, QWidget* parent = 0);
  static void       write(const QString& fileName, const ResourcesModel* resourcesModel);
  static Resource*  typeDialog(QWidget* parent = 0);

  static QString    fileName()        { return m_fileName; }
  QString           id() const        { return m_id; }
  void              setId(QString id) { m_id = id; }
  virtual QString   type() const = 0;
  virtual Resource* clone() const = 0;

  static bool       lessThan(const Resource* lhv, const Resource* rhv)    { return lhv->id() < rhv->id(); }
  static bool       greaterThan(const Resource* lhv, const Resource* rhv) { return lhv->id() > rhv->id(); }

  static const QStringList TYPES;
  static const QStringList LOAD_TYPES;

signals:
  void              dataChanged();

protected slots:
  virtual void      isModified();

protected:
  static void       checkError(QDataStream* stream, const QString& what, bool write = false);
  virtual void      parse(QDataStream* in) = 0;
  virtual void      write(QDataStream* out) const = 0;

private:
  static bool       tocOffsetLessThan(const TocEntry &e1, const TocEntry &e2);
  static bool       tocPositionLessThan(const TocEntry &e1, const TocEntry &e2);

  QString           m_id;

  static QString    m_fileName;
};
