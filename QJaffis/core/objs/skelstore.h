/*****************************************************************************
Name:        skelstore.h
Author :     John Q Metro
Purpose :    holding html skeletons
Created:     November 16, 2009
// Conversion to Qt started March 12, 2013
Updated:     November 9, 2013
******************************************************************************/
#ifndef SKELSTORE_H_INCLUDED
#define SKELSTORE_H_INCLUDED
#endif // SKELSTORE_H_INCLUDED
#ifndef BASEOBJ_H_INCLUDED
    #include "baseobj.h"
#endif // BASEOBJ_H_INCLUDED
/*****************************************************************************/
enum jfs_Skeltype {jfs_SINGLE, jfs_MULTIPLE, jfs_FICPART};
//=============================================================================
class jfSkeletonCore : public jfBaseObj {
  public:
    // public data
    QString header_skel;
    QString item_skel;
    QString item_separator;
    QString footer_skel;
    // the constructor
    jfSkeletonCore();
    // getting data
    QString GetGuide() const;
    virtual jfs_Skeltype GetSkelType() const = 0;
    // setting data
    void SetGuide(const QString& inguide);
  protected:
    // internal helper methods
    void CopyOver(jfSkeletonCore& src);
    // overloaded methods
    virtual bool AddMiddleToFile(QTextStream* outfile) const;
    virtual bool ReadMiddleFromFile(jfFileReader* infile);
    // some extra stuff
    virtual bool AddMoreToFile(QTextStream* outfile) const = 0;
    virtual bool ReadMoreFromFile(jfFileReader* infile) = 0;
    // internal data
    QString skelguide;
};
//============================================================================
class jfSkeletonBase : public jfSkeletonCore {
  public:
    // public data
    QString cat_header;
    QString cat_footer;
    // the constructor
    jfSkeletonBase();
  protected:
    // internal helper methods
    void CopyOver(jfSkeletonBase& src);
    // overloaded methods
    virtual bool AddMoreToFile(QTextStream* outfile) const;
    virtual bool ReadMoreFromFile(jfFileReader* infile);
    // internal data
    QString skelguide;
};
//==========================================================================
class jfSkeletonSingle : public jfSkeletonBase {
  public:
    // the copy constructor
    jfSkeletonSingle();
    jfSkeletonSingle(jfSkeletonSingle* src);
    // created methods
    virtual QString GetTypeID() const;
    virtual jfs_Skeltype GetSkelType() const;
  protected:
    virtual bool AddRestToFile(QTextStream* outfile) const;
    virtual bool ReadRestFromFile(jfFileReader* infile);
};
//==========================================================================
class jfSkeletonMultiple : public jfSkeletonBase {
  public:
    // the copy constructor
    jfSkeletonMultiple();
    jfSkeletonMultiple(jfSkeletonMultiple* src);
    // additional skeletons
    QString catlink;
    QString catlink_separator;
    QString top_footer;
    QString cat_separator;
    // methods
    virtual QString GetTypeID() const;
    virtual jfs_Skeltype GetSkelType() const;
  protected:
    virtual bool AddRestToFile(QTextStream* outfile) const;
    virtual bool ReadRestFromFile(jfFileReader* infile);
};
//====================================================================
/* Skeleton for handling downloading fics fron Fanfiction.Net. We have 4 extra fields for handling
groups links and part links. One of each for the link and separator. */
class jfSkeletonFicPart : public jfSkeletonCore {
  public:
    // constructors
    jfSkeletonFicPart();
    jfSkeletonFicPart(jfSkeletonFicPart* src);
    // additional skeletons
    QString grouplink;
    QString grouplink_separator;
    QString partlink;
    QString partlink_separator;
    // methods
    virtual QString GetTypeID() const;
    virtual jfs_Skeltype GetSkelType() const;
  protected:
    // file i/o
    virtual bool AddRestToFile(QTextStream* outfile) const;
    virtual bool ReadRestFromFile(jfFileReader* infile);
    // some extra stuff
    virtual bool AddMoreToFile(QTextStream* outfile) const;
    virtual bool ReadMoreFromFile(jfFileReader* infile);
};
/*****************************************************************************/
// a class uses to store multiple classes that go together
class jfHtmlParams {
  public:
    QTextStream *outfile;
    jfSkeletonParser* parse;
    jfSkeletonBase* base;
    mutable QString errmsg;
    // the constructor
    jfHtmlParams();
    // info
    bool CheckOk() const;
    QString MakeResult(QString inskel);
    // data access
    jfSkeletonSingle* Single();
    jfSkeletonMultiple* Multiple();
    size_t GetResCat() const;
    bool SetResCat(size_t inval);
    bool AppendLine(const QString oline);
  private:
    size_t rescindex;
};
/*****************************************************************************/
