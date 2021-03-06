/////////////////////////////////////////////////////////////////////////////
// Name:        jaffis_filterbase.h
// Author :     John Q Metro
// Purpose :    Declares the most basic filtering classes
// Created:     20.02.06
// Conversion to QT Started April 8, 2013
// Updated:     August 21, 2012
/////////////////////////////////////////////////////////////////////////////
// ----------------------------------------------------------------------------
#ifndef BASEOBJ_H_INCLUDED
    #include "../../objs/baseobj.h"
#endif // BASEOBJ_H_INCLUDED
#define JFBASEFILTER
//****************************************************************************
class jfBasePD;
//*****************************************************************************
class jfBaseFilter : public jfBaseObj {
  public:
    jfBaseFilter();
    // testing the type id
    bool MatchTypeid(const QString& tomatch) const;
    // match against the filter
    virtual bool isEmpty() const = 0;
    bool Match(const jfBasePD* testelem) const;
    // loading from a string representation
    virtual bool FromString(const QString& sourcedata) = 0;
    virtual QString ToString() const = 0;
    // gets a description
    virtual QString GetTypeDescription() const = 0;
    // copy
    virtual jfBaseFilter* GenCopy() const = 0;
    // returns a general filter type
    virtual size_t GetFilType() const;
  protected:
    // the core matching method
    virtual bool CoreMatch(const jfBasePD* testelem) const = 0;
    // subitem copying method, so we do not have to replicate for each child class
    void CopyOver(jfBaseFilter* core) const;
    /* since different types are stored together, the text file reprentation
    may have objects of varying length */
    virtual size_t ExtraLines() const = 0;
    // the only thing in line 3 is the ExtraLines info
    virtual bool AddMiddleToFile(QTextStream* outfile) const;
    virtual bool ReadMiddleFromFile(jfFileReader* infile);
};
//============================================================================
/* filters objects of jfBasePD (and descendants) by matching/excluding using
the url */
class jfUrlFilter : public jfBaseFilter {
  public:
    // public booleans
    bool cimatch;
    bool https_incl;
    // the new constructor
    jfUrlFilter();
    jfUrlFilter(const jfUrlFilter& source);
    // info methods
    virtual bool isEmpty() const;
    // redefined virtual functions
    virtual bool FromString(const QString& sourcedata);
    virtual QString ToString() const;
    // manipulating internal data
    size_t SetAlternates(QStringList* mmatch, bool replace = true);
    size_t AppendToAlternates(QString url2add);
    QStringList* GetAlternates() const;
    size_t SetExcludes(QStringList* excl, bool replace = true);
    size_t AppendToExcludes(QString url2add);
    QStringList* GetExcludes() const;
    QStringList* GetExcludes2();
    // basic constants
    virtual QString GetTypeID() const;
    virtual QString GetTypeDescription() const;
    // copying
    jfUrlFilter* Copy() const;
    jfBaseFilter* GenCopy() const;
  protected:
    // private methods
    virtual bool CoreMatch(const jfBasePD* testelem) const;
    virtual void SetValid();
    virtual size_t ExtraLines() const;
    virtual bool AddRestToFile(QTextStream* outfile) const;
    virtual bool ReadRestFromFile(jfFileReader* infile);
    // private data
    QStringList *alternates;
    bool excludeonly;
    QStringList *excludes;
};
///////////////////////////////////////////////////////////////////////////////
