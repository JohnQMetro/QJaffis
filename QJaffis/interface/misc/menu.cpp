/*****************************************************************************
Name:        menu.cpp
Author :     John Q Metro
Purpose :    Defines custom popup menu classes
Created:     January 5, 2010
Conversion to Qt Started February 22, 2013
Updated:     March 25, 2023
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
    desc = GetSource()->SourceLink()->GetSummary();
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
jfResultUnit* jfPopupMenu::GetSource() const {
    if (sdata==NULL) return NULL;
    else return sdata->item_clicked;
}
//===================================================================
jfItemPopupMenuBase::jfItemPopupMenuBase():jfPopupMenu() {
    ignore_toggle = addAction("Ignore Item",this,SLOT(OnToggleIgnore()));
    ignore_toggle->setCheckable(true);
    ignore_toggle->setEnabled(false);
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void jfItemPopupMenuBase::OnToggleIgnore() {
    const QString fname = "jfItemPopupMenuBase::OnToggleIgnore";
    jfResultUnit* group_pointer = GetSource();
    if (xNN() && group_pointer->HasFlags() ) {
        /**/JDEBUGLOG(fname,1)
        group_pointer = GetSource();
        jfItemMetaFlags* mflags = group_pointer->Flags();

        /**/JDEBUGLOGB(fname, 2, mflags->ignore)
        /**/JDEBUGLOGB(fname, 3, defcolour::ignored.isValid());

        if (mflags->ignore) mflags->background_color = defcolour::ignored;
        else mflags->background_color = defcolour::base;
        // updating the display...
        bool xtest = (sdata->model_pointer)->SignalRowChanged(sdata->row_index);
        /**/JDEBUGLOGB(fname,4,xtest)
    }
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void jfItemPopupMenuBase::EnableDisable() {
    if (xNN()) {
        ignore_toggle->blockSignals(true);
        if (GetSource()->HasFlags()) {
            ignore_toggle->setEnabled(true);
            bool is_ignored = GetSource()->Flags()->ignore;
            ignore_toggle->setChecked(is_ignored);
        }
        else {
            ignore_toggle->setChecked(false);
            ignore_toggle->setEnabled(false);
        }
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
