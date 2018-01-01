/***************************************************************
 * Name:      makeparser.cpp
 * Purpose:   a single function to make a fic parser
 * Author:    John Q Metro
 * Created:   July 10, 2016
 * Updated:   July 10, 2016
 *
 **************************************************************/
#ifndef MAKEPARSER_H
  #include "makeparser.h"
#endif // MAKEPARSER_H
//-------------------------
#ifndef AO3PARSER_H
  #include"ao3parser.h"
#endif // AO3PARSER_H
#ifndef FFNPARSER_H
  #include "ffnparser.h"
#endif // FFNPARSER_H
#ifndef HPFPARSER_H
  #include "hpfparser.h"
#endif // HPFPARSER_H
#ifndef MMOPARSER_H
  #include "mmoparser.h"
#endif // MMOPARSER_H
/**************************************************************/
jfStoryPartParseBase* MakeTypedFicParser(jf_FICTYPE in_ftype) {
  if (in_ftype == jfft_FIM) {
    return new jfFIM_FicPartParser();
  }
  else if (in_ftype == jfft_FFN) {
    return new jfFFN_FicPartParser();
  }
  else if (in_ftype == jfft_AO3) {
    return new jfAO3_FicPartParser();
  }
  else if (in_ftype == jfft_HPF) {
    return new jfHPF_FicPartParser();
  }
  else if (in_ftype == jfft_MMO) {
    return new jfMMO_FicPartParser();
  }
  else return NULL;
}

/**************************************************************/

