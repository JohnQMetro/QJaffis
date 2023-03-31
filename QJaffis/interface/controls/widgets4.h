/******************************************************************************
Name    :   widgets4.h
Author  :   John Q Metro
Purpose :   Some misc user interface stuff
Created :   February 12, 2018
Updated :   February 12, 2018
******************************************************************************/
#ifndef WIDGETS4_H
    #define WIDGETS4_H
#endif // WIDGETS4_H
//------------------------------
#include <QPlainTextEdit>
#include <QWidget>
#include <QColor>
#include <QLineEdit>
#include <QPushButton>
#include <QHBoxLayout>
/*****************************************************************************/
// annoyingly, QPlainTextEdit is still too unplain for some things I want
// so a custom class to provide simple extra functions for expression editors
//----------------------------------------------------------------
class jfExprTextEdit : public virtual QPlainTextEdit {
    Q_OBJECT
  public:
    jfExprTextEdit(QWidget* parent = 0);
    jfExprTextEdit(const QString& text, QWidget* parent = 0);
    // color get and Set
    QColor currentColor() const;
    void changeColor(const QColor& newcolor);
    void setColorToDefault();
    // also
    void setModified(bool isModified);

  protected:
    QColor current_colour;
    QColor default_colour;
};
//===================================================================
/* Expression editor widgets come in two types. They have much in common,
 * so I decided to get around to making a common base class. */
class jfExprEditBase : public QWidget {
    Q_OBJECT
  public:
    jfExprEditBase(bool mline_in, QWidget* parent = NULL);
    // checking method
    QString getText() const;
    virtual bool CheckExpr(const QString& inexpr,bool& outempty,QString& outmessage) const = 0;
    // setting data
    bool SetData(const QString& inexpr,QString& outmessage);
  public slots:
    void PressCheckFilter(bool value);
    void TextChanged();
    void TextChanged2(QString value);
  protected:
    void setText(const QString intext);
    // internal helper
    void SetStatus(size_t instatus);
    // the internal controls
    QLineEdit* editcore1;
    jfExprTextEdit* editcore2;
    QPushButton* fcheck_btn;
    // sizers (horizontal, buttons on the right)
    QHBoxLayout* topsizer;
    // error info
    size_t error_code;
    bool mline;
};

/*****************************************************************************/
