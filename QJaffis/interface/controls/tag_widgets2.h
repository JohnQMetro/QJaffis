/******************************************************************************
Name    :   tag_widgets2.h
Author  :   John Q Metro
Purpose :   Tag categorizing widgets
Created :   July 27, 2016
Updated :   December 30, 2017 ( FIM related changes )
******************************************************************************/
#ifndef TAG_WIDGETS2_H
  #define TAG_WIDGETS2_H
#endif // TAG_WIDGETS2_H
//----------------------------------------
#include <QWidget>
#include <QLabel>
#include <QBoxLayout>
#include <QPushButton>
#include <QPalette>
#include <vector>
#include <QListWidget>
//---------------------------------------
#ifndef JFMISCTYPES1
  #include "../../core/objs/misc_types1.h"
#endif
/*****************************************************************************/
// a single listbox view 4 possible states for each item can be annoyingly complicated
//=================================================================
// custom row widget with buttons for the 3 extra options
class jfTagRowView : public QWidget {
    Q_OBJECT
  public:
    jfTagRowView(bool do_alternate, QWidget* parent = NULL);
    jfTagRowView(bool do_alternate, QString tagtext, QWidget* parent = NULL);
    bool SetStatus(jfTAG_STATUS in_status);
    bool SetTagString(const QString& intaglabel);
    jfTAG_STATUS GetStatus() const;
    QString GetTagString() const;
    bool CheckAndSetStatus(const QString& ctaglabel,jfTAG_STATUS in_status);
    size_t getButtonSize() const;
  signals:
    void statusChange();
  public slots:
    void handleIncludeClicked(bool value);
    void handleExcludeClicked(bool value);
    void handleAlternateClicked(bool value);
  protected:
    void SetupWidgets(QString tagtext);
    // setup helpers
    void setupPalettes();
    void setupButton(QPushButton* tbtn);
    // GUI widgets
    QLabel* tag_display;
    QPushButton* include_btn;
    QPushButton* exclude_btn;
    QPushButton* alternate_btn;
    QHBoxLayout* layout;
    size_t button_size;
    // pre-stored palettes
    QPalette *inc_picked, *excl_picked,  *alt_picked, *unpicked;

    // data
    bool has_alternate;
    jfTAG_STATUS tstatus;
};
//=================================================================
// wraps a QListWidget and feeds it jfTagListings
class jfTagStatusPicker : public QWidget {
    Q_OBJECT
  public:
    jfTagStatusPicker(QString header, bool do_alternate, bool force_scrollar = false, QWidget* parent = NULL);
    // load and get tags
    bool SetTagList(jfTagListing* in_tags);
    bool ChangeTagStatuses(jfTagListing* in_tags);
    bool SetOrChangeTags(jfTagListing* in_tags);
    jfTagListing* GetTagList();
    bool SetDefault(const QStringList* tags);
    bool SetDefault(const QString tag_array[], size_t tagcount);
  public slots:
    void handleStatusChange();
  protected:
    // helper methods
    size_t ListSize() const;
    bool SetListFromTags();
    bool LoadStatusesToList();
    bool SaveStatusesFromList();
    void ClearList();
    // GUI items
    QLabel* top_label;
    QListWidget* main_list;
    QVBoxLayout* tsizer;
    std::vector<jfTagRowView*> tagview_list;

    // data
    jfTagListing* tag_data;
    bool has_alternate;
    bool internal;
};

/*****************************************************************************/
