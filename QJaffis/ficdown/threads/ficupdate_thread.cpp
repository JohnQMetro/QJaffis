/***************************************************************
 * Name:      ficupdate_thread.cpp
 * Purpose:   Downloaded fanfic updater thread
 * Author:    John Q Metro
 * Created:   July 25, 2016
 * Updated:   July 26, 2016
 *
 **************************************************************/
#ifndef FICUPDATE_THREAD_H
  #include "ficupdate_thread.h"
#endif // FICUPDATE_THREAD_H
#ifndef UTILS3_H_INCLUDED
  #include "../../core/utils/utils3.h"
#endif // UTILS3_H_INCLUDED
//------------------------------
#include <assert.h>
#include <QDir>
#include <QDirIterator>
#include <QFile>
#include <QFileInfo>
/**************************************************************/
bool jfFicExtractArray::IdIncluded(const size_t& idtocheck,jf_FICTYPE idcheck) const {
  // variables
  size_t floop,fcount,ficid;
  bool identity;
  // we start
  fcount = size();
  for (floop=0;floop<fcount;floop++) {
    ficid = (*this)[floop]->fic_id;
    identity = ((idtocheck==ficid) && (idcheck==GetFtype(floop)));
    if (identity) break;
  }
  return (floop!=fcount);
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// returns a pointer to an extract by id
jfFicExtract* jfFicExtractArray::GetByID(const size_t& idtocheck,jf_FICTYPE idcheck) const {
  // variables
  size_t floop,fcount,ficid;
  bool identity = false;
  // we start
  fcount = size();
  for (floop=0;floop<fcount;floop++) {
    ficid = (*this)[floop]->fic_id;
    identity = ((idtocheck==ficid) && (idcheck==GetFtype(floop)));
    if (identity) break;
  }
  if (identity) return (*this)[floop];
  else return NULL;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// handles file deletion
size_t jfFicExtractArray::DeleteSourceFiles(size_t inindex) {
  // out of bounds
  if (inindex>=size()) return 0;
  // no file list
  if ((*this)[inindex]->fnamelist==NULL) return 0;
  // otherwise
  size_t sfcount = (*this)[inindex]->fnamelist->count();
  for (size_t sfindex=0;sfindex<sfcount;sfindex++) {
    QFile::remove((*this)[inindex]->fnamelist->at(sfindex));
  }
  // last stuff
  delete (*this)[inindex]->fnamelist;
  (*this)[inindex]->fnamelist = NULL;
  return sfcount;
}
//----------------------------------------------------------------------
// if the fic had one file, we delete only that file
bool jfFicExtractArray::DeleteOnlySource(size_t inindex) {
  // out of bounds
  if (inindex>=size()) return false;
  // no file list
  if ((*this)[inindex]->fnamelist==NULL) return false;
  // otherwise
  size_t sfcount = (*this)[inindex]->fnamelist->count();
  if (sfcount!=1) return false;
  QFile::remove((*this)[inindex]->fnamelist->at(0));
  // last stuff
  delete (*this)[inindex]->fnamelist;
  (*this)[inindex]->fnamelist = NULL;
  return true;
}
//----------------------------------------------------------------------
bool jfFicExtractArray::ClearSourceFiles(size_t inindex) {
  // out of bounds
  if (inindex>=size()) return false;
  if ((*this)[inindex]->fnamelist==NULL) return true;
  else delete (*this)[inindex]->fnamelist;
  (*this)[inindex]->fnamelist = NULL;
  return true;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// deletes all the contained fics and empties the array
void jfFicExtractArray::ClearContents() {
  // variables
  size_t floop,fcount;
  // we start
  fcount = size();
  for (floop=0;floop<fcount;floop++) {
    delete (*this)[floop];
    (*this)[floop] = NULL;
  }
  clear();
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
jf_FICTYPE jfFicExtractArray::GetFtype(size_t index) const {
  assert(index<size());
  return (*this)[index]->GetFicType();
}
//£££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££££





/*******************************************************************************************/
// --- [METHODS for jfFicUpdateThread] -----------------------
//+++++++++++++++++++++++++++++++++++++++++++++++++
jfFicUpdateThread::jfFicUpdateThread(size_t in_max_threads):jfFicDownloaderBase(in_max_threads) {
  phase = 0;
  fic_index = 0;
  filelist = NULL;
  subdirs = NULL;
  ficlist = NULL;
  halting = false;
  docomplete = false;
}
//-------------------------------------
bool jfFicUpdateThread::SetParams(const QString& indir, const size_t& in_sizeguide, bool in_docomplete) {
  if (indir.isEmpty()) return false;
  sourcedir = indir;
  sizeguide = in_sizeguide;
  docomplete = in_docomplete;
  return true;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++
void jfFicUpdateThread::StartProcessing(){
  const QString fname = "jfFicUpdateThread::StartProcessing";
  bool okay = true;
  initLog("UpdateFanfics");
  emit SendStart(true);
  started = true;
  /**/tLog(fname,1);
  emit sendSectionCount(10);
  // looking for files
  phase = 1;
  /**/tLog(fname,2);
  size_t file_count = LoadHtmlFilelist();
  /**/tLogS(fname,3,file_count);
  // if we havefiles, then...
  if (file_count != 0) {
    phase = 2;
    /**/tLog(fname,4);
    size_t fic_count = CheckAllFiles();
    /**/tLogS(fname,5,fic_count);
    if (!halting) {
      emit sendSectionCount(2+fic_count);
      delete filelist;
      phase = 3;
      /**/tLog(fname,6);
      if (fic_count!=0) {

        /**/tLog(fname,7);
        okay = DoAllFanfics();
        ficlist->ClearContents();
        /**/tLog(fname,8);
        delete ficlist;
        delete subdirs;
      }
      else {
        if (ficlist!=NULL) delete ficlist;
        emit sendSectionCount(2);
        emit sendNothing(true);
      }
    }
  }
  else {
    emit sendSectionCount(1);
    /**/tLog(fname,9);
    delete filelist;
    emit sendNothing(false);
  }

  // done
  AllDone(okay);
}

//+++++++++++++++++++++++++++++++++++++++++++++++++
bool jfFicUpdateThread::InterCatCheckStop() {
  if ((pauser->CheckPauseStop()) || (pauser->CheckPauseStopAfter())) {
    current_error = jfa_HALT;
    emit SendHaltMessage();
    return true;
  }
  return false;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++
// component parts (pre fic-download)
//-------------------------------------
size_t jfFicUpdateThread::LoadHtmlFilelist() {
  // variables
  QDirIterator* loodir;
  QStringList filters;
  size_t fcount = 0;
  // the start message dispatch
  emit sendOneActionSection("Loading","the list of HTML files...");
  // starting to get the list
  filters << "*.html" << "*.htm";
  loodir = new QDirIterator(sourcedir,filters,QDir::Files | QDir::NoSymLinks ,QDirIterator::Subdirectories);
  filelist = new QStringList();
  // building the list
  while (loodir->hasNext()) {
    filelist->append(loodir->next());
    fcount++;
  }
  delete loodir;
  // post-lits done messages
  emit SendItemDone();
  emit sendSectionDone(false);
  // done
  return filelist->count();
}

//-------------------------------------
size_t jfFicUpdateThread::CheckAllFiles(){
  const QString fname = "jfFicUpdateThread::CheckAllFiles";
  // variables
  size_t filecount, ficcount;
  bool rval,hval;
  QFileInfo fpath;
  // trivial case
  filecount = filelist->count();
  if (filecount==0) return 0;
  ficcount = 0;
  // setting up the category
  section_msg_data.startaction = "Scanning";
  section_msg_data.item_label = "the HTML files for fics...";
  section_msg_data.item_index = -1;
  emit sendNewSection(section_msg_data);
  emit SendItemCount(filecount);
  // preparing for the loop
  infoToSend.startaction = "Loading";
  infoToSend.item_label = "HTML File";
  infoToSend.part_count = filecount;
  ficlist = new jfFicExtractArray();
  subdirs = new QStringList();
  // the scanning loop
  for (size_t fileindex = 1 ; fileindex <= filecount ; fileindex++ ) {
    // sending the new signal
    infoToSend.item_name = filelist->at(fileindex-1);
    infoToSend.item_index = fileindex;
    emit SendNewItem(infoToSend);
    // loading and checking the item
    rval = CheckHtmlFile(fileindex-1);
    if (rval) {
      fpath.setFile(infoToSend.item_name);
      subdirs->append(fpath.absolutePath());
      ficcount++;
    }
    emit SendItemDone();
    hval = pauser->CheckPauseStop();
    if (hval) break;
  }
  if (!hval) emit sendSectionDone(false);
  else {
    current_error = jfa_HALT;
    emit SendHaltMessage();
    halting = true;
  }
  // done
  return ficcount;
}

//-------------------------------------
bool jfFicUpdateThread::CheckHtmlFile(size_t index) {
  const QString fname = "jfFicUpdateThread::CheckHtmlFile";
  // variables
  QString filedata,ficcore;
  bool rval;
  jfFicExtract* testdata;
  jfFicExtract* checkdata;
  jfStringParser* sparser;
  jf_FICTYPE fout_type;
  // checking for out of bounds
  assert(index<filelist->count());
  // getting the file data
  rval = FromFile(filedata,filelist->at(index));
  assert(rval);
  // next, processing
  emit SendNewItemAction("Checking");
  // next, we process the file data to get the fic info
  sparser = new jfStringParser(filedata);
  if (!sparser->GetDelimited(STARTINFO_HEADER,"-->",ficcore)) {
    // not a fic
    delete sparser;
    return false;
  }
  delete sparser;
  // processing the extract into a jfFicExtract
  testdata = MakeExtract(fout_type,ficcore);
  if (testdata==NULL) return false;
  // checking if the file exists already
  checkdata = ficlist->GetByID(testdata->fic_id,fout_type);
  if (checkdata==NULL) {
    testdata->fnamelist = new QStringList();
    testdata->fnamelist->append(filelist->at(index));
    ficlist->push_back(testdata);
    return true;
  }
  else {
    checkdata->fnamelist->append(filelist->at(index));
    return false;
  }
}
//----------------------------------------
jfFicExtract* jfFicUpdateThread::MakeExtract(jf_FICTYPE& outtype, QString ficcore) {
  const QString fname = "jfUpdateFicThread::MakeExtract";
  // variables
  jfLineParse* lpval;
  jfFicExtract* testdata;
  bool rval;
  // starting
  ficcore = ficcore.trimmed();
  lpval = ParseExtractSource(ficcore, outtype);
  if (lpval==NULL) return NULL;
  // determining the type
  if (outtype==jfft_FFN) testdata = new jfFicExtract_FFN();
  else if (outtype==jfft_FIM) testdata = new jfFicExtract_FIM();
  else if (outtype==jfft_HPF) testdata = new jfFicExtract_HPF();
  else if (outtype==jfft_MMO) testdata = new jfFicExtract_MMO();
  else if (outtype==jfft_AO3) testdata = new jfFicExtract_AO3();
  else {
    delete lpval;
    return NULL;
  }
  testdata->fnamelist = NULL;
  // loading from parsed data
  rval = testdata->GetFromParse(lpval);
  delete lpval;
  if (!rval) {
    delete testdata;
    return NULL;
  }
  return testdata;
}
//----------------------------------------------
bool jfFicUpdateThread::DoAllFanfics() {
  // variables
  size_t ficcount;
  bool res,halting;
  // starting
  ficcount = ficlist->size();
  emit sendFicCount(ficcount);
  // prepping the start messages
  section_msg_data.startaction = "Checking";
  section_msg_data.item_label = "Fanfic";
  section_msg_data.part_count = ficcount;
  // the fic handling loop
  for (fic_index = 1; fic_index <= ficcount; fic_index++) {
    // getting the info
    fic_data = ficlist->at(fic_index-1);
    section_msg_data.item_index = fic_index;
    section_msg_data.item_name = fic_data->fic_title + " by " + fic_data->author_name;
    emit sendNewSection(section_msg_data);
    if (!docomplete) {
      if (fic_data->complete) {
        emit sendSectionDone(true);
        continue;
      }
    }
    // doing the fic
    fictype = fic_data->GetFicType();
    res = DownloadFic();
    // handling the results
    halting = HandleFanficResult(res);
    if (halting) break;
  }
  return (halting);
}
//------------------------------------------
bool jfFicUpdateThread::HandleFanficResult(bool fres) {
  const QString fname = "jfFicUpdateThread::HandleFanficResult";
  ficur::jfFICUR updval;
  // determining update result code, and whether we go on or stop
  /**/tLogB(fname,1,abort_fetch);
  /**/tLogB(fname,2,fres);
  /**/tLog(fname,3,jf_Aftermath2String(current_error));
  bool stopres = false;
  if(abort_fetch) updval = ficur::NotUpdated;
  else if (fres) updval = ficur::Updated;
  else if (current_error == jfa_PARSEERR) updval = ficur::ParseError;
  else if (current_error == jfa_MISSING) updval = ficur::Missing;
  else if (current_error == jfa_HALT) {
    updval = ficur::Halted;
    stopres = true;
  }
  else updval = ficur::Failed;

  // sending the result code
  emit sendUpdateResult(updval,section_msg_data.item_name);
  // closing the section
  if (fres) emit sendSectionDone(false);
  else if (stopres || abort_fetch) emit sendSectionDone(true);
  else emit sendSectionDone(true);
  return InterCatCheckStop();
}

//+++++++++++++++++++++++++++++++++++++++++++++++++
// custom virtual abstract methods
//-------------------------------------
bool jfFicUpdateThread::TestForAbort(jfFicExtract* new_extract) const {
  assert(new_extract!=NULL);
  // variables and setup
  const QString fname = "jfFicUpdateThread::TestForAbort";

  bool changed, changed2;
  jf_FICTYPE ft;
  // a quick check
  /**/tLog(fname,1);
  changed = false;
  // otherwise...
  jfFicExtract* curr_stored = ficlist->at(fic_index-1);
  ft = new_extract->GetFicType();
  /**/tLog(fname,2,curr_stored->fic_title);
  /**/tLog(fname,3,new_extract->fic_title);
  changed = false;
  if (!changed) {
    changed = (new_extract->updated_date) > (curr_stored->updated_date);
    // because I've found that old fanfiction.net chapter counts are unreliable
    changed2 = ((new_extract->pcount)!=(curr_stored->pcount));
    changed = (changed || changed2);
    /**/tLogB(fname,5,changed);
  }
  // done
  /**/tLogB(fname,6,changed);
  return !changed;

}
//-------------------------------------
bool jfFicUpdateThread::SetFileBase() {
  if (fanfic==NULL) return false;
  if (ficlist==NULL) return false;
  if (subdirs==NULL) return false;
  fanfic->SetFileBase((*ficlist)[fic_index-1]->fname_core,(*subdirs)[fic_index-1],sizeguide);
  return true;
}

/**************************************************************/
