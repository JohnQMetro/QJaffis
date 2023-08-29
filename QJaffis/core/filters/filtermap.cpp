/******************************************************************************
Name    :   filtermap.cpp
Author  :   John Q Metro
Purpose :   A Filter collection using the new Base Filter type
Created :   April 3, 2023
Updated :   August 6, 2023
******************************************************************************/
#include "filtermap.h"

#ifndef EXPRESSION_H
  #include "base/expression.h"
#endif // EXPRESSION_H

#ifndef LOGGING_H_INCLUDED
  #include "../utils/logging.h"
#endif // LOGGING_H_INCLUDED

#ifndef PAIRFILTER_H_INCLUDED
  #include "../filters/extended/pairfilter.h"
#endif // PAIRFILTER_H_INCLUDED
/*****************************************************************************/
jfFilterMap::jfFilterMap() {
    exlocmap = NULL;
}
// --------------------------
jfFilterMap::jfFilterMap(const QString& map_name_in) {
    map_name = map_name_in;
    exlocmap = NULL;
}
//---------------------------
bool jfFilterMap::SetMapPointer(const jfFilterMap* nval) {
    exlocmap = nval;
    return (nval != NULL);
}
// ------------------------------------------------
size_t jfFilterMap::CopyLoad() {
    // local variables
    stl_FilterMap::iterator findex;
    stl_FilterMap::iterator thelast;
    QString tagtype;
    jfExpressionFilter* exprfil;
    size_t resval;
    // intialization
    thelast = coredata.end();
    resval = 0;
    // the loop
    for (findex=coredata.begin();findex!=thelast;findex++) {
        tagtype = findex->second->GetTypeIdentifier();
        if (tagtype == EXPRESSION_FILTER_INFO.Identifier()) {
            exprfil = dynamic_cast<jfExpressionFilter*>(findex->second);
            exprfil->CopyLoad();
            resval++;
        }
    }
    // done
    return resval;
}
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++
jfFilterMap::~jfFilterMap() {
    exlocmap = NULL;
}
// --------------------------------------
// deletes all filters, emptying the map
void jfFilterMap::DeleteAll() {
    stl_FilterMap::iterator deldex;
    if (!IsEmpty()) {
        // deleting all filters
        for (deldex=coredata.begin();deldex!=coredata.end();deldex++) {
            delete (deldex->second);
            deldex->second = NULL;
        }
        // emptying the list
        coredata.clear();
    }
}
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++
// data
// --------------------------
QString jfFilterMap::GetName() const {
    return map_name;
}
// --------------------------
bool jfFilterMap::SetName(const QString& newname) {
    if (newname.isEmpty()) return false;
    else {
        map_name = newname;
        return true;
    }
}
// --------------------------
const QString jfFilterMap::GetDescription() const {
    return map_description;
}
// --------------------------
void jfFilterMap::SetDescription(const QString& new_description) {
    map_description = new_description;
}
// --------------------------
bool jfFilterMap::IsEmpty() const {
    return coredata.empty();
}
// ---------------------------
size_t jfFilterMap::FilterCount() const {
    return coredata.size();
}
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++
bool jfFilterMap::VerifyNames(QString& omsg, bool notinmap) const {
  // local variables
  stl_FilterMap::const_iterator findex;
  stl_FilterMap::const_iterator thelast;
  QString tagtype;
  jfExpressionFilter* exprfil;
  bool resval;
  jfNameVerif* vval;
  // intialization
  thelast = coredata.end();
  vval = new jfNameVerif();
  resval = true;
  // the loop
  for (findex=coredata.begin();findex!=thelast;findex++) {
    tagtype = findex->second->GetTypeIdentifier();
    if (tagtype == EXPRESSION_FILTER_INFO.Identifier()) {
      exprfil = dynamic_cast<jfExpressionFilter*>(findex->second);
      resval = exprfil->VerifyNames(vval,notinmap);
      if (!resval) {
        omsg = vval->GetError();
        break;
      }
    }
  }
  delete vval;
  // done
  return resval;
}
// ------------------------------------------------------------------
// calls pairfilter methods regarding statistics logging
size_t jfFilterMap::PairFilterLogging(bool write) {
    // local variables
    stl_FilterMap::iterator findex;
    stl_FilterMap::iterator thelast;
    //
    jfPairFilterCore* exprfil;
    size_t resval;
    // intialization
    thelast = coredata.end();
    resval = 0;
    // the loop
    for (findex = coredata.begin() ; findex != thelast ; findex++) {
        if (IsOldPairFilter(findex->second)) {
            exprfil = dynamic_cast<jfPairFilterCore*>(findex->second);
            if (write) exprfil->WriteLog();
            else exprfil->StartLogging();
            resval++;
        }
    }
    //
    // done
    return resval;
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++
// item operations
// --------------------------
bool jfFilterMap::NameExists(const QString& fname) const {
    stl_FilterMap::const_iterator findex;
    // we do it
    if (coredata.size() > 0) {
        findex = coredata.find(fname);
        if (findex!=(coredata.end())) return true;
        else return false;
    }
    else return false;
}
// --------------------------
stl_FilterMap::const_iterator jfFilterMap::RenameElem(bool& ok, const QString& oldname, const QString& newname) {
    // local varaibles
    jfFilterBase* temp;
    // checks
    assert(NameExists(oldname));
    // the new name is already in?
    if (NameExists(newname)) {
      ok = false;
      return coredata.end();
    }
    // the names are the same, nothing is done
    if (oldname==newname) {
      ok = true;
      return coredata.find(oldname);
    }
    // we are ready to change the name. It seems we must remove the element,
    temp = coredata[oldname];
    coredata.erase(oldname);
    // rename the element,
    temp->SetName(newname);
    // and re-add it to the map
    coredata[newname] = temp;
    // done
    ok = true;
    return coredata.find(newname);
}
//-----------------------------------------------------------------------------
bool jfFilterMap::DeleteByName(const QString& findname) {
    stl_FilterMap::iterator findex;
    // if empty, return false
    if (IsEmpty()) return false;
    // we try to get an iterator
    findex = coredata.find(findname);
    if (findex==coredata.end()) return false;
    // deleting
    delete findex->second;
    findex->second = NULL;
    coredata.erase(findname);
    return true;
}
// --------------------------
jfFilterBase* jfFilterMap::GetItem(const QString& itemname) const {
    // constants
    const QString fname = "jfFilterMap::GetItem";
    // variables
    stl_FilterMap::const_iterator findex;
    // starting
    if (IsEmpty()) return NULL;
    // we get an iterator
    findex = coredata.find(itemname);
    if (findex==coredata.end()) return NULL;
    else return findex->second;
}
// -------------------------------
// adds a filter to the map, the position is given in outindex
bool jfFilterMap::AddFilter(jfFilterBase* infilter, size_t& outindex) {
    // local variables
    QString filtername;
    bool xres;
    size_t qindex=88;
    // the two case where we return false
    if (infilter==NULL) return false;
    filtername = infilter->GetName();
    if (NameExists(filtername)) return false;
    // otherwise, we do the normal insertion...
    coredata[filtername] = infilter;
    // and get the index
    xres = GetIndexByName(filtername,qindex);
    // repositioning the index
    outindex = qindex;
    return xres;
}
//----------------------------------
bool jfFilterMap::ReplaceByName(QString nameold, jfFilterBase* newfilter, size_t& outindex) {
    // local variables
    stl_FilterMap::iterator findex;
    QString newname;

    // starting checks and searches
    if (newfilter==NULL) return false;
    findex = coredata.find(nameold);
    if (findex==coredata.end()) return false;

    // we delete the old filter
    delete findex->second;
    findex->second = NULL;
    // we now check if the name is the same
    newname = newfilter->GetName();
    if (nameold==newname) {
        // same name means we just stick newfilter in the old element
        findex->second = newfilter;
    }
    else {
        // otherwise, we erase the old element, the reinsert
        coredata.erase(nameold);
        coredata[newname] = newfilter;
    }

    // finding the location
    outindex = 0;
    for (findex=coredata.begin();findex!=coredata.end();findex++) {
        if ((findex->first)==newname) break;
        outindex++;
    }
    // done
    return true;

}
// ------------------------------------------------------------
bool jfFilterMap::ReplaceSame(jfFilterBase* newfilter, size_t& outindex) {
    // local variables
    stl_FilterMap::iterator findex;
    QString newname;

    // starting checks and searches
    if (newfilter==NULL) return false;
    newname = newfilter->GetName();
    findex = coredata.find(newname);
    // two options: the item is not here...
    if (findex==coredata.end()) {
        return AddFilter(newfilter,outindex);
    }
    // or it is...
    else {
        // we delete the old filter
        delete findex->second;
        findex->second = NULL;
        // we replace it
        findex->second = newfilter;
        // finding the location
        outindex = 0;
        for (findex=coredata.begin();findex!=coredata.end();findex++) {
            if ((findex->first)==newname) break;
            outindex++;
        }
        // done
        return true;
    }
}
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++
bool jfFilterMap::MatchAll(const jfSearchResultItem* inval) const {
  const QString fname = "jfFilterMap::MatchAll";
  // local variables
  stl_FilterMap::const_iterator findex;
  bool iresult;
  QString mstring;
  // a check
  jerror::AssertLog(inval!=NULL,fname,"MatchAll parameter is not null");
  // the loop
  for (findex = coredata.begin(); findex!=coredata.end(); findex++) {
    mstring = (findex->second)->ToString();
    iresult = (findex->second)->Match(inval);
    if (!iresult) return false;
  }
  return true;
}
// ------------------------------------------------------
stl_FilterMap::const_iterator jfFilterMap::GetStartIterator() const {
    return coredata.begin();
}
// -----------------------------------------------------
bool jfFilterMap::IsEnd(const stl_FilterMap::const_iterator& iterator) const {
    return (coredata.end() == iterator);
}
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++
// file i/o
// --------------------------
bool jfFilterMap::AddToFile(QTextStream* outfile) const {
    // constants and variables
    const QString fname = "jfFilterMap::AddToFile";


    // checking and special conditions
    if (outfile==NULL) return false;
    jfOutString s_out;
    // line 1
    s_out << QString("FilterMap") << map_name << map_description << coredata.size();
    QString qbuf = "###" + s_out;
    (*outfile) << qbuf << '\n';

    // local variables
    stl_FilterMap::const_iterator descdex,enddex;
    // the usual checks
    if (outfile==NULL) return false;
    // we prepare for the loop
    enddex = coredata.end();
    // the loop
    for (descdex = coredata.begin() ; descdex != enddex ; descdex++) {
      // writing
      (descdex->second)->AddToFile(outfile);
    }

    // adding on the footer
    (*outfile) << "###\n";
    return true;
}
// --------------------------
bool jfFilterMap::GetFromFile(jfFileReader* infile) {
    const QString funcname="jfFilterMap::GetFromFile";
    // input data
    size_t expected_count;

    // starting checks
    assert(infile!=NULL);
    /**/JDEBUGLOG(funcname,1)
    // Reading and processing line 1
    if (!infile->ReadParseSLine(4,funcname)) {
        if (infile->Backup(funcname)) {
            return GetFromFileCompat(infile);
        }
        else return false;
    }
    map_name = infile->lp.UnEscStr(1);
    map_description = infile->lp.UnEscStr(2);
    if (!infile->lp.SIntVal(3, expected_count)) {
        return infile->BuildError("The filter count could not be parsed!");
    }
    bool okay = LoadFiltersFromFile(infile, expected_count);
    if (!okay) return false;

    // we should be left just before a final line...
    if (!infile->ReadLast(funcname)) return false;
    return true;
}
// +++++++++++++++++++++++++++++++++++++++++++++++++++++
bool jfFilterMap::LoadFiltersFromFile(jfFileReader* infile, size_t expected_count) {
    const QString funcname = "jfFilterMap::LoadFiltersFromFile";
    // local variables
    size_t loopc;
    QString typid;
    jfFilterBase* newitem;
    // the loop
    /**/JDEBUGLOG(funcname,1)
    for (loopc = 0; loopc< expected_count; loopc++) {
        /**/JDEBUGLOGST(funcname,1,loopc)
        // gets the filter type string (and also the line index)
        if (!infile->GetType(typid,funcname)) return false;
        /**/JDEBUGLOGS(funcname,3,typid)

        // we make an empty default filter
        newitem = allfilters::MakeFilter(typid, "(default " + typid + " name)");
        if (newitem==NULL) {
            jerror::Log(funcname, "Unrecognized Filter type!");
            return infile->BuildError("Unrecognized Filter type! " + typid);
        }
        /**/JDEBUGLOG(funcname,5)

        // expression filters
        if (typid == EXPRESSION_FILTER_INFO.Identifier()) {
            /**/JDEBUGLOG(funcname,6)
            jfExpressionFilter* exp_item = dynamic_cast<jfExpressionFilter*>(newitem);
            if (exlocmap!=NULL) exp_item->SetFiltermapLink(exlocmap);
            else exp_item->SetFiltermapLink(this);
        }

        assert(newitem!=NULL);
        //JDEBUGLOG(fname,7)
        // and then load
        if (!newitem->GetFromFile(infile)) {
            delete newitem;
            jerror::Log(funcname,"Filtermap load from file failed.");
            return false;
        }
        //JDEBUGLOGS(fname,9,newitem->GetName())
        // we add the newly loaded item to the list
        coredata[newitem->GetName()] = newitem;
        //JDEBUGLOG(fname,10)
    }
    return true;
}
// -------------------------------------------------
// added so we can still load old filter maps
bool jfFilterMap::GetFromFileCompat(jfFileReader* infile) {
    const QString funcname="jfFilterMap::GetFromFileCompat";
    // input data
    QString buffer;

    // starting checks
    assert(infile!=NULL);
    // Reading and processing line 1
    if (!infile->ReadParseSLine(3,funcname)) return false;
    buffer = infile->lp.UnEscStr(0);
    if (buffer != "jfFilterMap") {
      return infile->BuildError("The type (" + buffer + ") does not match!");
    }
    size_t num_id_ignore;
    // field two is a number id
    if (!infile->lp.SIntVal(1,num_id_ignore)) {
      return infile->BuildError("The id is not an integer!");
    }
    bool validdata_ignore;
    // field number 3 is the validdata attribute
    if (!infile->lp.BoolVal(2,validdata_ignore)) {
      return infile->BuildError("Validdata is not a boolean!");
    }
    // we get the next line
    if (!infile->ReadParseLine(2,funcname)) return false;
    // the name
    map_name = infile->lp.UnEscStr(0);
    // if (name.isEmpty()) infile->BuildError("The name is blank!");
    // description
    map_description = infile->lp.UnEscStr(1);

    // next, the number of items
    unsigned long tval;
    // we do checks and read the line
    if (!infile->ReadULong(tval,funcname)) return false;
    size_t expected_count = tval;

    bool okay = LoadFiltersFromFile(infile, expected_count);
    if (!okay) return false;

    // we should be left just before a final line...
    if (!infile->ReadLast(funcname)) return false;
    return true;
    // done
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++
// returns the index of a filter with a particular name
bool jfFilterMap::GetIndexByName(const QString& findname, size_t& outindex) const {
  stl_FilterMap::const_iterator findex;
  // finding the name...
  outindex = 0;
  for (findex=coredata.begin();findex!=coredata.end();findex++) {
    if ((findex->first)==findname) break;
    outindex++;
  }
  return (findex!=coredata.end());
}


/*****************************************************************************/
