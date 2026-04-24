#pragma once

#include <QColor>
#include <QHash>
#include <QSettings>
#include <QVector>

typedef struct {
  int color;
  int pattern;
} Material;

typedef QHash<QString, QString> StringMap;
typedef QVector<QRgb>           Palette;
typedef QList<Material>         Materials;

class Settings : public QSettings
{
  Q_OBJECT

public:
  Settings();

  QString           getFilePath(const QString& path);
  void              setFilePath(const QString& path, const QString& filePath);
  QString           restoreFilePath(const QString& path);

  StringMap         getStringMap(const QString& path);
  void              setStringMap(const QString& path, const StringMap& map);
  void              restoreStringMap(const QString& path);

  Palette           getPalette(const QString& path);
  void              setPalette(const QString& path, const Palette& pal);
  Palette           restorePalette(const QString& path);

  Materials         getMaterials();
  void              setMaterials(const Materials& materials);
  Materials         restoreMaterials();

  static const char APP_NAME[];
  static const char APP_VER[];
  static const char APP_DESC[];
  static const char APP_AUTHOR[];
  static const char APP_CONTACT[];
  static const char ORG_NAME[];
  static const char ORG_URL[];
  static const char CONTRIB_URL[];
  static const char MAN_URL[];

  static const char DEFAULTS[];

  static const char PATH_MAIN_CONF_VERSION[];
  static const char PATH_MATERIALS_COLORS[];
  static const char PATH_MATERIALS_PATTERNS[];
  static const char PATH_PALETTES_VGA[];
  static const char PATH_TYPES[];
  static const char PATH_PATHS_RESOURCE[];

  static Palette    m_vgaPalette;
  static Palette    m_egaPalette;
  static Materials  m_loadedMaterials;

private:
  void              mergeDefaults();

  Palette           parsePalette(const QStringList& colorList);
  Materials         parseMaterials(const QStringList& colors, const QStringList& patterns);

  static bool       m_initialized;
};
