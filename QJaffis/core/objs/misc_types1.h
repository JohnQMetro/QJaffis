/*****************************************************************************
 Name:        misc_types1.h
 Author :     John Q Metro
 Purpose :    Some types
 Created:     April 2, 1010
 Conversion to Qt Started March 11, 2013
 Updated:     December 31, 2017
 ******************************************************************************/
#define JFMISCTYPES1
#include <map>
#include <QString>
#include <QStringList>
#include <QMetaType>
#include <set>
//*****************************************************************************
/* some fanfic (and other things as well), attach text tags to stories/items.
 Therefore, one can has a filter that filters on these. tags can be excluded,
 necessary, alternate... we need a types to hold that data */

enum jfTAG_STATUS { jfts_NONE, jfts_INCLUDE, jfts_ALTERNATE, jfts_EXCLUDE };

Q_DECLARE_METATYPE(jfTAG_STATUS);

class jfTagListing : public std::map<QString,jfTAG_STATUS> {
	public:
		// to and from a single string (semicolon delimited)
    QString ToString() const;
    bool FromString(const QString& instr);
		// initializing
    bool FromArray(const QString insrc[], size_t incount);
    bool FromStrArray(const QStringList* insrc);
		// special, retuns false if empty or if all values are jfts_NONE
    bool Valset() const;
		// tag replacement
    bool ReplaceTag(QString oldt, QString newt);
    bool ReplaceTags(QStringList* replist);
		// special, returns true if the tags match the given list
    bool MatchTags(const char* inlist[], size_t inlistcount);
		/* special, compares inputted tags to tags already in the list. Returns
		FALSE if there are tags in the list not in input. Tags in input that are
		not in the list are added. falsechoice controls what we do if we return
		false. options are 0:ignore, 1:remove, and 2:abort */
    bool Reconcile(const char* inlist[], size_t inlistcount, size_t falsechoice);
	protected:
};

//=======================================================
// pair of integers
typedef struct IntPair {
  int A;
  int B;
} jfIntPair;
//=======================================================
class QStringSet : public std::set<QString> {
  public:
    bool contains(const QString& test) const;
};
//*****************************************************************************
