/***************************************************************
 * Name:      ficpart.cpp
 * Purpose:   Very basic fic download datatypes
 * Author:    John Q Metro
 * Created:   July 3, 2016
 * Updated:   July 4, 2016
 *
 **************************************************************/
#ifndef FICPART_H
  #include "ficpart.h"
#endif // FICPART_H
//------------------------------
#include <assert.h>
/*************************************************************/
jfFicPart::jfFicPart() {
  partid = 0;
  partindex = 0;
  partcount = 0;
}
//----------------------------------------
jfFicPart::jfFicPart(const jfFicPart& source) {
  part_name = source.part_name;
  part_contents = source.part_contents;
  partid = source.partid;
  partindex = source.partindex;
  partcount = source.partcount;
  startnotes = source.startnotes;
  endnotes = source.endnotes;
}
//-----------------------------------------
void jfFicPart::LoadCoreValues(jfSkeletonParser* inparser) const {
  assert(inparser!=NULL);
  inparser->AddText("FICPART_NAME",part_name);
  inparser->AddText("FICPART_TEXT",part_contents);
  inparser->AddUInt("FICPART_INDEX",partindex);
  inparser->AddText("FICPART_LABEL","Part"+QString::number(partindex));
}
//========================================================
jfFicPair::jfFicPair(){
  extract = NULL;
  part_one = NULL;
}
/**************************************************************/

