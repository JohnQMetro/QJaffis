/*****************************************************************************
Name    : stringcounter.h
Basic   : Utility class for counting string matches (useful for checking which
          AO3 extra tags are used the most).
Author  : John Q Metro
Started : July 28, 2022
Updated : March 18, 2023 (added mutex)

******************************************************************************/
#pragma once
// ---------------------------------------------
#include <QString>
#include <QHash>
#include <QStringList>
#include <QPair>
#include <QList>
#include <QMutex>
/*****************************************************************************/
class jfStringCounter {
  public:
    jfStringCounter(bool is_case_sensitive, bool do_record_pairs);
    // recording
    bool RecordItem(const QString& item);
    bool ClearPairPrevious();
    bool RecordList(const QStringList& items, bool pair_clear);
    // reporting
    int DifferentItemCount() const;
    int DifferentPairCount() const;
    // saving
    bool SaveRecordedCounts(const QString& filename, size_t downto) const;
    bool SaveRecordedPairs(const QString& filename, size_t downto) const;

    virtual ~jfStringCounter();
  protected:
    void InnerRecordItem(const QString& item, QHash<QString, size_t>& target);
    bool RecordPair(const QString second_item);
    bool RecordItemZ(const QString& item);
    QList<QPair<QString,size_t>>* GetAllPairs(const QHash<QString, size_t>& target) const;
    bool SaveSortedList(const QString& filename, const QList<QPair<QString,size_t>>& source_list, size_t downto) const;

    mutable QMutex mutex;
    bool case_sensitive;
    QString last_recorded;

    QHash<QString,size_t> string_count;

    /* in AO3, many authors use tags like parts of a sentence, so pairs might
     * be able to reveal patters useful in filtering. */
    bool record_pairs;
    QHash<QString,size_t>* pair_count;

};

/*****************************************************************************/
