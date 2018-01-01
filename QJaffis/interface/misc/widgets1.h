/******************************************************************************
Name    :   widgets1.h
Author  :   John Q Metro
Purpose :   Some misc user interface stuff
Created :   April 1, 2009
Conversion to Qt : Started September 8, 2013
Updated :   July 18, 2018
******************************************************************************/
#ifndef WIDGETS1_H_INCLUDED
#define WIDGETS1_H_INCLUDED
#endif // WIDGETS1_H_INCLUDED
#ifndef BASEOBJ_H_INCLUDED
    #include "../../core/objs/baseobj.h"
#endif // BASEOBJ_H_INCLUDED
#ifndef VECTORS_H_INCLUDED
  #include "../../core/structs/vectors.h"
#endif // VECTORS_H_INCLUDED
//---------------------------------
#include <QWidget>
#include <QLayout>
#include <QLabel>
#include <QLineEdit>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QDialog>
#include <QPlainTextEdit>
#include <QSize>
#include <QListWidget>
#include <vector>
//****************************************************************************

/* Edit panel for name and description, using a grid layout */
class jfNameDescEditor : public QWidget {
    Q_OBJECT
  public:
    // the constructor
    jfNameDescEditor(QWidget* parent,bool isvert, bool ismulti, QString default_name = "default name",QString default_desc = "");
    // adding extra objects or sizers
    virtual void AddItem(QWidget* widget, size_t prop = 0);
    virtual void AddItem(QLayout* sizer, size_t prop = 0);
    // getting some info
    QString TryGetName() const;
    bool IsNameEmpty() const;
    QString GetDesc() const;
    bool NameStatus() const;
    bool IsVertical() const;
    bool IsMultiline() const;
    bool ChangeObj(jfBaseObj* inobj) const;
    // setting some info
    bool SetName(QString newname);
    void SetDescription(const QString& newdesc);
    bool SetFromObj(const jfBaseObj* inobj);
    // some extra methods
    bool TryCopy() const;
    QString GetBackup() const;
  signals:
    void SendNewName(QString in_nname);
  public slots:
    void HandleNameChange(const QString& newname);
  protected:
    // an internal helper method
    void ShowEmptyMessage() const;
    // the main widgets
    QLabel* name_label;
    QLabel* desc_label;
    QLineEdit*   name_edit;
    QLineEdit*   desc_edit1;
    QPlainTextEdit*   desc_edit2;
    // sizers
    QGridLayout* mainsizer;
    QHBoxLayout* topline;
    QVBoxLayout* vertmsizer;
    // extra flags
    bool multiline,vertical;
    // internal data
    mutable QString backup_data;
    size_t extra_count;
};
//==========================================================================
// a buttonless non-modal text label status display dialog
class jfTextStatusDialog : public QDialog {
    Q_OBJECT
	public:
		// the constructor
    jfTextStatusDialog(QWidget *parent, const QString& title, const QString& display_msg, QSize insize);
		// other methods
    QString GetMessage() const;
    void ChangeMessage(const QString new_message);
  public slots:
    void handleAction(QString action_label);
    void handleDone();
    void handleLoading(QString loaded_thing);
    void handleString(QString statement_string);
	protected:
		// GUI stuff
    QLabel* info_display;
    QHBoxLayout* top_sizer;
};
//===========================================================================
/* Qt replacement for wxCheckListBox Qt::CheckState */
class jfCheckList : public QListWidget {
    Q_OBJECT
  public:
    // the constructor
    jfCheckList(bool is_tristate, QWidget* parent = NULL);
    // adding items
    void appendCheckItem(const QString& label);
    void appendMultiCheckItems(const QStringList& labels);
    void appendMultiCheckItems(const QString inlabels[], size_t labcount);
    void replaceItems(const QStringList& labels);
    // information
    Qt::CheckState GetState(size_t which) const;
    bool GetBoolState(size_t which) const;
    int FindString(const QString& svalue, bool casesen) const;
    bool CurrNotChecked() const;
    // changing checkstate
    size_t SetCheckAll(Qt::CheckState new_value);
    int CheckByName(QString name, Qt::CheckState new_value);
    bool SetCheck(size_t which,Qt::CheckState new_value);
    bool SetCheckNS(size_t which,Qt::CheckState new_value);
    void CheckAllDisable(bool check);
    // bulk checking
    size_t CheckNames(QStringList* inlist);
    size_t CheckAndDisableNames(jfStrBoolList* inlist);
    // other
    bool setSelectionNS(size_t which);
    bool clearAll();
    bool RemoveCheckItem(size_t index);
    bool RemoveCheckItem(const QString& label);
  signals:
    void sendChangeCheck(size_t which, Qt::CheckState what);
  public slots:
    void handleItemChanged(QListWidgetItem* item);
  protected:
    bool tristate;
    std::vector<Qt::CheckState> cs_store;
};

//****************************************************************************
