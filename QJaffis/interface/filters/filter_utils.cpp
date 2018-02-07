/******************************************************************************
Name    :   filter_utils.cpp
Author  :   John Q Metro
Purpose :   A filter editor creator, and anything else that comes to mind
Created :   December 27, 2010
Conversion to Qt Started Oct 24, 2013
Updated :   January 21, 2018
******************************************************************************/
#ifndef FILTER_UTILS_H_INCLUDED
  #include "filter_utils.h"
#endif // FILTER_UTILS_H_INCLUDED
//-----------------------------------------------------------
#ifndef EXPRESSION_H
  #include "../../core/filters/base/expression.h"
#endif // EXPRESSION_H
#ifndef MOREFILTERS1_H_INCLUDED
  #include "../../core/filters/extended/morefilters1.h"
#endif
#ifndef SEXP_FILTERS_H_INCLUDED
  #include "../../core/filters/base/sexp_filters.h"
#endif // SEXP_FILTERS_H_INCLUDED
#ifndef BFILT_EDIT_H_INCLUDED
  #include "bfilt_edit.h"
#endif // BFILT_EDIT_H_INCLUDED

#ifndef FILTERS_EDIT1_H_INCLUDED
  #include "extended/filters_edit1.h"
#endif // FILTERS_EDIT1_H_INCLUDED
#ifndef SFILT_EDIT_H_INCLUDED
  #include "base/sfilt_edit.h"
#endif // SFILT_EDIT_H_INCLUDED
#ifndef PAIRS_EDIT_H_INCLUDED
  #include "extended/pairs_edit.h"
#endif // PAIRS_EDIT_H_INCLUDED
#ifndef SEXP_EXTENDED_H_INCLUDED
  #include "extended/sexp_extended.h"
#endif // SEXP_EXTENDED_H_INCLUDED

#ifndef DRANGE_EDIT_H_INCLUDED
  #include "extended/drange_edit.h"
#endif // DRANGE_EDIT_H_INCLUDED

#ifndef FIM1_FILEDIT_H_INCLUDED
  #include "fim/fim1_filedit.h"
#endif // FIM1_FILEDIT_H_INCLUDED

#ifndef AO3_STRINGFILTERS_H_INCLUDED
  #include "ao3/ao3_sexpeditors.h"
#endif // AO3_STRINGFILTERS_H_INCLUDED
#ifndef AO3_SPECIALS1_H_INCLUDED
  #include "ao3/ao3_special1.h"
#endif // AO3_SPECIALS1_H_INCLUDED
#ifndef AO3_SPECIAL2_H_INCLUDED
  #include "ao3/ao3_special2.h"
#endif // AO3_SPECIAL2_H_INCLUDED
#ifndef AO3_SPECIAL3_H
  #include "ao3/ao3_special3.h"
#endif // AO3_SPECIAL3_H
#ifndef AO3_SPECIALS1_H_INCLUDED
  #include "../../core/filters/ao3/ao3_specials1.h"
#endif // AO3_SPECIALS1_H_INCLUDED
#ifndef AO3_SPECIAL2_H_INCLUDED
  #include "../../core/filters/ao3/ao3_specials2.h"
#endif // AO3_SPECIAL2_H_INCLUDED
#ifndef AO3_SPECIALS3_H
  #include "../../core/filters/ao3/ao3_specials3.h"
#endif // AO3_SPECIALS3_H
#ifndef FFN_SPECIAL1_H
  #include "../../core/filters/ffn/ffn_special1.h"
#endif // FFN_SPECIAL1_H


#ifndef FIM2_FILEDIT_H
    #include "fim/fim2_filedit.h"
#endif // FIM2_FILEDIT_H
#ifndef FIM3_FILEDIT_H
    #include "fim/fim3_filedit.h"
#endif // FIM3_FILEDIT_H
#ifndef FFN_FILTEREDIT1_H
  #include "ffn/ffn_filteredit1.h"
#endif // FFN_FILTEREDIT1_H

//---------------------------------------------------
#include <assert.h>


//***************************************************************************
// creates an appropriate editor using the provided links
jfBaseFilterEditor* MakeFilterEditor(QString in_typeid, QWidget* parent, jfFilterMap* containing_map,
                           jfBaseFilter* input_filter, bool isglobal) {
  // we first check the inputs. anything wrong results in assert faliure
  assert(parent!=NULL);
  assert(containing_map!=NULL);
  assert(input_filter!=NULL);
  // variables
  // filters of the right type
  jfUrlFilter*          a01;
  jfExpressionFilter*   a02;

  jfAuthExprFilter*     a03;
  jfCharExprFilter*     a04;
  jfFFN_CategoryExprFilter* a05;
  jfWordCountFilter*    a06;

  jfFFNGenresFilter*       a07;
  jfPairFilterSingle*   a08;
  jfPairFilterList*     a09;
  jfPairFilterMultiple* a10;

  jfCompletedFilter*    a11;
  jfLanguageExprFilter* a12;
  jfFFNRatingFilter*    a13;
  jfUpdatedDateFilter*  a14;
  jfPublishedDateFilter*a15;

  jfFimThumbsFilter*    a16;
  jfFIMGenreFilter*     a17;
  jfFIMCharacterFilter* a18;
  jfFimRatingFilter*    a19;

  jfExtraTagFilter*     a20;
  jfAO3FandomFilter*  a21;
  jfAO3RatingFilter*      a22;
  jfAO3OrientationFilter* a23;
  jfAO3PairFilter*        a24;
  jfAO3WarnFilter*        a25;
  jfAO3KudoFilter*        a26;
  jfFFNFavsFilter*        a27;

  jfFimThumbPercentFilter* a28;
  jfFIMGroupSCFilter* a29;
  jfFIMGroupMCFilter* a30;
  jfFIM_ShortDesc_ExprFilter*  a31;
  jfFIM_DualDesc_ExprFilter*   a32;

  jfFIMContentTypeFilter* a33;
  jfFIMWarningsFilter* a34;


  // filter link
  jfBaseFilterEditor* result;
  // the main check
  if (in_typeid=="UrlFilter") {
    a01 = dynamic_cast<jfUrlFilter*>(input_filter);
    result = new jfUrlFilterEditor(containing_map,a01,parent);
  }
  else if (in_typeid=="ExprFilter") {
    a02 = dynamic_cast<jfExpressionFilter*>(input_filter);
    result = new jfExprFilterEditor(containing_map,isglobal,a02,parent);
  }

  else if (in_typeid=="AuthorFilter") {
    a03 = dynamic_cast<jfAuthExprFilter*>(input_filter);
    result = new jfAuthorFilterEditor(containing_map,a03,parent);
  }
  else if (in_typeid=="CharacterFilter") {
    a04 = dynamic_cast<jfCharExprFilter*>(input_filter);
    result = new jfCharFilterEditor(containing_map,a04,parent);
  }
  else if (in_typeid=="CategoryFilter") {
    a05 = dynamic_cast<jfFFN_CategoryExprFilter*>(input_filter);
    result = new jfFFN_CategoryFilterEditor(containing_map,a05,parent);
  }
  else if (in_typeid=="WordCountFilter") {
    a06 = dynamic_cast<jfWordCountFilter*>(input_filter);
    result = new jfWCFilterEditor(containing_map,a06,parent);
  }
  else if (in_typeid=="FFNTagFilter") {
    a07 = dynamic_cast<jfFFNGenresFilter*>(input_filter);
    result = new jfFFNGenresFilterEditor(containing_map,a07,parent);
  }
  else if (in_typeid=="PairFilterSingle") {
    a08 = dynamic_cast<jfPairFilterSingle*>(input_filter);
    result = new jfPairFS_Editor(containing_map,a08,parent);
  }
  else if (in_typeid=="PairFilterList") {
    a09 = dynamic_cast<jfPairFilterList*>(input_filter);
    result = new jfPairFL_Editor(containing_map,a09,parent);
  }
  else if (in_typeid=="PairFilterMultiple") {
    a10 = dynamic_cast<jfPairFilterMultiple*>(input_filter);
    result = new jfPairFM_Editor(containing_map,a10,parent);
  }
  else if (in_typeid=="CompletedFilter") {
    a11 = dynamic_cast<jfCompletedFilter*>(input_filter);
    result = new jfCompletedFilterEditor(a11,containing_map,parent);
  }
  else if (in_typeid=="LanguageFilter") {
    a12 = dynamic_cast<jfLanguageExprFilter*>(input_filter);
    result = new jfLanguageFilterEditor(containing_map,a12,parent);
  }
  else if (in_typeid=="FFNRatingFilter") {
    a13 = dynamic_cast<jfFFNRatingFilter*>(input_filter);
    result = new jfFFN_RatingFilterEditor(a13,containing_map,parent);
  }
  else if (in_typeid=="UpdatedDateFilter") {
    a14 = dynamic_cast<jfUpdatedDateFilter*>(input_filter);
    result = new jfUpdatedDateFilterEditor(a14, containing_map,parent);
  }
  else if (in_typeid=="PublishedDateFilter") {
    a15 = dynamic_cast<jfPublishedDateFilter*>(input_filter);
    result = new jfPublishedDateFilterEditor(a15,containing_map,parent);
  }
  else if (in_typeid=="FimThumbsFilter") {
    a16 = dynamic_cast<jfFimThumbsFilter*>(input_filter);
    result = new jfFimThumbsFilterEditor(containing_map,a16,parent);
  }
  else if (in_typeid=="FIMGenreFilter") {
    a17 = dynamic_cast<jfFIMGenreFilter*>(input_filter);
    result = new jfFimGenreFilterEditor(containing_map,a17,parent);
  }
  else if (in_typeid=="FIMCharacterFilter") {
    a18 = dynamic_cast<jfFIMCharacterFilter*>(input_filter);
    result = new jfFimCharacterFilterEditor(containing_map,a18,parent);
  }
  else if (in_typeid=="FIMContentTypeFilter") {
    a33 = dynamic_cast<jfFIMContentTypeFilter*>(input_filter);
    result = new jfFimContentTypeFilterEditor(containing_map,a33,parent);
  }
  else if (in_typeid=="FIMWarningFilter") {
    a34 = dynamic_cast<jfFIMWarningsFilter*>(input_filter);
    result = new jfFimWarningsFilterEditor(containing_map,a34,parent);
  }
  else if (in_typeid=="FimRatingFilter") {
    a19 = dynamic_cast<jfFimRatingFilter*>(input_filter);
    result = new jfFIM_RatingFilterEditor(a19,containing_map,parent);
  }
  else if (in_typeid=="ExtraTagFilter") {
    a20 = dynamic_cast<jfExtraTagFilter*>(input_filter);
    result = new jfExtraTagFilterEditor(containing_map,a20,parent);
  }
  else if (in_typeid=="AO3FandomFilter") {
    a21 = dynamic_cast<jfAO3FandomFilter*>(input_filter);
    result = new jfAO3FandomFilterEditor(a21,containing_map,parent);
  }
  else if (in_typeid=="AO3RatingFilter") {
    a22 = dynamic_cast<jfAO3RatingFilter*>(input_filter);
    result = new jfAO3_RatingFilterEditor(a22,containing_map,parent);
  }
  else if (in_typeid=="AO3OrientationFilter") {
    a23 = dynamic_cast<jfAO3OrientationFilter*>(input_filter);
    result = new jfAO3_OrientFilterEditor(a23,containing_map,parent);
  }
  else if (in_typeid=="AO3PairFilter") {
    a24 = dynamic_cast<jfAO3PairFilter*>(input_filter);
    result = new jfAO3_PairingFilterEditor(a24,containing_map,parent);
  }
  else if (in_typeid=="AO3WarnFilter") {
    a25 = dynamic_cast<jfAO3WarnFilter*>(input_filter);
    result = new jfAO3_WarnFilterEditor(a25,containing_map,parent);
  }
  else if (in_typeid=="AO3KudoCountFilter") {
    a26 = dynamic_cast<jfAO3KudoFilter*>(input_filter);
    result = new jfAO3KudoFilterEditor(containing_map,a26,parent);
  }
  else if (in_typeid=="FFNFavsFilter") {
    a27 = dynamic_cast<jfFFNFavsFilter*>(input_filter);
    result = new jfFFNFavsFilterEditor(containing_map,a27,parent);
  }
  else if (in_typeid=="FimThumbPercentFilter") {
    a28 = dynamic_cast<jfFimThumbPercentFilter*>(input_filter);
    result = new jfFimThumbPercentFilterEditor(containing_map,a28,parent);
  }
  else if (in_typeid=="FIMGroupSCFilter") {
    a29 = dynamic_cast<jfFIMGroupSCFilter*>(input_filter);
    result = new jfFIMGroupSCFilterEditor(containing_map,a29,parent);
  }
  else if (in_typeid=="FIMGroupMCFilter") {
    a30 = dynamic_cast<jfFIMGroupMCFilter*>(input_filter);
    result = new jfFIMGroupMCFilterEditor(containing_map,a30,parent);
  }
  else if (in_typeid=="FIMShortDescFilter") {
    a31 = dynamic_cast<jfFIM_ShortDesc_ExprFilter*>(input_filter);
    result = new jfFIMShortDesc_FilterEditor(containing_map,a31,parent);
  }
  else if (in_typeid=="FIMDualDescFilter") {
    a32 = dynamic_cast<jfFIM_DualDesc_ExprFilter*>(input_filter);
    result = new jfFIMDualDesc_FilterEditor(containing_map,a32,parent);
  }

  else assert(false);
  // done
  return result;
}
//***************************************************************************
