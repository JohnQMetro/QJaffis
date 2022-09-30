/******************************************************************************
Name    :   ao3cat_thread.cpp
Author  :   John Q Metro
Purpose :   Downloader class for AO3 sections (fandom listings)
Created :   July 15, 2016
Updated :   June 11, 2022
******************************************************************************/
#ifndef AO3CAT_THREAD_H
  #include "ao3cat_thread.h"
#endif // AO3CAT_THREAD_H
//-----------------------------------
#ifndef AO3CAT_PARSER_H
  #include "ao3cat_parser.h"
#endif // AO3CAT_PARSER_H

#ifndef AO3_CATS_H_INCLUDED
  #include "../../data/ao3/ao3_cats.h"
#endif // AO3_CATS_H_INCLUDED

#ifndef FICEXTRACT_H_INCLUDED
  #include "../../ficdown/data/ficextract.h"
#endif // FICEXTRACT_H_INCLUDED
/*****************************************************************************/
jfAO3CategoryDownloader::jfAO3CategoryDownloader():jfGeneralCategoryDownloaderBase() {
  tlogname ="AO3CategoryThread";
}
//-----------------------------
bool jfAO3CategoryDownloader::SetInitialData() {
  return SetCategoryDataPointers(ao3_catdata::ao3_catmanager,ao3_catdata::ao3_nameslinks);
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++=
// virtual methods that have to be implemented by derived threads
jfPageParserBase* jfAO3CategoryDownloader::makeParser() {
  return new jfAO3CatParser();
}
jfParseFetchPackage* jfAO3CategoryDownloader::MakeParserFetcher() {
    jfPageParserBase* ao3_parser = makeParser();
    if (ao3_parser == NULL) return NULL;

    jfParseFetchPackage* result = CategoryParseFetchMaker(jfft_AO3, ao3_parser);
    if (result == NULL) delete ao3_parser;
    return result;
}


/*****************************************************************************/

