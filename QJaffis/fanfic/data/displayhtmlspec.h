/******************************************************************************
Name    :   displayhtmlspec.h
Author  :   John Q Metro
Purpose :   Data objects and helpers for ToDisplayHTML()
Created :   August 7, 2022
Updated :   August 8, 2022

******************************************************************************/
#ifndef DISPLAYHTMLSPEC_H
#define DISPLAYHTMLSPEC_H
#endif // DISPLAYHTMLSPEC_
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#ifndef JFMISCTYPES1
  #include "../../core/objs/misc_types1.h"
#endif

#include <QMap>
//****************************************************************************
// font info
class jfDisplayHTMLFont {
  public:
    QString color;
    int relative_size;
    bool bold;
    bool italic;
    bool underline;

    jfDisplayHTMLFont();
    jfDisplayHTMLFont(const QString& incolor, int in_relative_size);

    QString makeStart(int relative_modifier) const;
    QString makeEnd(int relative_modifier) const;
    QString wrapText(const QString& text, int relative_modifier) const;
    QString wrapHyperlink(const QString& url, const QString& text, int relative_modifier) const;
    QString wrapTextNoEsc(const QString& text, int relative_modifier) const;

    bool hasFont(int relative_modifier) const;
    bool hasEffects() const;
    bool isEmpty(int relative_modifier) const;

};
//****************************************************************************
// includes a dont collection and a few utilities
class jfDisplayHTMLHelper {
  public:
    jfDisplayHTMLHelper(QString in_name, int in_relative_modifier);
    // basic info
    QString GetName() const;
    int GetRelativeBaseFontSize() const;
    // font change
    bool SetFont(const QString& font_name, jfDisplayHTMLFont* font);
    bool MakeAndSetFont(const QString& font_name,const QString& incolor, int in_relative_size);
    bool DeleteFont(const QString& font_name);
    // getting font
    jfDisplayHTMLFont* GetFont(const QString& font_name);
    const jfDisplayHTMLFont* GetReadonlyFont(const QString& font_name) const;
    // applyers and helpers
    QString WrapText(const QString& font_name, const QString& text, bool nl = false) const;
    QString WrapHyperlink(const QString& url, const QString& font_name, const QString& text, bool nl = false) const;
    QString WrapTextNoEsc(const QString& font_name, const QString& text, bool nl = false) const;

    QString ConditionalWrapText(const QString& font_name, bool prefixSeparator, const QString& pretext, bool bold_pretext, const QString& text, bool nl = false) const;

    // special
    QString UpdateStatus(jf_FUpdateStatus status) const;
    QString GetMarkColor(size_t mindex) const;

    virtual ~jfDisplayHTMLHelper();

  protected:
    QString name;
    int relative_modifier;
    QMap<QString,jfDisplayHTMLFont*> fontCollection;
};

//****************************************************************************
namespace HTMLHelpers {
    jfDisplayHTMLHelper* startFicItemHelper(const QString& name, int in_relative_modifier);
    jfDisplayHTMLHelper* makeAO3ItemHelper(int in_relative_modifier);
    jfDisplayHTMLHelper* makeFFNItemHelper(int in_relative_modifier);
    jfDisplayHTMLHelper* makeFIMItemHelper(int in_relative_modifier);

    extern const jfDisplayHTMLHelper* ao3_item_helper;
    extern const jfDisplayHTMLHelper* ffn_item_helper;
    extern const jfDisplayHTMLHelper* fim_item_helper;
    bool Initialize();
    void Clear();
}




//****************************************************************************
