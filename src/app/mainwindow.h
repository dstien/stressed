#pragma once

#include <QMainWindow>

#include "resource.h"
#include "ui_mainwindow.h"

class ResourcesModel;
class QLabel;

class MainWindow : public QMainWindow
{
  Q_OBJECT

public:
  MainWindow(QWidget* parent = 0, Qt::WindowFlags flags = Qt::WindowFlags());

  void              loadFile(const QString& fileName);

protected:
  void              closeEvent(QCloseEvent* event);

private slots:
  bool              reset();
  void              open();
  void              save();
  void              saveAs();

  void              manual();
  void              about();

  void              setCurrent(const QModelIndex& index);

  void              moveResources(int direction);
  void              moveFirstResources();
  void              moveUpResources();
  void              moveDownResources();
  void              moveLastResources();
  void              sortResources();

  void              insertResource();
  void              duplicateResource();
  void              renameResource();
  void              removeResources();
  void              resourcesContextMenu(const QPoint& pos);

  void              isModified();

private:
  void              saveFile(const QString& fileName);
  void              updateWindowTitle();
  void              updateStatusBar();
  QString           unpackedExtension(const QString& extension);
  QString           unpackedPathlessName(const QString& fileName);
  bool              changeToSafeFileName(const QString& safeFileName);
  void              saveAsAsIs(bool nameWasChanged);

  Ui::MainWindow    m_ui;

  ResourcesModel*   m_resourcesModel;
  Resource*         m_currentResource;

  QLabel*           m_statusLabel;

  QString           m_currentFileName;
  QString           m_currentFilePath;
  QString           m_currentFileFilter;

  bool              m_modified;

  static const char FILE_SETTINGS_PATH[];
  static const char FILE_FILTERS_LOAD[];
  static const char FILE_FILTERS_SAVE[];
};
