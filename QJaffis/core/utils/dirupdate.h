/////////////////////////////////////////////////////////////////////////////
// Name:        dirupdate.h
// Author :     John Q Metro
// Purpose :    A Tool for updating directory contents to match a source
// Created:     June 9, 2015
// conversion to Qt Started August 1, 2016
// Updated:     July 6, 2015
/////////////////////////////////////////////////////////////////////////////
#ifndef DIRUPDATE_H_INCLUDED
  #define DIRUPDATE_H_INCLUDED
#endif // DIRUPDATE_H_INCLUDED
//-----------------------------------------
#include <QString>
#include <QDateTime>
#include <vector>
#include <QFileInfo>
//***************************************************************************
// an enumerated type to identify what to do
enum jfe_UpdateAction {jfe_NOTCHECKED, jfe_DONOTHING, jfe_NEW, jfe_UPDATE, jfe_DELETE};
QString UpdateToString(const jfe_UpdateAction &inact);
//---------------------------------------------------
// file data class
class jfFileInfo {
  public:
    // core data
    bool isdir;
    QString pathname;
    jfe_UpdateAction xup;
    QDateTime moddate;
    QFileInfo* fileptr;
    // constructor and destructor
    jfFileInfo();
    virtual ~jfFileInfo();
    // methods
    QFileInfo* MakeFilePointer(QString basedir) const;
    jfFileInfo* MakeNewCopy(QString basedir, bool withptr) const;
    QString GetStringRep() const;
  protected:
};
//---------------------------------------------------------------------------
// the full sync class
class jfDirectoryUpdater {
  public:
  // constructor
    jfDirectoryUpdater(bool casesen_in);
    // loading the dirctory info
    bool LoadSource(QString ldir);
    bool LoadTarget(QString ldir);
    // processing
    bool CompareSourceAndTarget();
    void ResetTargetIndex();
    bool NextTarget();
    bool DoCurrentTargetAction();
    // target info
    size_t TargetCount() const;
    QString CurrentTargetPath() const;
    bool CurrentTargetOkay() const;
    QString GetCurrentTargetAction() const;
    QString GetCurrentTargetInfo() const;
    // destructor
    virtual ~jfDirectoryUpdater();
  protected:
    // helper methods
    bool ClearList(bool target);
    bool LoadList(QString loaddir, std::vector<jfFileInfo*> &thelist);
    bool SettingUpDelete(std::vector<jfFileInfo*>* &xnewlist);
    int CompareItems() const;
    // source
    bool sourceloaded;
    QString sourcedir;
    std::vector<jfFileInfo*> sourcelist;
    // target
    bool targetloaded;
    QString targetdir;
    std::vector<jfFileInfo*> targetlist;
    int target_index;
    // sync
    size_t tindex, sindex;
    // other
    bool casesen;
};
//***************************************************************************
