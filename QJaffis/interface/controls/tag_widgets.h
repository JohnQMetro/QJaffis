/******************************************************************************
Name    :   tag_widgets.h
Author  :   John Q Metro
Purpose :   Tag categorizing widgets
Created :   July 27, 2016
Updated :   December 31, 2017 (bold Items)
******************************************************************************/
#ifndef TAG_WIDGETS_H
  #define TAG_WIDGETS_H
#endif // TAG_WIDGETS_H
//-------------------------------
#ifndef JFMISCTYPES1
  #include "../../core/objs/misc_types1.h"
#endif
//-------------------------------
#include <QWidget>
#include <QLabel>
#include <QListWidget>
#include <QBoxLayout>
#include <vector>
#include <QStringList>
#include <QDragEnterEvent>
/*****************************************************************************/
// listbox used in jfTagContainer, mostly for the restricted drag and drop
class jfTC_ListBox : public QListWidget {
  public:
    jfTC_ListBox(QWidget* parent = NULL);
    bool addAllowedSourceLink(const jfTC_ListBox* ok_ptr);
    void setBoldStrings(const QStringSet* boldset_in);
    void addItemSpecial(const QString& newitem);
  protected:
    virtual void dragEnterEvent(QDragEnterEvent * event);
    virtual void dropEvent(QDropEvent *event);
    bool makeStringBold(const QString& this_item);
    const QStringSet* boldset;

    bool AllowedSource(const QWidget* source);
    std::vector<const jfTC_ListBox*> drag_sources;

};

//=========================================================
// a single list of tags of a particular status type (like 'include', 'exclude', etc)
// with drag and drop
class jfTagContainer : public QWidget {
  public:
    //constructor
    jfTagContainer(jfTAG_STATUS in_stat_type, QWidget* parent = NULL);
    // setup
    bool SetTagLabel(const QString& in_label);
    void setBoldStrings(const QStringSet* boldset_in);
    // basic info data
    jfTAG_STATUS getStatusType() const;
    bool ContainsTag(const QString& tag) const;
    const jfTC_ListBox* getListPointer() const;
    // actions
    bool AddTag(const QString& tag);
    void ClearContents();
    QStringList* Contents() const;
    bool addContainerLink(const jfTC_ListBox* ok_ptr);
    void linkToContainers(jfTagContainer *contr1, jfTagContainer *contr2,jfTagContainer *contr3);
  protected:
    // gui
    QLabel* type_label;
    jfTC_ListBox* tag_list;
    QVBoxLayout* stack_layout;
    // data
    QString tag_label;  // identifies (plural) of what the tag is
    jfTAG_STATUS stat_type;

};
//============================================================================
class jfTagSorter : public QWidget {
  public:
    // constructor and setup
    jfTagSorter(bool leftnone, bool doalternate, QWidget* parent =NULL);
    bool SetInitialData(const QString& tag_label, jfTagListing* in_tag_data, bool copy);
    bool SetInitialEmpty(const QString& tag_label, const QStringList* tags);
    bool SetInitialEmpty(const QString& tag_label, const QString tag_array[], size_t tagcount);
    void setBoldStrings(const QStringSet* boldset_in);
    // changing data afterwards
    bool ChangeTagData(jfTagListing* in_tagdata, bool copy, bool delete_old);
    jfTagListing* SaveAndGetTagData();

  protected:
    bool LoadDataToContainers(bool alt_to_none);
    bool SaveToTagData(bool make_data);
    void SaveTagsForHolder(jfTagContainer* hold_ptr);
    bool SetTagLabel(const QString& tag_label);

  // critical info
  bool has_alternate;
  jfTagListing* tag_data;
  bool internal;
  const QStringSet* bold_these_items;

  // GUI items
  bool none_to_left;
  jfTagContainer* NoneHolder;
  jfTagContainer* IncludeHolder;
  jfTagContainer* ExcludeHolder;
  jfTagContainer* AlternateHolder;
  QVBoxLayout* interest_column;
  QHBoxLayout* main_layout;

};

/*****************************************************************************/
