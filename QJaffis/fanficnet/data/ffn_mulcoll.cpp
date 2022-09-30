//*******************************************************************************
// Name:        ffn_mulcoll.cpp
// Author :     John Q Metro
// Purpose :    Holder for multiple-category search results from Fanfiction.Net
// Created:     June 17, 2010
// Conversion to Qt Started July 11, 2014
// Updated:     August 22, 2012
//*******************************************************************************
#ifndef FFN_MULTICOLL
  #include "ffn_mulcoll.h"
#endif
#ifndef LOGGING_H_INCLUDED
  #include "../../core/utils/logging.h"
#endif // LOGGING_H_INCLUDED
//*******************************************************************************
// the constructor
jfFFNResColl::jfFFNResColl(jfFFNSearch* xsearch_in):jfResultMultiCollection(xsearch_in, false) {
  typed_search = xsearch_in;
  cindex = 0;
  cat_links = NULL;
}
//---------------------------------------------------------------------------
bool jfFFNResColl::SetCatLinks(const jfFFN_Categories* inlinks) {
  if (inlinks==NULL) return false;
  cat_links = inlinks;
  return true;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
QString jfFFNResColl::GetTypeID() const {
  return "jfFFNResColl";
}
//--------------------------------------------------------------------------
const jfFFN_CategoryCore* jfFFNResColl::GetCat(size_t zindex) const {
  // variables and checks
  jfFFNItemCollection* temp;
  assert(CheckIndex(zindex));
  // going ahead
  temp = dynamic_cast<jfFFNItemCollection*>((*collections)[zindex]);
  return temp->GetCat();
}
//----------------------------------------------------------
size_t jfFFNResColl::FirstIDAtIndex(size_t index_in) const {
  // variables and checks
  const jfFFNItemCollection* temp;
  assert(CheckIndex(index_in));
  // going ahead
  temp = dynamic_cast<const jfFFNItemCollection*>((*collections)[index_in]);
  return temp->first_id;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// methods for integrating updates
//-----------------------------------------------------------------
/* a new collection is made using the data from new_coll_source (which is
emptied), and inserted at the current index, which is incremented */
bool jfFFNResColl::InsertNewCollection(jfFFNItemCollection* new_coll_src) {
  // variables
  jfFFNItemCollection* newitem;
  // we start
  newitem = new jfFFNItemCollection(new_coll_src,false);
  // done
  return InsertItem(newitem);
}
//-----------------------------------------------------------------
/* The data new_coll_source (which is emptied) is inserted into the collection at the
current index, which is incremented */
bool jfFFNResColl::InsertFromNewCollection(jfFFNItemCollection* new_coll_src) {
  // checks
  if (itemcount==0) return false;
  if (cindex>=itemcount) return false;
  // variables
  bool res;
  jfFFNItemCollection* olditem;
  // we get the current collection
  olditem = dynamic_cast<jfFFNItemCollection*>((*collections)[cindex]);
  // we call the insert method
  res = olditem->InsertNewResults(new_coll_src);
  toNextCollection();
  return res;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// i/o
bool jfFFNResColl::WriteToHTML(size_t result_category) {
  // constants
  const QString fname = "jfFFNResColl::WriteToHTML";
  // local variables
  jfHtmlParams *output;
  jfFFNItemCollection* temp;
  size_t ccloop;
  size_t numres;
  // checks
  assert(result_category<33);
  // starting
  output = OpenHTMLOutfile(result_category);
  // we prepare the skeletop
  output->base = search->GetLocalSkeleton();
  if ((output->base)==NULL) {
    output->base = mainskelc->AtIndexM(1);
  }
  // we load up with values first
  search->LoadValues(output->parse,output->GetResCat());
  // header
  WriteHtmlHeader(output);
  // body
  for (ccloop=0; ccloop<itemcount; ccloop++) {
    temp = dynamic_cast<jfFFNItemCollection*>((*collections)[ccloop]);
    numres = temp->CountRCategory(output->GetResCat());
    if (numres>0) temp->WriteToHTML(output,false);
  }
  // finishing off
  WriteFooterClose(output);
  return true;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// gets the fic extract (an abstract of the fic data used for fic downloading)
jfFicExtract* jfFFNResColl::GetFicExtract(size_t c_index, size_t i_index) const {
  // constants
  const QString fname = "jfFFNResColl::GetFicExtract";
  // checking the collections...
  /**/JDEBUGLOGST(fname,1,c_index);
  /**/JDEBUGLOGST(fname,2,itemcount);
  assert(c_index<itemcount);
  // variables
  jfFFNItemCollection* typox;
  // starting
  (*cprotect)[c_index]->lock();
  typox = dynamic_cast<jfFFNItemCollection*>((*collections)[c_index]);
  // getting the data
  jfFicExtract* xres = typox->FicExt_AtIndex(i_index);
  (*cprotect)[c_index]->unlock();
  // done
  return xres;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void jfFFNResColl::WriteHtmlHeader(jfHtmlParams* indata) {
  // constants
  const QString fname = "jfFFNResColl::WriteHtmlHeader";
  // local variables
  QString buffer;
  size_t ccloop;
  size_t numres;
  bool firstdone;
  bool lvres;
  QString skeldata;
  jfFFNItemCollection* temp;
  // the usual checks
  jerror::AssertLog(indata!=NULL,fname,"The input data is NULL!");
  jerror::AssertLog(indata->CheckOk(),fname,"The input data failed checks!");
  // loading values
  // the start of the header
  /**/JDEBUGLOG(fname,1)
  buffer = indata->MakeResult(indata->base->header_skel);
  (*(indata->outfile)) << buffer << "\n";
  firstdone = false;
  /**/JDEBUGLOG(fname,2)
  // category links
  for (ccloop=0; ccloop<itemcount; ccloop++) {
    /**/JDEBUGLOGST(fname,3,ccloop)
    // getting the collection and loading the values...
    temp = dynamic_cast<jfFFNItemCollection*>((*collections)[ccloop]);
    /**/JDEBUGLOGS(fname,4,temp->GetCatName())
    lvres = temp->LoadValues(indata->parse,indata->GetResCat());
      jerror::AssertLog(lvres,fname,"Item Collection load values failed! " + QString::number(ccloop));
    numres = temp->CountRCategory(indata->GetResCat());
    /**/JDEBUGLOGST(fname,5,numres)
    // the cat link
    if (numres>0) {
      // the cat separator
      if (firstdone) {
        skeldata = indata->Multiple()->catlink_separator;
        buffer = indata->MakeResult(skeldata);
        buffer += " ";
      }
      /**/JDEBUGLOG(fname,8)
      skeldata = indata->Multiple()->catlink;
      if (firstdone) buffer += indata->MakeResult(skeldata);
      else buffer = indata->MakeResult(skeldata);
      /**/JDEBUGLOG(fname,9);
      // writing the result
      (*(indata->outfile)) << buffer << "\n";
      firstdone = true;
    }
    /**/JDEBUGLOG(fname,10);
  }
  // then we have a footer below...
  skeldata = indata->Multiple()->top_footer;
  buffer = indata->MakeResult(skeldata);
  /**/JDEBUGLOG(fname,11);
  (*(indata->outfile)) << buffer << "\n";
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// i/o methods
//----------------------------
// we do not actually have anything extra to add
bool jfFFNResColl::AddRestToFile(QTextStream* outfile) const {
  return true;
}
//----------------------------
// we create an empty Fanfiction.Net Collection
jfUrlItemCollection* jfFFNResColl::MakeEmptyCollection() const {
  return new jfFFNItemCollection();
}
//----------------------------
/* although no new items are to be loaded, we need to link each collection
with its appropriate category */
bool jfFFNResColl::ReadRestFromFile(jfFileReader* infile) {
  // constants
  const QString fname = "jfFFNResColl::ReadRestFromFile";
  // variables
  jfFFNItemCollection* temp;
  const jfFFN_CategoryCore* cresult;
  size_t ccloop;
  QString cfindstr;
  bool dres;
  // a check
  /**/JDEBUGLOG(fname,1);
  assert(cat_links!=NULL);
  /**/JDEBUGLOG(fname,2);
  // we loop over each collection
  for (ccloop=0;ccloop<itemcount;ccloop++) {
    // getting the collection
    /**/JDEBUGLOGST(fname,3,ccloop);
    temp = dynamic_cast<jfFFNItemCollection*>((*collections)[ccloop]);
    // looking for the cat link
    cfindstr = temp->GetFinder();
    cresult = cat_links->FindCatFromFinder(cfindstr);
    // a null result is badd
    if (cresult==NULL) {
      /**/JDEBUGLOGS(fname,4,cfindstr);
      // so therefore we delete the entire category
      dres = DeleteAtIndex(ccloop);
      assert(dres);
      ccloop--;
    }
    else {
      // if not null, we call the function thats set the cat
      temp->ReplaceCat(cresult);
    }
    /**/JDEBUGLOG(fname,5);
  }
  /**/JDEBUGLOG(fname,6);
  // done okay!
  return true;
}
//*******************************************************************************

