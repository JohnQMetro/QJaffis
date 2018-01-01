/*****************************************************************************
Name:        menu.cpp
Author :     John Q Metro
Purpose :    Defines custom popup menu classes
Created:     January 5, 2010
Conversion to Qt Started February 22, 2013
Updated:     July 12, 2017
******************************************************************************/
#ifndef MENU_H_INCLUDED
  #include "menu.h"
#endif // MENU_H_INCLUDED

#ifndef FICBASE_H
  #include "../../fanfic/data/ficbase.h"
#endif // FICBASE_H


#ifndef FFNDOWN_DIALOG_H_INCLUDED
  #include "../../ficdown/interface/ffndown_dialog.h"
#endif // FFNDOWN_DIALOG_H_INCLUDED
#ifndef FIM_GROUPOBJ_H_INCLUDED
  #include "../../fanfic/data/fim/fim_groupobj.h"
#endif // FIM_GROUPOBJ_H_INCLUDED
#ifndef HTML_DISPLAY_H_INCLUDED
  #include "../display/html_display.h"
#endif // HTML_DISPLAY_H_INCLUDED
#ifndef DEFCOLORS_H
    #include "../defcolors.h"
#endif // DEFCOLORS_H
//-----------------------------
#include <QDesktopServices>
#include <QUrl>

/*****************************************************************************/
jfPopupMenu::jfPopupMenu():QMenu() {
  sdata = NULL;
  load_action = addAction("Launch in Browser...",this,SLOT(OnLoadURL()));
  load_action->setEnabled(false);
  append_action = addAction("Append Description to Filter...",this,SLOT(OnAppendToExpressionFilter()));
  append_action->setEnabled(false);
}
//-----------------------------------
jfPopupMenu::~jfPopupMenu() {
    if (sdata!=NULL) delete sdata;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 // setting data
//----------------------------------------------------
void jfPopupMenu::SetPopupData(struct jfRClickData indata) {
    if (sdata != NULL) delete sdata;
    sdata = new jfRClickData;
    sdata->list_index = indata.list_index;
    sdata->item_clicked = indata.item_clicked;
    sdata->model_pointer = indata.model_pointer;
    sdata->row_index = indata.row_index;
    // enabling or disabling menu items
    load_action->setEnabled((sdata->item_clicked) != NULL);
    append_action->setEnabled((sdata->item_clicked) != NULL);
    EnableDisable();
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void jfPopupMenu::OnLoadURL() {
    if (xNN()) {
        QDesktopServices::openUrl(QUrl((sdata->item_clicked)->GetUrl(),QUrl::TolerantMode));
    }
}
//----------------------------------------------------
void jfPopupMenu::OnAppendToExpressionFilter() {
  QString desc;
  if (xNN()) {
    desc = GetSource()->GetDescription();
    emit sendDescription(desc);
  }
}
//+++++++++++++++++++++++++++++++++++++++++++++
bool jfPopupMenu::xNN() const {
    if (sdata==NULL) return false;
    else if ((sdata->item_clicked) == NULL) return false;
    else return true;
}
//--------------------------------------------
const jfBasePD* jfPopupMenu::GetSource() {
    if (sdata==NULL) return NULL;
    else if ((sdata->item_clicked)  == NULL) return NULL;
    else return (sdata->item_clicked)->Sourcelink();
}

//===========================================================================
/*
jfGooxPopupMenu::jfGooxPopupMenu(wxWindow* destination):jfPopupMenu(destination) {
  miExcludeUrl = new wxMenuItem(this,jfID_RMENU_EXLURL,"Add the url to the exclude list..."),wxT("Edit the url to add to the url filter",wxITEM_NORMAL);
  Append(miExcludeUrl);
  Connect(jfID_RMENU_EXLURL,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&jfGooxPopupMenu::OnExcludeUrl);
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void jfGooxPopupMenu::OnExcludeUrl(wxCommandEvent& event) {
    // local variables
  wxPoint* outpair;
  // creating the out-event
  wxCommandEvent outevent(jf_RMENU_EXLUDEURL_EVENT,-1);
  // setting up the index information
  outpair = new wxPoint(coll_index,item_index);
  outevent.SetClientData(outpair);
  // posting
  wxPostEvent(dtarget,outevent);
}
*/
//===================================================================
jfItemPopupMenuBase::jfItemPopupMenuBase():jfPopupMenu() {
    ignore_toggle = addAction("Ignore Item",this,SLOT(OnToggleIgnore()));
    ignore_toggle->setCheckable(true);
    ignore_toggle->setEnabled(false);
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void jfItemPopupMenuBase::OnToggleIgnore() {
    const QString fname = "jfItemPopupMenuBase::OnToggleIgnore";
    jfBasePD* group_pointer;
    if (xNN()) {
        /**/JDEBUGLOG(fname,1)
        bool xtest;
        group_pointer = const_cast<jfBasePD*>(GetSource());
        group_pointer->ignore = !(group_pointer->ignore);
         /**/JDEBUGLOGB(fname,2,group_pointer->ignore)
        /**/JDEBUGLOGB(fname,3,defcolour::ignored.isValid());
        if (group_pointer->ignore) sdata->item_clicked->bgcolor = defcolour::ignored;
        else sdata->item_clicked->bgcolor = defcolour::base;
        // updating the display...
        xtest = (sdata->model_pointer)->SignalRowChanged(sdata->row_index);
         /**/JDEBUGLOGB(fname,4,xtest)
    }
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void jfItemPopupMenuBase::EnableDisable() {
    if (xNN()) {
        ignore_toggle->setEnabled(true);
        ignore_toggle->blockSignals(true);
        if (GetSource()->ignore) ignore_toggle->setChecked(true);
        else ignore_toggle->setChecked(false);
        ignore_toggle->blockSignals(false);
    }
    else ignore_toggle->setEnabled(false);
    EnableDisableMore();
}
//===================================================================

jfFanficPopupMenu::jfFanficPopupMenu():jfItemPopupMenuBase() {
  down_fanfic = addAction("Download Fanfic...",this,SLOT(OnDownloadFic()));
  down_fanfic->setEnabled(false);
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void jfFanficPopupMenu::OnDownloadFic() {
  // local variables
  const jfGenericFanfic* gfic;
  jfFicExtract* gficdata;
  jfFicDownDialog* mdialog;
  if (xNN()) {
    // we start by getting the fic and then the extract
    gfic = dynamic_cast<const  jfGenericFanfic*>(GetSource());
    gficdata = gfic->GetExtract();
    // prepare a dialog that is used to download the fic
    mdialog = new jfFicDownDialog(NULL,gficdata);
    mdialog->show();
  }
}
//----------------------------------------------------------
void jfFanficPopupMenu::EnableDisableMore() {
    if (xNN()) down_fanfic->setEnabled(true);
    else down_fanfic->setEnabled(false);
}
//===================================================================
jfFIMGPopupMenu::jfFIMGPopupMenu():jfItemPopupMenuBase() {

}
//++++++++++++++++++++++++++++++++++++++++++++++++++++
void jfFIMGPopupMenu::EnableDisableMore() {}

/*****************************************************************************/
