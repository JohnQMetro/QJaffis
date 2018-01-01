/***************************************************************
 * Name:      ficpart.h
 * Purpose:   Very basic fic download datatypes
 * Author:    John Q Metro
 * Created:   July 3, 2016
 * Updated:   July 3, 2016
 *
 **************************************************************/
#ifndef FICPART_H
#define FICPART_H
#endif // FICPART_H
//------------------------------
#ifndef HTMLSKELTON_H_INCLUDED
  #include "../../core/utils/htmlskelton.h"
#endif // HTMLSKELTON_H_INCLUDED
#ifndef FICEXTRACT_H_INCLUDED
  #include "../../ficdown/data/ficextract.h"
#endif // FICEXTRACT_H_INCLUDED

#include <QString>
/*************************************************************/
// hold a part of a downloaded fanfic
class jfFicPart {
  public:
    jfFicPart();
    jfFicPart(const jfFicPart& source);
    void LoadCoreValues(jfSkeletonParser* inparser) const;

    // the data
    QString part_name;
    QString part_contents;
    size_t partcount, partindex;
    size_t partid;
    QString startnotes, endnotes;

};
//========================================
//a fic part and fic extract pair
class jfFicPair {
  public:
    jfFicPair();
    jfFicExtract* extract;
    jfFicPart* part_one;
};

/**************************************************************/
