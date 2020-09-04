/******************************************************************************
Name    :   fim_groupobj.cpp
Author  :   John Q Metro
Purpose :   Group object for fimfiction.net
Created :   July 31, 2013
Started conversion to Qt August 3, 2015
Updated :   September 4 2020
******************************************************************************/
#ifndef FIM_GROUPOBJ_H_INCLUDED
  #include "fim_groupobj.h"
#endif // FIM_GROUPOBJ_H_INCLUDED
#ifndef HTMLPARSE_H_INCLUDED
  #include "../../../core/utils/htmlparse.h"
#endif // HTMLPARSE_H_INCLUDED
#ifndef FIM_GROUPSEARCH_H_INCLUDED
  #include "fim_groupsearch.h"
#endif // FIM_GROUPSEARCH_H_INCLUDED
//----------------------------------------------------------------------

//******************************************************************************
// constructor
jfFIMGroup::jfFIMGroup():jfBasePD() {
  ficcount = memcount = 0;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// data setting
bool jfFIMGroup::SetFromString(const QString& inval, QString& parse_err) {
  const QString fname = "jfFIMGroup::SetFromString";
  // variables
  unsigned long tval;
  QString buffer;
  jfStringParser mparser;
  // starting
  validdata = false;
  mparser.ChangeData(inval);
  /**/JDEBUGLOG(fname,1)
  // here. we assume we have skipped past the <!--Content--> tag
  if (!ParseLinkExtract(mparser,parse_err)) return false;
  // looking for the name
  if (!mparser.MovePast("<a class=\"group_name\"")) {
    parse_err = "Cannot find group name";
    return false;
  }
  if (!mparser.GetDelimited("style=\"\">","</a>",buffer)) {
    parse_err = "Cannot extract group name";
    return false;
  }
  name = buffer.trimmed();
  if (name.isEmpty()) name = "{EMPTY NAME}";
  // getting the founder
  if (!mparser.MovePast("Founded by")) {
    parse_err = "Cannot find founder name";
    return false;
  }
  if (!mparser.GetDelimited("\"><b>","</b></a>",founder)) {
    parse_err = "Cannot extract founder name";
    return false;
  }
  /**/JDEBUGLOGS(fname,2,founder)
  // getting the story count
  if (!mparser.MovePast("<ul class=\"buttons\">")) {
      parse_err = "Cannot find start of special buttons!";
      return false;
  }
  /**/JDEBUGLOG(fname,3)
  if (!mparser.GetDelimitedULong(" class=\"button\">","<br />",tval,parse_err)) {
    parse_err += " While getting the fic count.";
    return false;
  }
  /**/JDEBUGLOGU(fname,4,tval)
  ficcount = tval;
  // getting the number of members
  if (!mparser.GetDelimitedULong(" class=\"button\">","<br />",tval,parse_err)) {
    parse_err += " While getting the member count.";
    return false;
  }
  memcount = tval;
  /**/JDEBUGLOGST(fname,3,memcount)
  // the description
  // the description now comes in multiple blocks...
  rawhtml_desc = "";
  while (mparser.MovePast("<article class=\"article group_description bbcode\">")) {
      if (!mparser.GetMovePast("</article>",buffer)) {
          parse_err = "Malformed group description block!";
          return false;
      }
      rawhtml_desc += buffer.trimmed() + "\n";
  }
  // handling description
  description = htmlparse::Text2BasicHTML(htmlparse::HTML2Text(rawhtml_desc));
  description.replace("!","! ");
  description.replace("?","? ");
  // getting the date of the last comment
  if (mparser.MovePast("<div class=\"comment_list\"")) {
      if (mparser.GetDelimited("data-time=\"","\"",buffer)) {
          if (!ParseDateSet(buffer)) {
              parse_err = "Could not parse last comment date!";
              last_comment = QDate();
              return false;
          }
      }
      else last_comment = QDate();
  }
  else last_comment = QDate();
  /**/JDEBUGLOG(fname,5)
  // done
  validdata = true;
  return true;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
bool jfFIMGroup::SetFromCardString(const QString& inval, QString& parse_err) {
    const QString fname = "jfFIMGroup::SetFromCardString";
    // variables
    jfStringParser mparser(inval);
    // starting up...
    validdata = false;
    // link data
    if (!ParseLinkExtract(mparser,parse_err)) return false;
    // group name
    QString buffer;
    if (!mparser.GetDelimited("<div class=\"group-name\">","</div>",buffer)) {
        parse_err = "Unable to extract group name!";
        return false;
    }
    name = buffer.trimmed();
    if (name.isEmpty()) name = "{EMPTY NAME}";
    // name of the founder
    if (!mparser.GetDelimited("<div class=\"group-info\">","<b>",buffer)) {
        parse_err = "Could not extract founder name!";
        return false;
    }
    founder = buffer.trimmed();
    // creation date...
    if (!mparser.GetDelimited("data-time=\"","\"",buffer)) {
        parse_err = "Could not extract creation date!";
        return false;
    }
    // member count
    ulong tval;
    if (!mparser.GetDelimitedULong("<b>","</b> members",tval,parse_err)) {
        parse_err += " : Could not extract member count!";
        return false;
    }
    memcount = tval;
    // moving
    if (!mparser.MovePast("<div class=\"bottom-links\"")) {
        parse_err = "Could not find count divider!";
        return false;
    }
    // story count
    if (!mparser.GetDelimitedULong("<span class=\"number\">","</span>",tval,parse_err)) {
        parse_err += " : Could not extract story count!";
        return false;
    }
    ficcount = tval;
    // the cards do not includes description or date of last comment, so this is it...
    validdata = true;
    return true;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// data getting
//------------------------------
size_t jfFIMGroup::GetFicCount() const {
  return ficcount;
}
//------------------------------
size_t jfFIMGroup::GetMemCount() const {
  return memcount;
}
//------------------------------
QString jfFIMGroup::GetForumlink() const {
  return forumlink;
}
//------------------------------
QDate jfFIMGroup::GetDate() const {
  return last_comment;
}
//------------------------------
QString jfFIMGroup::GetFounder() const {
  return founder;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// implemented virtual methods
//------------------------------
QString jfFIMGroup::ToText() const {
  QString result,buffer;
  // building the title and founder line
  result += name + " founded by " + founder + "\n";
  // next line: link display
  result += primarylink + "\n";
  // adding the main description
  result += description + "\n";
  // next up.. critical information
  result += "Stories : " + QString::number(ficcount);
  result += " - Members : " + QString::number(memcount);
  result += " - Last Comment : ";
  if (!last_comment.isValid()) result += "Never";
  else result += datelabel;
  // else result += last_comment.Format(wxT("%m-%d-%Y"));
  result += "\n";
  // finally, the forum link
  result += forumlink + "\n";
  // done
  return result;
}
//------------------------------
QString jfFIMGroup::ToDisplayHTML() const {
  const QString fname = "jfFIMGroup::ToDisplayHTML";
  QString result;
  // we start with the table
  result = "<table width=99%><tr><td>";
  // building the title line
  result += "<font size=+3 color=#010101><a href=\"";
  result += primarylink + "\">";
  result += name + "</a>";
  // adding the author stuff
  result += " founded by " + founder;
  result += "</font></td></tr>\n";
  // next line: link display
  result += "<tr><td>";
  result += "<font color=\"green\" size=+1>";
  result += primarylink + "</font><br>\n";
  // adding the main description
  result += "<font color=#010101 size=+2>" + GetDescExtract(5,500) + "<br>\n";
  // next up.. critical information
  result += "<font color=gray size=+1>Published: ";
  result += "Stories : " + QString::number(ficcount);
  result += " - Members : " + QString::number(memcount);
  result += " - Last Comment : ";
  // if (last_comment==wxInvalidDateTime) result += wxT("Never");
  result += datelabel;
  // else result += last_comment.Format(wxT("%m-%d-%Y"));
  result += "</font><br>\n";
  // the forum link
  result += "<font color=green size=+1>";
  result += forumlink + "</font></td></tr>\n</table>";
  // done
  return result;
}
//------------------------------
bool jfFIMGroup::LoadValues(jfSkeletonParser* inparser) const {
  QString rbuffer;
  LoadCoreValues(inparser);
  inparser->AddUInt("ITEMG_FICCOUNT",ficcount);
  inparser->AddUInt("ITEMG_MEMCOUNT",memcount);
  inparser->AddText("ITEMG_FORUMURL",forumlink);
  inparser->AddText("ITEMG_FOUNDER",founder);
  inparser->AddText("ITEMG_PUBDATE",datelabel);
  inparser->AddText("ITEMG_DESCEXT",GetDescExtract(4,300));
  return true;
}
//--------------------------------------
void jfFIMGroup::ProcessDescription() {
}
//------------------------------
QString jfFIMGroup::GetTypeID() const {
  return "FIMGroup";
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// parsing the custom date time
bool jfFIMGroup::ParseDateSet(const QString& invalue) {
    // the input should be a string UNIX timestamp
    ulong tval;
    if (!Str2ULong(invalue,tval)) return false;
    QDateTime temp_datetime;
    temp_datetime.setTime_t((uint)tval);
    if (!temp_datetime.isValid()) return false;
    // here, the QDateTime is valid...
    last_comment = temp_datetime.date();
    datelabel = last_comment.toString("d MMMM yyyy");
    return true;
}
//---------------------------------------------------------
bool jfFIMGroup::ParseLinkExtract(jfStringParser& xparser, QString& perr_out) {
    // getting link data
    ulong tval;
    if (!xparser.GetDelimitedULong("<a href=\"/group/","/",tval, perr_out)) {
      perr_out += " While getting the group link";
      return false;
    }
    num_id = tval;
    QString buffer;
    if (!xparser.GetMovePast("\"",buffer)) {
      perr_out = "Cannot find end of group link";
      return false;
    }
    // creating the link
    primarylink = "https://www.fimfiction.net/group/" + QString::number(num_id);
    primarylink += "/" + buffer;
    forumlink = primarylink + "/forum";
    // done
    return true;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// file i/o output
//------------------------------
bool jfFIMGroup::AddMiddleToFile(QTextStream* outfile) const {
  // local variables
  jfOutString xresult;
  // checking and special conditions
  if (outfile==NULL) return false;
  // line 4
  xresult << ficcount << memcount << founder;
  (*outfile) << xresult << '\n';
  // done
  return true;
}
//------------------------------
bool jfFIMGroup::AddRestToFile(QTextStream* outfile) const {
    // local variables
  jfOutString xresult;
  // checking and special conditions
  if (outfile==NULL) return false;
  // line 5
  xresult << last_comment << forumlink;
  (*outfile) << xresult << '\n';
  // done
  return true;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// file i/o input
//------------------------------
bool jfFIMGroup::ReadMiddleFromFile(jfFileReader* infile) {
  const QString funcname = "jfFIMGroup::ReadMiddleFromFile";
  // reading the line and basic parsing
  assert(infile!=NULL);
  if (!infile->ReadParseLine(3,funcname)) return false;
  // getting the values
  if (!infile->lp.SIntVal(0,ficcount)) return infile->BuildError("The fic count is not valid!");
  if (!infile->lp.SIntVal(1,memcount)) return infile->BuildError("The member count is not valid!");
  founder = infile->lp.UnEscStr(2);
  return true;
}
//------------------------------
bool jfFIMGroup::ReadRestFromFile(jfFileReader* infile) {
  const QString funcname = "jfFIMGroup::ReadRestFromFile";
  // reading the line and basic parsing
  assert(infile!=NULL);
  if (!infile->ReadParseLine(2,funcname)) return false;
  if (!infile->lp.DateVal(0,last_comment)) {
    return infile->BuildError("Unable to convert the last comment date field properly");
  }
  forumlink = infile->lp.UnEscStr(1);
  return true;
}

//*****************************************************************************
// alternate constructor for use when loading data from file
jfFIMGroupCollection::jfFIMGroupCollection():jfTypedCollection<jfFIMGroup>() {
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// implemented page methods
//-----------------------------------------------------
QString jfFIMGroupCollection::GetTypeID() const {
  return "FIMGroupCollection";
}
//-----------------------------------------------------
bool jfFIMGroupCollection::LoadValues(jfSkeletonParser* inparser,size_t which) const {
  LoadCoreValues(inparser,which);
  return true;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// some more virtual i/o methods
//-----------------------------------------------------
bool jfFIMGroupCollection::AddDelta(QTextStream* outfile) const {
  return true;
}
//-----------------------------------------------------
bool jfFIMGroupCollection::ReadDelta(jfFileReader* infile) {
  return true;
}
//*****************************************************************************
