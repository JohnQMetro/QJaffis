/******************************************************************************
Name    :   fim_groupobj.h
Author  :   John Q Metro
Purpose :   Group object for fimfiction.net
Created :   July 31, 2013
Started conversion to Qt August 3, 2015
Updated :   July 12, 2017
******************************************************************************/
#ifndef FIM_GROUPOBJ_H_INCLUDED
#define FIM_GROUPOBJ_H_INCLUDED
#endif // FIM_GROUPOBJ_H_INCLUDED
//----------------------------------------------------------------------
#ifndef BASEOBJ_H_INCLUDED
    #include "../../../core/objs/baseobj.h"
#endif // BASEOBJ_H_INCLUDED

#ifndef TYPEDCOLLECT_H_INCLUDED
  #include "../../../core/structs/typedcollect.h"
#endif // TYPEDCOLLECT_H_INCLUDED

//******************************************************************************
class jfFIMGroup : public jfBasePD {
  public:
    // constructor
    jfFIMGroup();
    // data setting
    bool SetFromString(const QString& inval, QString& parse_err);
    // data getting
    size_t GetFicCount() const;
    size_t GetMemCount() const;
    QString GetForumlink() const;
    QDate GetDate() const;
    QString GetFounder() const;
    // implemented virtual methods
    virtual QString ToText() const;
    virtual QString ToDisplayHTML() const;
    virtual bool LoadValues(jfSkeletonParser* inparser) const;
    virtual void ProcessDescription();
    virtual QString GetTypeID() const;
  protected:
    bool ParseDateSet(const QString& invalue);
    // file i/o output
    virtual bool AddMiddleToFile(QTextStream* outfile) const;
    virtual bool AddRestToFile(QTextStream* outfile) const;
    // file i/o input
    virtual bool ReadMiddleFromFile(jfFileReader* infile);
    virtual bool ReadRestFromFile(jfFileReader* infile);
    // internal data
    size_t ficcount,memcount;
    QString forumlink;
    QString founder,datelabel;
    QDate last_comment;
    QString rawhtml_desc;
};
//==================================================================
// a forward declaration
class jfFIMGroupSearch;
//----------------------------
class jfFIMGroupCollection : public jfTypedCollection<jfFIMGroup> {
  public:
    // the constructors
    jfFIMGroupCollection();
    // implemented page methods
    virtual QString GetTypeID() const;
    virtual bool LoadValues(jfSkeletonParser* inparser,size_t which) const;
  protected:
    // some more virtual i/o methods
    virtual bool AddDelta(QTextStream* outfile) const;
    virtual bool ReadDelta(jfFileReader* infile);
};
//******************************************************************************
