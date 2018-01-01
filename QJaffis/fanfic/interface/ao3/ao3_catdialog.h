/******************************************************************************
Name    :   ao3_catdialog.h
Author  :   John Q Metro
Purpose :   Dialog for AO3 Catalog Downloading
Created :   August 25, 2012
Conversion to Qt Started March 15, 2014
Updated :   July 15, 2016
******************************************************************************/
#ifndef AO3_CATDIALOG_H_INCLUDED
  #define AO3_CATDIALOG_H_INCLUDED
#endif // AO3_CATDIALOG_H_INCLUDED
//------------------------------------------
#ifndef GCAT_DIALOG_H_INCLUDED
  #include "../gcat_dialog.h"
#endif // GCAT_DIALOG_H_INCLUDED
//*******************************************************************************
class jfAO3Cat_DownDialog : public jfGenCat_DownDialog {
  public:
    // the constructor
    jfAO3Cat_DownDialog(QWidget* parent);
  protected:
    // virtual methods
    virtual jfGeneralCategoryDownloaderBase* MakeTypedThread() const;
};
//*******************************************************************************
