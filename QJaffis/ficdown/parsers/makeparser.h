/***************************************************************
 * Name:      makeparser.h
 * Purpose:   a single function to make a fic parser
 * Author:    John Q Metro
 * Created:   July 10, 2016
 * Updated:   July 10, 2016
 *
 **************************************************************/
#ifndef MAKEPARSER_H
  #define MAKEPARSER_H
#endif // MAKEPARSER_H
//-------------------------
#ifndef FICEXTRACT_H_INCLUDED
  #include "../data/ficextract.h"
#endif // FICEXTRACT_H_INCLUDED
#ifndef BASEFIM_H
  #include "basefimparser.h"
#endif // BASEFIM_H
/**************************************************************/
jfStoryPartParseBase* MakeTypedFicParser(jf_FICTYPE in_ftype);
/**************************************************************/
