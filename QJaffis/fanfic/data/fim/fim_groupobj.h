/******************************************************************************
Name    :   fim_groupobj.h
Author  :   John Q Metro
Purpose :   Group object for fimfiction.net
Created :   July 31, 2013
Started conversion to Qt August 3, 2015
Updated :   March 21, 2023
******************************************************************************/
#ifndef FIM_GROUPOBJ_H_INCLUDED
#define FIM_GROUPOBJ_H_INCLUDED
#endif // FIM_GROUPOBJ_H_INCLUDED
//----------------------------------------------------------------------
#include "../../../core/objs/baseitem.h"
#include "../../../core/objs/itemutils.h"

#ifndef TYPEDCOLLECT_H_INCLUDED
  #include "../../../core/structs/typedcollect.h"
#endif // TYPEDCOLLECT_H_INCLUDED

//******************************************************************************
const QString& IdForFIMGroups();

class jfFIMGroup : public jfSearchResultItem {
    friend class jfFIMGroupItemParser;
  public:
    // constructor
    jfFIMGroup();
    jfFIMGroup(const jfFIMGroup& init_data);
    jfFIMGroup(const jfSearchResultItem& source);
    // data getting
    size_t GetFicCount() const;
    size_t GetMemCount() const;
    QString GetForumlink() const;
    QDate GetDate() const;
    QString GetFounder() const;
    // implemented virtual methods
  protected:
    virtual bool LoadMoreValues(jfSkeletonParser* inparser) const override;
    // file i/o output
    virtual bool AddRestToFile(QTextStream* outfile) const override;
    // file i/o input
    virtual bool ReadRestFromFile(jfFileReader* infile) override;
    // internal data
    size_t ficcount,memcount;
    QString forumlink;
    QString founder;
    QDate last_comment;
};
// =================================================================
/** Hold the methods for outputting Text and HTML digests of the source. */
class jfFIMGroupOutputter: public jfResultItemOutputter {
  public:
    jfFIMGroupOutputter();
    void SetSource(const jfFIMGroup* in_source, const jfItemMetaFlags* in_flags);
    virtual void SetItemSource(const jfSearchResultItem* in_source, const jfItemMetaFlags* in_flags) override;
    virtual QString ToText() const override;
    virtual QString ToDisplayHTML() const override;

    virtual ~jfFIMGroupOutputter();
  protected:
    QString FormatLastDate() const;

    const jfFIMGroup* fim_group_source;
};
// =================================================================
/** Generating a jfFIMGroup object from raw html source. */
class jfFIMGroupItemParser : public jfItemParseBase {
  public:
    jfFIMGroupItemParser();
    jfItemParseResultState ParseFromCardSource(const QString& source);
    virtual void Clear() override;
    virtual const jfSearchResultItem* GetUntypedItem() override;
    jfFIMGroup* GetGroupItem();
    ~jfFIMGroupItemParser();
  protected:
    virtual void ClearItem() override;
    virtual jfItemParseResultState InnerParse() override;
    jfItemParseResultState InnerCardParse();
    bool ParseDateSet(const QString& invalue, QDate& result) const;
    bool ParseLinkExtract(QString& perr_out);

    jfFIMGroup* item_result;
    // temp values
    size_t num_id;
    QString url, forumurl;
};

//==================================================================
// a forward declaration
class jfFIMGroupSearch;
//----------------------------
class jfFIMGroupCollection : public jfSearchResultsCollection<jfFIMGroup> {
  public:
    // the constructors
    jfFIMGroupCollection(QString&& in_name, size_t in_num_id);
    jfFIMGroupCollection(const QString& in_name, size_t in_num_id);
    // implemented page methods
    virtual QString TypeId() const override;
    virtual bool LoadValues(jfSkeletonParser* inparser,size_t which) const override;
  protected:
    // some more virtual i/o methods
    virtual bool ReadItemFromFile(jfFileReader* infile, jfItemFlagGroup& target_group) const override;
    virtual bool AddDelta(QTextStream* outfile) const override;
    virtual bool ReadDelta(jfFileReader* infile) override;
};
//******************************************************************************
