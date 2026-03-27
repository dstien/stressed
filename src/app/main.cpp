#include <QApplication>

#include "mainwindow.h"
#include "settings.h"

int main(int argc, char** argv)
{
  QApplication app(argc, argv);
  app.setOrganizationName(Settings::ORG_NAME);
  app.setApplicationName(Settings::APP_NAME);

  // Scan argument list for filename.
  QString fileName;
  for (int i = 1; i < argc; i++) {
    if (argv[i][0] != '-') {
      fileName = argv[i];
      break;
    }
  }

  // Init settings.
  Settings();

  // Init main window.
  MainWindow mainWindow;
  mainWindow.show();

  // Load file from command line argument.
  if (!fileName.isEmpty()) {
    mainWindow.loadFile(fileName);
  }

  return app.exec();
}
