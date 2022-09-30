/*****************************************************************************
Name:        result_panel.h
Author :     John Q Metro
Purpose :    Declares a panel for displaying search results
Created:     July 26, 2009
Conversion to Qt Started November 20, 2013
Updated:     April 30, 2022: changing downloader class
******************************************************************************/
#ifndef RESULT_PANEL_H_INCLUDED
#define RESULT_PANEL_H_INCLUDED
#endif // RESULT_PANEL_H_INCLUDED
//-----------------------------------------------------------------------------
#ifndef TYPED_DISPLAY_H_INCLUDED
  #include "typed_display.h"
#endif // TYPED_DISPLAY_H_INCLUDED
#ifndef CATEGORIES_H_INCLUDED
  #include "../../core/structs/categories.h"
#endif // CATEGORIES_H_INCLUDED
#ifndef VECTORS_H_INCLUDED
  #include "../../core/vectors.h"
#endif // VECTORS_H_INCLUDED
#ifndef SEARCHCORE_H_INCLUDED
  #include "../../core/objs/searchcore.h"
#endif // SEARCHCORE_H_INCLUDED
#ifndef PROGRESS_PANEL_H
  #include "../../fetching/panel/progress_panel.h"
#endif // PROGRESS_PANEL_H
#ifndef RESULTCOLLECT_H_INCLUDED
  #include "../../core/structs/resultcollect.h"
#endif // RESULTCOLLECT_H_INCLUDED
#ifndef MULTICATROOT_H
    #include "../../fetching/loopget/multicatroot.h"
#endif // MULTICATROOT_H

//------------------------------------------------------------------------------
#include <QThread>
//*****************************************************************************
enum rtype_enum { rtype_BASIC,rtype_FFN,rtype_MCNU };
//===========================================================================
/* here, we group a Html result collection with a progress panel to have something that
shows results in general */
class jfResultDisplay: public QWidget {
    Q_OBJECT
  public:
    // constructor
    jfResultDisplay(rtype_enum rinval, header_enum hinval, QWidget* parent = NULL);
    // when we launch the search, some information has to be used to setup things
    bool LaunchParameters(jfResultCollection* results, jfDownloadRootItems* in_downloader);
    // functions to return info that has to be passed to the search thread
    jfPauseStop* GetPauser();
    // additional data to set
    void SetPopupMenu(jfPopupMenu* inmenu);
    // we call this when we want to clear old results
    void ClearOld();
		// this is generally called when a search is aborted
		void SearchAbort();
    // some status and clearing methods
    bool InProgress() const;
    size_t SelectedIndex() const;
    bool CloseDownload();
  signals:
    void DisplaySendStart();
    void DisplaySendDone();
  public slots:
    void HandleStart();
    void HandleDone(bool);
    void ThreadDone();
  private:
    // helper methods for lauch parameters
    bool SetupValues(jfResultCollection* results, jfDownloadRootItems* in_downloader);
    bool ConnectSignalsAndSlots();
    // gui elements
    jfHtmlResultCollection* mainviewer;
    jfProgressPanelBase* progpanel;
    // sizers
    QVBoxLayout* topsizer;
    // internal data
    bool started, multicat;
    header_enum hype;
    jfPopupMenu* p_menu;
    // thread data and control
    jfSearchCore* search_data;
    jfResultCollection* data_collection;
    jfMultiCatRootDownloader* download_ptr_multi;
    jfDownloadRootItems* download_ptr_single;
    QThread* thread_object;

};
//*****************************************************************************
