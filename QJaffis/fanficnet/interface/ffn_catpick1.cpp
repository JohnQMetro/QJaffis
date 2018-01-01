/////////////////////////////////////////////////////////////////////////////
// Name:        ffn_catpick1.cpp
// Author :     John Q Metro
// Purpose :    Fanfiction.Net catgeory picker
// Created:     May 16, 2010
// Conversion to Qt started July 16, 2014
// Updated:     August 14, 2014
/////////////////////////////////////////////////////////////////////////////
#ifndef FFNCATPICK
  #include "ffn_catpick1.h"
#endif
//------------------------------
#ifndef LOGGING_H_INCLUDED
  #include "../../core/utils/logging.h"
#endif // LOGGING_H_INCLUDED
//------------------------------
#include <assert.h>


//**************************************************************************
// the constructor
jfFFN_CoreTwinCheckboxPicker::jfFFN_CoreTwinCheckboxPicker(const jfFFN_Categories* in_mcatdata,
     jfFFN_CatP_SelCat* inselector, size_t prop, QWidget* parent):QWidget(parent) {
  const QString fname = "jfFFN_CoreTwinCheckboxPicker::jfFFN_CoreTwinCheckboxPicker";
  // checking the inputs
  /**/JDEBUGLOG(fname,0);
  assert(in_mcatdata!=NULL);
  assert(prop>=1);
  mcatdata = in_mcatdata;
  selection_data = inselector;
  // creating the widgets
  first_list = new jfCheckList(false);
  cat_list = new jfCheckList(false);
  // layout creation
  top_sizer = new QHBoxLayout();
  top_sizer->addWidget(first_list,1);
  top_sizer->addWidget(cat_list,prop);
  setLayout(top_sizer);
  // connecting
  connect(first_list,SIGNAL(sendChangeCheck(size_t, Qt::CheckState)),this,SLOT(HanFirstChecked(size_t,Qt::CheckState)));
  connect(cat_list,SIGNAL(sendChangeCheck(size_t, Qt::CheckState)),this,SLOT(HanCategoryChecked(size_t,Qt::CheckState)));
  connect(first_list,SIGNAL(currentRowChanged(int)),this,SLOT(HanFirstChanged(int)));
  connect(cat_list,SIGNAL(currentRowChanged(int)),this,SLOT(HanCategoryChanged(int)));
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void jfFFN_CoreTwinCheckboxPicker::HanCategoryChecked(size_t which, Qt::CheckState what) {
  const QString fname = "jfFFN_CoreTwinCheckboxPicker::HanCategoryChecked";
  // some core checks
  assert(selection_data!=NULL);
  assert(what!=Qt::PartiallyChecked);
  // variables
  QString string1, string2;
  bool check_d;
  // setting the variables
  check_d = (what==Qt::Checked);
  string1 = first_list->currentItem()->text();
  /**/JDEBUGLOGS(fname,4,string1);
  string2 = cat_list->item(which)->text();
  /**/JDEBUGLOGS(fname,5,string2);
  // doing it
  CoreCatCheck(string1,string2,check_d);
  /**/JDEBUGLOG(fname,6);
  emit sendCheckSig(check_d);
  /**/JDEBUGLOG(fname,7);
}
//---------------------------------------------------------
void jfFFN_CoreTwinCheckboxPicker::HanCategoryChanged(int currentRow) {}

//*********************************************************************************************************
// constructor
jfFFN_SectionPicker::jfFFN_SectionPicker(const jfFFN_Categories* in_mcatdata, jfFFN_CatP_SelCat* inselector,
         QWidget* parent):jfFFN_CoreTwinCheckboxPicker(in_mcatdata,inselector,2,parent) {
  const QString fname = "jfFFN_SectionPicker::jfFFN_SectionPicker";
  // loading items into the check list boxes
  /**/JDEBUGLOG(fname,0);
  first_list->appendMultiCheckItems(jfFFN_SECDATA::section_names,jfFFN_SECDATA::scount);
  first_list->setSelectionNS(0);
  QStringList* xcat_list = mcatdata->GetCategories(0);
  /**/JDEBUGLOG(fname,2);
  cat_list->appendMultiCheckItems(*xcat_list);
  delete xcat_list;
  cat_list->setSelectionNS(0);
  /**/JDEBUGLOG(fname,3);
  // finally
  if (selection_data!=NULL) ChangeSelector(selection_data);
  /**/JDEBUGLOG(fname,4);
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// special methods
bool jfFFN_SectionPicker::ChangeSelector(jfFFN_CatP_SelCat* newselection) {
  const QString fname = "jfFFN_SectionPicker::ChangeSelector";
  // variables
  size_t selindex;
  int sval;
  QStringList* slist;
  // starting
  if (newselection==NULL) return false;
  else {
    sval = first_list->currentRow();
    if (sval<0) {
      first_list->setSelectionNS(0);
      sval = 0;
    }
    selection_data = newselection;
    cat_list->CheckAllDisable(false);
    // the loop
    for (selindex = 0 ; selindex<jfFFN_SECDATA::scount ; selindex++) {
      first_list->SetCheckNS(selindex,Qt::Unchecked);
      slist = selection_data->FindSection(jfFFN_SECDATA::section_names[selindex]);
      if (slist!=NULL) {
        if ((slist->size())==0) {
          first_list->SetCheckNS(selindex, Qt::Checked);
        }
      }
      if (sval==selindex) {
        if (slist!=NULL) {
          if ((slist->size())==0) cat_list->CheckAllDisable(true);
          else cat_list->CheckNames(slist);
        }
      }
      delete slist;
    }
  }
  return true;
}
//--------------------------------------------------------
bool jfFFN_SectionPicker::UncheckAndRemove(QString xsection, QString xcategory) {
  const QString fname = "jfFFN_SectionPicker::UncheckAndRemove";
  // variables
  bool all_section, rem_selected;
  QString sel_string;
  bool rbvalue;
  int rvalue;
  QStringList* xcat_list;
  // doing some checks
  /**/JDEBUGLOGS(fname,1,xsection);
  /**/JDEBUGLOGS(fname,2,xcategory);
  all_section = (xcategory=="*");
  if (first_list->currentRow()<0) rem_selected = false;
  else {
    sel_string = first_list->currentItem()->text();
    /**/JDEBUGLOGS(fname,3,sel_string);
    rem_selected = (sel_string==xsection);
  }
  /**/JDEBUGLOG(fname,4);
  // removing and unchecking based upon the boolean values
  if (all_section) {
    // removing and unchecking
    /**/JDEBUGLOG(fname,5);
    rbvalue = selection_data->RemoveSection(xsection);
    /**/JDEBUGLOGB(fname,6,rbvalue);
    if (!rbvalue) return false;
    rvalue = first_list->CheckByName(xsection,Qt::Unchecked);
    /**/JDEBUGLOG(fname,7);
    if (rvalue<0) return false;
    // if this affects the selected section, we have to uncheck and re-do the category list
    if (rem_selected) {
      /**/JDEBUGLOG(fname,8);
      cat_list->CheckAllDisable(false);
      xcat_list = selection_data->FindSection(xsection);
      /**/JDEBUGLOGB(fname,9,xcat_list==NULL);
      if (xcat_list!=NULL) {
        cat_list->CheckNames(xcat_list);
        delete xcat_list;
      }
      /**/JDEBUGLOG(fname,10);
    }
    /**/JDEBUGLOG(fname,11);
  }
  // not all section
  else {
    /**/JDEBUGLOG(fname,2);
    rbvalue = selection_data->RemoveCategory(xsection,xcategory);
    /**/JDEBUGLOGB(fname,13,rbvalue);
    if (!rbvalue) return false;
    /**/JDEBUGLOG(fname,14);
    if (rem_selected) cat_list->CheckByName(xcategory,Qt::Unchecked);
    // otherwise, we do nothing
  }
  /**/JDEBUGLOG(fname,15);
  // done
  return true;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// implemented slots
//-------------------------------
void jfFFN_SectionPicker::HanFirstChecked(size_t which, Qt::CheckState what) {
  const QString fname = "jfFFN_SectionPicker::HanFirstChecked";
  QString csec_name;
  bool dcheck;
  QStringList* xcat_list;
  // some quick asserts
  /**/JDEBUGLOG(fname,1);
  assert(what!=Qt::PartiallyChecked);
  assert(which<(first_list->count()));
  /**/JDEBUGLOG(fname,2);
  // assigning some values
  dcheck = (what==Qt::Checked);
  csec_name = jfFFN_SECDATA::section_names[which];
  /**/JDEBUGLOGS(fname,3,csec_name);
  if (dcheck) selection_data->AddSection(csec_name, false);
  else selection_data->RemoveSection(csec_name);
  // updating the display
  if (which==(first_list->currentRow())) {
    /**/JDEBUGLOG(fname,4);
    if (!dcheck) {
      cat_list->CheckAllDisable(false);
      xcat_list = selection_data->FindSection(csec_name);
      /**/JDEBUGLOG(fname,5);
      cat_list->CheckNames(xcat_list);
      delete xcat_list;
    }
    else cat_list->CheckAllDisable(true);
  }
  /**/JDEBUGLOG(fname,8);
  emit sendCheckSig(dcheck);
}

//-------------------------------
void jfFFN_SectionPicker::HanFirstChanged(int currentRow) {
  const QString fname = "jfFFN_SectionPicker::HanFirstChanged";
  if (currentRow>=0) {
    /**/JDEBUGLOG(fname,1);
    QStringList* xcat_list = mcatdata->GetCategories(currentRow);
    assert(xcat_list!=NULL);
    /**/JDEBUGLOG(fname,2);
    cat_list->replaceItems(*xcat_list);
    delete xcat_list;
    /**/JDEBUGLOG(fname,3);
    xcat_list = selection_data->FindSection(first_list->currentItem()->text());
    /**/JDEBUGLOGB(fname,4,xcat_list==NULL);
    if (xcat_list!=NULL) {
      /**/JDEBUGLOG(fname,4);
      if (xcat_list->size()==0) cat_list->CheckAllDisable(true);
      else cat_list->CheckNames(xcat_list);
      /**/JDEBUGLOG(fname,5);
      delete xcat_list;
    }
  }
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void jfFFN_SectionPicker::CoreCatCheck(QString firsts, QString cats, bool dcheck) {
  const QString fname = "jfFFN_SectionPicker::CoreCatCheck";
  /**/JDEBUGLOGS(fname,1,firsts)
  /**/JDEBUGLOGS(fname,2,cats)
  if (dcheck) selection_data->AddCategory(firsts,cats,false);
  else selection_data->RemoveCategory(firsts,cats);
}
//======================================================================================
jfFFN_CrossGroupPicker::jfFFN_CrossGroupPicker(const jfFFN_Categories* in_mcatdata,
     jfFFN_CatP_SelCat* inselector, QWidget* parent):jfFFN_CoreTwinCheckboxPicker(
                                       in_mcatdata,inselector,1,parent) {
  // loading top level section 0
  section_index = 0;
  wnew = true;
  LoadSection();
  // finally
  if (selection_data!=NULL) ChangeSelector(selection_data);
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// special methods
bool jfFFN_CrossGroupPicker::ChangeSelector(jfFFN_CatP_SelCat* newselection) {
  if (newselection==NULL) return false;
  // clearing old
  if (!wnew) {
    cat_list->CheckAllDisable(false);
    first_list->CheckAllDisable(false);
  }
  // changing
  selection_data = newselection;
  ApplyChecks();
  wnew = false;
  return true;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// extra methods
//--------------------
bool jfFFN_CrossGroupPicker::SetSection(size_t sec_index, bool ischecked) {
  const QString fname = "jfFFN_CrossGroupPicker::SetSection";
  // false cases
  /**/JDEBUGLOG(fname,1);
  if (sec_index>=jfFFN_SECDATA::scount) return false;
  /**/JDEBUGLOG(fname,2);
  if (section_index==sec_index) return false;
  /**/JDEBUGLOG(fname,3);
  // general case
  section_index = sec_index;
  /**/JDEBUGLOG(fname,4);
  LoadSection();
  /**/JDEBUGLOG(fname,5);
  if (ischecked) EverythingChecked(section_index,true);
  else ApplyChecks();
  /**/JDEBUGLOG(fname,6);
  return true;
}

//--------------------
bool jfFFN_CrossGroupPicker::EverythingChecked(size_t sec_index, bool qcheck) {
  // constants and variables
  const QString fname = "jfFFN_CrossGroupPicker::EverythingChecked";
  jfStrBoolList* xcat_list;
  QString curr_name, tsec_name;
  /**/JDEBUGLOG(fname,1);
  if (sec_index>jfFFN_SECDATA::scount) return false;
  /**/JDEBUGLOG(fname,2);
  if (sec_index==section_index) {
    /**/JDEBUGLOG(fname,3);
    if (qcheck) {
      cat_list->CheckAllDisable(true);
      /**/JDEBUGLOG(fname,4);
      first_list->CheckAllDisable(true);
    }
    else {
      // clear general checks
      cat_list->CheckAllDisable(false);
      /**/JDEBUGLOG(fname,5);
      first_list->CheckAllDisable(false);
      // reload more specific checks
      ApplyChecks();
      /**/JDEBUGLOG(fname,6);
    }
    /**/JDEBUGLOG(fname,7);
    return true;
  }
  // we need to be more selective...
  else {
    if (first_list->CurrNotChecked()) {
      tsec_name = jfFFN_SECDATA::section_names[section_index];
      curr_name = first_list->currentItem()->text();
      xcat_list = selection_data->FindSectionCross(tsec_name,curr_name);
      cat_list->CheckAllDisable(false);
      cat_list->CheckAndDisableNames(xcat_list);
      delete xcat_list;
    }
    return true;
  }
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// events
//--------------------
void jfFFN_CrossGroupPicker::HanFirstChecked(size_t which, Qt::CheckState what) {
  const QString fname = "jfFFN_CrossGroupPicker::HanFirstChecked";
  QString csec_name, tsec_name, curr_name;
  bool dcheck;
  jfStrBoolList* xcat_list;
  // some quick asserts
  assert(what!=Qt::PartiallyChecked);
  assert(which<(first_list->count()));
  // assigning some values
  dcheck = (what==Qt::Checked);
  csec_name = first_list->item(which)->text();
  curr_name = first_list->currentItem()->text();
  tsec_name = jfFFN_SECDATA::section_names[section_index];
  // adding the info to the selector object
  if (dcheck) selection_data->AddCrossoverGroup(tsec_name,csec_name,false);
  else selection_data->RemoveCrossoverGroup(tsec_name,csec_name);
  // -- updating the display --
  if (which==(first_list->currentRow())) {
    // unchecking the current section
    if (!dcheck) {
      cat_list->CheckAllDisable(false);
      xcat_list = selection_data->FindSectionCross(tsec_name,csec_name);
      cat_list->CheckAndDisableNames(xcat_list);
      delete xcat_list;
    }
    // checking the current section
    else cat_list->CheckAllDisable(true);
  }
  // some other section was changed
  else if (!first_list->GetBoolState(first_list->currentRow())) {
    cat_list->CheckAllDisable(false);
    xcat_list = selection_data->FindSectionCross(tsec_name,curr_name);
    cat_list->CheckAndDisableNames(xcat_list);
    delete xcat_list;
  }
  emit sendCheckSig(dcheck);
}

//--------------------
void jfFFN_CrossGroupPicker::HanFirstChanged(int currentRow) {
  // variables
  const QString fname = "jfFFN_CrossGroupPicker::HanFirstChanged";
  QStringList* xcat_list;
  QString cg_name;
  jfStrBoolList* bdis_list;
  // changing
  /**/JDEBUGLOG(fname,1);
  if (currentRow>=0) {
    // loading category names
    /**/JDEBUGLOG(fname,2);
    cg_name = first_list->currentItem()->text();
    /**/JDEBUGLOGS(fname,5,cg_name)
    xcat_list = mcatdata->GetCrossoverCategories(section_index,cg_name);
    cat_list->replaceItems(*xcat_list);
    /**/JDEBUGLOG(fname,4);
    delete xcat_list;
    // checking them
    bdis_list = selection_data->FindSectionCross(jfFFN_SECDATA::section_names[section_index],cg_name);
    /**/JDEBUGLOG(fname,5);
    if (bdis_list!=NULL) {
      /**/JDEBUGLOG(fname,6);
      if (bdis_list->size()==0) cat_list->CheckAllDisable(true);
      else cat_list->CheckAndDisableNames(bdis_list);
      delete bdis_list;
    }
    /**/JDEBUGLOG(fname,7);
  }
  /**/JDEBUGLOG(fname,8);
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void jfFFN_CrossGroupPicker::CoreCatCheck(QString firsts, QString cats, bool dcheck) {
  QString topname = jfFFN_SECDATA::section_names[section_index];
  if (dcheck) selection_data->AddCrossoverCategory(topname,firsts,cats,false);
  else selection_data->RemoveCrossoverCategory(topname,firsts,cats);
}
//-------------------------------------------------------------------
void jfFFN_CrossGroupPicker::LoadSection() {
  const QString fname = "jfFFN_CrossGroupPicker::LoadSection";
  QStringList* xcat_list;
  QString cg_name;
  // loading the first box
  /**/JDEBUGLOG(fname,1);
  xcat_list = mcatdata->GetCrossSectionList(section_index);
  assert(xcat_list!=NULL);
  first_list->replaceItems(*xcat_list);
  first_list->setSelectionNS(0);
  /**/JDEBUGLOG(fname,2);
  delete xcat_list;
  // loading the individual crossover cat list
  cg_name = first_list->currentItem()->text();
  xcat_list = mcatdata->GetCrossoverCategories(section_index,cg_name);
  /**/JDEBUGLOG(fname,3);
  assert(xcat_list!=NULL);
  cat_list->replaceItems(*xcat_list);
  cat_list->setSelectionNS(0);
  /**/JDEBUGLOG(fname,4);
  delete xcat_list;
}
//------------------------------------------------------------------
void jfFFN_CrossGroupPicker::ApplyChecks() {
  // variables
  const QString fname = "jfFFN_CrossGroupPicker::ApplyChecks";
  int cgindex;
  QStringList* cname_list;
  QString tsec_name, cg_name;
  jfStrBoolList* xcat_list;
  // getting the list of checked stuff for first_list
  /**/JDEBUGLOG(fname,1);
  tsec_name = jfFFN_SECDATA::section_names[section_index];
  cname_list = selection_data->FindCrossSection(tsec_name,false);
  /**/JDEBUGLOGS(fname,2,tsec_name)
  // this option means check everything!
  if ((cname_list!=NULL) && ((cname_list->size())==0)) {
    /**/JDEBUGLOG(fname,3);
    cat_list->CheckAllDisable(true);
    first_list->CheckAllDisable(true);
    delete cname_list;
  }
  // some crossover groups are checked
  else if (cname_list!=NULL) {
    /**/JDEBUGLOG(fname,4);
    first_list->CheckNames(cname_list);
    delete cname_list;
  }
  /**/JDEBUGLOG(fname,5);
  // Once here, what about the category list?
  cgindex = first_list->currentRow();
  /**/JDEBUGLOGI(fname,6,cgindex);
  if (cgindex<0) {
    cgindex = 0;
    /**/JDEBUGLOG(fname,7);
    first_list->setSelectionNS(0);
  }
  /**/JDEBUGLOG(fname,8);
  // category group is checked
  if (first_list->GetBoolState(cgindex)) cat_list->CheckAllDisable(true);
  // category group is not checked
  else {
    /**/JDEBUGLOG(fname,9);
    cg_name = first_list->currentItem()->text();
    /**/JDEBUGLOG(fname,10);
    xcat_list = selection_data->FindSectionCross(tsec_name,cg_name);
    /**/JDEBUGLOG(fname,11);
    cat_list->CheckAndDisableNames(xcat_list);
    /**/JDEBUGLOG(fname,12);
    delete xcat_list;
  }
  /**/JDEBUGLOG(fname,13);
}
//**************************************************************************
jfFFN_CrossoverPicker::jfFFN_CrossoverPicker(const jfFFN_Categories* in_mcatdata
         , jfFFN_CatP_SelCat* inselector, QWidget* parent):QWidget(parent) {
  // assigning some values
  assert(in_mcatdata!=NULL);
  mcatdata = in_mcatdata;
  // setting up the GUI layout
  topclist = new jfCheckList(false);
  sub_lists = new jfFFN_CrossGroupPicker(mcatdata,inselector);
  tlayout = new QHBoxLayout();
  tlayout->addWidget(topclist,1);
  tlayout->addWidget(sub_lists,4);
  setLayout(tlayout);
  // loading info for the top list
  topclist->appendMultiCheckItems(jfFFN_SECDATA::section_names,jfFFN_SECDATA::scount);
  topclist->setSelectionNS(0);
  selection_data = inselector;
  if (selection_data!=NULL) {
    QStringList *xlist = inselector->FindCheckedCrossoverSections();
    if (xlist->size()>0) topclist->CheckNames(xlist);
    delete xlist;
  }
  // connecting
  connect(sub_lists,SIGNAL(sendCheckSig(bool)),this,SLOT(HandleCheckSig(bool)));
  connect(topclist,SIGNAL(sendChangeCheck(size_t,Qt::CheckState)),this,SLOT(HandleTopCheck(size_t,Qt::CheckState)));
  connect(topclist,SIGNAL(currentRowChanged(int)),this,SLOT(HandleTopChanged(int)));
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// change the selector
bool jfFFN_CrossoverPicker::ChangeSelector(jfFFN_CatP_SelCat* newselection) {
  // basic assignments
  if (newselection==NULL) return false;
  selection_data = newselection;
  // clearing any top checks
  topclist->SetCheckAll(Qt::Unchecked);
  QStringList *xlist = selection_data->FindCheckedCrossoverSections();
  if (xlist->size()>0) topclist->CheckNames(xlist);
  delete xlist;
  // doing the other lists
  return sub_lists->ChangeSelector(selection_data);
}
//---------------------------------------------------------
bool jfFFN_CrossoverPicker::UncheckAndRemove(QString xsection, QString xgroup, QString xcategory) {
  // variables
  bool all_section, rem_selected;
  QString sel_string;
  bool rbvalue;
  int rvalue;
  // doing some checks
  all_section = ((xgroup=="*") && (xcategory=="*"));
  if (topclist->currentRow()<0) rem_selected = false;
  else {
    sel_string = topclist->currentItem()->text();
    rem_selected = (sel_string==xsection);
  }
  // removing from the selector
  if (all_section) rbvalue = selection_data->RemoveCrossoverSection(xsection);
  else if (xcategory=="*") rbvalue = selection_data->RemoveCrossoverGroup(xsection,xgroup);
  else rbvalue = selection_data->RemoveCrossoverCategory(xsection,xgroup,xcategory);
  if (!rbvalue) return false;
  // handling checking
  // if it affects the current section, we just redo all checks
  if (rem_selected) return ChangeSelector(selection_data);
  // non-current section, but the whole section is affected?
  else if (all_section) {
    rvalue = topclist->CheckByName(xsection,Qt::Unchecked);
    if (rvalue<0) return false;
  }
  // otherwise, we do nothing
  return true;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// slot handlers
//---------------------------
void jfFFN_CrossoverPicker::HandleCheckSig(bool svalue) {
  emit CheckSignal(svalue);
}
//---------------------------
void jfFFN_CrossoverPicker::HandleTopCheck(size_t which, Qt::CheckState what) {
  // special cases
  assert(which<jfFFN_SECDATA::scount);
  assert(what!=Qt::PartiallyChecked);
  // checking
  if (what==Qt::Checked) {
    selection_data->AddCrossoverSection(jfFFN_SECDATA::section_names[which],false);
    sub_lists->EverythingChecked(which,true);
  }
  // unchecking
  else {
    selection_data->RemoveCrossoverSection(jfFFN_SECDATA::section_names[which]);
    sub_lists->EverythingChecked(which,false);
  }
  emit CheckSignal(what==Qt::Checked);
}

//---------------------------
void jfFFN_CrossoverPicker::HandleTopChanged(int currentRow) {
  assert(currentRow>=0);
  sub_lists->SetSection(currentRow,topclist->GetBoolState(currentRow));
}


//**************************************************************************

