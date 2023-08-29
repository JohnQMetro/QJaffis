/******************************************************************************
Name    :   global_filter_editor.h
Author  :   John Q Metro
Purpose :   Declares the interface for the global filter editor
Created :   August 13, 2009
Separated from the local filter editor April 9 2023
Updated :   April 9, 2023
******************************************************************************/
#pragma once

#ifndef GLOBAL_FILTER_EDITOR_H
    #define GLOBAL_FILTER_EDITOR_H
#endif // GLOBAL_FILTER_EDITOR_H
// ----------------------------------------------------------
#include <QWidget>
#include <QString>
#include <QPushButton>
#include <QGroupBox>
#include <QVBoxLayout>
#include <QListWidget>

#include "../../core/filters/filtermap.h"

#ifndef FILTER_EDITOR_H_INCLUDED
  #include "filter_editor.h"
#endif // FILTER_EDITOR_H_INCLUDED


/******************************************************************************/
// const size_t global_ratio = 5;
// const size_t local_ratio = 2;
// const size_t edit_size_ratio = 2;
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//==============================================================================
/* Used in Global filter editor, a panel including a name editor and a local
filter editor */
class jfMapEditPanel : public QWidget {
    Q_OBJECT
  public:
    // the constructor
    jfMapEditPanel(jfFilterMap* inlink, QWidget* parent = NULL);
    // accessible methods
    bool ChangeMap(jfFilterMap* inlink);
    QString GetName();
    QString GetDescription() const;
    void ChangeBoxName(QString newname);
  public slots:
    void CatchSaved(bool cval);
  signals:
    void ThrowSaved();
  protected:
    // gui elements
    jfNameDescEditor* ndesce;
    QPushButton* save_name;
    jfFilterMapEditor* map_editor;
    // sizers
    QGroupBox* wrapper;
    QVBoxLayout *innerl, *outerl;
    // internal data
    bool mappset;
    jfFilterMap* itemlink;
};
//==============================================================================
/* the global filter editor, which has a list of maps, a map editor, two buttons
for adding and deleting maps, and an editor for the map name+desc
*/
class jfGlobalFilterEditor : public QWidget {
    Q_OBJECT
  public:
    // the constructor
    jfGlobalFilterEditor(jfMapOfFiltermaps* inlink, QWidget* parent = NULL);
    // methods
    void SetNoMap();
    bool LoadSelectedMap();
    bool ChangeMap(const size_t& index);
  public slots:
    //event handlers
    void NewPressed(bool inval);
    void DeletePressed(bool inval);
    void SavePressed();
    void MapPicked();
  protected:
    // helper methods
    size_t MakeDefaultMap(QString thename);
    // gui elemts
    QPushButton* add_btn;
    QPushButton* dele_btn;
    QListWidget* map_list;
    QWidget* blank_panel;
    jfMapEditPanel* main_mapedit;
    bool isblank;
    // sizers
    QHBoxLayout* btn_sizer;
    QVBoxLayout* side_sizer;
    QHBoxLayout* top_sizer;
    // internal data
    jfMapOfFiltermaps* maindata;
    size_t selindex;
    QString selname;
    jfFilterMap* selmap;
};
/******************************************************************************/
