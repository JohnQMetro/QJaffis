//***************************************************************************
// Name:        pairf_editor.h
// Author :     John Q Metro
// Purpose :    Relationship/Pairing filters editing, various panels.
// Created:     January 1, 2011
// Conversion to Qt Started October 16, 2013
// Updated:     January 2, 2011
//***************************************************************************
#ifndef PAIRF_EDITOR_H_INCLUDED
#define PAIRF_EDITOR_H_INCLUDED
#endif // PAIRF_EDITOR_H_INCLUDED
//----------------------------------------------------------------------------
#ifndef PAIRFILTER_H_INCLUDED
  #include "../../../core/filters/extended/pairfilter.h"
#endif // PAIRFILTER_H_INCLUDED
//-------------------------------------------
#include <QWidget>
#include <QLabel>
#include <QCheckBox>
#include <QLineEdit>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QPlainTextEdit>
//***************************************************************************
// pair filter editing, the core panel
class jfPairFilEditPanelCore : public QWidget {
  public:
    // the constructor
    jfPairFilEditPanelCore(QWidget* parent = NULL);
    // virtual load and store stuff
    virtual bool LoadFilterData(const jfPairFilterCore* inval) = 0;
    virtual bool SaveFilterData(jfPairFilterCore* outval) const = 0;
    // checking function
    bool CheckValid() const;
  protected:
    // loading data
    bool LoadData(const jfPairFilterCore* inval);
    // getting data
    bool SaveData(jfPairFilterCore* outval) const;
    // other
    bool CheckSeps() const;
    virtual bool CheckNames() const = 0;
    // gui elements
    QLabel* spec_label;
    QLineEdit* spec_edit;
    QLabel* delim_label;
    QLineEdit* delim_edit;
    QCheckBox* usetokens;
    QCheckBox* sidespaces;
    QCheckBox* nofriend;
    QCheckBox* nospace;
    // sizers
    QHBoxLayout* delim_sizer;
    QGridLayout* cb_sizer;
    QVBoxLayout* top_sizer;

};
//==========================================================================
// single pair filter edit panel
class jfPairFilEditPanelSingle : public jfPairFilEditPanelCore {
  public:
    // constructor
    jfPairFilEditPanelSingle(const jfPairFilterSingle* infil, QWidget* parent = NULL);
    // virtual load and store stuff
    virtual bool LoadFilterData(const jfPairFilterCore* inval);
    virtual bool SaveFilterData(jfPairFilterCore* outval) const;
    // checking function
  protected:
    virtual bool CheckNames() const;
    // gui elements
    QLabel* name1_label;
    QLineEdit* name1_edit;
    QLabel* name2_label;
    QLineEdit* name2_edit;
};
//==========================================================================
// list pair filter edit panel
class jfPairFilEditPanelList : public jfPairFilEditPanelCore {
  public:
    // constructor
    jfPairFilEditPanelList(const jfPairFilterList* infil, QWidget* parent = NULL);
    // virtual load and store stuff
    virtual bool LoadFilterData(const jfPairFilterCore* inval);
    virtual bool SaveFilterData(jfPairFilterCore* outval) const;
  protected:
    virtual bool CheckNames() const;
    // gui elements
    QLabel* name1_label;
    QLineEdit* name1_edit;
    QLabel* namelist_label;
    QPlainTextEdit* namelist_edit;
};
//==========================================================================
// multiple pair filter edit panel
class jfPairFilEditPanelMulti : public jfPairFilEditPanelCore {
  public:
    // constructor
    jfPairFilEditPanelMulti(const jfPairFilterMultiple* infil, QWidget* parent = NULL);
    // virtual load and store stuff
    virtual bool LoadFilterData(const jfPairFilterCore* inval);
    virtual bool SaveFilterData(jfPairFilterCore* outval) const;
  protected:
    virtual bool CheckNames() const;
    // gui elements
    QLabel* name_label;
    QPlainTextEdit* names_edit;
};
//***************************************************************************
