/******************************************************************************
Name    :   displayhtmlspec.cpp
Author  :   John Q Metro
Purpose :   Data objects and helpers for ToDisplayHTML()
Created :   August 7, 2022
Updated :   March 11, 2023

******************************************************************************/
#ifndef DISPLAYHTMLSPEC_H
    #include "displayhtmlspec.h"
#endif // DISPLAYHTMLSPEC_
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#ifndef HTMLPARSE_H_INCLUDED
  #include "../../core/utils/htmlparse.h"
#endif // HTMLPARSE_H_INCLUDED

#include <QMap>
//****************************************************************************
// font info

jfDisplayHTMLFont::jfDisplayHTMLFont() {
    color = "";
    relative_size = 0;
    bold = false;
    italic = false;
    underline = false;
}
// ---
jfDisplayHTMLFont::jfDisplayHTMLFont(const QString& incolor, int in_relative_size) {
    color = incolor;
    relative_size = in_relative_size;
    bold = false;
    italic = false;
    underline = false;
}
// ---
QString jfDisplayHTMLFont::makeStart(int relative_modifier) const {
    QString res;
    if (hasFont(relative_modifier)) {
        res = "<font ";
        if (color.size() > 0) res += "color=\"" + color + "\" ";
        int effective_size = relative_size - relative_modifier;
        if (effective_size != 0) {
            res += "size=";
            if (effective_size > 0) res += '+';
            res += QString::number(effective_size);
        }
        res += ">";
    }
    if (bold) res += "<b>";
    if (italic) res += "<i>";
    if (underline) res += "<u>";
    return res;
}
// ---
QString jfDisplayHTMLFont::makeEnd(int relative_modifier) const {
    QString res;
    if (underline) res += "</u>";
    if (italic) res += "</i>";
    if (bold) res += "</b>";
    if (hasFont(relative_modifier)) res += "</font>";
    return res;
}
// ---
QString jfDisplayHTMLFont::wrapText(const QString& text, int relative_modifier) const {
    if (isEmpty(relative_modifier)) {
        return "<span>" + htmlparse::Text2BasicHTML(text) + "</span>";
    }
    else {
        return makeStart(relative_modifier) + htmlparse::Text2BasicHTML(text) + makeEnd(relative_modifier);
    }
}
// ---
QString jfDisplayHTMLFont::wrapHyperlink(const QString& url, const QString& text, int relative_modifier) const {
    bool is_empty = isEmpty(relative_modifier);
    QString res;
    if (!is_empty) res = makeStart(relative_modifier);
    res += "<a href=\"" + url + "\">" + htmlparse::Text2BasicHTML(text) + "</a>";
    if (!is_empty) res += makeEnd(relative_modifier);
    return res;
}
// ---
QString jfDisplayHTMLFont::wrapTextNoEsc(const QString& text, int relative_modifier) const {
    if (isEmpty(relative_modifier)) return "<span>" + text + "</span>";
    else return makeStart(relative_modifier) + text + makeEnd(relative_modifier);
}
// ---
bool jfDisplayHTMLFont::hasFont(int relative_modifier) const {
    return (color.size() > 0) || ((relative_size - relative_modifier) != 0);
}
// ---
bool jfDisplayHTMLFont::hasEffects() const {
    return bold || italic || underline;
}
// ---
bool jfDisplayHTMLFont::isEmpty(int relative_modifier) const {
    return !(hasFont(relative_modifier) || hasEffects());
}


//****************************************************************************
// includes a font collection and a few utilities

jfDisplayHTMLHelper::jfDisplayHTMLHelper(QString in_name, int in_relative_modifier) {
    name = in_name;
    relative_modifier = in_relative_modifier;
}
//++++++++++++++++++++++++++++++++++++++
// basic info
QString jfDisplayHTMLHelper::GetName() const {
    return name;
}
int jfDisplayHTMLHelper::GetRelativeBaseFontSize() const {
    return relative_modifier;
}
//++++++++++++++++++++++++++++++++++++++
// font change
bool jfDisplayHTMLHelper::SetFont(const QString& font_name, jfDisplayHTMLFont* font) {
    if ((font == NULL) || font_name.isEmpty()) return false;
    else {
        jfDisplayHTMLFont* old = NULL;
        if (fontCollection.contains(font_name)) {
            old = fontCollection.take(font_name);
        }
        fontCollection.insert(font_name, font);
        if (old != NULL) delete old;
        return true;
    }
}
// ------------------
bool jfDisplayHTMLHelper::MakeAndSetFont(const QString& font_name,const QString& incolor, int in_relative_size) {
    if (font_name.isEmpty()) return false;
    jfDisplayHTMLFont* new_font = new jfDisplayHTMLFont(incolor, in_relative_size);
    return SetFont(font_name, new_font);
}
// --------------------
bool jfDisplayHTMLHelper::DeleteFont(const QString& font_name) {
    jfDisplayHTMLFont* old = NULL;
    if (fontCollection.contains(font_name)) {
        old = fontCollection.take(font_name);
        delete old;
        return true;
    }
    else return false;
}
//++++++++++++++++++++++++++++++++++++++
// getting font
jfDisplayHTMLFont* jfDisplayHTMLHelper::GetFont(const QString& font_name){
    return fontCollection[font_name]; // returns NULL if the font does not exist
}
// -------------------------------------
const jfDisplayHTMLFont* jfDisplayHTMLHelper::GetReadonlyFont(const QString& font_name) const {
    return fontCollection.value(font_name, NULL);
}
//++++++++++++++++++++++++++++++++++++++
// applyers and helpers
QString jfDisplayHTMLHelper::WrapText(const QString& font_name, const QString& text, bool nl) const {
    const jfDisplayHTMLFont* font = GetReadonlyFont(font_name);
    if (font == NULL) return "";
    QString res = font->wrapText(text, relative_modifier);
    if (nl) res += "<br>\n";
    return res;
}
// ------------------
QString jfDisplayHTMLHelper::WrapHyperlink(const QString& url, const QString& font_name, const QString& text, bool nl) const {
    const jfDisplayHTMLFont* font = GetReadonlyFont(font_name);
    if (font == NULL) return "";
    QString res = font->wrapHyperlink(url, text, relative_modifier);
    if (nl) res += "<br>\n";
    return res;
}
// ------------------
QString jfDisplayHTMLHelper::WrapTextNoEsc(const QString& font_name, const QString& text, bool nl) const {
    const jfDisplayHTMLFont* font = GetReadonlyFont(font_name);
    if (font == NULL) return "";
    QString res = font->wrapTextNoEsc(text, relative_modifier);
    if (nl) res += "<br>\n";
    return res;
}
// ------------------
QString jfDisplayHTMLHelper::ConditionalWrapText(const QString& font_name, bool prefixSeparator, const QString& pretext, bool bold_pretext, const QString& text, bool nl) const {
    if (text.isEmpty()) return "";
    QString wrapped;
    if (bold_pretext) {
        QString prefix = "<b>" + htmlparse::Text2BasicHTML(pretext) + "</b>";
        QString main_text = htmlparse::Text2BasicHTML(text);
        wrapped = WrapTextNoEsc(font_name, prefix + main_text, nl);
    }
    else {
        wrapped = WrapText(font_name, pretext + text, nl);
    }
    if (wrapped.isEmpty()) return "";
    QString prefix = (prefixSeparator) ? " &nbsp;&mdash;&nbsp; " : "";
    return prefix + wrapped;
}
// +++++++++++++++++++++++++++++++++++++
// special
QString jfDisplayHTMLHelper::UpdateStatus(jfUpdateStatus status) const {
    if (status == jfUpdateStatus::NONE) return "";
    int current_size = 2 - relative_modifier;
    QString result = "<font";
    if (current_size != 0) {
        result += " size=";
        if (current_size > 0) result += '+';
        result += QString::number(current_size);
    }
    result += " color=";
    if (status==jfUpdateStatus::UPDATED) result += "Lime><b>[Updated";
    else if (status==jfUpdateStatus::MISSING) result += "Red><b>[Missing";
    else if (status==jfUpdateStatus::NEW) result += "Aqua><b>[New";
    else if (status==jfUpdateStatus::AUTHORNC) result += "Yellow><b>[Author Name Change";
    result += "!]</b></font>";
    return result;
}

//++++++++++++++++++++++++++++++++++++++
jfDisplayHTMLHelper::~jfDisplayHTMLHelper() {
    if (fontCollection.size() > 0) {
        jfDisplayHTMLFont* font = NULL;
        QMap<QString,jfDisplayHTMLFont*>::iterator font_iterator;
        for (font_iterator = fontCollection.begin(); font_iterator != fontCollection.end(); ) {
            font = font_iterator.value();
            font_iterator = fontCollection.erase(font_iterator);
            delete font;
        }
        fontCollection.clear();
    }
}

// ****************************************************************************************
jfDisplayHTMLHelper* HTMLHelpers::startFicItemHelper(const QString& name, int in_relative_modifier) {
    jfDisplayHTMLHelper* html_helper = new jfDisplayHTMLHelper(name, in_relative_modifier);
    html_helper->MakeAndSetFont("fictitle", "#010101", 2);
    return html_helper;
}
// ++++++++++++++++++++++++++++++++++++++++++++++++++++
jfDisplayHTMLHelper* HTMLHelpers::makeAO3ItemHelper(int in_relative_modifier) {
    jfDisplayHTMLHelper* ao3_helper = startFicItemHelper("AO3", in_relative_modifier);
    ao3_helper->MakeAndSetFont("fandoms", "green", 0);
    ao3_helper->MakeAndSetFont("ficlink", "green", 0);
    ao3_helper->MakeAndSetFont("description", "#010101", 1);
    ao3_helper->MakeAndSetFont("series", "darkviolet",0);
    ao3_helper->MakeAndSetFont("basicinfo", "gray", 1);
    ao3_helper->MakeAndSetFont("warnings", "red", 0);
    ao3_helper->MakeAndSetFont("characters", "darkblue", 1);
    ao3_helper->MakeAndSetFont("pairings", "olive", 0);
    ao3_helper->MakeAndSetFont("tags", "teal", 0);

    return ao3_helper;
}

jfDisplayHTMLHelper* HTMLHelpers::makeFFNItemHelper(int in_relative_modifier) {
    jfDisplayHTMLHelper* ffn_helper = startFicItemHelper("FFN", in_relative_modifier);
    ffn_helper->MakeAndSetFont("ficlink", "green", 0);
    ffn_helper->MakeAndSetFont("description", "#010101", 1);
    ffn_helper->MakeAndSetFont("basicinfo", "gray", 1);
    ffn_helper->MakeAndSetFont("characters", "darkblue", 1);
    ffn_helper->MakeAndSetFont("pairings", "olive", 0);
    ffn_helper->MakeAndSetFont("genres", "teal", 1);

    return ffn_helper;
}

jfDisplayHTMLHelper* HTMLHelpers::makeFIMItemHelper(int in_relative_modifier) {
    jfDisplayHTMLHelper* fim_helper = startFicItemHelper("FIM", in_relative_modifier);
    fim_helper->MakeAndSetFont("ficlink", "green", 0);
    fim_helper->MakeAndSetFont("description", "#010101", 1);
    fim_helper->MakeAndSetFont("basicinfo", "gray", 1);
    fim_helper->MakeAndSetFont("genres", "teal", 1);
    fim_helper->MakeAndSetFont("warnings", "red", 1);
    fim_helper->MakeAndSetFont("characters", "darkblue", 1);

    return fim_helper;
}
// ++++++++++++++++++++++++++++++++++++++++++++++++++++
const jfDisplayHTMLHelper* HTMLHelpers::ao3_item_helper = NULL;
const jfDisplayHTMLHelper* HTMLHelpers::ffn_item_helper = NULL;
const jfDisplayHTMLHelper* HTMLHelpers::fim_item_helper = NULL;

bool HTMLHelpers::Initialize() {
    int relative_modifier = 0;
    #ifdef Q_OS_WIN
    relative_modifier = 0;
    #endif
    ao3_item_helper = makeAO3ItemHelper(relative_modifier);
    ffn_item_helper = makeFFNItemHelper(relative_modifier);
    fim_item_helper = makeFIMItemHelper(relative_modifier);
    return true;

}
void HTMLHelpers::Clear() {
    if (ao3_item_helper != NULL) {
        delete ao3_item_helper;
        ao3_item_helper = NULL;
    }
    if (ffn_item_helper != NULL) {
        delete ffn_item_helper;
        ffn_item_helper = NULL;
    }
    if (fim_item_helper != NULL) {
        delete fim_item_helper;
        fim_item_helper = NULL;
    }
}

