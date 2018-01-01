/******************************************************************************
Name    :   widgets3.h
Author  :   John Q Metro
Purpose :   Some misc user interface stuff
Created :   December 5, 2010
Conversion to Qt Started May 22, 2013
Updated :   August 3, 2016
******************************************************************************/
#ifndef WIDGETS3_H_INCLUDED
#define WIDGETS3_H_INCLUDED
#endif // WIDGETS3_H_INCLUDED
//-------------------------------
#ifndef JFMISCTYPES1
  #include "../../core/objs/misc_types1.h"
#endif
//---------------------------------------------
#include <QWidget>
#include <QLayout>
#include <QLineEdit>
#include <QLabel>
#include <QSpinBox>
#include <QPushButton>
#include <QDir>
#include <QComboBox>
#include <QGroupBox>
//****************************************************************************
// jfIntEdit can be replaced with QSpinBox
//--------------------------------------------
// just to reduce layout, a bundled line editor and label
class jfLabeledEdit : public QWidget {
  public:
    // constructor
    jfLabeledEdit(QWidget* parent, QString inlabel, bool horiz, QString intext = "");
    // getting and setting
    void SetLabel(QString inlabel);
    QString GetLabel() const;
    void SetText(QString intext);
    void SetFiletag(QString inval);
    QString GetText() const;
  protected:
    QString clabel;
    QLabel* thelabel;
    QLineEdit* thetext;
    QBoxLayout* layout;
};
//--------------------------------------------------------------
// just to reduce layout, a bundled integer entry and label
class jfLabeledIntEdit : public QWidget {
  public:
    // constructor
    jfLabeledIntEdit(QWidget* parent, QString inlabel, bool horiz, int min, int max);
    // getting and setting
    void SetLabel(QString inlabel);
    QString GetLabel() const;
    bool SetMinMax(int min, int max);
    bool SetValue(int invalue);
    int GetValue() const;
  protected:
    QString clabel;
    QLabel* thelabel;
    QSpinBox* thenumber;
    QBoxLayout* layout;
};

//--------------------------------------------------------------------------
class jfDirPicker : public QWidget {
    Q_OBJECT
  public:
    // constructor
    jfDirPicker(QWidget* parent,bool vertical, bool left, QString indirlabel, QString indir = QDir::homePath());
    // i/o
    QString GetPath() const;
    bool SetPath(QString inval);
    void SetPathUnchecked(const QString& inval);
    void SetLEnabled(bool enable);
  signals:
    void SendPathChanged();
  public slots:
    void HandleClick(bool checked);
  protected:
    // helper methods
    void ArrangeWidgets(bool vertical, bool left);
    // gui elemets
    QPushButton* launch_dialog;
    QLabel* dlabel;
    QLabel* dir_display;
    // sizers
    QBoxLayout* mlay, *slay;
    // internal data
    QString thedir, dirlabel;
};
//---------------------------------------------------------------------------
// sometimes, I need to initialize a QComboBox from an array of strings, not a QStringList
bool LoadCBoxFromArray(QComboBox* destination, const QString source[], size_t lcount);
//-----------------------------------------------------------------
/* There are a number of filters/pickers that requre numeric min/max ranges. I've decided to
 * make a custom widget to avoid reduplicating work.
Range : if SetRange is used, upper bound > lower bound, and max >= min
        else, for SetZero Max, the range is 0 to upper bound, upper bound > 0, and max >= min
        unless max is zero (zero indicates unbounded).
*/
class jfMinMaxEditor : public QWidget {
  public:
    // constructor
    jfMinMaxEditor(QString inname, bool wrapped, bool horizontal, bool shortnames = true, QWidget* parent = NULL );
    // checking
    bool Check() const;
    bool CheckMessage() const;
    // setting things    
    bool SetMin(int inval);
    bool SetMax(int inval);
    virtual bool SetMinMax(int in_min, int in_max);
    virtual bool SetMinMax(IntPair* invalue);
    void ResetValues();
    // getting things
    int GetMin() const;
    int GetMax() const;
    IntPair* GetMixMax() const;
    bool LoadIntoPair(IntPair* destination) const;
  protected:
    bool SetRange(int in_lbound, int in_ubound);
    // helper methods
    void SetupLayout(bool wrapped, bool horizontal);

    // widgets
    QLabel * min_label;
    QLabel * max_label;
    QSpinBox* min_entry;
    QSpinBox* max_entry;
    // layouts
    QGroupBox* wrapper;
    QHBoxLayout* hlayout;
    QHBoxLayout* wrap_holder;
    QGridLayout* vlayout;
    // stored info
    QString stored_name;
    int lbound, ubound;
    bool zeromaxokay;

};
//--------------------------------------------------------------------
// a specialization of jfMinMaxEditor
class jfZeroToMaxEditor : public jfMinMaxEditor {
  public:
    jfZeroToMaxEditor(QString inname, bool wrapped, bool horizontal, bool shortnames = true, QWidget* parent = NULL );
    bool SetMaxVal(size_t inmaxval);
  protected:
};
//--------------------------------------------------------------------
// jfMixMaxEditor without set range hiding
class jfGenMinMaxEditor : public jfMinMaxEditor {
  public:
    jfGenMinMaxEditor(QString inname, bool wrapped, bool horizontal, bool shortnames = true, QWidget* parent = NULL );
    bool SetValueRange(int in_lbound, int in_ubound);
};



//****************************************************************************
