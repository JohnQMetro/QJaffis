#include "mainwindow.h"
#include <QApplication>
#include <QFont>
#include <iostream>

#ifndef INITEND_H_INCLUDED
  #include "initend.h"
#endif // INITEND_H_INCLUDED

#ifdef _WIN32
  #define jfEXITCODE 4
#else
  #define jfEXITCODE 1
#endif

// ********************************************************************************

int main(int argc, char *argv[]) {
    // turn dpi scaling on
    qputenv("QT_AUTO_SCREEN_SCALE_FACTOR", "1");
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

  QApplication a(argc, argv);

  QFont defaultFont = QApplication::font("QMessageBox");
  #ifdef Q_OS_WIN
  defaultFont.setPointSize(defaultFont.pointSize() + 2);
  #endif

  QApplication::setFont(defaultFont);

  std::cout << "Start A" << std::endl;
  jfTextStatusDialog* loader = jglobal::startstop.MakeLoadDisplay();
  std::cout << "Start B" << std::endl;
  loader->show();
  std::cout << "Start C" << std::endl;
  bool okay = jglobal::startstop.LoadAtStart();
  std::cout << "Start D" << std::endl;
  if (!okay) return jfEXITCODE;
  jfMainWindow w;
  loader->close();
  std::cout << "Start E" << std::endl;
  w.show();
  return a.exec();
}
