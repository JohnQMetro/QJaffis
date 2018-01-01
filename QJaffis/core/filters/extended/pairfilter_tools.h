//***************************************************************************
// Name:        pairfilter_tools.h
// Author :     John Q Metro
// Purpose :    Relationship/Pairing filters are complicated enough to make a
//              separate source file for utilities and data
// Created:     December 29, 2010
// Conversion to QT Started April 11, 2013
// Updated:     January 1, 2011
//***************************************************************************
//-----------------------------------------------------------------
#ifndef PAIRFILTER_TOOLS_H_INCLUDED
#define PAIRFILTER_TOOLS_H_INCLUDED
#endif // PAIRFILTER_TOOLS_H_INCLUDED
//---------------------------------------------------------------
#ifndef BASECOLLECT_H_INCLUDED
  #include "../../structs/basecollect.h"
#endif // BASECOLLECT_H_INCLUDED
/*
#ifndef VECTORS_H_INCLUDED
  #include "../../structs/vectors.h"
#endif // VECTORS_H_INCLUDED
*/

#include <map>
//***************************************************************************
// a method to strip duplicate characters from a string
bool StripDuplicateChars(QString& delm_str);
//---------------------------------------------------------------------------
// flag holder (to shorten parameters)
class jfPF_flaghold {
  public:
    // the flags
    bool usetokens;
    bool sidespaces;
    bool nofriend;
    bool nospace;
    QChar curr_char;
    // constructor
    jfPF_flaghold();
    // to and from string
    QString ToString() const;
    bool FromString(QString inval);
  protected:
};
//---------------------------------------------------------------------------
// used in the LookForPairSingle  function.. very restricted
bool LookForPatternString(QString lookin, QString pattern, const jfPF_flaghold& flags);
bool LookForPatternToken(const QStringList* lookin, QString pattern, const jfPF_flaghold& flags);
// function to look for a name pair using one character
bool LookForPairSingleString(QString lookin, QString name1, QString name2, const jfPF_flaghold& flags);
bool LookForPairSingleToken(const QStringList* lookin, QString name1, QString name2, const jfPF_flaghold& flags);
// functions to look for a name pair via multiple characters
bool LookForPairString(QString lookin, QString name1, QString name2, QString inchars,jfPF_flaghold& flags);
bool LookForPairToken(const QStringList* lookin, QString name1, QString name2, QString inchars,jfPF_flaghold& flags);
//--------------------------------------------------------------------------
class jfPaircount : public std::map<QString,size_t,ltstr> {
  public:
    bool AddOrIncrement(const QString& inval);
};
//----------------------------------------------------------------------------------
// special class used to keep track of statistics
class jfPairStats {
  public:
    // constructor
    jfPairStats(QString inchars);
    // the data gathering method
    bool LogFind(QString name1, QString name2, QChar inchar);
    bool LogSpecial(QString identifier);
    // file name stuff
    QString GetFilename() const;
    void SetFilename(QString infilename, QString newname);
    // file i/o
    void WriteResults() const;
  protected:
    // helper methods
    QString MakeCharResult(size_t index) const;
    QString MakeNameResult(const jfPaircount::const_iterator& inval, size_t padlength) const;
    // file i/o helper methods
    QTextStream* OpenResult() const;
    void WriteSecHead(QString inname, QTextStream* outfile_in, bool spacebefore=false) const;
    // internal data
    QString sepchars;
    jfPaircount specials;
    jfPaircount maindata;
    std::vector<size_t> charfcount;
    QString filename;
    QString topname;
    size_t maxname_length;
    size_t maxspec_length;
};
//***************************************************************************
