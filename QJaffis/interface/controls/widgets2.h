/******************************************************************************
Name    :   widgets2.h
Author  :   John Q Metro
Purpose :   Some more miscuser interface stuff
Created :   November 24, 2009
Conversion to Qt started : September 15, 2013
Updated :   July 31, 2016 (deleting tag widget)
******************************************************************************/
#ifndef WIDGETS2_H_INCLUDED
  #define WIDGETS2_H_INCLUDED
#endif // WIDGETS2_H_INCLUDED
#ifndef JFMISCTYPES1
#include "../../core/objs/misc_types1.h"
#endif
//--------------------------------------------------------------------------
#include <QWidget>
#include <QLabel>
#include <QLineEdit>
#include <QPlainTextEdit>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QListWidget>
/*****************************************************************************/
// a panel containing two vertically stacked extCtrl and labels
class jfTwoTexts : public QWidget {
  public:
    // the constructor
    jfTwoTexts(QString label1, QString label2, size_t prp, bool multiline, QWidget* parent = NULL);
    // setting strings
    void Set1(QString inval);
    void Set2(QString inval);
    void SetBoth(QString inval1, QString inval2);
    // getting strings
    QString Get1() const;
    QString Get2() const;
    void GetBoth(QString& out1, QString& out2) const;
  protected:
    // gui elements
    QLabel* slabel1,*slabel2;
    QLineEdit *lebox1, *lebox2;
    QPlainTextEdit *tebox1, *tebox2;
    // sizers
    QVBoxLayout*   topsizer;
    size_t        prop;
    bool ismulti;
};
//===========================================================================
// a line editor with label that is meant for nonempty names
class jfNameEdit : public QWidget {
    Q_OBJECT
  public:
    jfNameEdit(const QString& ltext, const QString& starttext, int margin = -1, QWidget* parent = NULL);
    // setting names
    bool ChangeText(const QString& starttext);
    bool ChangeTextNS(const QString& starttext);
    // getting names
    bool IsTextEmpty() const;
    QString TryGetName() const;
    QString GetName() const;
    // an override (because Qt can be stupid)
    virtual QSize sizeHint() const;
  signals:
    void SendNewName(QString in_nname);
  public slots:
    void HandleNameChange(const QString& newname);
  protected:
    // an internal helper method
    void ShowEmptyMessage() const;
    mutable QString backup_data;
    // GUI widgets
    QLabel* name_label;
    QLineEdit* name_edit;
    QHBoxLayout* layout;
};


/*****************************************************************************/
