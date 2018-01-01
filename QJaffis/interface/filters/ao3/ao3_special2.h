/******************************************************************************
Name    :   ao3_special2.h
Author  :   John Q Metro
Purpose :   Declares some interface panels for some AO3 Filters
Created :   October 6, 2012
Conversion to Qt Started October 19, 2013
Updated :   October 10 2012
******************************************************************************/
#ifndef AO3_SPECIAL2_H_INCLUDED
#define AO3_SPECIAL2_H_INCLUDED
#endif // AO3_SPECIAL2_H_INCLUDED
//---------------------------------
#ifndef AO3_SPECIALS2_H_INCLUDED
  #include "../../../core/filters/ao3/ao3_specials2.h"
#endif // AO3_SPECIALS2_H_INCLUDED
#ifndef WIDGETS3_H_INCLUDED
  #include "../../misc/widgets3.h"
#endif // WIDGETS3_H_INCLUDED
#ifndef BFILT_EDIT_H_INCLUDED
  #include "../base/bfilt_edit.h"
#endif // BFILT_EDIT_H_INCLUDED
#ifndef CHARO_EDITOR_H_INCLUDED
  #include "../../misc/charo_editor.h"
#endif // CHARO_EDITOR_H_INCLUDED
/*****************************************************************************/
class jfAO3PairingPanel : public QWidget {
  public:
    // constructor
    jfAO3PairingPanel(bool orient, QWidget* parent = NULL);
    // i/o
    bool SetFromString(const QString& inval);
    void SetAlternateCheck(bool cval);
    QString GetStringValue() const;
    bool GetAlternateCheck() const;
    // filter based i/o
    jfAO3PairFilter* GetFilter() const;
    bool StoreToFilter(jfAO3PairFilter* outval);
    bool LoadFromFilter(const jfAO3PairFilter* inval);
  protected:
    // GUI data
    QCheckBox* altbox;
    jfLabeledEdit* pairlistbox;
    QVBoxLayout* bsizer;
    // non gui data
    size_t omode;
};
//=============================================================
// ao3 ratings filter editor
class jfAO3_PairingFilterEditor : public jfBaseFilterEditor {
  public:
    // the default constructor
    jfAO3_PairingFilterEditor(const jfBaseFilter* infilt, const jfFilterMap* infmap, QWidget* parent = NULL);
    // implemented virtual methods
    virtual void LoadFilter(const jfBaseFilter* infilter);
    virtual jfBaseFilter* GetFilter();
    virtual bool GeneralCheck() const;
  protected:
    jfAO3PairingPanel* insert_panel;
};
//====================================================================
// ao3 ratings filter editor
class jfAO3_WarnFilterEditor : public jfBaseFilterEditor {
  public:
    // the default constructor
    jfAO3_WarnFilterEditor(const jfBaseFilter* infilt, const jfFilterMap* infmap, QWidget* parent = NULL);
    // implemented virtual methods
    virtual void LoadFilter(const jfBaseFilter* infilter);
    virtual jfBaseFilter* GetFilter();
    virtual bool GeneralCheck() const;
  protected:
    jfCharCheckBoxGroup* insert_panel;
};
/*****************************************************************************/
