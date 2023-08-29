/******************************************************************************
Name    :   fim3_filedit.cpp
Author  :   John Q Metro
Purpose :   Editors for fim filters : 2018 and later
Created :   January 21, 2018
Updated :   August 24, 2023
******************************************************************************/
#ifndef FIM3_FILEDIT_H
    #include "fim3_filedit.h"
#endif // FIM3_FILEDIT_H
//-------------------------------
#ifndef FIM_TAGS_H
    #include "../../../fanfic/data/fim/fim_tags.h"
#endif // FIM_TAGS_H
#ifndef FIM_CONSTANTS_H
    #include "../../../fanfic/data/fim/fim_constants.h"
#endif // FIM_CONSTANTS_H
#include <assert.h>
/*****************************************************************************/
// the default constructor
jfFimContentTypeFilterEditor::jfFimContentTypeFilterEditor(const jfFIMContentTypeFilter* infilt,
                                            QWidget* parent):jfTagFilterEditor(infilt,parent) {
  CompleteConstruction("Content Types",infilt);
  if (infilt==NULL) LoadBlank();
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// extra methods
//---------------------------
jfTagFilterCore* jfFimContentTypeFilterEditor::GetTagFilter() {
  jfFIMContentTypeFilter* result;
  result = new jfFIMContentTypeFilter(namedesc_edit->TryGetName());
  result->SetToEmpty();
  return result;
}
//---------------------------
bool jfFimContentTypeFilterEditor::isListLong() const {
  return false;
}
//---------------------------
void jfFimContentTypeFilterEditor::LoadBlank() {
  assert(fimcon::tags != NULL);
  jfTagListing* new_blank_list = new jfTagListing();
  fimcon::tags->SetTagsToEmpty(*new_blank_list,FIMT_content);
  shortlist_panel->SetTagList(new_blank_list);
}
//============================================================================
// the default constructor
jfFimWarningsFilterEditor::jfFimWarningsFilterEditor(const jfFIMWarningsFilter* infilt,
                                QWidget* parent):jfTagFilterEditor(infilt,parent) {
  CompleteConstruction("Warnings",infilt);
  if (infilt==NULL) LoadBlank();
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// extra methods
//---------------------------
jfTagFilterCore* jfFimWarningsFilterEditor::GetTagFilter() {
  jfFIMWarningsFilter* result;
  result = new jfFIMWarningsFilter(namedesc_edit->TryGetName());
  result->SetToEmpty();
  return result;
}
//---------------------------
bool jfFimWarningsFilterEditor::isListLong() const {
  return false;
}
//---------------------------
void jfFimWarningsFilterEditor::LoadBlank() {
  assert(fimcon::tags != NULL);
  jfTagListing* new_blank_list = new jfTagListing();
  fimcon::tags->SetTagsToEmpty(*new_blank_list,FIMT_warning);
  shortlist_panel->SetTagList(new_blank_list);
}
/*****************************************************************************/
