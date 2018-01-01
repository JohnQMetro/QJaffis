/******************************************************************************
Name    :   gcat_picker.cpp
Author  :   John Q Metro
Purpose :   Category picking panel for generic section/category picking
Created :   August 25, 2012
Conversion to Qt Started April 6, 2014
Updated :   April 29, 2014
******************************************************************************/
#ifndef GCAT_PICKER_H_INCLUDED
  #include "gcat_picker.h"
#endif // GCAT_PICKER_H_INCLUDED
//------------------------------------------------
#ifndef LOGGING_H_INCLUDED
  #include "../../core/utils/logging.h"
#endif // LOGGING_H_INCLUDED

#include <assert.h>
#include <QMessageBox>
/*****************************************************************************/
jfTwinPanelGCPicker::jfTwinPanelGCPicker(jfGen_CatHolder* inmain, jfGenCat_PickData* inselection,
                   size_t prop, QWidget* parent):QWidget(parent) {
  // some checks and asserts
  const QString fname = "jfTwinPanelGCPicker::jfTwinPanelGCPicker";
  /**/jfAssertLog(inmain!=NULL,fname,"The Holder is NULL");
  /**/jfAssertLog(inselection!=NULL,fname,"The Selection is NULL");
  /**/jfAssertLog(prop<10,fname,"The Prop is 10 or more");
  // assigning initial values
  /**/JDEBUGLOG(fname,1);
  QStringList* snames = NULL;
  main_catdata = inmain;
  selection_data = inselection;
  // creating the gui elements
  section_list = new jfCheckList(false);
  cat_list = new jfCheckList(false);
  // arranging them
  top_sizer = new QHBoxLayout();
  top_sizer->addWidget(section_list,prop);
  top_sizer->addWidget(cat_list,prop);
  // done
  setLayout(top_sizer);
  // we get the section names
  snames = main_catdata->GetSecNames();
  assert(snames!=NULL);
  assert(snames->count()>0);
  // loading the section names into the list
  section_list->appendMultiCheckItems(*snames);
  delete snames;
  // connections
  connect(section_list,SIGNAL(sendChangeCheck(size_t, Qt::CheckState)),this,SLOT(HanSectionChecked(size_t,Qt::CheckState)));
  connect(cat_list,SIGNAL(sendChangeCheck(size_t, Qt::CheckState)),this,SLOT(HanCategoryChecked(size_t,Qt::CheckState)));
  connect(section_list,SIGNAL(currentRowChanged(int)),this,SLOT(HanSectionChanged(int)));
  connect(cat_list,SIGNAL(currentRowChanged(int)),this,SLOT(HanCategoryChanged(int)));
  // setting the index
  SetSection(0);
  // setting the check marks
  SetSectionChecks();
  /**/JDEBUGLOG(fname,7);
  CheckSelectedSection();
  /**/JDEBUGLOG(fname,8);
  // done
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// handles unchecking an item...
bool jfTwinPanelGCPicker::UnCheck(QString insname, QString incatname) {
  //local variables
  const QString fname = "jfTwinPanelGCPicker::UnCheck";
  int floc,svalue;
  // we start
  insname = insname.trimmed();
  incatname = incatname.trimmed();
  floc = section_list->FindString(insname,true);
  if (floc<0) return false;
  // checking to see if we have that now...
  svalue = section_list->currentRow();
  if (svalue==floc) {
    if (incatname=="*") {
      SetCheckAll(false);
      section_list->SetCheck(floc,Qt::Unchecked);
    }
    else return NameUncheck(incatname);
  }
  else {
    if (incatname=="*") section_list->SetCheck(floc,Qt::Unchecked);
  }
  return true;
}
//-----------------------------------------------------------
bool jfTwinPanelGCPicker::ChangeSelector(jfGenCat_PickData* newselection, bool selzero) {
  const QString fname = "jfTwinPanelGCPicker::ChangeSelector";
  // changing the link
  if (newselection==NULL) return false;
  selection_data = newselection;
  // only 2 things to call
  SetSectionChecks();
  CheckSelectedSection();
  return true;

}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// slot methods
//--------------------------------------------------------------
void jfTwinPanelGCPicker::HanSectionChecked(size_t which, Qt::CheckState what) {
  // local variables
  bool cvalue;
  QString sel_string;
  // we get the checked value and the string
  cvalue = !(what==Qt::Unchecked);
  sel_string = section_list->item(which)->text();
  // going ahead...
  AddRemove(cvalue, sel_string);
  if (section_list->currentRow()==which) SetCheckAll(cvalue);
  emit sendCheckSig(cvalue);
}
//--------------------------------------------------------------
void jfTwinPanelGCPicker::HanCategoryChecked(size_t which, Qt::CheckState what) {
  const QString fname = "jfTwinPanelGCPicker::CategoryChecked";
  // local variables
  bool cvalue,cres;
  QString sname,cname;
  // checked or no?
  cvalue = !(what==Qt::Unchecked);
  sname = section_list->currentItem()->text();
  cname = cat_list->item(which)->text();
  // if checked
  cres = CatAddRemove(cvalue, sname, cname);
  emit sendCheckSig(cvalue);
}
//--------------------------------------------------------------
void jfTwinPanelGCPicker::HanSectionChanged(int currentRow) {
  // constants
  const QString fname = "jfTwinPanelGCPicker::SectionChanged";
  // local variables
  int selec_value;
  // the section is changed!
  selec_value = section_list->currentRow();
  /* annoyingly, I've come across a case where calling ChangeTopSection causes
  the event to be launched despite nothing being selected */
  if (selec_value>=0) {
    /**/JDEBUGLOGI(fname,2,selec_value);
    LoadCatNames(selec_value);
    /**/JDEBUGLOG(fname,3);
    CheckSelectedSection();
  }
  /**/JDEBUGLOG(fname,4);
}
//--------------------------------------------------------------
void jfTwinPanelGCPicker::HanCategoryChanged(int currentRow) {
  // we do nothing here
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// internal methods to set the selection
//------------------------------------------------------------------------------
// changes the selected section and loads the categories
bool jfTwinPanelGCPicker::SetSection(size_t sindex) {
  // an intial check
  if (sindex>=(section_list->count())) return false;
  // setting with no event
  section_list->blockSignals(true);
  section_list->setCurrentRow(sindex);
  section_list->blockSignals(false);
  // cat names
  return LoadCatNames(sindex);
}
//---------------------------------------------------------------------------
 bool jfTwinPanelGCPicker::LoadCatNames(size_t sindex) {
   // varibles
   QStringList* cname_list = NULL;
   // checking
   if (sindex>=(section_list->count())) return false;
   cname_list = main_catdata->GetCatNames(sindex);
   cat_list->clear();
   if (cname_list!=NULL) {
     cat_list->appendMultiCheckItems(*cname_list);
     delete cname_list;
   }
   return true;
 }
//---------------------------------------------------------------------------
// checks the checkmarks for the currectly selected item (in the cat list)
bool jfTwinPanelGCPicker::CheckSelectedSection() {
  // local variables
  const QString fname="jfTwinPanelGCPicker::CheckSelectedSection";
  int sel_index;
  QStringList* cpicked;
  QString sel_name;
  size_t csloop, cscount;
  int rval;
  // we start with getting the selected index, then clearing old checks
  sel_index = section_list->currentRow();
  if (sel_index<0) return false;
  /**/JDEBUGLOG(fname,3);
  SetCheckAll(false);
  /**/JDEBUGLOG(fname,4);
  // everything is checked!
  if (section_list->GetBoolState(sel_index)) {
    /**/JDEBUGLOG(fname,5);
    SetCheckAll(true);
  }
  // for a selective checks, we look at the selection list
  else {
    /**/JDEBUGLOG(fname,4);
    // getting the info
    sel_name = section_list->currentItem()->text();
    cpicked = selection_data->FindSection(sel_name);
    /**/JDEBUGLOG(fname,5);
    // interpreting the info
    if (cpicked==NULL) {}
    else if (cpicked->isEmpty()) {
      /**/JDEBUGLOG(fname,6);
      // this should be an error, the section list should have been checked
      delete cpicked;
      return false;
    }
    // otheriwse, we loop over things
    else {
      /**/JDEBUGLOG(fname,7);
      cscount = cpicked->count();
      for (csloop=0; csloop<cscount; csloop++) {
        rval = cat_list->FindString((*cpicked)[csloop],true);
        assert(rval>=0);
        cat_list->SetCheckNS(rval,Qt::Checked);
      }
      /**/JDEBUGLOG(fname,8);
      delete cpicked;
    }
    /**/JDEBUGLOG(fname,9);
  }
  /**/JDEBUGLOG(fname,10);
  return true;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// sets the checkmarks in the section list, return false if everything is checked
bool jfTwinPanelGCPicker::SetSectionChecks() {
  // local variables
  QStringList* sec_namel = NULL;
  size_t csloop;
  // we start looping over the sections
  for (csloop = 0; csloop<(main_catdata->SCount());csloop++) {
    sec_namel= selection_data->FindSection(section_list->item(csloop)->text());
    // we check if...
    if ((sec_namel!=NULL) && (sec_namel->isEmpty())) {
      section_list->SetCheckNS(csloop,Qt::Checked);
    }
  }
  return true;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// adds or removes a section that has been checked
bool jfTwinPanelGCPicker::AddRemove(bool add, QString sname) {
  if (add) selection_data->AddSection(sname,false);
  else selection_data->RemoveSection(sname);
  return add;
}
//--------------------------------------------------------------------
bool jfTwinPanelGCPicker::CatAddRemove(bool add, QString sname, QString cname) {
  if (add) selection_data->AddCategory(sname,cname, false);
  else selection_data->RemoveCategory(sname,cname);
  return add;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// helper methods
void jfTwinPanelGCPicker::SetCheckAll(bool value) {
  const QString fname = "jfTwinPanelGCPicker::SetCheckAll";
  Qt::CheckState tvalue;
  tvalue = (value)?(Qt::Checked):(Qt::Unchecked);
  cat_list->SetCheckAll(tvalue);
  cat_list->setEnabled(!value);
}

//-------------------------------------------------------------
/* checks if there is an item with the given name in the category list, and
if there is, unchecks it. True if the name is found, false otherwise */
bool jfTwinPanelGCPicker::NameUncheck(const QString& name) {
  //local variables
  int floc;
  // we start
  floc = cat_list->FindString(name,true);
  if (floc<0) return false;
  else {
    cat_list->SetCheckNS(floc,Qt::Unchecked);
    return true;
  }
}
//*********************************************************************************
jfGCPickerDispl::jfGCPickerDispl(jfGenCat_PickData* inselection, QWidget* parent):QWidget(parent) {
  // startup checks
  assert(inselection!=NULL);
  // assigning values
  selection_data = inselection;
  // we create the GUI elements
  display = new QListWidget();
  del_button = new QPushButton("Delete");
  // arraging them in a sizer
  top_sizer = new QVBoxLayout();
  top_sizer->addWidget(display,1);
  top_sizer->addWidget(del_button,0,Qt::AlignRight);
  // setting the values in display
  QStringList* namevals = selection_data->GetListing();
  display->addItems(*namevals);
  delete namevals;
  // finsihing
  connect(del_button,SIGNAL(clicked(bool)),this,SLOT(OnDeleteBtn(bool)));
  setLayout(top_sizer);
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// methods
//-----------------------------------------------------------------------
void jfGCPickerDispl::UpdateDisplay() {
  const QString fname = "jfGCPickerDispl::UpdateDisplay";
  // local variables
  QString sitem;
  int sloc;
  bool has_selection;
  QStringList* namevals;
  // we start
  has_selection = (0<=(display->currentRow()));
  if (has_selection) {
    sitem = display->currentItem()->text();
  }
  // we update the display
  namevals = selection_data->GetListing();
  display->clear();
  display->addItems(*namevals);
  delete namevals;
  // now what about the selection...
  if (has_selection) {
    sloc = FindDisplayName(sitem,true);
    if (sloc>=0) display->setCurrentRow(sloc);
  }
}
//-----------------------------------------------------------
void jfGCPickerDispl::Disable(bool yes) {
  display->setEnabled(!yes);
  del_button->setEnabled(!yes);
}
//-------------------------------------------------------------
bool jfGCPickerDispl::ChangeSelector(jfGenCat_PickData* newselection) {
  if (newselection==NULL) return false;
  selection_data = newselection;
  UpdateDisplay();
  return true;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void jfGCPickerDispl::OnDeleteBtn(bool checked) {
  // local variables
  int sloc;
  QString svalue;
  bool rsres;
  QListWidgetItem* titem;
  // we start
  sloc = display->currentRow();
  if (sloc>=0) {
    // we get the string
    titem = display->takeItem(sloc);
    svalue = titem->text();
    // svalue = display->currentItem()->text();
    rsres = selection_data->RemoveByString(svalue);
    assert(rsres);
    emit SendSItemDeleted(svalue);
  }
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
int jfGCPickerDispl::FindDisplayName(const QString& fname, bool cassen) const {
  assert(display!=NULL);
  // special cases
  if (display->count()==0) return -1;
  if (fname.isEmpty()) return -1;
  // variables
  QList<QListWidgetItem *> qogy;
  Qt::MatchFlags matcher = Qt::MatchFixedString;
  // doing it
  if (cassen) matcher |= Qt::MatchCaseSensitive;
  qogy = display->findItems(fname,matcher);
  if (qogy.isEmpty()) return -1;
  else return display->row(qogy[0]);
}
//*********************************************************************
    // the constructor
jf_Gen_CatPicker::jf_Gen_CatPicker(jfGen_CatHolder* inmain, jfGenCat_PickData* inselection,
          size_t prop, QWidget* parent):QWidget(parent) {
// constants
  const QString fname="jf_Gen_CatPicker::jf_Gen_CatPicker";
  // variables
  QString omsg;
  bool atest;
  // checks
  assert(inmain!=NULL);
  assert(inselection!=NULL);
  assert(prop<10);
  // assigning some values
  main_catdata = inmain;
  selection_data = inselection;
  // we start creating gui elements
  section_panel = new jfTwinPanelGCPicker(main_catdata,selection_data,4);
  picked_display = new jfGCPickerDispl(selection_data);
  // next up is the expression filters
  and_label = new QLabel("Catgeory must match       :");
  and_edit = new jfSimpleExprEdit(false);
  atest = and_edit->SetData(inselection->GetAnd(),omsg);
  assert(atest);
  or_label = new QLabel("Include if Category matches:");
  or_edit = new jfSimpleExprEdit(false);
  atest = or_edit->SetData(inselection->GetOr(),omsg);
  assert(atest);
  // the filter sizer
  expredit_sizer = new QGridLayout();
  expredit_sizer->addWidget(and_label,0,0,Qt::AlignVCenter);
  expredit_sizer->addWidget(and_edit,0,1);
  expredit_sizer->addWidget(or_label,1,0,Qt::AlignVCenter);
  expredit_sizer->addWidget(or_edit,1,1);
  expredit_sizer->setColumnStretch(1,1);
  // arranging...
  middle_sizer = new QHBoxLayout();
  middle_sizer->addWidget(section_panel,prop);
  middle_sizer->addWidget(picked_display,10-prop);
  // the top level sizer
  top_sizer = new QVBoxLayout();
  top_sizer->addLayout(middle_sizer,1);
  top_sizer->addLayout(expredit_sizer);
  // connections
  connect(section_panel,SIGNAL(sendCheckSig(bool)),this,SLOT(OnCheckEvent(bool)));
  connect(picked_display,SIGNAL(SendSItemDeleted(QString)),this,SLOT(OnSItemDeleted(QString)));
  // done
  setLayout(top_sizer);
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// event handlers
//-----------------------------------------------------------------------
bool jf_Gen_CatPicker::ChangeSelector(jfGenCat_PickData* newselection) {
  const QString fname = "jf_Gen_CatPicker::ChangeSelector";
  // assertion...
  /**/JDEBUGLOG(fname,1);
  if (newselection==NULL) return false;
  selection_data = newselection;
  section_panel->ChangeSelector(newselection,false);
  /**/JDEBUGLOG(fname,2);
  picked_display->ChangeSelector(newselection);
  QString oomsg;
  and_edit->SetData(newselection->GetAnd(),oomsg);
  /**/JDEBUGLOGS(fname,3,oomsg);
  or_edit->SetData(newselection->GetOr(),oomsg);
  /**/JDEBUGLOGS(fname,4,oomsg);
  return true;
}
//--------------------------------------------------------------------
// saving the expression filters
bool jf_Gen_CatPicker::SaveFilters() {
  // variables
  const QString nvalid = "Expression Not Valid";
  QString omsg;
  jfSimpleExpr *result_expression;
  QMessageBox message;
  // we start, and filter first
  result_expression = and_edit->CheckFilter(omsg);
  if (result_expression==NULL) {
    message.setIcon(QMessageBox::Critical);
    message.setWindowTitle(nvalid);
    message.setText(omsg);
    message.exec();
    return false;
  }
  else {
    selection_data->SetAndFilter(result_expression);
  }
  // or filter next
  result_expression = or_edit->CheckFilter(omsg);
  if (result_expression==NULL) {
    message.setIcon(QMessageBox::Critical);
    message.setWindowTitle(nvalid);
    message.setText(omsg);
    message.exec();
    return false;
  }
  else {
    selection_data->SetOrFilter(result_expression);
  }
  // done
  return true;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// slot handlers
//---------------------------------------------------------
// one of the
void jf_Gen_CatPicker::OnCheckEvent(bool value) {
  /**/jfXLogString("jf_Gen_CatPicker::OnCheckEvent");
  picked_display->UpdateDisplay();
}
//-----------------------------------------------------------------------
void jf_Gen_CatPicker::OnSItemDeleted(QString del_value) {
  // constants and variables
  const QChar pstr = ';';
  QStringList* pvalue;
  // we parse
  pvalue = ListToStrArr( del_value, pstr );
  /**/jfXLogString(del_value);
  assert(pvalue!=NULL);
  section_panel->UnCheck(pvalue->at(0),pvalue->at(1));
  delete pvalue;
}
//******************************************************************************
