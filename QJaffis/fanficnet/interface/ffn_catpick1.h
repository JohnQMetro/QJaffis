/////////////////////////////////////////////////////////////////////////////
// Name:        ffn_catpick1.h
// Author :     John Q Metro
// Purpose :    Fanfiction.Net catgeory picker
// Created:     May 16, 2010
// Conversion to Qt started July 15, 2014
// Updated:     July 26, 2014
/////////////////////////////////////////////////////////////////////////////
#ifndef FFNCATPICK
  #define FFNCATPICK
#endif
//-----------------------------------------
#ifndef WIDGETS1_H_INCLUDED
  #include "../../interface/controls/widgets1.h"
#endif // WIDGETS1_H_INCLUDED
#ifndef FFNCATPDATA
  #include "../categories/ffn_catpdata.h"
#endif
#ifndef CATGROUP_H
  #include "../categories/catgroup.h"
#endif // CATGROUP_H
//**************************************************************************
// base twin check listbox category picker
class jfFFN_CoreTwinCheckboxPicker : public QWidget {
    Q_OBJECT
  public:
    // the constructor
    jfFFN_CoreTwinCheckboxPicker(const jfFFN_Categories* in_mcatdata, jfFFN_CatP_SelCat* inselector, size_t prop, QWidget* parent = NULL);
    // special methods
    virtual bool ChangeSelector(jfFFN_CatP_SelCat* newselection) = 0;
  signals:
    void sendCheckSig(bool value);
  public slots:
    // events
    virtual void HanFirstChecked(size_t which, Qt::CheckState what) = 0;
    void HanCategoryChecked(size_t which, Qt::CheckState what);
    virtual void HanFirstChanged(int currentRow) = 0;
    void HanCategoryChanged(int currentRow);
  protected:
    virtual void CoreCatCheck(QString firsts, QString cats, bool dcheck) = 0;
    // the gui elements
    QHBoxLayout* top_sizer;
    jfCheckList* first_list;
    jfCheckList* cat_list;
    // external data
    jfFFN_CatP_SelCat* selection_data;
    const jfFFN_Categories* mcatdata;
};
//===============================================================================
// section and category picker
class jfFFN_SectionPicker : public jfFFN_CoreTwinCheckboxPicker {
    Q_OBJECT
  public:
    // the constructor
    jfFFN_SectionPicker(const jfFFN_Categories* in_mcatdata, jfFFN_CatP_SelCat* inselector, QWidget* parent = NULL);
    // special methods
    virtual bool ChangeSelector(jfFFN_CatP_SelCat* newselection);
    bool UncheckAndRemove(QString xsection, QString xcategory);
  public slots:
    // events
    virtual void HanFirstChecked(size_t which, Qt::CheckState what);
    virtual void HanFirstChanged(int currentRow);
  protected:
    virtual void CoreCatCheck(QString firsts, QString cats, bool dcheck);
};
//===============================================================================
// crossover category and crossover group picker
class jfFFN_CrossGroupPicker : public jfFFN_CoreTwinCheckboxPicker {
    Q_OBJECT
  public:
    // the constructor
    jfFFN_CrossGroupPicker(const jfFFN_Categories* in_mcatdata, jfFFN_CatP_SelCat* inselector, QWidget* parent = NULL);
    // special methods
    virtual bool ChangeSelector(jfFFN_CatP_SelCat* newselection);
    // extra methods
    bool SetSection(size_t sec_index, bool ischecked);
    bool EverythingChecked(size_t sec_index, bool qcheck);
  public slots:
    // events
    virtual void HanFirstChecked(size_t which, Qt::CheckState what);
    virtual void HanFirstChanged(int currentRow);
  protected:
    virtual void CoreCatCheck(QString firsts, QString cats, bool dcheck);
    void LoadSection();
    void ApplyChecks();
    // internal data
    int section_index;
    bool wnew;
};

//==================================================================================
// Top-level crossover picker, adds an extra checklist box
class jfFFN_CrossoverPicker : public QWidget {
      Q_OBJECT
  public:
    // constructor
    jfFFN_CrossoverPicker(const jfFFN_Categories* in_mcatdata, jfFFN_CatP_SelCat* inselector, QWidget* parent = NULL);
    // special methods
    bool ChangeSelector(jfFFN_CatP_SelCat* newselection);
    bool UncheckAndRemove(QString xsection, QString xgroup, QString xcategory);
  signals:
    void CheckSignal(bool value);
  public slots:
    void HandleCheckSig(bool svalue);
    void HandleTopCheck(size_t which, Qt::CheckState what);
    void HandleTopChanged(int currentRow);
  protected:
    // GUI elements
    jfCheckList* topclist;
    jfFFN_CrossGroupPicker* sub_lists;
    QHBoxLayout* tlayout;
    // external data
    jfFFN_CatP_SelCat* selection_data;
    const jfFFN_Categories* mcatdata;

};


//***************************************************************************

