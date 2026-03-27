#pragma once

#include "app/resource.h"

namespace Ui
{
  class BitmapResource;
 }

class BitmapResource : public Resource
{
  Q_OBJECT

public:
  BitmapResource(QString id, QWidget* parent = 0, Qt::WindowFlags flags = Qt::WindowFlags());
  BitmapResource(const BitmapResource& res);
  BitmapResource(QString id, QDataStream* in, QWidget* parent = 0, Qt::WindowFlags flags = Qt::WindowFlags());
  ~BitmapResource();

  QString              type() const  { return "bitmap"; }
  Resource*            clone() const { return new BitmapResource(*this); }

protected:
  void                 parse(QDataStream* in);
  void                 write(QDataStream* out) const;

private slots:
  void                 toggleAlpha(bool alpha);
  void                 scale();
  void                 exportFile();
  void                 importFile();

private:
  void                 setup();

  Ui::BitmapResource*   m_ui;

  QImage*              m_image;

  static QString       m_currentFilePath;
  static QString       m_currentFileFilter;

  static const char    FILE_SETTINGS_PATH[];
  static const char    FILE_FILTERS[];

  static const int     MAX_WIDTH   = 0xFFFF;
  static const int     MAX_HEIGHT  = 0xFFFF;
  static const int     ALPHA_INDEX = 0xFF;
};
