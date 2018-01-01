/////////////////////////////////////////////////////////////////////////////
// Name:        dirupdate.cpp
// Author :     John Q Metro
// Purpose :    A Tool for updating directory contants to match a source
// Created:     June 11, 2015
// conversion to Qt started August 1, 2016
// Updated:     July 6, 2015
/////////////////////////////////////////////////////////////////////////////
#ifndef DIRUPDATE_H_INCLUDED
  #include "dirupdate.h"
#endif // DIRUPDATE_H_INCLUDED
//-----------------------------------------
#ifndef UTILS3_H_INCLUDED
  #include "utils3.h"
#endif // UTILS3_H_INCLUDED
#ifndef LOGGING_H_INCLUDED
  #include "logging.h"
#endif // LOGGING_H_INCLUDED
#ifndef UTILS1_H_INCLUDED
  #include "utils1.h"
#endif // UTILS1_H_INCLUDED
//-----------------------------------------
#include <assert.h>
#include <QDir>
#include <QFile>
//***************************************************************************

QString UpdateToString(const jfe_UpdateAction &inact) {
  if (inact == jfe_NOTCHECKED) return "NOTCHECKED ERROR";
  if (inact == jfe_DONOTHING) return "Skipping";
  if (inact == jfe_NEW) return "Copying Over";
  if (inact == jfe_UPDATE) return "Updating";
  if (inact == jfe_DELETE) return "Deleting";
  else {
    assert(false);
    return "";
  }
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
jfFileInfo::jfFileInfo() {
  xup = jfe_NOTCHECKED;
  fileptr = NULL;
}
//----------------------------------------
jfFileInfo::~jfFileInfo() {
  if (fileptr!=NULL) delete fileptr;
  fileptr = NULL;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// methods
QFileInfo* jfFileInfo::MakeFilePointer(QString basedir) const {
  const QString fname = "jfFileInfo::MakeFilePointer";
  // special cases
  if (pathname.isEmpty()) return NULL;
  if (basedir.isEmpty()) return NULL;
  // creation
  basedir = QDir::fromNativeSeparators(basedir.trimmed());
  if (!basedir.endsWith('/')) basedir += '/';
  QString qstro = basedir + pathname;
  /**/JDEBUGLOGS(fname,1,qstro);
  QFileInfo* result = new QFileInfo(qstro);
  return result;
}
//-------------------------------------------------------------------
jfFileInfo* jfFileInfo::MakeNewCopy(QString basedir, bool withptr) const {
  const QString fname = "jfFileInfo::MakeNewCopy";
  jfFileInfo* result;
  result = new jfFileInfo();
  result->isdir = isdir;
  result->pathname = pathname;
  /**/JDEBUGLOGS(fname,1,pathname);
  result->xup = jfe_NEW;
  if (!isdir) {
    result->moddate = moddate;
    if (withptr) result->fileptr = MakeFilePointer(basedir);
  }
  return result;
}
//----------------------------------------------------------------------
QString jfFileInfo::GetStringRep() const {
  QString result = pathname + " ;; ";
  result += UpdateToString(xup) + " ;; ";
  if (isdir) result += "DIRECTORY ;;";
  else result += "FILE ;; ";
  if (fileptr!=NULL) result += "Has File Pointer";
  result += "\n";
  return result;
}
//***************************************************************************
// constructor
jfDirectoryUpdater::jfDirectoryUpdater(bool casesen_in) {
  sourceloaded = false;
  targetloaded = false;
  target_index = -1;
  casesen = casesen_in;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// loading the dirctory info
//--------------------------------------
bool jfDirectoryUpdater::LoadSource(QString ldir) {
  ClearList(false);
  if (LoadList(ldir,sourcelist)) {
    sourceloaded = true;
    sourcedir = ldir;
    return true;
  }
  else return false;
}
//--------------------------------------
bool jfDirectoryUpdater::LoadTarget(QString ldir) {
  ClearList(true);
  if (LoadList(ldir,targetlist)) {
    targetloaded = true;
    targetdir = ldir;
    return true;
  }
  else return false;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// processing
//--------------------------------------
bool jfDirectoryUpdater::CompareSourceAndTarget() {
  const QString fname = "jfDirectoryUpdater::CompareSourceAndTarget";
  /**/JDEBUGLOG(fname,1);
  if ((!sourceloaded)||(!targetloaded)) return false;
  // variables
  std::vector<jfFileInfo*> *newlist;
  jfFileInfo* tcopy;
  size_t scount;
  // preparing before we start
  scount = sourcelist.size();
  /**/JDEBUGLOGST(fname,2,scount);
  tindex = 0;
  newlist = new std::vector<jfFileInfo*>();
  /**/JDEBUGLOG(fname,3);
  // the outer loop
  for (sindex=0;sindex<scount;sindex++) {
    // the lists are sorted
    while (true) {
      /**/JDEBUGLOGST(fname,4,sindex);
      /**/JDEBUGLOGS(fname,5,sourcelist[sindex]->pathname);
      // targets exhaused, copy everything
      if (tindex>=targetlist.size()) {
        tcopy = sourcelist[sindex]->MakeNewCopy(sourcedir,true);
        /**/JDEBUGLOGST(fname,6,tindex);
        newlist->push_back(tcopy);
        break;
      }
      // here, a file or directory is in the source but not the target
      else if (CompareItems()<0) {
        tcopy = sourcelist[sindex]->MakeNewCopy(sourcedir,true);
        /**/JDEBUGLOGS(fname,7,targetlist[tindex]->pathname);
        newlist->push_back(tcopy);
        break;
      }
      // file or directory present in both
      else if (CompareItems()==0) {
        /**/JDEBUGLOGS(fname,8,sourcelist[sindex]->pathname);
        // this item is a directory, do nothing
        if (sourcelist[sindex]->isdir) {
          /**/JDEBUGLOGST(fname,9,sindex);
          tcopy = sourcelist[sindex]->MakeNewCopy(sourcedir,false);
          tcopy->xup = jfe_DONOTHING;
        }
        else {
            // otherwise, compare dates
            int difftime = (targetlist[tindex]->moddate).secsTo((sourcelist[sindex]->moddate));
            if (abs(difftime) > 3660) { // 61 min difference, to cover DST issues and variance
                QString date1 = (sourcelist[sindex]->moddate).toString();
                QString date2 = (targetlist[tindex]->moddate).toString();
                /**/JDEBUGLOGS(fname,10,date1)
                /**/JDEBUGLOGS(fname,10,date2)
                /**/JDEBUGLOGI(fname,10,difftime)
                tcopy = sourcelist[sindex]->MakeNewCopy(sourcedir,true);
                tcopy->xup = jfe_UPDATE;
            }
            else {
                /**/JDEBUGLOGST(fname,11,sindex);
                tcopy = sourcelist[sindex]->MakeNewCopy(sourcedir,false);
                tcopy->xup = jfe_DONOTHING;
            }
        }
        newlist->push_back(tcopy);
        /**/JDEBUGLOGST(fname,12,sindex);
        tindex++;
        /**/JDEBUGLOGST(fname,13,tindex);
        break;
      }
      // file in target but not source : delete!
      else {
        /**/JDEBUGLOGST(fname,14,sindex);
        SettingUpDelete(newlist);
      }
      /**/JDEBUGLOGST(fname,15,sindex);
    }
    // end of the for loop
  }
  // done
  SettingUpDelete(newlist);
  // finishing
  ClearList(true);
  targetlist = (*newlist);
  targetloaded = true;
  // return true;
  return true;
}
//--------------------------------------
void jfDirectoryUpdater::ResetTargetIndex() {
  target_index = -1;
}
//--------------------------------------
bool jfDirectoryUpdater::NextTarget() {
  if (target_index==targetlist.size()) return false;
  target_index++;
  return true;
}
//--------------------------------------
bool jfDirectoryUpdater::DoCurrentTargetAction() {
    const QString fname = "jfDirectoryUpdater::DoCurrentTargetAction";
    // variables
    jfFileInfo* ctag;
    jfe_UpdateAction qup;
    QString sourcepath, destpath;
    bool reval;
    // special cases
    if (target_index<0) return false;
    if (target_index>=targetlist.size()) return false;
    // starting
    ctag = targetlist[target_index];
    qup = ctag->xup;
    // do nothing case
    if ((qup==jfe_NOTCHECKED)||(qup==jfe_DONOTHING)) { }
    // do stuff
    else {
        // building target info
        destpath= targetdir + "/" + (targetlist[target_index]->pathname);
        // we are dealing with a directory
        if (ctag->isdir) {
            if (qup==jfe_NEW) {
                /**/JDEBUGLOGS(fname,2,destpath);
                QDir dir = QDir::root();
                dir.mkpath(destpath);
            }
            else if (qup==jfe_DELETE) {
                reval = DeleteDirectoryTree(destpath);
            }
            else assert(false);
        }
        // we are dealing with a file
        else {
            bool fresult;
            /**/JDEBUGLOG(fname,3);
            // overwrite or copy file
            if ((qup==jfe_NEW) || (qup==jfe_UPDATE)) {
                /**/JDEBUGLOGS(fname,4,destpath)
                if (QFile::exists(destpath)) QFile::remove(destpath);
                sourcepath = ctag->fileptr->absoluteFilePath();
                /**/JDEBUGLOGS(fname,5,sourcepath)
                fresult = QFile::copy(sourcepath,destpath);
                /**/JDEBUGLOGB(fname,6,fresult)
            }
            // deleting file in target
            else {
                fresult = QFile::remove(destpath);
                /**/JDEBUGLOGB(fname,7,fresult)
            }
        }
    }
    return true;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Target / Writable info
//-----------------------------------------------
size_t jfDirectoryUpdater::TargetCount() const {
  return targetlist.size();
}
//-------------------------------------
QString jfDirectoryUpdater::CurrentTargetPath() const {
  if (target_index<0) return "";
  else if (targetlist[target_index]==NULL) return "";
  else return targetlist[target_index]->pathname;
}
//--------------------------------------
bool jfDirectoryUpdater::CurrentTargetOkay() const {
  QString xfilen;
  // basic bad conditions
  if (target_index<0) return false;
  if (targetlist[target_index]==NULL) return false;
  // checking the file pointer
  jfe_UpdateAction qxup = targetlist[target_index]->xup;
  if ((qxup==jfe_NEW)||(qxup==jfe_UPDATE)) {
    if (!(targetlist[target_index]->isdir)) {
      if ((targetlist[target_index]->fileptr)==NULL) return false;
      if (!(targetlist[target_index]->fileptr)->isReadable()) return false;
    }
  }
  // checking writable (skip)
  return true;
}
//--------------------------------------
QString jfDirectoryUpdater::GetCurrentTargetAction() const {
  if (target_index<0) return "";
  if (target_index>=targetlist.size()) return "";
  return UpdateToString(targetlist[target_index]->xup);
}
//-------------------------------------
QString jfDirectoryUpdater::GetCurrentTargetInfo() const {
  if (target_index<0) return "";
  if (target_index>=targetlist.size()) return "";
  return targetlist[target_index]->GetStringRep();
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// destructor
jfDirectoryUpdater::~jfDirectoryUpdater() {
  ClearList(true);
  ClearList(false);
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// helper methods
//--------------------------------------
bool jfDirectoryUpdater::ClearList(bool target) {
  size_t idxer, llen;
  if (target) llen = targetlist.size();
  else llen = sourcelist.size();
  for (idxer=0;idxer<llen;idxer++) {
    if (target) delete targetlist[idxer];
    else delete sourcelist[idxer];
  }
  if (target) {
    targetlist.clear();
    targetloaded = false;
    target_index = -1;
  }
  else {
    sourcelist.clear();
    sourceloaded = false;
  }
  return true;
}
//--------------------------------------
bool jfDirectoryUpdater::LoadList(QString loaddir, std::vector<jfFileInfo*> &thelist) {
  const QString fname = "jfDirectoryUpdater::LoadList";
  QDir tdir = QDir();
  // initial check
  if (!tdir.exists(loaddir)) return false;
  // base variables
  jfFileInfo* cfile;
  QFileInfo* cfilex;
  QStringList flist;
  QString cname;
  bool pprefixed;
  size_t findex, fcount;
  // setting things up
  /**/JDEBUGLOG(fname,1);
  loaddir = QDir::fromNativeSeparators(loaddir.trimmed());
  if (!loaddir.endsWith('/')) loaddir += '/';
  // getting the list of files
  GetDirectoryTreeList(loaddir,false,flist);
  SortStringList(&flist,casesen);
  fcount = flist.size();
  /**/JDEBUGLOGST(fname,2,fcount);
  // going over the list
  for (findex=0;findex<fcount;findex++) {
    /**/JDEBUGLOGST(fname,3,findex);
    cname = flist.at(findex);
    /**/JDEBUGLOGS(fname,4,cname)
    cfilex = new QFileInfo(cname);
    // loading the file info
    cfile = new jfFileInfo();
    cfile->isdir = cfilex->isDir();
    /**/JDEBUGLOGB(fname,5,cfile->isdir)
    if (!cfile->isdir) {
      /**/JDEBUGLOG(fname,6)
      cfile->moddate = cfilex->lastModified();
    }
    /**/JDEBUGLOGS(fname,7,loaddir)
    pprefixed = cname.startsWith(loaddir);
    /**/JDEBUGLOGB(fname,8,pprefixed)
    assert(pprefixed);
    cfile->pathname = cname.mid(loaddir.length());
    /**/JDEBUGLOGS(fname,9,cfile->pathname)
    // clearing
    delete cfilex;
    // adding
    thelist.push_back(cfile);
    /**/JDEBUGLOG(fname,10)
  }
  // done
  return true;
}
//--------------------------------------
bool jfDirectoryUpdater::SettingUpDelete(std::vector<jfFileInfo*>* &xnewlist) {
  const QString fname = "jfDirectoryUpdater::SettingUpDelete";
  // variables
  bool dirdelete;
  QString deletedir;
  jfFileInfo* tcopy;
  // starting
  dirdelete = false;
  // special case
  if (tindex>=targetlist.size()) return true;
  // deletion loop
  while (true) {
    // directory deleting mode
    if (dirdelete) {
      // anything covered by a dir delete should be deleted already
      if ((targetlist[tindex]->pathname).startsWith(deletedir)) {
        tcopy = targetlist[tindex]->MakeNewCopy(targetdir,false);
        tcopy->xup = jfe_DONOTHING;
        xnewlist->push_back(tcopy);
        tindex++;
      }
      // breaking the dir delete
      else dirdelete = false;
    }
    // not in directory deleting mode
    else {
      // starting directory deleting mode
      if (targetlist[tindex]->isdir) {
        deletedir = targetlist[tindex]->pathname;
        dirdelete = true;
        tcopy = targetlist[tindex]->MakeNewCopy(targetdir,false);
      }
      // deleting isolated files
      else {
        tcopy = targetlist[tindex]->MakeNewCopy(targetdir,false);
      }
      tcopy->xup = jfe_DELETE;
      xnewlist->push_back(tcopy);
      tindex++;
    }
    // checking before the next while loop
    if (tindex>=targetlist.size()) break;
    if (sindex>=sourcelist.size()) continue;
    if (CompareItems() <= 0) break;
  }
  // done
  return true;
}
//-----------------------------------------------------
int jfDirectoryUpdater::CompareItems() const {
  // ensuring indexes are within range
  assert((sindex>=0) && (sindex<sourcelist.size()));
  assert((tindex>=0) && (tindex<targetlist.size()));
  // getting the strings
  QString spath = sourcelist[sindex]->pathname;
  QString tpath = targetlist[tindex]->pathname;
  // case insensetive
  Qt::CaseSensitivity cs = (casesen)?(Qt::CaseSensitive):(Qt::CaseInsensitive);

  return QString::compare(spath,tpath,cs);
}

//***************************************************************************
