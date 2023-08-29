/******************************************************************************
Name    :   filter_utils.cpp
Author  :   John Q Metro
Purpose :   A filter editor creator, and anything else that comes to mind
Created :   December 27, 2010
Conversion to Qt Started Oct 24, 2013
Updated :   August 24, 2023
******************************************************************************/
#ifndef FILTER_UTILS_H_INCLUDED
  #include "filter_utils.h"
#endif // FILTER_UTILS_H_INCLUDED
//-----------------------------------------------------------
#ifndef LOGGING_H_INCLUDED
  #include "../../core/utils/logging.h"
#endif // LOGGING_H_INCLUDED

#ifndef EXPRESSION_H
  #include "../../core/filters/base/expression.h"
#endif // EXPRESSION_H
#ifndef MOREFILTERS1_H_INCLUDED
  #include "../../core/filters/extended/morefilters1.h"
#endif

#ifndef BFILT_EDIT_H_INCLUDED
  #include "bfilt_edit.h"
#endif // BFILT_EDIT_H_INCLUDED

#include "../../core/filters/base/structpair.h"

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
#ifndef AO3_PERCENT_TAGS_FILTER_H
    #include "../../core/filters/ao3/ao3_percent_tags_filter.h"
#endif // AO3_PERCENT_TAGS_FILTER_H

#ifndef FFN_SPECIAL1_H
  #include "../../core/filters/ffn/ffn_special1.h"
#endif // FFN_SPECIAL1_H

#include "../../core/filters/extended/list_sexp.h"


#ifndef FIM2_FILEDIT_H
    #include "fim/fim2_filedit.h"
#endif // FIM2_FILEDIT_H
#ifndef FIM3_FILEDIT_H
    #include "fim/fim3_filedit.h"
#endif // FIM3_FILEDIT_H
#ifndef FFN_FILTEREDIT1_H
  #include "ffn/ffn_filteredit1.h"
#endif // FFN_FILTEREDIT1_H

#ifndef AO3_LIST_PERCENT_FILTER_EDITOR_H
    #include "ao3/ao3_list_percent_filter_editor.h"
#endif // AO3_LIST_PERCENT_FILTER_EDITOR_H

#include "base/relpair_edit.h"
#include "extended/listexpr_edit.h"

//---------------------------------------------------
#include <assert.h>


//***************************************************************************
// creates an appropriate editor using the provided links
jfBaseFilterEditor* MakeFilterEditor(QWidget* parent,
                                     jfFilterMap* containing_map, bool isglobal,
                                     jfFilterBase* input_filter) {
    const QString fname = "MakeFilterEditor";
  // we first check the inputs. anything wrong results in assert faliure
    /**/JDEBUGLOG(fname,1)
  assert(parent!=NULL);
  assert(containing_map!=NULL);
  assert(input_filter!=NULL);
  /**/JDEBUGLOG(fname,2)
  // variables
  // filters of the right type

  jfExpressionFilter*   a02;

  jfAuthExprFilter*     a03;
  jfCharListExprFilter*     a04;
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
  jfStructuredPairFilter* a24;
  jfAO3WarnFilter*        a25;
  jfFavKudoFilter*        a26;


  jfFimThumbPercentFilter* a28;
  jfFIMGroupSCFilter* a29;
  jfFIMGroupMCFilter* a30;
  jfFIM_ShortDesc_ExprFilter*  a31;
  jfFIM_DualDesc_ExprFilter*   a32;

  jfFIMContentTypeFilter* a33;
  jfFIMWarningsFilter* a34;
  jfAO3ExtraTagsPercentFilter* a35;


  // filter link
  jfBaseFilterEditor* result;
  // the main check
  if (input_filter->GetTypeIdentifier() == EXPRESSION_FILTER_INFO.Identifier()) {
    a02 = dynamic_cast<jfExpressionFilter*>(input_filter);
    result = new jfExprFilterEditor(containing_map,isglobal,a02,parent);
  }

  else if (input_filter->GetTypeIdentifier() == AUTHOR_FILTER_INFO.Identifier()) {
    a03 = dynamic_cast<jfAuthExprFilter*>(input_filter);
    result = new jfAuthorFilterEditor(a03,parent);
  }
  else if (input_filter->GetTypeIdentifier() == CHARACTER_LIST_FILTER_INFO.Identifier()) {
    a04 = dynamic_cast<jfCharListExprFilter*>(input_filter);
    result = new jfCharacterListExprFilterEditor(a04,parent);
  }
  else if (input_filter->GetTypeIdentifier() =="CategoryFilter") {
    a05 = dynamic_cast<jfFFN_CategoryExprFilter*>(input_filter);
    result = new jfFFN_CategoryFilterEditor(a05,parent);
  }
  else if (input_filter->GetTypeIdentifier() =="WordCountFilter") {
    a06 = dynamic_cast<jfWordCountFilter*>(input_filter);
    result = new jfWCFilterEditor(a06,parent);
  }
  else if (input_filter->GetTypeIdentifier() =="FFNTagFilter") {
    a07 = dynamic_cast<jfFFNGenresFilter*>(input_filter);
    result = new jfFFNGenresFilterEditor(a07,parent);
  }
  else if (input_filter->GetTypeIdentifier() =="PairFilterSingle") {
    a08 = dynamic_cast<jfPairFilterSingle*>(input_filter);
    result = new jfPairFS_Editor(a08,parent);
  }
  else if (input_filter->GetTypeIdentifier() =="PairFilterList") {
    a09 = dynamic_cast<jfPairFilterList*>(input_filter);
    result = new jfPairFL_Editor(a09,parent);
  }
  else if (input_filter->GetTypeIdentifier() =="PairFilterMultiple") {
    a10 = dynamic_cast<jfPairFilterMultiple*>(input_filter);
    result = new jfPairFM_Editor(a10,parent);
  }
  else if (input_filter->GetTypeIdentifier() =="CompletedFilter") {
    a11 = dynamic_cast<jfCompletedFilter*>(input_filter);
    result = new jfCompletedFilterEditor(a11, parent);
  }
  else if (input_filter->GetTypeIdentifier() =="LanguageFilter") {
    a12 = dynamic_cast<jfLanguageExprFilter*>(input_filter);
    result = new jfLanguageFilterEditor(a12,parent);
  }
  else if (input_filter->GetTypeIdentifier() =="FFNRatingFilter") {
    a13 = dynamic_cast<jfFFNRatingFilter*>(input_filter);
    result = new jfFFN_RatingFilterEditor(a13, parent);
  }
  else if (input_filter->GetTypeIdentifier() =="UpdatedDateFilter") {
    a14 = dynamic_cast<jfUpdatedDateFilter*>(input_filter);
    result = new jfUpdatedDateFilterEditor(a14, parent);
  }
  else if (input_filter->GetTypeIdentifier() =="PublishedDateFilter") {
    a15 = dynamic_cast<jfPublishedDateFilter*>(input_filter);
    result = new jfPublishedDateFilterEditor(a15, parent);
  }
  else if (input_filter->GetTypeIdentifier() =="FimThumbsFilter") {
    a16 = dynamic_cast<jfFimThumbsFilter*>(input_filter);
    result = new jfFimThumbsFilterEditor(a16,parent);
  }
  else if (input_filter->GetTypeIdentifier() =="FIMGenreFilter") {
    a17 = dynamic_cast<jfFIMGenreFilter*>(input_filter);
    result = new jfFimGenreFilterEditor(a17,parent);
  }
  else if (input_filter->GetTypeIdentifier() =="FIMCharacterFilter") {
    a18 = dynamic_cast<jfFIMCharacterFilter*>(input_filter);
    result = new jfFimCharacterFilterEditor(a18,parent);
  }
  else if (input_filter->GetTypeIdentifier() =="FIMContentTypeFilter") {
    a33 = dynamic_cast<jfFIMContentTypeFilter*>(input_filter);
    result = new jfFimContentTypeFilterEditor(a33,parent);
  }
  else if (input_filter->GetTypeIdentifier() =="FIMWarningFilter") {
    a34 = dynamic_cast<jfFIMWarningsFilter*>(input_filter);
    result = new jfFimWarningsFilterEditor(a34,parent);
  }
  else if (input_filter->GetTypeIdentifier() =="FimRatingFilter") {
    a19 = dynamic_cast<jfFimRatingFilter*>(input_filter);
    result = new jfFIM_RatingFilterEditor(a19,parent);
  }
  else if (input_filter->GetTypeIdentifier() =="ExtraTagFilter") {
    a20 = dynamic_cast<jfExtraTagFilter*>(input_filter);
    result = new jfExtraTagFilterEditor(a20,parent);
  }
  else if (input_filter->GetTypeIdentifier() =="AO3FandomFilter") {
    a21 = dynamic_cast<jfAO3FandomFilter*>(input_filter);
    result = new jfAO3FandomFilterEditor(a21,parent);
  }
  else if (input_filter->GetTypeIdentifier() =="AO3RatingFilter") {
    a22 = dynamic_cast<jfAO3RatingFilter*>(input_filter);
    result = new jfAO3_RatingFilterEditor(a22,parent);
  }
  else if (input_filter->GetTypeIdentifier() =="AO3OrientationFilter") {
    a23 = dynamic_cast<jfAO3OrientationFilter*>(input_filter);
    result = new jfAO3_OrientFilterEditor(a23,parent);
      /**/JDEBUGLOG(fname,5)
  }
  else if (input_filter->GetTypeIdentifier() =="StructuredPairFilter") {
    a24 = dynamic_cast<jfStructuredPairFilter*>(input_filter);
    result = new jfStr_PairingFilterEditor(a24,parent);
  }
  else if (input_filter->GetTypeIdentifier() =="AO3WarnFilter") {
    a25 = dynamic_cast<jfAO3WarnFilter*>(input_filter);
    result = new jfAO3_WarnFilterEditor(a25,parent);
  }
  else if (input_filter->GetTypeIdentifier() =="KudoCountFilter") {
    a26 = dynamic_cast<jfFavKudoFilter*>(input_filter);
    result = new jfAO3KudoFilterEditor(a26,parent);
  }
  else if (input_filter->GetTypeIdentifier() =="FimThumbPercentFilter") {
    a28 = dynamic_cast<jfFimThumbPercentFilter*>(input_filter);
    result = new jfFimThumbPercentFilterEditor(a28,parent);
  }
  else if (input_filter->GetTypeIdentifier() =="FIMGroupSCFilter") {
    a29 = dynamic_cast<jfFIMGroupSCFilter*>(input_filter);
    result = new jfFIMGroupSCFilterEditor(a29,parent);
  }
  else if (input_filter->GetTypeIdentifier() =="FIMGroupMCFilter") {
    a30 = dynamic_cast<jfFIMGroupMCFilter*>(input_filter);
    result = new jfFIMGroupMCFilterEditor(a30,parent);
  }
  else if (input_filter->GetTypeIdentifier() =="FIMShortDescFilter") {
    a31 = dynamic_cast<jfFIM_ShortDesc_ExprFilter*>(input_filter);
    result = new jfFIMShortDesc_FilterEditor(a31,parent);
  }
  else if (input_filter->GetTypeIdentifier() =="FIMDualDescFilter") {
    a32 = dynamic_cast<jfFIM_DualDesc_ExprFilter*>(input_filter);
    result = new jfFIMDualDesc_FilterEditor(a32,parent);
  }
  else if (input_filter->GetTypeIdentifier() =="AO3ExtraTagsPercentFilter") {
    a35 = dynamic_cast<jfAO3ExtraTagsPercentFilter*>(input_filter);
    result = new jfAO3PercentExpressionTagFilterEditor(a35,parent);
  }

  else assert(false);
  // done
  return result;
}
//***************************************************************************
