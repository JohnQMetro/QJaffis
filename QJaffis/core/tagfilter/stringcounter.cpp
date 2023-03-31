/*****************************************************************************
Name    : stringcounter.cpp
Basic   : Utility class for counting string matches (useful for checking which
          AO3 extra tags are used the most).
Author  : John Q Metro
Started : July 29, 2022
Updated :

******************************************************************************/
#include "stringcounter.h"

#ifndef UTILS3_H_INCLUDED
  #include "../utils/utils3.h"
#endif // UTILS3_H_INCLUDED
// ---------------------------------------------
#include <QTextStream>
/*****************************************************************************/
// sorting pairs
bool ReverseSortByCount(const QPair<QString,size_t>& first, const QPair<QString,size_t>& second) {
    return first.second > second.second;
}
void ReverseSortPairList(QList<QPair<QString,size_t>>& list_to_sort) {
    std::sort(list_to_sort.begin(), list_to_sort.end(), ReverseSortByCount);
}
QString FormatPair(const QPair<QString,size_t>& pair_to_convert) {
    QString result = QString::number(pair_to_convert.second).rightJustified(6,' ');
    return result + " : " + pair_to_convert.first;
}
// **********************************************************************************************
jfStringCounter::jfStringCounter(bool is_case_sensitive, bool do_record_pairs) {
    case_sensitive = is_case_sensitive;
    string_count.reserve(10000);
    if (do_record_pairs) {
        pair_count = new QHash<QString,size_t>();
        pair_count->reserve(1000000);
    }
    else pair_count = NULL;
    record_pairs = do_record_pairs;
}
//++++++++++++++++++++++++++++++++++++++++
// recording
bool jfStringCounter::RecordItem(const QString& item) {
    QMutexLocker mlock(&mutex);
    return RecordItemZ(item);
}
// -------------------------------
bool jfStringCounter::ClearPairPrevious() {
    QMutexLocker mlock(&mutex);
    if (record_pairs) return false;
    else {
        last_recorded.clear();
        return true;
    }
}
// -------------------------------
bool jfStringCounter::RecordList(const QStringList& items, bool pair_clear) {
    QMutexLocker mlock(&mutex);
    if (items.isEmpty()) return false;
    int recorded_count = 0;
    for (int item_dex = 0; item_dex < items.size(); item_dex++) {
        if (RecordItemZ(items.at(item_dex))) recorded_count++;
    }
    if (pair_clear) last_recorded.clear();
    return (recorded_count > 0);
}
//++++++++++++++++++++++++++++++++++++++++
int jfStringCounter::DifferentItemCount() const {
    QMutexLocker mlock(&mutex);
    return string_count.size();
}
// --------------------------------------
int jfStringCounter::DifferentPairCount() const {
    QMutexLocker mlock(&mutex);
    return (pair_count == NULL) ? 0 : pair_count->size();
}
//++++++++++++++++++++++++++++++++++++++++
bool jfStringCounter::SaveRecordedCounts(const QString& filename, size_t downto) const {
    if (filename.isEmpty()) return false;
    QList<QPair<QString,size_t>>* pair_list = NULL;
    pair_list = GetAllPairs(string_count);
    if (pair_list == NULL) return false;
    ReverseSortPairList(*pair_list);
    bool saved_okay = SaveSortedList(filename, *pair_list, downto);
    delete pair_list;
    return saved_okay;
}
// -----------------------------------------
bool jfStringCounter::SaveRecordedPairs(const QString& filename, size_t downto) const {
    if (filename.isEmpty() || (!record_pairs)) return false;
    QList<QPair<QString,size_t>>* pair_list = NULL;
    pair_list = GetAllPairs(*pair_count);
    if (pair_list == NULL) return false;
    ReverseSortPairList(*pair_list);
    bool saved_okay = SaveSortedList(filename, *pair_list, downto);
    delete pair_list;
    return saved_okay;
}
//++++++++++++++++++++++++++++++++++++++++
jfStringCounter::~jfStringCounter() {
    if (pair_count != NULL) {
        delete pair_count;
        pair_count = NULL;
    }
    string_count.clear();
}
//++++++++++++++++++++++++++++++++++++++++
void jfStringCounter::InnerRecordItem(const QString& item, QHash<QString, size_t>& target) {
    QHash<QString,size_t>::iterator recorded_item = target.find(item);
    if (recorded_item == target.end()) {
        target.insert(item,1);
    }
    else {
        recorded_item.value() += 1;
    }
}
// -------------------------------------
bool jfStringCounter::RecordPair(const QString second_item) {
    if (last_recorded.isEmpty()) return false;
    else if (pair_count == NULL) return false;
    else {
        // records last_recorded,second_item
        QString pair = last_recorded + "," + second_item;
        InnerRecordItem(pair, *pair_count);
        return true;
    }
}
//--------------------------------------
// full record, but without a mutex
bool jfStringCounter::RecordItemZ(const QString& item) {
    if (item.isNull()) return false;
    QString rkey = (case_sensitive) ? item.trimmed() : item.toLower().trimmed();
    if (rkey.isEmpty()) return false;
    InnerRecordItem(rkey, string_count);
    if (record_pairs) {
        RecordPair(item);
        last_recorded = rkey;
    }
    return true;
}
// -------------------------------------
QList<QPair<QString,size_t>>* jfStringCounter::GetAllPairs(const QHash<QString, size_t>& target) const {
    QList<QPair<QString,size_t>>* result = new QList<QPair<QString,size_t>>();
    if (target.size() > 0) {
        result->reserve(target.size());
        QHash<QString,size_t>::const_iterator i;
        for (i = target.begin(); i != target.end(); ++i) {
            result->append(qMakePair(i.key(), i.value()));
        }
    }
    return result;
}
// -------------------------------------------------------------------------------
bool jfStringCounter::SaveSortedList(const QString& filename, const QList<QPair<QString,size_t>>& source_list, size_t downto) const {
    QTextStream* output_file = GetOpenFile(filename, true);
    if (output_file == NULL) return false;
    for (int i = 0; i < source_list.size(); i++) {
        if (source_list.at(i).second <= downto) break;
        QString outline = FormatPair(source_list.at(i));
        (*output_file) << outline << endl;
    }
    bool closed_okay = CloseFileStream(output_file, true);
    return closed_okay;
}

/*****************************************************************************/
