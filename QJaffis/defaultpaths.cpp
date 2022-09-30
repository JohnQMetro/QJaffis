/***************************************************************
 * Name:      defaultpaths.cpp
 * Purpose:   A settings class that holds default directories
 * Author:    John Q Metro
 * Created:   December 31, 2021
 * Updated:   June 22, 2022 fix parsing for fetch basics
 **************************************************************/
#ifndef DEFAULTPATHS_H
    #include "defaultpaths.h"
#endif // DEFAULTPATHS_H
//--------------------------------
#ifndef UTILS2_H_INCLUDED
  #include "core/utils/utils2.h"
#endif // UTILS2_H_INCLUDED
//--------------------------------
#include <QDir>
#include <QStandardPaths>
/*************************************************************/
bool jglobal::ValidPathType(jglobal::DEFAULT_PATH_TYPE path_type) {
    return (path_type >= 0) && (path_type < path_type_count);
}
bool jglobal::IsDownloadPathType(DEFAULT_PATH_TYPE path_type) {
    switch(path_type) {
        case SAVED_FANFIC_A:
        case SAVED_FANFIC_B:
        case SAVED_FANFIC_C:
        case SYNC_SOURCE:
        case SYNC_TARGET: return true;
        default: return false;
    }
}


jglobal::jfDefaultPaths::jfDefaultPaths() {
    for(size_t pdex = 0; pdex < path_type_count; pdex++) {
        paths_holder.append("");
    }
    paths_labels << "Fallback Folder" << "Updatable Results" << "Saved Search Results" <<
                    "Saved Searches" << "Main Fanfics" << "Secondary Fanfics 1" <<
                    "Secondary Fanfics 2" << "Sync Source" << "Sync Target" <<
                    "Default Script Folder" << "Script Working Folder";
}
//++++++++++++++++++++++++++++++++++++++
// path info
size_t jglobal::jfDefaultPaths::TotalPathCount() const {
    return path_type_count;
}
//------
bool jglobal::jfDefaultPaths::HasPathFor(DEFAULT_PATH_TYPE path_type) const {
    if (ValidPathType(path_type)) {
        if (path_type >= paths_holder.length()) return false;
        else return (paths_holder.at(path_type).isEmpty() == false);
    }
    else return false;
}
//------
bool jglobal::jfDefaultPaths::HasValidPathFor(DEFAULT_PATH_TYPE path_type) const {
    if (HasPathFor(path_type)) {
        return IsValidPath(paths_holder.at(path_type));
    }
    else return false;
}
//------
QString jglobal::jfDefaultPaths::GetPathFor(DEFAULT_PATH_TYPE path_type) const {
    if (HasPathFor(path_type)) return paths_holder.at(path_type);
    else return "";
}
QString jglobal::jfDefaultPaths::LabelFor(DEFAULT_PATH_TYPE path_type) const {
    if (ValidPathType(path_type)) return paths_labels.at(path_type);
    else return "";
}
//++++++++++++++++++++++++++++++++++++++++++
// changing paths
void jglobal::jfDefaultPaths::MakePathsDefault() {
    // making default paths
    QString default_path = DefaultDocPath();
    QString downpath = GetDefaultPath(true,default_path);
    // assigning the paths
    for(size_t pdex = 0; pdex < path_type_count; pdex++) {
        if (IsDownloadPathType((DEFAULT_PATH_TYPE)pdex)) paths_holder[pdex] = downpath;
        else paths_holder[pdex] = default_path;
    }
}
//-----------------
bool jglobal::jfDefaultPaths::MakePathDefaultFor(DEFAULT_PATH_TYPE path_type) {
    if (ValidPathType(path_type)) {
        if (path_type == DEFAULT_PATH) paths_holder[DEFAULT_PATH] = GetDefaultPath(false,"");
        else if (IsDownloadPathType(path_type)) {
            paths_holder[path_type] = GetDefaultPath(true,DefaultDocPath());
        }
        else paths_holder[path_type] = DefaultDocPath();
        return true;
    }
    else return false;
}
// -------------
bool jglobal::jfDefaultPaths::SetPathFor(DEFAULT_PATH_TYPE path_type, const QString& changeto) {
    if (ValidPathType(path_type) && IsValidPath(changeto)) {
        paths_holder[path_type] = changeto;
        return true;
    }
    else return false;
}
// -------------
bool jglobal::jfDefaultPaths::MakeInvalidPathDefault() {
    QString default_path = DefaultDocPath();
    QString downpath = GetDefaultPath(true,default_path);
    for (size_t pdex = 0; pdex < path_type_count; pdex++) {
        DEFAULT_PATH_TYPE ptype = (DEFAULT_PATH_TYPE)pdex;
        if (HasValidPathFor(ptype)) continue;
        else if (IsDownloadPathType(ptype)) paths_holder[pdex] = downpath;
        else paths_holder[pdex] = default_path;
    }
    return true;
}
// -------------
// file i/o
bool jglobal::jfDefaultPaths::LoadPathsFrom(jfFileReader* input_file) {
    if (input_file == NULL) return false;
    QString readpath;
    for (size_t pdex = 0; pdex < path_type_count; pdex++) {
        QString linename = "Default path " + QString::number(pdex);
        bool readokay = input_file->ReadLine(readpath,linename);
        if (!readokay) return false;
        paths_holder[pdex] = readpath;
    }
    return true;
}
// -------------
bool jglobal::jfDefaultPaths::AddPathsTo(QTextStream* ofile) const {
    if (ofile == NULL) return false;
    for (size_t pdex = 0; pdex < path_type_count; pdex++) {
        (*ofile) << paths_holder.at(pdex) << "\n";
    }
    return true;
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++
bool jglobal::jfDefaultPaths::IsValidPath(const QString& test_path) const {
    if (test_path.isEmpty()) return false;
    QDir test_dir = QDir(test_path);
    if (test_dir.exists()) {
        return test_dir.isReadable();
    }
    else return false;
}
// -----------------
// guaranteed to return a path
QString jglobal::jfDefaultPaths::GetDefaultPath(bool downloads, const QString& fallback) const {
    // try the standard paths first
    QStandardPaths::StandardLocation location_type;
    location_type = (downloads) ? QStandardPaths::StandardLocation::DownloadLocation : QStandardPaths::StandardLocation::DocumentsLocation;
    QString rpath = QStandardPaths::writableLocation(location_type);
    if (rpath.isEmpty() || (QDir(rpath).exists() == false)) {
        if (fallback.isEmpty()) return QDir::homePath();
        else return fallback;
    }
    else return rpath;
}
//---------
QString jglobal::jfDefaultPaths::DefaultDocPath() const {
    if (HasValidPathFor(DEFAULT_PATH)) return GetPathFor(DEFAULT_PATH);
    else return GetDefaultPath(false,"");
}
// ============================================================================

// used to define how a page should be fetched
jglobal::jfFetchBasics::jfFetchBasics() {
    is_valid = false;
    site = jfft_FFN; //
    page_type = FPT_GENERAL;
    fetch_type = jfF_NORMAL;
    script_filename = "";
    transfer_base = "";
}
//++++++++++++++++++++++++++++++++++++++++
// info
bool jglobal::jfFetchBasics::IsValid() const {
    return is_valid;
}
jf_FICTYPE jglobal::jfFetchBasics::SiteTag() const {
    return site;
}
jglobal::jfFICPAGE_TYPE jglobal::jfFetchBasics::PageType() const {
    return page_type;
}
jglobal::jfFETCHER jglobal::jfFetchBasics::FetchType() const {
    return fetch_type;
}
QString jglobal::jfFetchBasics::ScriptFilename() const {
    return script_filename;
}
QString jglobal::jfFetchBasics::TransferBase() const {
    return transfer_base;
}
bool jglobal::jfFetchBasics::ForType(jf_FICTYPE site_tomatch, jfFICPAGE_TYPE page_type_tomatch) const {
    return (site == site_tomatch) && (page_type == page_type_tomatch);
}
//++++++++++++++++++++++++++++++++++++++++
// I/O
bool jglobal::jfFetchBasics::SetFromSCString(const QString& source) {
    jfLineParse lp = jfLineParse(source);
    QString errbuff;
    bool core_valid;
    if (lp.NNotX(6)) return false;
    if (!lp.BoolVal(0,core_valid)) return false;
    // enum types
    int site_int;
    if (!lp.IBoundVal(1,0,4,site_int,errbuff)) return false;
    site = static_cast<jf_FICTYPE>(site_int);
    is_valid = false;
    if (!lp.IBoundVal(2,0,4,site_int,errbuff)) return false;
    page_type = static_cast<jfFICPAGE_TYPE>(site_int);
    if (!lp.IBoundVal(3,0,2,site_int,errbuff)) return false;
    fetch_type = static_cast<jfFETCHER>(site_int);
    // script stuff, if
    script_filename = lp.UnEscStr(4).trimmed();
    transfer_base = lp.UnEscStr(5).trimmed();
    // checking python attribs
    if (fetch_type == jfF_PYTHON) {
        if (script_filename.isEmpty() || transfer_base.isEmpty()) return false;
    }
    // okay
    if (core_valid) is_valid = true;
    return is_valid;
}
QString jglobal::jfFetchBasics::ToIOString() const {
    jfOutString outval;
    outval << is_valid << site << page_type << fetch_type << script_filename << transfer_base;
    return outval;
}
