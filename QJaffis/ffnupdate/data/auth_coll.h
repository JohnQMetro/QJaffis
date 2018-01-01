/******************************************************************************
* Name:        auth_coll.h
* Author :     John Q Metro
* Purpose :    Collections involving author data
* Created:     January 19, 2011
* Updated:     July 11, 2012
******************************************************************************/
#ifndef AUTH_COLL_H_INCLUDED
#define AUTH_COLL_H_INCLUDED
#endif // AUTH_COLL_H_INCLUDED
//----------------------------------------
#include <map>
/////////////////////////////////////////////////////////////////////////////
#ifndef UPD_TYPES_H_INCLUDED
  #include "upd_types.h"
#endif // UPD_TYPES_H_INCLUDED
#ifndef JFFNFICOBJ
  #include "../../fanficnet/data/ffn_ficobj.h"
#endif // BASEOBJ_H_INCLUDED
//*****************************************************************************
// holds the fic contents of a Fanfiction.Net author page
class jfAuthorStories : public QObject {
  public:
    // construction
    jfAuthorStories();
    jfAuthorStories(size_t author_id, const QString& author_name);
    // inserting new items
    bool InsertNewFic(jfFFNItemAuthor* new_item);
    // getting the author info
    size_t GetAuthID() const;
    QString GetAuthName() const;
    const jfAuthorInfo* GetAuthorInfo() const;
    // public checking methods
    bool InclFic(size_t ficidin) const;
    bool IsUpdated(const jfFFNItem* tocheck) const;
    const jfFFNItemAuthor* GetItemPtr(size_t inficidx) const;
    size_t GetICount() const;
    // clearing and emptying data
    void Clear();
    ~jfAuthorStories();
  protected:
    // internal data
    const jfAuthorInfo* author_data;
    std::map<size_t,jfFFNItemAuthor*> maindata;
};
//============================================================================
// holds multiplem jfAuthorStories
class jfAuthorCollection : public QObject {
  public:
    // finding an author
    bool HasAuthor(const jfAuthorInfo* in_authdata) const;
    bool HasAuthor(size_t authorid) const;
    // getting info
    size_t GetAuthCount() const;
    const jfAuthorStories* GetAuthorStories(size_t inauthid) const;
    const jfAuthorInfo* GetAuthorInfo(size_t inauthid) const;
    // adding an author (do not delete in_authdata after, it tis not copied)
    bool AddAuthor(jfAuthorStories* in_authdata, bool replace = false);
    // deleting
    void Clear();
    ~jfAuthorCollection();
  protected:
    // internal data
    std::map<size_t,jfAuthorStories*> maindata;
};
//*****************************************************************************
