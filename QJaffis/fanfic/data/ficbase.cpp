/******************************************************************************
Name    :   ficbase.cpp
Author  :   John Q Metro
Purpose :   Defines base fanfic objects that hold fanfic info
Created :   April 2, 2012
Conversion to QT started : April 20, 2013
Updated :   February 25, 2023
Renamed from ficobj2.cpp
******************************************************************************/
#ifndef FICBASE_H
  #include "ficbase.h"
#endif // FICBASE_H

#ifndef LOGGING_H_INCLUDED
  #include "../../core/utils/logging.h"
#endif // LOGGING_H_INCLUDED

#ifndef UTILS1_H_INCLUDED
  #include "../../core/utils/utils1.h"
#endif // UTILS1_H_INCLUDED

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#include <assert.h>
//****************************************************************************
jfGenericFanfic::jfGenericFanfic():jfBasePD() {
  ustatus = jud_NONE;
  validdata = false;
  part_count = 1;
}
//--------------------------------------------------------------------------
jfGenericFanfic::jfGenericFanfic(const jfGenericFanfic& src) {
  // stuff from jfBaseObj
  src.CopyOver(this);
  // new stuff for FFN Fic objects
  part_count = src.part_count;
  updated_date = src.updated_date;
  author_name = src.author_name;
  author_url = src.author_url;
  // update status
  ustatus = src.ustatus;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// getting values
//-----------------------------------------------------------------------
QString jfGenericFanfic::GetAuthor() const {
  return author_name;
}
//-----------------------------------------------------------------------
QString jfGenericFanfic::GetAuthorUrl() const {
  return author_url;
}
//-----------------------------------------------------------------------
size_t jfGenericFanfic::GetChapterCount() const {
  return part_count;
}
//-----------------------------------------------------------------------
QDate jfGenericFanfic::GetUpdates() const {
  return updated_date;
}
//-----------------------------------------------------------------------
QString jfGenericFanfic::GetPDesc(size_t elen) const {
  QString slice;
  if (description.length()<elen) return description;
  else {
    slice = description.left(elen).trimmed();
    slice += " ...";
    return slice;
  }
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// getting the extract
jfFicExtract* jfGenericFanfic::GetExtract() const {
  jfFicExtract* result_val = MakeExtract();
  result_val->author_name = author_name;
  result_val->fic_id = num_id;
  result_val->pcount = part_count;
  result_val->fic_title = name;
  result_val->updated_date = updated_date;
  LoadIntoExtract(result_val);
  return result_val;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void jfGenericFanfic::SetMissing() {
  ustatus = jud_MISSING;
}
//-------------------------------------------------------------------------
void jfGenericFanfic::SetNew(bool ival) {
  if (ival) ustatus = jud_NEW;
  else ustatus = jud_NONE;
}
//-------------------------------------------------------------------
bool jfGenericFanfic::ChangeAuthorname(QString newname) {
  if (newname!=author_name) {
    author_name = newname;
    ustatus = jud_AUTHORNC;
    author_url = MakeAuthorUrl();
    return true;
  }
  else return false;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void jfGenericFanfic::LoadMoreValues1(jfSkeletonParser* inparser) const {
  // checking
  assert(inparser!=NULL);
  // values
  inparser->AddText("ITEMF_AUTHORNAME",author_name);
  inparser->AddText("ITEMF_AUTHORURL",author_url);
  inparser->AddUInt("ITEMF_CHAPTERCOUNT",part_count);
  inparser->AddText("ITEMF_UPDDATE",updated_date.toString("MM-dd-yyyy"));
  // status
  inparser->AddText("ITEMF_USTATUS",MakeUStatus());
}
//--------------------------------------------------------------
void jfGenericFanfic::StoreToCopy(jfGenericFanfic* destination) const {
  assert(destination!=NULL);
  CopyOver(destination);
  destination->author_name = author_name;
  destination->part_count = part_count;
  destination->updated_date = updated_date;
  destination->ustatus = ustatus;
  destination->author_url = author_url;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
QString jfGenericFanfic::MakeUStatus() const {
  QString result;
  if (ustatus==jud_NONE) return "";
  else {
    // the span indicating the text color
    result = "<span style=\"color:";
    if (ustatus==jud_UPDATED) result += "lime";
    else if (ustatus==jud_MISSING) result += "red";
    else if (ustatus==jud_NEW) result += "aqua";
    else if (ustatus==jud_AUTHORNC) result += "yellow";
    else assert(false);
    // middle part
    result += "\">[";
    // the label
    result += UpdateStatus2String(ustatus);
    // finishing
    result += "]</span>";
  }
  // done
  return result;
}
//--------------------------------------------------------------
QString jfGenericFanfic::DisplayHTMLHeader(size_t mindex) const {
  const QString fname = "jfGenericFanfic::DisplayHTMLHeader";
  // asserts
  // assert(mindex<fms_count);
  assert(mindex<3);
  assert(validdata);
  // varibales
  QString result,texmpx;
  // building the result
  // we start with the table
  result = "<table width=99%";
  // background color based on fic marks...
  texmpx = GetMarkColor(mindex);
  if (!texmpx.isEmpty()) result += " bgcolor=\"#" + texmpx + "\"";
  result += "><tr><td>";
  // building the title line
  result += "<font size=+3 color=#010101><a href=\"";
  result += primarylink + "\">";
  result += name + "</a>";
  // adding the author stuff
  result += " by " + author_name;
  result += "</font> ";
  // status
  if (ustatus!=jud_NONE) {
    result += "<font size=+2 color=";
    if (ustatus==jud_UPDATED) result += "Lime><b>[Updated";
    else if (ustatus==jud_MISSING) result += "Red><b>[Missing";
    else if (ustatus==jud_NEW) result += "Aqua><b>[New";
    else if (ustatus==jud_AUTHORNC) result += "Yellow><b>[Author Name Change";
    result += "!]</b></font>";
  }
  return result;
}
QString jfGenericFanfic::DisplayHTMLHeader(size_t mindex, const jfDisplayHTMLHelper* helper ) const {
    assert(mindex<3);
    assert(validdata);
    // varibales
    QString result,texmpx;
    // building the result
    // we start with the table
    result = "<table width=99%";
    // background color based on fic marks...
    texmpx = helper->GetMarkColor(mindex);
    if (!texmpx.isEmpty()) result += " bgcolor=\"#" + texmpx + "\"";
    result += "><tr><td>";
    // building the title line
    result += helper->WrapHyperlink(primarylink,"fictitle",  name);
    // adding the author stuff
    result += helper->WrapText("fictitle", " by " + author_name);
    // status
    result += helper->UpdateStatus(ustatus);
    return result;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// core extract methods
void jfGenericFanfic::LoadIntoExtractCore(jfFicExtract* into) const {
  // checks
  assert(into!=NULL);
  into->author_name = author_name;
  into->fic_id = num_id;
  into->pcount = part_count;
  into->fic_title = name;
  into->updated_date = updated_date;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// fic mark methods
//------------------------------------------
bool jfGenericFanfic::IsMarked(size_t mindex) const {
  /*
  assert(mindex<fms_count);
  return (ficm::data->fd)[mindex]->ContainsID(num_id);
  */
  return false;
}
//------------------------------------------
QString jfGenericFanfic::GetMarkColor(size_t mindex) const {
    /*jf_FicMark rval;
    assert(mindex<fms_count);
    rval = (ficm::data->fd)[mindex]->GetValue(num_id);
    return fmcolors[(size_t)rval];
    */
  return "";
}
//===========================================================================
jfGenericFanfic2::jfGenericFanfic2():jfGenericFanfic() {
  word_count = 0;
  completed = false;
}
//--------------------------------------------------------
jfGenericFanfic2::jfGenericFanfic2(const jfGenericFanfic2& src):jfGenericFanfic(src) {
  word_count = src.word_count;
  completed = src.completed;
  characters = src.characters;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
size_t jfGenericFanfic2::GetWordcount() const {
  return word_count;
}
//--------------------------------------------------------
bool jfGenericFanfic2::IsComplete() const {
  return completed;
}
//--------------------------------------------------------
const QStringList& jfGenericFanfic2::GetCharacterList() const {
    return characters;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void jfGenericFanfic2::LoadMoreValues2(jfSkeletonParser* inparser) const {
  assert(inparser!=NULL);
  inparser->AddUInt("ITEMF_WORDCOUNT",word_count);
  inparser->AddBool("ITEMF_COMPLETED",completed);
  inparser->AddText("ITEMF_CHARACTERS",characters.join(", "));
}
//---------------------------------------------------
void jfGenericFanfic2::StoreToCopy2(jfGenericFanfic2* destination) const {
    assert(destination!=NULL);
    StoreToCopy(destination);
    destination->word_count = word_count;
    destination->completed = completed;
    destination->characters = characters;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// file i/o output
//----------------------------------------------------------
bool jfGenericFanfic2::AddRestToFile(QTextStream* outfile) const {
  // local variables
  jfOutString xresult;
  // checking and special conditions
  if (outfile==NULL) return false;
  // preparing line 4
  xresult << part_count << completed << word_count << updated_date;
  (*outfile) << xresult << "\n";
  xresult.FullClear();
  // doing line 5
  xresult << author_name << author_url;
  (*outfile) << xresult << "\n";
  xresult.FullClear();
  // line 6 is just characters
  xresult << characters;
  (*outfile) << xresult << "\n";

  // done
  return AddExtraStuff(outfile);
}
//----------------------------------------------------------
bool jfGenericFanfic2::ReadRestFromFile(jfFileReader* infile) {
  const QString funcname = "jfGenericFanfic2::ReadRestFromFile";
  const QString inan = " is not a number!";
  // local variables
  QString buffer;
  // starting
  assert(infile!=NULL);
  // line 4
  if (!infile->ReadParseLine(3,funcname)) return false;
  if (!infile->lp.SIntVal(0,part_count)) return infile->BuildError("The part count"+inan);
  if (!infile->lp.BoolVal(1,completed)) return infile->BuildError("The completed value is not boolean!");
  if (!infile->lp.SIntVal(2,word_count)) return infile->BuildError("The word_count is not a number!");
  // the next value of line 4 is a bit more complicated because it is a date
  buffer = infile->lp.UnEscStr(3);
  updated_date = QDate::fromString(buffer,"M'-'d'-'yy");
  if (!updated_date.isValid()) {
    return infile->BuildError("Unable to convert the Updated Date field properly");
  }

  // line 5
  if (!infile->ReadParseLine(2,funcname)) return false;
  author_name = infile->lp.UnEscStr(0);
  author_url = infile->lp.UnEscStr(1);

  // line 6, characters. The 0 fields is a signal that the field count does not matter
  if (!infile->ReadParseLine(0, funcname)) return false;
  if ((infile->lp.Num()) == 0) characters.clear();
  else {
      QStringList* tlist = (infile->lp).UnEscRest(0);
      characters = *tlist;
      delete tlist;
  }

  // done with the lines
  return ReadExtraStuff(infile);
}
// ==========================================================================
// a mixin class for fanfic items that have pairing info
//++++++++++++++++++++++++++++++++++++++++++++++
int jfFanficPairsMixin::RelationshipCount() const {
    return char_pairs.size();
}
// -------------------------
const QVector<const jfPairingStruct*>& jfFanficPairsMixin::GetRelationships() const {
    return char_pairs;
}
// -------------------------
QString jfFanficPairsMixin::RelationshipsAsDisplayString(bool rom_pipe) const  {
    int csize = char_pairs.size();
    if (csize == 0) return "";
    else if (csize == 1) return char_pairs.at(0)->toDisplayString(rom_pipe);
    else {
        QString res = char_pairs.at(0)->toDisplayString(rom_pipe);
        for (int i = 1; i < csize ; i++) {
            res += ", " + char_pairs.at(i)->toDisplayString(rom_pipe);
        }
        return res;
    }
}
//++++++++++++++++++++++++++++++++++++++++++++++
QString jfFanficPairsMixin::RelationshipsAsStorageString() const {
    int csize = char_pairs.size();
    if (csize == 0) return "";
    else if (csize == 1) return char_pairs.at(0)->toStorageString();
    else {
        QString res = char_pairs.at(0)->toStorageString();
        for (int i = 1; i < csize ; i++) {
            res += "," + char_pairs.at(i)->toStorageString();
        }
        return res;
    }
}
//++++++++++++++++++++++++++++++++++++++++++++++
void jfFanficPairsMixin::ClearPairingVector(QVector<const jfPairingStruct*>& target) const {
    for (int i = 0 ; i < target.size(); i++) {
        const jfPairingStruct* pair = target.at(i);
        target[i] = NULL;
        delete pair;
    }
    target.clear();
}
// -------------------------
void jfFanficPairsMixin::RelationshipCopy(const jfFanficPairsMixin& source) {
    if (! char_pairs.isEmpty()) ClearPairingVector(char_pairs);
    for (const jfPairingStruct* source_pair : source.char_pairs) {
        char_pairs.append(new jfPairingStruct(*source_pair));
    }
}
// -------------------------
void jfFanficPairsMixin::LoadRelationships(jfSkeletonParser* inparser, bool rom_pipe) const {
    assert(inparser != NULL);
    QString relationships = RelationshipsAsDisplayString(rom_pipe);
    inparser->AddText("ITEMF_RELATIONSHIPS",relationships);
}
// -------------------------
bool jfFanficPairsMixin::SetRelationshipsFromString(const QString& source, bool display) {
    QString trim_source = source.trimmed();
    if (trim_source.isEmpty()) {
        ClearPairingVector(char_pairs);
        return true;
    }
    else {
        QVector<const jfPairingStruct*> temp_char_pairs;
        QStringList rpairs = SplitTrim(source, ",");
        for (QString rawpair : rpairs) {
            const jfPairingStruct* tpair = NULL;
            if (display) tpair = jfPairFilterUtils::ParsePairingDisplay(rawpair);
            else tpair = jfPairFilterUtils::ParsePairingStorage(rawpair);
            if (tpair == NULL) {
                ClearPairingVector(temp_char_pairs);
                return false;
            }
            temp_char_pairs.append(tpair);
        }
        // success
        ClearPairingVector(char_pairs);
        if (temp_char_pairs.size() > 0) {
            char_pairs.append(temp_char_pairs);
        }
        return true;
    }
}
// -------------------------------------------
bool jfFanficPairsMixin::ParseAndAddPair(const QString& source, bool display) {
    const jfPairingStruct* tpair = NULL;
    if (display) tpair = jfPairFilterUtils::ParsePairingDisplay(source);
    else tpair = jfPairFilterUtils::ParsePairingStorage(source);

    if (tpair == NULL) return false;
    char_pairs.append(tpair);
    return true;
}

// QVector<const jfPairingStruct*> char_pairs;

//===========================================================================
// default constructors
//----------------------------------------------------------
jfGenericFanfic3::jfGenericFanfic3():jfGenericFanfic2() {
  author_id = 0;
}
//----------------------------------------------------------
jfGenericFanfic3::jfGenericFanfic3(const jfGenericFanfic3& src):jfGenericFanfic2(src) {
  author_id = src.author_id;
  genres = src.genres;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// data getting methods
QString jfGenericFanfic3::GetGenres() const {
  return genres;
}
//----------------------------------------------------------------------------
size_t jfGenericFanfic3::GetAuthorID() const {
  return author_id;
}
//-------------------------------------------------------
void jfGenericFanfic3::ChangeAuthorID(size_t newid) {
  author_id = newid;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void jfGenericFanfic3::LoadMoreValues3(jfSkeletonParser* inparser) const {
    assert(inparser!=NULL);
    inparser->AddText("ITEMF_GENRES",genres);
    inparser->AddUInt("ITEMF_AUTHORID",author_id);
}
// -------------------------------------------
void jfGenericFanfic3::StoreToCopy3(jfGenericFanfic3* destination) const {
    assert(destination!=NULL);
    StoreToCopy2(destination);
    destination->author_id = author_id;
    destination->genres = genres;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    // file i/o output
    //----------------------------------------------------------
bool jfGenericFanfic3::AddExtraStuff(QTextStream* outfile) const {
  // local variables
  jfOutString xresult;
  // checking and special conditions
  if (outfile==NULL) return false;
  // preparing line 7
  xresult << author_id << genres;
  (*outfile) << xresult << "\n";
  xresult.FullClear();
  // done
  return AddMoreExtraStuff(outfile);
}
//----------------------------------------------------------
bool jfGenericFanfic3::ReadExtraStuff(jfFileReader* infile) {
  const QString funcname = "jfGenericFanfic3::ReadRestFromFile";
  const QString inan = " is not a number!";
  // local variables
  QString buffer;
  // starting
  assert(infile!=NULL);
  // line 7
  if (!infile->ReadParseLine(2,funcname)) return false;
  if (!infile->lp.SIntVal(0,author_id)) return infile->BuildError("The author id"+inan);
  genres = infile->lp.UnEscStr(1);
  // done with the lines
  return ReadMoreExtraStuff(infile);
}
//****************************************************************************

