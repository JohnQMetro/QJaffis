/////////////////////////////////////////////////////////////////////////////
// Name:        ffndown_data.cpp
// Author :     John Q Metro
// Purpose :    Downloading fics, we need structures to hold the data
// Created:     June 19, 2011
// Conversion to QT started : April 18, 2013
// Updated:     October 13, 2019
/////////////////////////////////////////////////////////////////////////////
#ifndef FFNDOWN_DATA_H_INCLUDED
  #include "ffndown_data.h"
#endif // FFNDOWN_DATA_H_INCLUDED
#ifndef LOGGING_H_INCLUDED
  #include "../../core/utils/logging.h"
#endif // LOGGING_H_INCLUDED
#ifndef SKELCOLL_H_INCLUDED
  #include "../../core/structs/skelcoll.h"
#endif // SKELCOLL_H_INCLUDED
#ifndef UTILS3_H_INCLUDED
#include "../../core/utils/utils3.h"
#endif // UTILS3_H_INCLUDED
//------------------------------------------
#include <assert.h>
#include <math.h>
//****************************************************************************

jfBadCharsFilter::jfBadCharsFilter(const QString badchars, QObject* parent):QValidator(parent) {
    cfilt = QRegExp("[" + QRegExp::escape(badchars) + "]");
}
QValidator::State jfBadCharsFilter::validate(QString & input, int & pos) const {
    if (input.indexOf(cfilt)>0) fixup(input);
    if (input.isEmpty()) return QValidator::Intermediate;
    else return QValidator::Acceptable;
}
void jfBadCharsFilter::fixup(QString & input) const {
    input = input.replace(cfilt,"");
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/*
// groups together skeletons and thier outputs
class jfFD_OutStuff : public wxObject {
  public:
    // public data
    jfSkeletonParser* parse;
    jfSkeletonFicPart* base;
    */
// constructor
jfFD_OutStuff::jfFD_OutStuff() {
  parse = NULL;
  base = NULL;
  fileopen = false;
  infoset = false;
  filenames = NULL;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// public methods
//--------------------------
bool jfFD_OutStuff::CheckOk() const {
  if (parse==NULL) return false;
  if (base==NULL) return false;
  return true;
}
//--------------------------
QString jfFD_OutStuff::MakeResult(QString inskel) {
  assert(CheckOk());
  // doing the parsing
  parse->SetSkeleton(inskel);
  parse->ProduceIntermediate();
  parse->MakeResult();
  return parse->GetResult();
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// file i/o methods
//--------------------------
bool jfFD_OutStuff::SetNameInfo(QString inname, QString indir, size_t num_files) {
  // check the input
  if (num_files<1) return false;
  if (inname.isEmpty()) return false;
  // local variables
  QString trailer,ofname;
  size_t findex;
  // if file output is/has ocurred, we just erase and start again
  ClearFiles();
  // setting
  outname = inname;
  outdir = indir;
  gcount = num_files;
  filenames = new QStringList();
  for (findex=1;findex<=gcount;findex++) {
    if (gcount>1) trailer = FormatNumber(findex);
    trailer += ".html";
    if (gcount>1) trailer = "_" + trailer;
    QString ofname = outname + trailer;
    filenames->append(ofname);
  }
  infoset = true;
  return true;
}
//----------------------------
bool jfFD_OutStuff::GetFilename(QString& outresult, size_t gindex) const {
  // constants
  const QString fname = "jfFD_OutStuff::GetFilename";
  // stuff for which we return false
  if (!infoset) return false;
  if (gindex==0) return false;
  if (gindex>gcount) return false;
  // returning true
  outresult = (*filenames)[gindex-1];
  return true;
}
//--------------------------
bool jfFD_OutStuff::StartNewFile() {
  const QString fname = "jfFD_OutStuff::StartNewFile";
  // first, check if the pervious file is still open
  if (fileopen) return false;
  // information must be set
  if (!infoset) return false;
  // we should not exceed the preestablished filecount
  if (outfiles.size()==gcount) return false;
  // for the new file, we start with getting the name
  QString ofname;
  if (!outdir.isEmpty()) ofname = outdir + "/";
  ofname += (*filenames)[outfiles.size()];
  // next up, we create the outfile file
  QTextStream* newfile = GetOpenFile(ofname,true);
  if (newfile==NULL) return false;
  // finishing off things
  outfiles.push_back(newfile);
  fileopen = true;
  return true;
}
//--------------------------
bool jfFD_OutStuff::AddToFile(QString inval) {
    const QString fname = "jfFD_OutStuff::AddToFile";
  // problems
  if (!fileopen) return false;
  if (outfiles.size()==0) return false;
  // no problem
  (*outfiles[outfiles.size()-1]) << inval << QString::fromLatin1("\n");
  return true;
}
//--------------------------
bool jfFD_OutStuff::CloseFile() {
    const QString fname = "jfFD_OutStuff::CloseFile";
  // problems
  if (!fileopen) return false;
  if (outfiles.size()==0) return false;
  // no problem
  CloseFileStream(outfiles[outfiles.size()-1],true);
  fileopen = false;
  return true;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// destructor
jfFD_OutStuff::~jfFD_OutStuff() {
  ClearFiles();
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void jfFD_OutStuff::ClearFiles() {
  size_t findex, fcount;
  fcount = outfiles.size();
  if (fcount>0) {
    if (fileopen) CloseFile();
    for (findex=0;findex<fcount;findex++) {
      delete outfiles[findex];
      outfiles[findex] = NULL;
    }
    outfiles.clear();
  }
  if (filenames!=NULL) {
    delete filenames;
    filenames = NULL;
  }
}
//-----------------------------------------------------------------------
// takes the input number, 0 pads it to a certain width
QString jfFD_OutStuff::FormatNumber(size_t inval) const {
  // faliures
  assert(inval>0);
  assert(gcount>1);
  // getting the number of digits
  size_t num_digits = size_t(ceil(log10((double)gcount)));
  // building the format string
  QString reval = QString("%1").arg(QString::number(inval),num_digits,'0');
  // producting the results
  return reval;
}

//============================================================================
// constructor
jfFanficSplitter::jfFanficSplitter(size_t sguide_in) {
  assert(sguide_in>0);
  prepared_parts = NULL;
  size_guide = sguide_in;
  ofile_count = 0;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++
// adding parts
bool jfFanficSplitter::AddParts(QString inpart) {
  size_t lval,lvallen;
  float rize;
  if (prepared_parts==NULL) prepared_parts = new QStringList();
  prepared_parts->append(inpart);
  lval = (prepared_parts->size()) - 1;
  lvallen = (*prepared_parts)[lval].length();
  rize = float(lvallen)/1024;
  prep_sizes.push_back(rize);
  return true;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++
// getting info
//-----------------------------
size_t jfFanficSplitter::GetPartcount() const {
  if (prepared_parts == NULL) return 0;
  else return (prepared_parts->size());
}
//-----------------------------
size_t jfFanficSplitter::GetFilecount() const {
  return ofile_count;
}
//---------------------------------------------------
// returns the part index of each output file (1 indexed)
size_t jfFanficSplitter::GetFileIndex(size_t file_idx, bool start) const {
  // bad input
  if (file_idx==0) return 0;
  if (file_idx>ofile_count) return 0;
  // we do the result
  // startindex is easy
  if (start) return ofile_index[file_idx-1];
  // endindex is a bit more complicated
  else {
    if (file_idx==ofile_count) return GetPartcount();
    else return ofile_index[file_idx]-1;
  }
}
//-----------------------------------------------------------
// gets back the part text (1 indexed)
QString jfFanficSplitter::GetPart(size_t index) const {
  // bad results
  if (prepared_parts==NULL) return "";
  if (index==0) return "";
  if (index>(prepared_parts->size())) return "";
  // goo result
  return (*prepared_parts)[index-1];
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++
// analysis methods
size_t jfFanficSplitter::CalculateSplit() {
  // variables
  size_t pcount;
  size_t pindex,gindex;
  float sumsize,mixsize,runtotal;
  float avg_psize, pmult;
  // we start with special cases
  pcount = GetPartcount();
  if (pcount==0) return 0;
  if (pcount == 1) {
    ofile_count = 1;
    ofile_index.push_back(1);
    return 1;
  }
  // more general case, first we estimate the size of an even split
  sumsize = 0;
  for (pindex=0;pindex<pcount;pindex++) sumsize+=prep_sizes[pindex];
  /* Since we only measure part size, fics with many small parts can result in
  too large files. We attempt to compensate for that here. */
  avg_psize = sumsize / ((float)pcount);
  pmult = 7.0 / avg_psize;
  if (pmult<1) pmult = 1;
  // the final file count
  ofile_count = ceil((sumsize/size_guide)*pmult);
  /* we next loop thru the parts (if more than 1 output file), summing
  sizes to get the part that starts each file */
  if (ofile_count==1) ofile_index.push_back(1);
  else {
    mixsize = sumsize/(float(ofile_count));
    pindex = 0;
    for (gindex=0;gindex<ofile_count;gindex++) {
      /* special case : pindex is at the maximum already, so we should reduce
      ofile_count. This is usually caused by very big parts (>size_guide) */
      if (pindex>=pcount) {
        ofile_count = gindex;
        break;
      }
      // moving with the rest
      runtotal = 0;
      ofile_index.push_back(pindex+1);
      for (;pindex<pcount;pindex++) {
        runtotal += prep_sizes[pindex];
        if (runtotal>mixsize) {
          pindex++;
          break;
        }
      }
      /* before the next loop, recalculating mixsize because runtotal is always
       more than mixsize, which tends to later groups/files being undersized or
       even empty */
      sumsize -= runtotal;
      mixsize = sumsize/(float(ofile_count-gindex-1));
    }
  }
  // done
  return ofile_count;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++
// destructor
jfFanficSplitter::~jfFanficSplitter() {
  if (prepared_parts != NULL) {
    delete prepared_parts;
    prepared_parts = NULL;
  }
}
//============================================================================
// constructors
//------------------
jfFanfic::jfFanfic(jf_FICTYPE intype) {
  sdata = NULL;
  dataset = false;
  sguide = 350;
  theskel = mainskelc->AtIndexF(2);
  split_manager = NULL;
  output_manager = NULL;
  output_started = false;
  fictype = intype;
}
//------------------
jfFanfic::jfFanfic(jfFicExtract* inval) {
  sdata = NULL;
  dataset = false;
  sguide = 350;
  theskel = mainskelc->AtIndexF(2);
  split_manager = NULL;
  output_manager = NULL;
  output_started = false;
  if (inval!=NULL) SetFic(inval);
  fictype = inval->GetFicType();
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// data setting methods
//---------------------------------------------
bool jfFanfic::SetFic(jfFicExtract* inval, bool changefilebase) {
  const QString fname = "jfFanfic::SetFic";
  if (inval==NULL) return false;
  ClearStuff();
  sdata = inval;
  dataset = true;
  if (changefilebase) {
    filebase = UBReplace(inval->fic_title,UFNAME_DELIMS);
    sdata->fname_core = filebase;
  }
  return true;
}
//--------------------------------------------
bool jfFanfic::AddPart(jfFicPart* part_to_add) {
  if (part_to_add == NULL) return false;
  maindata.push_back(part_to_add);
  return true;
}
//-------------------------------------------
bool jfFanfic::SetFileBase(QString inval, QString indir, size_t in_sguide) {
  if (inval.isEmpty()) return false;
  filebase = inval;
  dirbase = indir;
  sguide = in_sguide;
  if (sdata!=NULL) sdata->fname_core = filebase;
  return true;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// info methods
//------------------
size_t jfFanfic::PartCount() const {
  if ((sdata==NULL) || (!dataset)) return 0;
  if (maindata.size()==0) return sdata->pcount;
  else return maindata[maindata.size()-1]->partcount;
}
//------------------
bool jfFanfic::MultiParts() const {
  return (PartCount()>1);
}
//------------------
bool jfFanfic::IsDataset() const {
  return dataset;
}
//-------------------------------------------
bool jfFanfic::LoadCoreValues(jfSkeletonParser* inparser) const {
  // we do not have the data included
  if (inparser==NULL) return false;
  if (!dataset) return false;
  // calling the FicExtract
  sdata->LoadCoreValues(inparser);
  return true;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// produces the the html output
bool jfFanfic::BuildResult() {
  // variables
  size_t lindex;
  QString work_text, bundle_text;
  // going ahead
  if (!StartResult()) return false;
  // the next thing to do it to make the files, we loop thru the files
  for (lindex=1;lindex<=(split_manager->GetFilecount());lindex++) {
    WriteFile(lindex);
  }
  // done
  return true;
}
//------------------------------------------------------------------------
bool jfFanfic::StartResult() {
  // constants
  const QString fname = "jfFanfic::StartResult";
  /**/JDEBUGLOG(fname,1);
  // first, we check to see if things are ok to go ahead
  if (!dataset) return false;
  /**/JDEBUGLOG(fname,2);
  if ((sdata->pcount)!=maindata.size()) return false;
  /**/JDEBUGLOGI(fname,3,maindata.size());
  if (theskel==NULL) return false;
  /**/JDEBUGLOG(fname,4);
  // variables
  size_t lindex, gfcount;
  QString work_text, bundle_text;
  bool lcvres;
  // initializing
  /**/JDEBUGLOG(fname,5);
  split_manager = new jfFanficSplitter(sguide);
  output_manager = new jfFD_OutStuff();
  output_manager->parse = new jfSkeletonParser();
  output_manager->base = theskel;
  // we now load some initial values
  /**/JDEBUGLOG(fname,6);
  lcvres = LoadCoreValues(output_manager->parse);
  assert(lcvres);
  // next, we loop to create the formatted output for the parts
  for (lindex=0;lindex<(sdata->pcount);lindex++) {
    maindata[lindex]->LoadCoreValues(output_manager->parse);
    work_text = output_manager->MakeResult(output_manager->base->item_skel);
    split_manager->AddParts(work_text);
  }
  // the parts are done, we now get and set the number of files
  /**/JDEBUGLOG(fname,8);
  split_manager->CalculateSplit();
  gfcount = split_manager->GetFilecount();
  output_manager->SetNameInfo(filebase,dirbase,gfcount);
  // next, the navbar for the files (if necessary)
  bundle_text = "";
  /**/JDEBUGLOG(fname,9);
  if (gfcount>1) {
      /**/JDEBUGLOG(fname,10);
    for (lindex=1;lindex<=gfcount;lindex++) {
      // we need to initialize 2 values
      lcvres = output_manager->GetFilename(work_text,lindex);
      assert(lcvres);
      output_manager->parse->AddText("OFILE_FILENAME",work_text);
      output_manager->parse->AddText("OFILE_LABEL",PLabelMaker(lindex));
      // next, we needs to make the item and separator
      work_text = output_manager->MakeResult(output_manager->base->grouplink);
      bundle_text += work_text;
      if (lindex!=gfcount) {
        work_text = output_manager->MakeResult(output_manager->base->grouplink_separator);
        bundle_text += work_text;
      }
    }
  }
  /**/JDEBUGLOG(fname,11);
  // adding the navbar to the parser
  output_manager->parse->AddText("OFILE_NAVBAR",bundle_text);
  // done
  output_started = true;
  return true;
}
//------------------------------------------------------------------------
// wriring an output file... StartResult() mult be run first
bool jfFanfic::WriteFile(size_t findex_in) {
  // constants
  const QString fname = "jfFanfic::WriteFile";
  // asserts
  /**/jerror::AssertLog(output_started,fname,"output_started is false");
  /**/jerror::AssertLog(findex_in!=0,fname,"the fic file index is 0");
  /**/jerror::AssertLog(findex_in<=(split_manager->GetFilecount()),fname,"the fic file index is out of bounds");
  // variables
  QString work_text, bundle_text;
  size_t pindex;
  size_t part_start, part_end, part_count;
  bool sfres;
  // we need to initialize 2 values
  /**/jerror::AssertLog(output_manager != NULL,fname,"The output manager is NULL!");
  assert(output_manager->GetFilename(work_text,findex_in));
  output_manager->parse->AddText("OFILE_FILENAME",work_text);
  output_manager->parse->AddText("OFILE_LABEL",PLabelMaker(findex_in));
  /**/JDEBUGLOG(fname,1);
  // next up, we need to build the navbar for the parts...
  part_start = split_manager->GetFileIndex(findex_in,true);
  part_end = split_manager->GetFileIndex(findex_in,false);
  part_count = part_end - part_start + 1;
  // if the part_count is 1, we skip it...
  bundle_text = "";
  /**/JDEBUGLOG(fname,2);
  if (part_count!=1) {
      /**/JDEBUGLOG(fname,3);
    for (pindex=part_start;pindex<=part_end;pindex++) {
      maindata[pindex-1]->LoadCoreValues(output_manager->parse);
      work_text = output_manager->MakeResult(output_manager->base->partlink);
      bundle_text += work_text;
      if (pindex!=part_end) {
        work_text = output_manager->MakeResult(output_manager->base->partlink_separator);
        bundle_text += work_text;
      }
    }
  }
  /**/JDEBUGLOG(fname,3);
  // here, we add the result to the parser
  output_manager->parse->AddText("PARTS_NAVBAR",bundle_text);
  // we also start building things
  // the header text
  work_text = output_manager->MakeResult(output_manager->base->header_skel);
  sfres = output_manager->StartNewFile();
  /**/JDEBUGLOG(fname,4);
  assert(sfres);
  output_manager->AddToFile(work_text);
  /**/JDEBUGLOGS(fname,5,work_text);
  // we now loop thru the parts...
  for (pindex=part_start;pindex<=part_end;pindex++) {
      /**/JDEBUGLOGST(fname,6,pindex);
    // this is probably not really necessary
    maindata[pindex-1]->LoadCoreValues(output_manager->parse);
    // adding the item text to the results
    output_manager->AddToFile(split_manager->GetPart(pindex));
    /**/JDEBUGLOG(fname,7);
    // the item separattor
    if (pindex!=part_end) {
      work_text = output_manager->MakeResult(output_manager->base->item_separator);
      output_manager->AddToFile(work_text);
    }
  }
  // done with the loop
  /**/JDEBUGLOG(fname,8);
  work_text = output_manager->MakeResult(output_manager->base->footer_skel);
  output_manager->AddToFile(work_text);
  // finishing
  output_manager->CloseFile();
  /**/JDEBUGLOG(fname,9);
  return true;
}
//--------------------------------------------
// clearing up some stuff after writing the files
void jfFanfic::FinishAfter() {
  assert(output_started);
  // clearing
  delete split_manager;
  split_manager = NULL;
  delete output_manager->parse;
  delete output_manager;
  output_manager = NULL;
  output_started = false;
}
//----------------------------------------------
size_t jfFanfic::GetFilecount() const {
  if (!output_started) return 0;
  else return split_manager->GetFilecount();
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// destructors
jfFanfic::~jfFanfic() {
  ClearStuff();
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// internal methods
void jfFanfic::ClearStuff() {
  for (int qx=0;qx<maindata.size();qx++) {
    delete maindata[qx];
    maindata[qx] = NULL;
  }
  maindata.clear();
  if (sdata!=NULL) {
    delete sdata;
    sdata = NULL;
  }
  if (split_manager!=NULL) {
    delete split_manager;
    split_manager = NULL;
  }

  dataset = false;
}
//-----------------------------------------------------------------------
QString jfFanfic::PLabelMaker(size_t indexv) const {
  // checks and asserts
  assert(indexv>=0);
  assert(split_manager!=NULL);
  // variables
  QString resval = "Part";
  bool single;
  size_t start,end;
  // we get the data values
  start = split_manager->GetFileIndex(indexv,true);
  end = split_manager->GetFileIndex(indexv,false);
  // checking them
  single = ((end-start)==0);
  // making the result
  if (single) resval+= " " + QString::number(start);
  else {
    resval += "s " + QString::number(start);
    resval += " to " + QString::number(end);
  }
  // sone
  return resval;
}
//****************************************************************************
