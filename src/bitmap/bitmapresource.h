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

  static void          setEgaMode(bool ega) { m_egaMode = ega; }
  static void          setTocSize(quint32 size) { m_tocSize = size; }

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
  void                 parseEga(QDataStream* in, quint16 width, quint16 height,
                                quint32 tocSize, quint8 unk0, quint8 unk1, quint8 unk2, quint8 unk3);

  Ui::BitmapResource*   m_ui;

  static bool           m_egaMode;

  QImage*              m_image;

  static QString       m_currentFilePath;
  static QString       m_currentFileFilter;
  static quint32       m_tocSize;

  static const char    FILE_SETTINGS_PATH[];
  static const char    FILE_FILTERS[];

  static const int     MAX_WIDTH   = 0xFFFF;
  static const int     MAX_HEIGHT  = 0xFFFF;
  static const int     ALPHA_INDEX = 0xFF;
};
