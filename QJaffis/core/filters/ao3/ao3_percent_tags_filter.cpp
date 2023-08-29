//***************************************************************************
// Name:        ao3_percent_tags_filter.cpp
// Author :     John Q Metro
// Purpose :    Matches individual extra tags against an expression, passing
//              if the percentags of tags that match exceed the defined percentage.
// Created:     July 23, 2022
// Updated:     April 8, 2023
//***************************************************************************
#ifndef AO3_PERCENT_TAGS_FILTER_H
    #include "ao3_percent_tags_filter.h"
#endif // AO3_PERCENT_TAGS_FILTER_H
//-------------------------------------------------------------------------
#ifndef AO3_FICOBJ_H_INCLUDED
  #include "../../../fanfic/data/ao3/ao3_ficobj.h"
#endif // AO3_FICOBJ_H_INCLUDED

#include "../../tagfilter/taglist.h"
//***************************************************************************
jfFilterBase* createAO3ExtraTagsPercentFilter(const QString& filterName) {
    return new jfAO3ExtraTagsPercentFilter(filterName);
}
// ---------------------------------------

const jfFilterTypeMeta AO3_EXTRATAGS_PERCENT_FILTER_INFO =
    jfFilterTypeMeta(jfFilterTypeGroup::GENRE, "AO3ExtraTagsPercentFilter",
          "AO3 Extra Tags Percent Filter",
          QString("The AO3 Extra Tags Percent Filter matches each extra tag attached to an ") +
          "AO3 Fanfic against a tag set, and a boolean expression. The percentage of tags " +
          "(ignoring those removed by the prefilter) that match must exceed the specified" +
          " percetage for the entire filter to match.",
          IdForAO3Fanfic(), createAO3ExtraTagsPercentFilter );

// ======================================================================

// the constructors
jfAO3ExtraTagsPercentFilter::jfAO3ExtraTagsPercentFilter(const QString& filter_name):
    jfMatchFilteredPercentTagFilterBase(filter_name,taglistings::typeListsGroup ) {

}
//+++++++++++++++++++++++++++++++++++++
// implemted virtual methods
jfFilterBase* jfAO3ExtraTagsPercentFilter::GenCopy() const {
    return Copy();
}
//--------------------
const jfFilterTypeMeta& jfAO3ExtraTagsPercentFilter::GetTypeMetaInfo() const {
    return AO3_EXTRATAGS_PERCENT_FILTER_INFO;
}
//+++++++++++++++++++++++++++++++++++++
// extra method
jfAO3ExtraTagsPercentFilter* jfAO3ExtraTagsPercentFilter::Copy() const {
    jfAO3ExtraTagsPercentFilter* result = new jfAO3ExtraTagsPercentFilter(name);
    result->CoreCopy((*this),true);
    return result;

}
//+++++++++++++++++++++++++++++++++++++
// the core matching method
bool jfAO3ExtraTagsPercentFilter::CoreMatch(const jfSearchResultItem* testelem) const {
    // checks and starts
    assert(testelem!=NULL);
    const jfAO3Fanfic* typed_item = dynamic_cast<const jfAO3Fanfic*>(testelem);
    // extracting the data
    QStringList extraTags = typed_item->GetExtraTags();
    return InternalMatch(extraTags);
}


//***************************************************************************


