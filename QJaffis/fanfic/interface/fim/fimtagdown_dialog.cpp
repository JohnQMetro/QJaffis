/******************************************************************************
Name    :   fimtagdown_dialog.cpp
Author  :   John Q Metro
Purpose :   Dialog displayed when downloading FIM Tag Data
Created :   December 29, 2017
Updated :   December 29, 2017
******************************************************************************/
#ifndef FIMTAGDOWN_DIALOG_H
    #include "fimtagdown_dialog.h"
#endif // FIMTAGDOWN_DIALOG_H
#ifndef FIM_CONSTANTS_H
    #include "../../data/fim/fim_constants.h"
#endif // FIM_CONSTANTS_H
//---------------------------------------
#include <QApplication>
/*****************************************************************************/
jfFIMTag_DownloadDialog::jfFIMTag_DownloadDialog(QWidget* parent):QDialog(parent) {
    // NULL pointer initialization
    parser = NULL;
    downloader = NULL;
    fetch_thread = NULL;
    // setting up the GUI
    info = new QLabel("Click on the Button to Download Tag information for Fimfiction.net.");
    doButton = new QPushButton("Download");
    connect(doButton,SIGNAL(clicked(bool)),this,SLOT(HandleStartClick(bool)));
    main_sizer = new QVBoxLayout();
    main_sizer->addWidget(info);
    main_sizer->addWidget(doButton,0,Qt::AlignCenter);
    setLayout(main_sizer);
}
//+++++++++++++++++++++++++++++++++++++++++++++++++
// slots
//-------------------------------
void jfFIMTag_DownloadDialog::HandleStartClick(bool checked) {
    doButton->setEnabled(false);
    info->setText("Starting operations...");
    SetupObjectsAndConnect();

    fetch_thread->start();
}
//-------------------------------
void jfFIMTag_DownloadDialog::HandleStart(bool okay) {
    if (okay) info->setText("Thread started successfully");
    else info->setText("There was a problem with setting up the thread");
}
//-------------------------------
void jfFIMTag_DownloadDialog::DownloadDone() {
    info->setText("Page downloaded, parsing underway");
}
//-------------------------------
void jfFIMTag_DownloadDialog::HandleDone(bool okay) {
    if (okay) {
        // replacing the old data (if there)
        jfFIMTag_Collection* result = reinterpret_cast<jfFIMTag_Collection*>(parser->getResults());
        fimcon::ReplaceFIMTags(result);
        info->setText("Page parsed, and tags set, with no errors. See fimtag_report.txt for details.");
    }
    else info->setText("Page failed to parse. See fimtag_report.txt for any valid info.");
}
//-------------------------------
void jfFIMTag_DownloadDialog::ThreadDone() {
    ThreadFinished();
    delete parser;
    parser = NULL;
    doButton->setEnabled(true);
}
//+++++++++++++++++++++++++++++++++++++++++++++++++
// helper methods for setup and completion
//-------------------------------
// before starting the download, we have to create the objects involved and connect things
void jfFIMTag_DownloadDialog::SetupObjectsAndConnect() {
    parser = new jfFIMTag_Extractor();
    downloader = new jfOnePageGetter();
    fetch_thread = new QThread();

    // standard thread/downloader connections
    connect(fetch_thread, SIGNAL(started()), downloader ,SLOT(StartProcessing()));
    connect(downloader,SIGNAL(sendQuit()),fetch_thread,SLOT(quit()));
    connect(fetch_thread,SIGNAL(finished()),this,SLOT(ThreadDone()));
    // some prep involving the downloader and parser...
    downloader->moveToThread(fetch_thread);
    downloader->setParameters("https://www.fimfiction.net/stories?q=",parser,5);
    // setting up informational signals to slots
    connect(downloader,SIGNAL(sendStart(bool)),this,SLOT(HandleStart(bool)));
    connect(downloader,SIGNAL(sendDownloaded()),this,SLOT(DownloadDone()));
    connect(downloader,SIGNAL(sendFinished(bool)),this,SLOT(HandleDone(bool)));
}
//-------------------------------
void jfFIMTag_DownloadDialog::ThreadFinished() {
    // disconnecting before we delete...
    disconnect(fetch_thread, 0,downloader ,0);
    disconnect(downloader,0,fetch_thread,0);
    disconnect(fetch_thread,SIGNAL(finished()),this,SLOT(ThreadDone()));
    disconnect(downloader,0,this,0);
    // deleting
    downloader->deleteLater();
    fetch_thread->deleteLater();
    downloader = NULL;
    fetch_thread = NULL;
}


/*****************************************************************************/
