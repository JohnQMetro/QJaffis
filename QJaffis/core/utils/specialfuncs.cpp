/******************************************************************************
Name    :   specialfuncs.cpp
Author  :   John Q Metro
Purpose :   Defines some special functions
Created :   january 1, 2010
Conversion to Qt Started February 18, 2014
Updated :   August 6, 2015
******************************************************************************/
#ifndef SPECIALFUNCS_H_INCLUDED
  #include "specialfuncs.h"
#endif // SPECIALFUNCS_H_INCLUDED
//---------------------------------------
#ifndef LOGGING_H_INCLUDED
  #include "logging.h"
#endif // LOGGING_H_INCLUDED
/*
#ifndef GOOGLESEARCH_H_INCLUDED
  #include "../../google/data/googlesearch.h"
#endif // GOOGLESEARCH_H_INCLUDED
*/
#ifndef JF_FFNSEARCH
  #include "../../fanficnet/data/ffn_search.h"
#endif
#ifndef FIM_SEARCH_H_INCLUDED
  #include "../../fanfic/data/fim/fim_search.h"
#endif // FIM_SEARCH_H_INCLUDED

#ifndef AO3_SEARCH_H_INCLUDED
  #include "../../fanfic/data/ao3/ao3_search.h"
#endif // AO3_SEARCH_H_INCLUDED

#ifndef FIM_GROUPSEARCH_H_INCLUDED
  #include "../../fanfic/data/fim/fim_groupsearch.h"
#endif // FIM_GROUPSEARCH_H_INCLUDED
/*****************************************************************************/
// a function that takes a file and creates a Search Object from it

jfSearchCore* MakeSearchFromFile(QString infpath, QString& outerr) {
  // constants
  const QString specf = "The specified file ";
	const QString fname = "MakeSearchFromFile";
  // local variables
  jfFileReader* infile;
  QString typid;
  // result variables
  // jfGoogleSearch* result01;
  jfFFNSearch* result02;
  jfFIMSearch* result03;
  jfAO3Search* result04;
  jfFIMGroupSearch* result05;

  // we start with the file
  infile = new jfFileReader(infpath);
  if (!infile->OpenFile()) {
    outerr = infile->err;
    /**/JDEBUGLOGS(fname,1,outerr)
    delete infile;
    return NULL;
  }
  // checking for the type
  if (!infile->GetType(typid,fname)) return false;
  // testing the type...
  /*
  if (typid=="GoogleSearch") {
    result01 = new jfGoogleSearch();
    if (!result01->GetFromFile(infile)) {
      outerr = infile->err;
      delete infile;
      delete result01;
      jfXLogMessage(fname,2,outerr);
      return NULL;
    }
    delete infile;
    return result01;
  }
  */

  // option 2 is ffn search
  if (typid=="FFNSearch") {
    result02 = new jfFFNSearch();
    if (!result02->GetFromFile(infile)) {
      outerr = infile->err;
      delete infile;
      delete result02;
      /**/JDEBUGLOGS(fname,3,outerr);
      return NULL;
    }
    delete infile;
    return result02;
  }
  // option 3 is fim search
  if (typid=="FIMSearch") {
    result03 = new jfFIMSearch();
    if (!result03->GetFromFile(infile)) {
      outerr = infile->err;
      delete infile;
      delete result03;
      /**/JDEBUGLOGS(fname,4,outerr)
      return NULL;
    }
    delete infile;
    return result03;
  }

  // option 4 is ao3 search
  else if (typid=="AO3Search") {
    result04 = new jfAO3Search();
    if (!result04->GetFromFile(infile)) {
      outerr = infile->err;
      delete infile;
      delete result04;
      /**/JDEBUGLOGS(fname,5,outerr)
      return NULL;
    }
    delete infile;
    return result04;
  }

  // option 5 is fim group search
  else if (typid=="FIMGroup") {
    result05 = new jfFIMGroupSearch();
    if (!result05->GetFromFile(infile)) {
      outerr = infile->err;
      delete infile;
      delete result05;
      /**/JDEBUGLOGS(fname,6,outerr)
      return NULL;
    }
    delete infile;
    return result05;
  }
  //
  else {
    delete infile;
    outerr = specf + "has an unrecognized type! :" + typid;
    /**/JDEBUGLOGS(fname,7,outerr)
    return NULL;
  }
}
/*****************************************************************************/
