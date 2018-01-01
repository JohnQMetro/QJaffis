/******************************************************************************
Name    :   charo_editor.h
Author  :   John Q Metro
Purpose :   Editor with a collection of checkboxes that have single-char i/o
Created :   September 8, 2012
Conversion to Qt Started October 18, 2013
Updated :   September 11, 2012
******************************************************************************/
#ifndef CHARO_EDITOR_H_INCLUDED
#define CHARO_EDITOR_H_INCLUDED
#endif // CHARO_EDITOR_H_INCLUDED
//---------------------------------------------
#include <vector>
#include <QWidget>
#include <QCheckBox>
#include <QBoxLayout>
#include <QGridLayout>
#include <QGroupBox>

/*****************************************************************************/
class jfCharCheckBoxGroup : public QWidget {
  public:
    // constructor
    jfCharCheckBoxGroup(QString inname, size_t icount, const QString inlabels[], const QString inchars,
                        size_t incols, QWidget* parent = NULL);
    // i/o
    bool SetFromString(const QString& inval);
    QString GetStringValue() const;
    // special
    void CheckAll(bool doit = true);
  protected:
    void SetupSizers(size_t tcount, size_t colcount);
    void PopulateBoxes(size_t tcount, size_t colcount);
    // GUI data
    std::vector<QCheckBox*> cboxes;
    size_t cbcount;
    QBoxLayout *tsizer, *wsizer;
    QGridLayout* fsizer;
    QGroupBox* mbox;
    bool usegrid;
    // non gui data
    QString charvals;
    QString mainlabel;
    size_t mode;
};
/*****************************************************************************/
