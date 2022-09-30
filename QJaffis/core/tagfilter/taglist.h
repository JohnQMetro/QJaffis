//***************************************************************************
// Name:        taglist.h
// Author :     John Q Metro
// Purpose :    Groupings of 'tags' (string ids attached to fanfics)
// Created:     August 13, 2022
// Updated:
//***************************************************************************
#pragma once

#ifndef UTILS2_H_INCLUDED
  #include "../utils/utils2.h"
#endif // UTILS2_H_INCLUDED
//-------------------------------------------------------------------------
#include <QString>
#include <QStringList>
#include <QList>
#include <QSet>
#include <QTextStream>
//***************************************************************************

class jfGeneralTagList {
  public:
    jfGeneralTagList(Qt::CaseSensitivity cs_in);
    // get
    bool IsSet() const;
    QString Name() const;
    int Count() const;
    bool IsEmpty() const;
    QString CSV() const;
    QString ToString() const;
    const QStringList* getList() const;
    // set
    bool SetName(const QString& new_name);
    bool SetFromString(const QString& source);
    bool AddTag(const QString &newvalue);
  protected:
    void MakeCSV();
    Qt::CaseSensitivity cs;
    QString name;
    QStringList excludes;
    QString csv_excludes;
    bool set;
};
//------------------------------------------------------------------
class jfGeneralTagListsGroup {
public:
    jfGeneralTagListsGroup(const QString& group_name_in);
    // info
    size_t ListCount() const;
    // names
    QString GetName() const;
    QStringList ExcludeListNames() const;

    // getting single items
    jfGeneralTagList* AccessListForName(const QString& name);
    const jfGeneralTagList* GetListForName(const QString& name) const;
    QSet<QString>* SetForName(const QString& name) const;

    // adding or removing single items
    jfGeneralTagList* ExtractListForName(const QString& name);
    bool AddList(jfGeneralTagList* new_list);
    bool AddOrReplaceList(jfGeneralTagList* new_list, bool delete_old);


    // getting tag lists/sets
    QList<const jfGeneralTagList*> GetListsForNames(const QStringList& names) const;
    QStringList* MergedListForNames(const QStringList& names) const;
    QSet<QString>* MergedSetForNames(const QStringList& names) const;

    // file i/o
    int LoadFromFile(jfFileReader* sourceReader);
    bool AddToFileStream(QTextStream* outfile) const;

    void ClearLists();
    virtual ~jfGeneralTagListsGroup();
protected:

    bool MakeAndAdd(const QString& source);

    QString group_name;
    QList<jfGeneralTagList*> list_of_lists;
};

//***************************************************************************
