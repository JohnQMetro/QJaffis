/*****************************************************************************
Name:        typed_display.h
Author :     John Q Metro
Purpose :    Typed HTML Listbox results
Created:     April 27, 2011
Started Conversion to Qt November 19, 2013
Updated:     October 6, 2015 (added HandleResultsOnePanel and HandleResultsMultiPanel)
******************************************************************************/
#ifndef TYPED_DISPLAY_H_INCLUDED
#define TYPED_DISPLAY_H_INCLUDED
#endif // TYPED_DISPLAY_H_INCLUDED
#ifndef HTML_DISPLAY_H_INCLUDED
  #include "html_display.h"
#endif // HTML_DISPLAY_H_INCLUDED
//-------------------------------------------
/*****************************************************************************/
class jfHtmlResultCollectionBasic : public jfHtmlResultCollection {
    Q_OBJECT
  public:
    // constructor
    jfHtmlResultCollectionBasic(header_enum inhtype, QWidget* parent = NULL);
  public slots:
    virtual void HandleNewResults(jfResultUnitVector* inresults);
    virtual void HandleIncreaseCount(size_t nitem_count);
    virtual void HandleNewCategory(struct jfCategoryInfoMsg incat);
  protected:
    void HandleResultsOnePanel(jfResultUnitVector* inresults);
    void HandleResultsMultiPanel(jfResultUnitVector* inresults);

};
/*****************************************************************************/
