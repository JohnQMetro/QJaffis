/******************************************************************************
// Name:        category_editor.cpp
// Author :     John Q Metro
// Purpose :    Defines category editing interface stuff
// Created:     April 3, 2009
// Conversion to Qt Started Oct 9, 2013
// Updated:     January 1, 2011
******************************************************************************/
#ifndef CATEGORY_EDITOR_H_INCLUDED
  #include "category_editor.h"
#endif // CATEGORY_EDITOR_H_INCLUDED
#ifndef UTILS1_H_INCLUDED
  #include "../../../core/utils/utils1.h"
#endif // UTILS1_H_INCLUDED
#ifndef LOGGING_H_INCLUDED
  #include "../../../core/utils/logging.h"
#endif // LOGGING_H_INCLUDED
//--------------------------------------------------------
#include <QDir>
#include <assert.h>
#include <QMessageBox>
#include <QFileDialog>
#include <QFileInfo>
//*****************************************************************************
const QString cbmd_base = "The path cannot be made relative to the base directory for results";
//=============================================================================
jfRelativeFilePicker::jfRelativeFilePicker(jfCategories* catparent, size_t cateindex, bool vertical, QWidget* parent): QWidget(parent) {
  // constants
  const QString fname="jfRelativeFilePicker::jfRelativeFilePicker";
  // assignments
  assert(catparent!=NULL);
  datalink = catparent;
  if (cateindex!=32) assert(cateindex<(datalink->GetCount()));
  this_index = cateindex;
  // file path picker
  frlabel = new QLabel("Result filename and location:");
  filepath = new QLabel(datalink->PathAtIndex(this_index));
  pickabsolute = new QCheckBox("Absolute Path");
  pickabsolute->setChecked(!(datalink->RelativePathAtIndex(this_index)));
  select_file = new QPushButton("Pick Path");
	//arraging options
	if (vertical) {
		// creating the top line
    filepath_sizer = new QHBoxLayout();
    filepath_sizer->addWidget(frlabel,0,Qt::AlignVCenter);
    filepath_sizer->addWidget(filepath,1,Qt::AlignVCenter);
		// the second row sizer
    line2sizer = new QHBoxLayout();
    line2sizer->addWidget(pickabsolute,0,Qt::AlignVCenter);
    line2sizer->addWidget(select_file,0);
		// making the toplevel sizer
    topsizer = new QBoxLayout(QBoxLayout::TopToBottom);
		// arranging within the top level
    topsizer->addLayout(filepath_sizer,0);
    topsizer->addLayout(line2sizer,0);
	}
	else {
    topsizer = new QBoxLayout(QBoxLayout::RightToLeft);
    topsizer->addWidget(frlabel,0,Qt::AlignVCenter);
    topsizer->addWidget(filepath,1,Qt::AlignVCenter);
    topsizer->addSpacing(8);
    topsizer->addWidget(pickabsolute,0,Qt::AlignVCenter);
    topsizer->addWidget(select_file,0);
	}
  // now, we set the layout
  setLayout(topsizer);
  // connections
  connect(pickabsolute, SIGNAL(stateChanged(int)), this, SLOT(ProcessFnameCheck(int)));
  connect(select_file, SIGNAL(clicked(bool)), this, SLOT(ProcessFnameBtn(bool)));
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// a helper method
void jfRelativeFilePicker::ChangeCategoryIndex(size_t newindex) {
  if (newindex!=33) assert(newindex<(datalink->GetCount()));
  this_index = newindex;
}
//-------------------------------------------------------------------------------
QString jfRelativeFilePicker::GetPath() const {
  return filepath->text();
}
//-------------------------------------------------------------------------------
bool jfRelativeFilePicker::IsAbsolute() const {
  return pickabsolute->isChecked();
}
//------------------------------------------------------------------------------
void jfRelativeFilePicker::ReloadNames() {
  filepath->setText(datalink->PathAtIndex(this_index));
  pickabsolute->setChecked(!(datalink->RelativePathAtIndex(this_index)));
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// event handlers
//---------------------------------------------------------------------------
void jfRelativeFilePicker::ProcessFnameCheck(int state) {
  QDir basedir;
  QString corepath, fullpath;
  bool rvalue;
  QMessageBox emsg;
  // we start
  basedir = QDir(datalink->base_outputdir);
  corepath = datalink->PathAtIndex(this_index);
  // checked, we convert from relative to absolute
  if (IsAbsolute()) {
    // if try to do this
    fullpath = basedir.absoluteFilePath(corepath);
    fullpath = QDir::cleanPath(fullpath);
    // error checking
    assert(!fullpath.isEmpty());
    // things okay, we set the values
    filepath->setText(fullpath);
    datalink->SetFilePath(this_index,fullpath,false);
  }
  // non checked, we convert from absolute to relative
  else {
    fullpath = basedir.relativeFilePath(corepath);
    rvalue = !fullpath.isEmpty();
    // err checking
    if (!rvalue) {
      emsg.setIcon(QMessageBox::Critical);
      emsg.setText(cbmd_base+",\nsince the current path is in a different volume than the base. Sorry.");
      emsg.setWindowTitle("The operation cannot be done");
      emsg.exec();
      // undoing the check
      pickabsolute->setChecked(true);
    }
    else {
      // we set the value
      filepath->setText(fullpath);
      datalink->SetFilePath(this_index,fullpath,true);
    }
  }
}
//-------------------------------------------------------------------------------
void jfRelativeFilePicker::ProcessFnameBtn(bool checked) {
  // constants
  const QString fname = "jfRelativeFilePicker::ProcessFnameBtn";
  const QString resfilter = "HTML files (*.html *.htm)";
  // variables
  QString corepath, fullpath;
  QDir basedir;
  QMessageBox emsg;
  QString res_fpath;
  int reval;
  // trying to get the filename and path to use as the default value
  basedir = QDir(datalink->base_outputdir);
  corepath = datalink->PathAtIndex(this_index);
  if (!IsAbsolute()) {
    // if try to do this
    fullpath = basedir.absoluteFilePath(corepath);
    fullpath = QDir::cleanPath(fullpath);
    // error checking
    assert(!fullpath.isEmpty());
  }
  else fullpath = corepath;
  // getting the result
  res_fpath = QFileDialog::getSaveFileName (this,"Choose a result path and filename", fullpath,resfilter);
  // we've got the result, if need be, we make it relative
  if (!(pickabsolute->isChecked())) {
    // trying to make the path relative
    corepath = basedir.relativeFilePath(res_fpath);
    if (corepath.isEmpty()) {
      // if this fails, we ask that the path be made absolute
      emsg.setIcon(QMessageBox::Critical);
      emsg.setText(cbmd_base+".\nDo we just make this path absolute?");
      emsg.setWindowTitle("Something must be done");
      emsg.setStandardButtons(QMessageBox::Yes|QMessageBox::Cancel);
      reval = emsg.exec();
      // handling the result
      if (reval==QMessageBox::Yes) {
        filepath->setText(res_fpath);
        datalink->SetFilePath(this_index,res_fpath,false);
        pickabsolute->setChecked(true);
      }
    }
    // if it does not fail, load the values
    else {
      filepath->setText(corepath);
      datalink->SetFilePath(this_index,corepath,true);
    }
  }
  // the path is supposed to be absolute, so we just copy it
  else {
    filepath->setText(res_fpath);
    datalink->SetFilePath(this_index,res_fpath,false);
  }
}
//=============================================================================
jfCategoryEditor::jfCategoryEditor(jfCategories* catparent, size_t cateindex, bool multiline, QWidget* parent):QWidget(parent) {
  // local variables
	const QString fname = "jfCategoryEditor::jfCategoryEditor";
  QString errout;
  // assignments
  assert(catparent!=NULL);
  datalink = catparent;
  assert(cateindex<(datalink->GetCount()));
  this_index = cateindex;
  currdata = datalink->GetAtIndex(this_index);
  // we have a group box containing everything
  outerl = new QVBoxLayout();
  innerl = new QVBoxLayout();
  mbox = new QGroupBox(MakeMainLabel());
  // we start preparing the top part, with name+description editor and delete
  namedesc = new jfNameDescEditor(NULL,false,false,"1","");
  del_btn = new QPushButton("Delete");
  namedesc->AddItem(del_btn);
  namedesc->SetFromFilter(currdata);
  // expresion editor
  expr_label = new QLabel("Sorting Expression");
  expr_edit = new jfExpressionEditor((datalink->localmap_ptr),true,multiline,false);
  expr_edit->SetData(currdata->ToString(),errout);
  // making the filepicker
  filepicker = new jfRelativeFilePicker(datalink,this_index,false);
  // arranging within the box
  innerl->addWidget(namedesc,0);
  innerl->addSpacing(5);
  innerl->addWidget(expr_label,0);
  if (multiline) innerl->addWidget(expr_edit,1);
  else innerl->addWidget(expr_edit,0);
  innerl->addWidget(filepicker,0);
  // setting up the box
  mbox->setLayout(innerl);
  outerl->addWidget(mbox,1);
  setLayout(outerl);
  // connections
  connect(del_btn, SIGNAL(clicked(bool)), this, SLOT(ProcessDeleteBtn(bool)));
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// changes the index of this editor
void jfCategoryEditor::ChangeCategoryIndex(size_t newindex) {
	const QString fname = "jfCategoryEditor::ChangeCategoryIndex";
  if (newindex!=33) assert(newindex<(datalink->GetCount()));
  this_index = newindex;
  mbox->setTitle(MakeMainLabel());
  filepicker->ChangeCategoryIndex(newindex);
}
//---------------------------------------------------------------------
// tries to save the contents of the widgets to the jfCategroies object
bool jfCategoryEditor::SaveData(QString& outmessage){
  // local variables
  jfExpressionFilter* expr_result;
  QString qname;
  // what we do is call the embedded expression editor to convert it's contents
  expr_result = expr_edit->CheckFilter(outmessage);
  // if wrong, we the error message is passed out and we fail
  if (expr_result == NULL) return false;
  // otherwise, we save the name and description...
  qname = namedesc->TryGetName();
  expr_result->SetName(qname);
  expr_result->SetDescription(namedesc->GetDesc());
  // and then copy over the result
  datalink->ReplaceAtIndex(this_index,expr_result);
	currdata = expr_result;
  // done
  return true;
}
//-------------------------------------------------------------------------
// creates the main label for the editor
QString jfCategoryEditor::MakeMainLabel() const {
  QString result;
	const QString fname="jfCategoryEditor::MakeMainLabel";
  result = "Category  " + QString::number(this_index);
  result += ": ";
  result += GetExpName();
  return result;
}
//-------------------------------------------------------------------------
QString jfCategoryEditor::GetExpName() const {
	return currdata->GetName();
}
//-------------------------------------------------------------------------
QString jfCategoryEditor::GetExpStr() const {
	return currdata->ToString();
}
//-------------------------------------------------------------------------
void jfCategoryEditor::UnEnable(bool doit) {
  del_btn->setEnabled(!doit);
  expr_edit->setEnabled(!doit);
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void jfCategoryEditor::ProcessDeleteBtn(bool checked) {
  /* we actually use the parent to handle most of this, but we need to
    identify the index */
  emit SendDelete(this_index);
}
//*****************************************************************************/

