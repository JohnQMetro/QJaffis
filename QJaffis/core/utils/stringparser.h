/*****************************************************************************
Name    : stringparser.h
Basic   : String parsing class
Author  : John Q Metro
Started : August 21, 2012 (spit from utils2.h)
Conversion to QT started : March 2, 2013
Updated : March 12, 2023
Notes   :

******************************************************************************/
#ifndef STRINGPARSER_H_INCLUDED
#define STRINGPARSER_H_INCLUDED
#endif // STRINGPARSER_H_INCLUDED
//----------------------------------------
#include <QString>
#include <QtGlobal>
#include <Qt>
#include <QStringList>
#include <QRegularExpression>

/*****************************************************************************/
/* another class used in parsing... this one is for more complicated things than delimited
lists, it is mainly there to simplify keeping track of a few things */
class jfStringParser {
  public:
    // the constructors
    jfStringParser();
    jfStringParser(QString indata, bool incasesen=false);
    // change indata
    void ChangeData(QString indata, bool incasesen=false);
    void Clear();
    // getting basic info
    bool IsReady() const;
    bool IsCaseSensetive() const;
    size_t GetIndex() const;
    size_t GetLength() const;
    QString GetData() const;
    QString GetBlock(size_t innlen) const;
    // changing some controllers
    void SetCaseSen(bool incasesen);
    bool SetIndex(size_t innindex);
    bool GoBack();
    // parsing methods
    // looks for <find>, if found, returns true and moves the index past the substring
    bool MovePast(QString find);
    bool MovePast(const QRegularExpression& find);
    bool MovePastTwice(QString find);
    bool MovePastLimit(QString find, QString limit);
    bool MovePastTwo(QString first, QString second);
    bool MovePastAlt(QString find1, QString find2);
    bool MovePastStart(QString start);
    bool MovePastStartAlt(QString start1, QString start2);
    QString GetSkipped() const;
    /* looks for <find>, if found, returns true, returns everything between the index
    and the start of <find> in outbuf, amd moves the index past the substring */
    bool GetMovePast(QString end, QString& outbuf);
    bool GetMovePast(const QRegularExpression end, QString& outbuf);
    bool MovePastOrEnd(QString endval, QString& outbuf);
    bool GetMovePastPair(QString end1, QString end2, QString& outbuf);
    bool GetMovePastPairI(QString end1, QString end2, QString& outbuf, bool& owhich);
    // special move upto methods
    bool MoveUpto(QString ifind);
    bool GetUpto(QString end, QString& outres);
    // similar to get move past, but tries to get a number
    bool GetMovePastULong(QString end, ulong& outval, QString& outerr);
    bool GetMovePastAltULong(QString end1, QString end2, ulong& outval, QString& outerr);
    /* a combination of MovePast and GetMovePast, return the value between <start> and
    <end> (if found), and move the end past <end> */
    bool GetDelimited(QString start, QString end, QString& outbuf);
    // regular expression extract
    bool GetUsingRegex(const QRegularExpression& regex, bool first_capture, QString& outbuf);
    bool GetULongUsingRegex(const QRegularExpression& regex, bool first_capture, ulong& outval, QString& outerr);
    // similar to GetDelimited, but tries to get a number
    bool GetDelimitedULong(QString start, QString end, ulong& outval, QString& outerr);
    bool GetDelimitedFloat(QString start, QString end, double& outval, QString& outerr);
    /* another version of get delimited which uses a pair of possible ends */
    bool GetDelimitedEndPair(QString start, QString end1,QString end2, QString& outbuf);
    bool GetDelimitedEndPairULong(QString start, QString end1,QString end2, ulong& outval, QString& outerr);
    /* returns true if the string or string at the index starts with a substring */
    bool StartsWith(QString start) const;
    bool StartsWithAtIndex(QString start) const;
    /* locates the first of a pair */
    bool Find1stPair(QString astr, QString bstr, size_t& which, size_t& outloc);
    /* locates the first of a multiple set */
    bool Find1stSet(const QStringList* instring, size_t& which, size_t& outloc) const;
    bool GetMovePastSet(const QStringList* instring, size_t& which, QString& outbuf);
    // * returns everything up to the end
    bool GetToEnd(QString& out);
    bool GetToEndULong(ulong& outval, QString& outerr);
    // gets the last delimited string/number before a substring
    bool GetDelimitedFromEnd(QString start, QString end, QString& outbuf, QString endmarker);
    bool GetDelimitedFromEndULong(QString start, QString end, unsigned long& outbuf, QString& outerr, QString endmarker);
    bool GetAfterLast(QString start, QString& outbuf, QString endmarker="");
    // another custom method, truncates the search after an endmarker to simplify things
    bool ChopAfter(QString endmark, bool chopmarker);
    // nested block extract methods
    bool GetRestOfBlock(const QString& blockstart, const QString& blockend, QString& outbuf);
    bool GetRestofBlockChar(QChar bstart, QChar bend, QString& outbuf);
    // line extract methods
    bool GetLine(QString& outbuf);
    bool GetLineOrTo(const QString& end, QString& outbuf, bool& endfound);
    bool GetLineOrToChar(const QChar end, QString& outbuf, bool& endfound);
    // skip and not skip whitespace
    bool SkipWhitespace();
    bool GetUntilWhitespace(QString& outbuf);
  private:
    size_t CalcSkipAmount(const size_t& lpos) const;
    bool CheckRegex(const QRegularExpression& toMatch) const;
    // internal data...
    QString rawdata;
    QString skipped_data;
    size_t oldindex, mainindex;
    bool ready;
    Qt::CaseSensitivity casesen;
    QRegularExpression xendl;
};
/*****************************************************************************/
