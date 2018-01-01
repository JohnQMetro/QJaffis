#include "mainwindow.h"
#include <QApplication>
#include <iostream>

#ifndef INITEND_H_INCLUDED
  #include "initend.h"
#endif // INITEND_H_INCLUDED

#ifdef _WIN32
  #define jfEXITCODE 4
#elif
  #define jfEXITCODE 1
#endif

int main(int argc, char *argv[])
{
  QApplication a(argc, argv);

  std::cout << "Start A" << std::endl;
  jfTextStatusDialog* loader = jglobal::settings.MakeLoadDisplay();
  std::cout << "Start B" << std::endl;
  loader->show();
  std::cout << "Start C" << std::endl;
  bool okay = jglobal::settings.LoadAtStart();
  std::cout << "Start D" << std::endl;
  if (!okay) return jfEXITCODE;
  jfMainWindow w;
  loader->close();
  std::cout << "Start E" << std::endl;
  w.show();
  return a.exec();
}
