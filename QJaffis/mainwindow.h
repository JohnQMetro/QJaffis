//***************************************************************************
// Name:        mainwindow.h
// Author :     John Q Metro
// Purpose :    Guess
// Created:     By QtCreator, forgot when
// Updated:     December 29, 2017
//***************************************************************************
#ifndef MAINWINDOW_H
  #define MAINWINDOW_H
#endif // MAINWINDOW_H
//----------------------------------------
#include <QMainWindow>
#include <QMenuBar>
#include <QTabWidget>
//------------------------------------
#ifndef PAGETRACKER_H_INCLUDED
  #include "core/structs/pagetracker.h"
#endif // PAGETRACKER_H_INCLUDED
#ifndef MAINSEARCH_H
  #include "interface/display/mainsearch.h"
#endif // MAINSEARCH_H
//***************************************************************************
class jfMainWindow : public QMainWindow {
    Q_OBJECT
  public:
    jfMainWindow(QWidget *parent = 0);
    ~jfMainWindow();
  public slots:
    // file slots
    void launchNewFFNSearch();
    void launchNewFIMSearch();
    void launchNewAO3Search();
    void openSavedSearch();
    void saveSearch();
    void writeCurrent2Html();
    void writeAll2Html();
    void CloseCurrentTab();
    void doExit();
    // tools slots
    void showGlobalSkeletons();
    void launchFanficDownload();
    void showPreferences();
    void showGlobalFilterEditor();
    void launchAO3FandomDownload();
    void launchFicUpdater();
    void launchFFNCategoryDownload();
    void launchFIMGroupSearch();
    void launchDirectorySync();
    void fimTagDownload();
    // non menu slots
    void handleTabChanged(int index);
    void handleSearchStart();
    void handleSearchEnd(bool complete);
    void handleFicUpdateEnd();
    void handleAO3FEnd();
    void handleFFNCEnd(bool xin);
  private:
    // helper methods
    bool GetJSSFilename(QString& outname, bool save);
    bool MenuEnabler(int menu_index);
    void ConnectSearch(jfMainSearchGroup* newpanel);
    bool PrepNewSearch(jfMainSearchGroup* nsearch, jf_tlPageType stype, bool usname, const QString& site_name);
    void CloseSearch();
    void EnableClose();
    // setting up the menu
    QMenuBar* main_menu;
    QMenu* fmenu, *tmenu;
    void SetupMainMenu();
    // the main part of the window
    QTabWidget* main_display;
    // extra data
    jfPageTracker* ptracker;
    size_t searchcount;
    bool closestop;
};
//***************************************************************************
