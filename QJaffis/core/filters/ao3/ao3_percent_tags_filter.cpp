//***************************************************************************
// Name:        ao3_percent_tags_filter.cpp
// Author :     John Q Metro
// Purpose :    Matches individual extra tags against an expression, passing
//              if the percentags of tags that match exceed the defined percentage.
// Created:     July 23, 2022
// Updated:     March 10, 2023
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
    // the constructors
jfAO3ExtraTagsPercentFilter::jfAO3ExtraTagsPercentFilter():jfMatchFilteredPercentTagFilterBase(taglistings::typeListsGroup) {
}

//+++++++++++++++++++++++++++++++++++++
// implemted virtual methods
QString jfAO3ExtraTagsPercentFilter::GetTypeDescription() const {
    return QString("The AO3 Extra Tags Percent Filter matches each extra tag attached to an ") +
                   "AO3 Fanfic against a tag set, and a boolean expression. The percentage of tags that match" +
                   " must exceed the specified percetage for the entire filter to match.";
}
//--------------------
jfBaseFilter* jfAO3ExtraTagsPercentFilter::GenCopy() const {
    return Copy();
}
//--------------------
QString jfAO3ExtraTagsPercentFilter::GetTypeID() const {
    return "AO3ExtraTagsPercentFilter";
}
//+++++++++++++++++++++++++++++++++++++
// extra method
jfAO3ExtraTagsPercentFilter* jfAO3ExtraTagsPercentFilter::Copy() const {
    jfAO3ExtraTagsPercentFilter* result = new jfAO3ExtraTagsPercentFilter();
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


