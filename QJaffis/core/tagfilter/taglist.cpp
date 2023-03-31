//***************************************************************************
// Name:        taglist.cpp
// Author :     John Q Metro
// Purpose :    Groupings of 'tags' (string ids attached to fanfics)
// Created:     August 13, 2022
// Updated:
//***************************************************************************
#include "taglist.h"

#ifndef UTILS2_H_INCLUDED
  #include "../utils/utils2.h"
#endif // UTILS2_H_INCLUDED
//***************************************************************************


jfGeneralTagList::jfGeneralTagList(Qt::CaseSensitivity cs_in) {
    set = false;
    cs = cs_in;
}
// +++++++++++++++++++++++++++++++++++++++
bool jfGeneralTagList::IsSet() const {
    return set;
}
QString jfGeneralTagList::Name() const {
    return name;
}
int jfGeneralTagList::Count() const {
    return excludes.length();
}
bool jfGeneralTagList::IsEmpty() const {
    return (set == false) || excludes.isEmpty();
}
QString jfGeneralTagList::CSV() const {
    return csv_excludes;
}
const QStringList* jfGeneralTagList::getList() const {
    return &excludes;
}
QString jfGeneralTagList::ToString() const {
    if (!set) return "";
    jfOutString rs;
    rs << name << csv_excludes;
    return QString(rs);
}
// +++++++++++++++++++++++++++++++++++++++
// set
bool jfGeneralTagList::SetFromString(const QString& source) {
    jfLineParse parsex = jfLineParse(source);
    if (parsex.Num() == 2) {
        QString rname = parsex.UnEscStr(0).trimmed();
        if (rname.isEmpty()) return false;
        if (SetName(parsex.UnEscStr(0))) {
            QString csvlist = parsex.UnEscStr(1).trimmed();
            excludes.clear();
            if (csvlist.isEmpty()) {
                csv_excludes.clear();
            }
            else {
                QStringList csvparts = csvlist.split(",");

                for (int csvi = 0; csvi < csvparts.length(); csvi++) {
                    QString rpart = csvparts.at(csvi).trimmed();
                    if (rpart.isEmpty()) continue;
                    else AddTag(rpart);
                }
                MakeCSV();
                return true;
            }
            set = true;
            return true;
        }
        else return false;
    }
    else return false;
}
//--------------------------------------------
bool jfGeneralTagList::AddTag(const QString &newvalue) {
    QString tvalue = newvalue.trimmed();
    if (tvalue.isEmpty()) return false;
    else {
        if (excludes.contains(tvalue,cs)) return false;
        else {
            excludes.append(tvalue);
            MakeCSV();
            return true;
        }
    }
}
bool jfGeneralTagList::SetName(const QString& new_name) {
    QString xname = new_name.trimmed();
    if (xname.isEmpty()) return false;
    else {
        name = xname;
        set = true;
        return true;
    }
}
// +++++++++++++++++++++++++++++++++++++++
void jfGeneralTagList::MakeCSV() {
    if (excludes.isEmpty()) csv_excludes = "";
    else csv_excludes = excludes.join(",");
}
// *****************************************************************************************************
jfGeneralTagListsGroup::jfGeneralTagListsGroup(const QString& group_name_in) {
    group_name = group_name_in;
}
// +++++++++++++++++++++++++++++++++++++++
// info
size_t jfGeneralTagListsGroup::ListCount() const {
    return list_of_lists.size();
}
// +++++++++++++++++++++++++++++++++++++++
// names
QString jfGeneralTagListsGroup::GetName() const {
    return group_name;
}
// ----------------------
QStringList jfGeneralTagListsGroup::ExcludeListNames() const {
    QStringList list_names;
    list_names.reserve(list_of_lists.size());
    for (jfGeneralTagList* const list : list_of_lists) {
        list_names.append(list->Name());
    }
    return list_names;
}
// +++++++++++++++++++++++++++++++++++++++
// getting single items
jfGeneralTagList* jfGeneralTagListsGroup::AccessListForName(const QString& name) {
    if (list_of_lists.isEmpty()) return NULL;
    for (jfGeneralTagList* const list : list_of_lists) {
        if ((list->Name()) == name.trimmed()) return list;
    }
    return NULL;
}
// ------------------------
const jfGeneralTagList* jfGeneralTagListsGroup::GetListForName(const QString& name) const {
    if (list_of_lists.isEmpty()) return NULL;
    for (jfGeneralTagList* const list : list_of_lists) {
        if ((list->Name()) == name.trimmed()) return list;
    }
    return NULL;
}
// ------------------------
QSet<QString>* jfGeneralTagListsGroup::SetForName(const QString& name) const {
    const jfGeneralTagList* list = GetListForName(name);
    if (list == NULL) return NULL;
    QSet<QString>* result = new QSet<QString>();
    result->reserve(list->Count());
    for (QString value : *(list->getList())) {
        result->insert(value);
    }
    return result;
}
// +++++++++++++++++++++++++++++++++++++++
// adding or removing single items
// ------------------------
jfGeneralTagList* jfGeneralTagListsGroup::ExtractListForName(const QString& name) {
    jfGeneralTagList* extracted = NULL;
    for (int i = 0; i < list_of_lists.size(); i++) {
        jfGeneralTagList* current = list_of_lists.at(i);
        if ((current->Name()) == name.trimmed()) {
            extracted = current;
            list_of_lists.removeAt(i);
            break;
        }
    }
    return extracted;
}
// ------------------------
bool jfGeneralTagListsGroup::AddList(jfGeneralTagList* new_list) {
    if (new_list == NULL) return false;
    for (jfGeneralTagList* const list : list_of_lists) {
        if ((list->Name()) == (new_list->Name())) return false;
    }
    list_of_lists.append(new_list);
    return true;
}
// ------------------------
bool jfGeneralTagListsGroup::AddOrReplaceList(jfGeneralTagList* new_list, bool delete_old) {
    if (new_list == NULL) return false;
    jfGeneralTagList* old_list = NULL;
    for (int i = 0; i < list_of_lists.size(); i++) {
        jfGeneralTagList* current = list_of_lists.at(i);
        if (new_list == current) return true; // tricky

        if ((current->Name()) == (new_list->Name())) {
            old_list = current;
            list_of_lists.replace(i,new_list);
            break;
        }
    }
    // case A: add
    if (old_list != NULL) {
        if (delete_old) delete old_list;
    }
    else list_of_lists.append(new_list);
    return true;
}

// +++++++++++++++++++++++++++++++++++++++
// getting tag lists/sets
// ------------------------
QList<const jfGeneralTagList*> jfGeneralTagListsGroup::GetListsForNames(const QStringList& names) const {
    QList<const jfGeneralTagList*> result;
    if (names.isEmpty()) return result;
    result.reserve(names.size());
    for(QString name : names) {
        const jfGeneralTagList* list = GetListForName(name);
        if (list != NULL) result.append(list);
    }
    return result;
}
// ------------------------
QStringList* jfGeneralTagListsGroup::MergedListForNames(const QStringList& names) const {
    QList<const jfGeneralTagList*> lists_to_use = GetListsForNames(names);
    // setting up the result list
    QStringList* result = new QStringList();
    if (lists_to_use.isEmpty()) return result;
    int sum_size = 0;
    for (const jfGeneralTagList* list : lists_to_use) {
        sum_size += list->Count();
    }
    result->reserve(sum_size);

    // getting and concatenating the arrays
    for (const jfGeneralTagList* list : lists_to_use) {
        result->append(*(list->getList()));
    }
    return result;
}
// ------------------------
QSet<QString>* jfGeneralTagListsGroup::MergedSetForNames(const QStringList& names) const {
    QList<const jfGeneralTagList*> lists_to_use = GetListsForNames(names);
    // setting up the result list
    QSet<QString>* result = new QSet<QString>();
    if (lists_to_use.isEmpty()) return result;
    int sum_size = 0;
    for (const jfGeneralTagList* list : lists_to_use) {
        sum_size += list->Count();
    }
    result->reserve(sum_size);

    // getting and concatenating the arrays
    for (const jfGeneralTagList* list : lists_to_use) {
        for (QString tag : *(list->getList())) {
            result->insert(tag);
        }
    }
    return result;
}
// +++++++++++++++++++++++++++++++++++++++
// file i/o
int jfGeneralTagListsGroup::LoadFromFile(jfFileReader* sourceReader) {
    const QString fname = "jfGeneralTagListsGroup::LoadFromFile";

    if ((sourceReader != NULL) && (sourceReader->IsOpenBeforeEnd())) {
        // read the number of lists
        unsigned long list_count;
        bool okay = sourceReader->ReadULong(list_count, fname);
        if (!okay) return -1;
        // then read the lines
        int read_count = 0;

        while (read_count < list_count) {
            QString cline;
            okay = sourceReader->ReadLine(cline, fname);
            if (!okay) return false;
            if (cline.isEmpty() || cline.startsWith("#")) continue;
            okay = MakeAndAdd(cline);
            if (!okay) {
                sourceReader->BuildError("The line '" + cline + "' could not be parsed.");
                return read_count;
            }
            read_count ++;
        }
        return read_count;
    }
    else return -1;
}
// --------------------------------
bool jfGeneralTagListsGroup::AddToFileStream(QTextStream* outfile) const {
    if (outfile == NULL) return false;
    (*outfile) << list_of_lists.size() << "\n";
    if (outfile->status() != QTextStream::Ok) return false;
    // writing the lists
    for (const jfGeneralTagList* const list : list_of_lists) {
        if (list != NULL) {
            (*outfile) << (list->ToString()) << "\n";
            if (outfile->status() != QTextStream::Ok) return false;
        }
    }
    return true;
}
// +++++++++++++++++++++++++++++++++++++++
jfGeneralTagListsGroup::~jfGeneralTagListsGroup() {
    ClearLists();
}
// +++++++++++++++++++++++++++++++++++++++
void jfGeneralTagListsGroup::ClearLists() {
    while (list_of_lists.isEmpty() == false) {
        jfGeneralTagList* to_delete = list_of_lists.takeLast();
        if (to_delete != NULL) delete to_delete;
    }
    list_of_lists.clear();
}
// +++++++++++++++++++++++++++++++++++++++
bool jfGeneralTagListsGroup::MakeAndAdd(const QString& source) {
    jfGeneralTagList* new_list = new jfGeneralTagList(Qt::CaseSensitive);
    bool set_okay = new_list->SetFromString(source);
    if (set_okay) {
        return AddOrReplaceList(new_list, true);
    }
    else {
        delete new_list;
        return false;
    }
}

