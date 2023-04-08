/*****************************************************************************
Name    : ffncatthread.cpp
Basic   : Fanfiction.Net category downloading
Author  : John Q Metro
Started : July 21, 2016
Updated : April 7, 2023 (fixed update bug)

******************************************************************************/
#ifndef FFNCATTHREAD_H
  #include "ffncatthread.h"
#endif // FFNCATTHREAD_H
//------------------------------------

#ifndef CATGROUP_H
  #include "../categories/catgroup.h"
#endif // CATGROUP_H
#ifndef FFNCATPARSER1_H
  #include "ffncatparser1.h"
#endif // FFNCATPARSER1_H
#ifndef FFNCATPARSER2_H
  #include "ffncatparser2.h"
#endif // FFNCATPARSER2_H

#ifndef DEFAULTPATHS_H
    #include "../../defaultpaths.h"
#endif // DEFAULTPATHS_H
#ifndef FICEXTRACT_H_INCLUDED
  #include "../../ficdown/data/ficextract.h"
#endif // FICEXTRACT_H_INCLUDED
#ifndef GLOBALSETTINGS_H
    #include "../../globalsettings.h"
#endif // GLOBALSETTINGS_H

#ifndef INITEND_H_INCLUDED
  #include "../../initend.h"
#endif // INITEND_H_INCLUDED
//------------------------------
#include <assert.h>
/*****************************************************************************/
// --- [ METHODS for jfFFN_CategoryDownloader ]-------------------------
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// constructors and data setters
//-----------------------------------
jfFFN_CategoryDownloader::jfFFN_CategoryDownloader():jfDownloadRoot() {
  phase = 0;
  catman_ptr = NULL;
  category_data = NULL;
  csection_index = 0;
  updatemode = false;
  crossover_parser = NULL;
}
//-----------------------------------
bool jfFFN_CategoryDownloader::SetCatManagerPtr(jfFFN_CatManager* in_catman_ptr, bool doupdate) {
  if (in_catman_ptr == NULL) return false;
  catman_ptr = in_catman_ptr;
  if (catman_ptr->Empty()) updatemode = false;
  else updatemode = doupdate;
  return true;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void jfFFN_CategoryDownloader::StartProcessing() {
  const QString fname = "jfFFN_CategoryDownloader::StartProcessing";

  // some initial setup before we start fetching
  initLog("FFNCategoryThread");
  emit SendStart(true);
  /**/tLog(fname,1);
  assert(catman_ptr != NULL);
  started = true;
  category_data = catman_ptr->GetNewHolder();
  category_data->ResetIndex();
  /**/tLog(fname,2);
  size_t esec_count = (updatemode)?4:3;
  size_t seccount = jfFFN_SECDATA::scount + esec_count;
  emit SendSectionCount(seccount);

  // downloading non-crossover sections
  bool xresult = DoSections(false);
  /**/tLogB(fname,3,xresult);
  if (xresult) {
    // donloading section-level indexes for crossover
    xresult = DoSections(true);
    /**/tLog(fname,4);
    if (xresult) {
      // if we are updating, there is aspecial category
      if (updatemode) {
        xresult = UpdateMarking();
      }
      else xresult = true;

      if (xresult) {
        /* the pages downloaded are for all crossovers for a fandom, a
        * 'category' groups all fandoms for a section. */
        phase = 3;
        skip_on_fail = true;
        /**/tLog(fname,5);
        category_data->MakeCrossStore();
        SetupWorkers(false);
        sectiondata.startaction = "Getting";
        sectiondata.part_count = jfFFN_SECDATA::scount;
        sectiondata.item_label = "Crossover Section";
        // the loop for each section
        for (csection_index = 0 ; csection_index < jfFFN_SECDATA::scount ; csection_index++) {
          /**/tLogL(fname,6,csection_index);
          xresult = DoCrossoverSection();
          if (!xresult) break;
        }
        crossover_parser = NULL;
        ClearWorkers(true);
      }
    }

  }
  // at the end...
  if (!xresult) {
    catman_ptr->DiscardNew();
  }
  else {
    /**/tLog(fname,7);
    // after all the pages and donnloaded and parsed, some extra processing is needed
    emit SendOneThingSection("Sorting and Completing","the Crossovers");
    /**/tLog(fname,8);
    xresult = category_data->SortAndComplete();
    /**/tLog(fname,9);
    if (xresult) {
      emit SendItemDone();
      /**/tLog(fname,10);
      catman_ptr->NewDone();
      emit SendSectionDone(false);
    }
    else {
      /**/tLog(fname,11);
      catman_ptr->DiscardNew();
      emit SendSectionFail();
    }
  }
  AllDone(xresult);
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// downloading section pages
bool jfFFN_CategoryDownloader::DoSections(bool iscrossover) {
  const QString fname = "jfFFN_CategoryDownloader::DoSections";
  // initial setup
  /**/tLogB(fname,1,iscrossover);
  phase = (iscrossover)?2:1;
  SetupWorkers(false);
  sectiondata.startaction = "Getting";
  sectiondata.part_count= -1;
  sectiondata.item_index = -1;
  if (iscrossover) sectiondata.item_label = "Crossover Index";
  else sectiondata.item_label = "Section";
  /**/tLog(fname,3);
  emit SendNewSection(sectiondata);
  pagecount = jfFFN_SECDATA::scount;
  /**/tLog(fname,4);
  emit SendItemCount(pagecount);
  url_page_index = result_page_index = 0;
  // doing it
  /**/tLog(fname,5);
  // SetupWorkers(false);
  bool do_result = xLoopGet();
  /**/tLogB(fname,6,do_result);
  ClearWorkers(true);
  // checking afterwards
  if (do_result) emit SendSectionDone(false);
  else emit SendSectionFail();
  return do_result;
}
//---------------------------------------
// crossover group section, csection_index determines which section is done
bool jfFFN_CategoryDownloader::DoCrossoverSection() {
  // initial
  category_data->UrlIndexToStart(csection_index);
  // section info signal
  QString csecname = jfFFN_SECDATA::section_names[csection_index];
  crossover_parser->SetHoldingSectionName(csecname);
  sectiondata.item_index = csection_index+1;
  sectiondata.item_name = jfFFN_SECDATA::crossover_names[csection_index];
  emit SendNewSection(sectiondata);
  // pagecount
  pagecount = category_data->ItemsInSection(csection_index);
  url_page_index = result_page_index = 0;
  emit SendItemCount(pagecount);

  // doing
  bool do_result = xLoopGet();

  // checking afterards
  if (do_result) emit SendSectionDone(false);
  else emit SendSectionFail();
  return do_result;
}
//---------------------------------------------------------
// A section used only when we are updating. After getting the crossover indexes,
// we check against the old to see if we download again or just copy the old data.
bool jfFFN_CategoryDownloader::UpdateMarking() {
  const QString fname = "jfFFN_CategoryDownloader::UpdateMarking";
  assert(updatemode);
  /**/tLog(fname,1);
  emit SendOneThingSection("Checking and Marking for Update","the Crossovers");

  // getting and setting the old data
  jfFFN_Categories* oldata = catman_ptr->GetHolder();
  if (oldata == NULL) {
      tError(fname,"The old data must not be NULL in update mode!");
    emit SendSectionFail();
    return false;
  }
  category_data->Set_OldDataLink(oldata);

  // calling the method which checks old vs new, and sees if we should re-download
  bool casur = category_data->CompareAndSetUpdates();
  // handling the result
  if (casur) {
    emit SendItemDone();
    emit SendSectionDone(false);
  }
  else emit SendSectionFail();
  // done
  return casur;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// implemented virtual methods of the base downloader
//-----------------------------------
QString* jfFFN_CategoryDownloader::makeURLforPage(size_t index) const {
  const QString fname = "jfFFN_CategoryDownloader::makeURLforPage";
  /* for the sections and crossover sections, the index must be between
   * 1 and jfFFN_SECDATA::scount (inclusive) */
  /**/tLog(fname,1);
  if (phase < 3) {
    /**/tLogS(fname,2,index);
      tAssert( (index >= 1) && (index <= jfFFN_SECDATA::scount) ,fname,"Index has an invalid value");
    if (phase == 1) return new QString(jfFFN_SECDATA::IndexToURL(index-1,false));
    if (phase == 2) return new QString(jfFFN_SECDATA::IndexToURL(index-1,true));
  }
  else if (phase == 3) {
    /**/tLogS(fname,4,csection_index);
    return category_data->CurrentCrossoverURL(csection_index);
  }
  assert(phase!=0);
  return NULL;
}
//-----------------------------------
bool jfFFN_CategoryDownloader::ProcessResults(void* resultdata) {
  if (phase == 1) {
    jfFFNSection* new_section = static_cast<jfFFNSection*>(resultdata);
    return category_data->SetSection(new_section);
  }
  else if (phase == 2) {
    jfFFN_CrossoverSection* cross_section = static_cast<jfFFN_CrossoverSection*>(resultdata);
    return category_data->SetCrossoverSection(cross_section);
  }
  else if (phase == 3) {
    jfFFN_CrossoverGroup* cgroup = static_cast<jfFFN_CrossoverGroup*>(resultdata);
    return category_data->SetCrossoverGroup(cgroup);
  }
  else assert(false);
  return false;
}

//-----------------------------------
void jfFFN_CategoryDownloader::DisposeResults(void* resultdata) {
  if (phase == 1) {
    jfFFNSection* new_section = static_cast<jfFFNSection*>(resultdata);
    delete new_section;
  }
  else if (phase == 2) {
    jfFFN_CrossoverSection* cross_section = static_cast<jfFFN_CrossoverSection*>(resultdata);
    delete cross_section;
  }
  else if (phase == 3) {
    jfFFN_CrossoverGroup* cgroup = static_cast<jfFFN_CrossoverGroup*>(resultdata);
    delete cgroup;
  }
  else assert(false);
}
//-----------------------------------
/* 'Item Info' is stuff that is sent to GUI widgets before we fetch the page. */
void jfFFN_CategoryDownloader::PrepareItemInfo(size_t pageIndex) {
  infoToSend.item_index = pageIndex;
  infoToSend.startaction = "Doing";
  if ((phase == 2) || (phase == 1)) {
    infoToSend.part_count = jfFFN_SECDATA::scount;
    if (phase == 1) {
      infoToSend.item_label = "Section";
      infoToSend.item_name = jfFFN_SECDATA::section_names[pageIndex-1];
    }
    else {
      infoToSend.item_label = "Section Crossover";
      infoToSend.item_name = jfFFN_SECDATA::crossover_names[pageIndex-1];
    }
  }
  else if (phase == 3) {
    infoToSend.item_label = "Crossover Group";
    infoToSend.part_count = category_data->ItemsInSection(csection_index);
    infoToSend.item_name = category_data->NameAtIndex();
  }
  else assert(false);
}
//-----------------------------------
bool jfFFN_CategoryDownloader::advanceFetchIndex() {
    const QString fname = "jfFFN_CategoryDownloader::advanceFetchIndex";
    bool updatable = false;
    bool is_valid;
    // the usual case
    if ( phase < 3 ) url_page_index++;
    // phase 3 updating
    else if (updatemode) {
        while (!updatable) {
            url_page_index++;
            if (url_page_index > pagecount) break;
            is_valid = category_data->NextUrlIndex(csection_index,updatable);
            if (!is_valid) break;
        }
    }
    // phase 3 non-updating
    else {
        url_page_index++;
        category_data->NextUrlIndex(csection_index);
    }
    return true;
}
//-----------------------------------
bool jfFFN_CategoryDownloader::advanceResultIndex() {
    // The young justice error happens in this method, it returns false.
    const QString fname = "jfFFN_CategoryDownloader::advanceResultIndex";
    bool testres;
    if ((!updatemode) || (phase < 3)) {
        result_page_index++;
        category_data->NextIndex();
    }
    else {
        size_t skip_quantity = 0;
        while (true) {
            result_page_index++;
            skip_quantity++;
            if (result_page_index > pagecount) {
                break;
            }
            category_data->NextIndex();
            // QString name1 = category_data->NameAtIndex();
            if (category_data->UpdatableAtIndex()) break;
            // if not updatable, copy old data to new...
            testres = category_data->CopyDataAtIndex();
            assert(testres);
        }
        skip_quantity--;
        if (skip_quantity>0) emit SendSkippedPages(skip_quantity);
        if (result_page_index > pagecount) {
            result_page_index = pagecount;
            return false;
        }
    }
    return true;
}
//-----------------------------------
jfPageParserBase* jfFFN_CategoryDownloader::makeParser() {
  if (phase==1) return new jfFFNSectionParser();
  else if (phase==2) return new jfFFNCrossoverSectionParser();
  else if (phase==3) {
    jfFFNCrossoverParser* result = new jfFFNCrossoverParser();
    result->SetCrossoverStoragePointer(category_data->GetCrossoverStoragePtr());
    crossover_parser = result;
    return result;
  }
  else assert(false);
  return NULL;
}

// -----------------------------------------------------------
jfParseFetchPackage* jfFFN_CategoryDownloader::MakeParserFetcher() {

    jfPageParserBase* page_parser = makeParser();
    if (page_parser == NULL) return NULL;
    jfParseFetchPackageMaker* parse_fetch_maker = jglobal::settings.getFetchPackageMaker();
    if (parse_fetch_maker == NULL) {
        delete page_parser;
        return NULL;
    }
    else {
        jglobal::jfFetchBasics fetch_type = jglobal::settings.FindFetchTypeFor(jfft_FFN, jglobal::FPT_CATEGORY_PAGE);
        if (fetch_type.IsValid()) {
            return parse_fetch_maker->makeFetchPackage(fetch_type, page_parser);
        }
        else {
            return parse_fetch_maker->makeGeneralFetchPackage(page_parser);
        }
    }

}
/*****************************************************************************/
