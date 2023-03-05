/*****************************************************************************
Name    : utils1.h
Basic   : Defclares some utility functions and things used in the other files
Author  : John Q Metro
Started : March 17, 2009
Conversion to QT started : February 21, 2013
Updated : February 25, 2023
Notes   : mostly taken from the earlier jaffis_utils.h and jaffis_utils2.h

******************************************************************************/
#ifndef UTILS1_H_INCLUDED
#define UTILS1_H_INCLUDED
#endif // UTILS1_H_INCLUDED
//------------------------------------------------
#include <QString>
#include <QtGlobal>
#include <QStringList>
#include <QRegularExpression>
//*****************************************************************************
// constants
extern const QString EXPR_DELIMS;
//*****************************************************************************
// short for QString::number(input,'f',-1)
QString Doubl2Str(const double& input);
// converts a boolean to a string
QString Bool2Str(const bool& inval);
//------------------------------------------------------------------------
// I prefer a boolean as the result, so I can embed the conversion in IF blocks
bool Str2ULong(const QString& inval, ulong& outval, bool hex=false);
// a version of the above that handles commas
bool Str2ULongC(const QString& inval, ulong& outval);
// handles values like '1.9K'
bool Count2ULong(const QString& inval, unsigned long& outval);
//-------------------------------------------------------------------------
/* takes an input string. if the string is "true" or "false", returns TRUE
and also passes back which in the reference parameter */
bool BoolFromStr(const QString& in_src, bool& result);
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/* Takes a string and returns the result with the specified chars in delim
preceeded with a backslash. Backslashes are also escapsed. Do not repeat
characters in delims. */
QString EscapeString(const QString& in_base, const QString& delims);
//-------------------------------------------------------------------------------
// this function finds the first character in delims that is not preceeded by
// a backslash
int FindNonEscaped(const QString& base, const QString& delims, size_t pos = 0);
//-------------------------------------------------------------------------------
/* using the delim list, replaces any escaped delimiters with the corresponding
character. backslashes are also delimters by default. they are treated first */
size_t UnEscape(QString& base, const QString& delims);
//-----------------------------------------------------------------
/* Grabs text between pos and the next instance of dchar not preceeded by an odd
  number of backslashes */
QString GetToUnescaped(const QString& base, QChar dchar, int& pos);
//----------------------------------------------------------------------------
/* Versions of EscapeString and UnEscape that also replace newlines with ^.
Thus, ^ is also a default escaped character and should not be in delims */
QString EscapeNL(const QString& in_base, const QString& delims);
size_t UnEscapeNL(QString& base, const QString& delims);
//------------------------------------------------------------------------
// converts a QStringList to a delimted list
QString StrArrToList(QStringList *source, QChar ichar);
//------------------------------------------------------------------------
// converts a delimited list into a QStringList
QStringList *ListToStrArr(const QString& source, QChar ichar, bool trims = true);
// -----------------------------------------------------------------------
// splits the string, trims the parts, and discards empty parts
QStringList SplitTrim(const QString& source, const QString& delimiter);
QStringList SplitTrim(const QString& source, const QChar& delimiter);
QRegularExpression makeNewlineExpression();
QStringList SplitTrimNL(const QString& source);
//------------------------------------------------------------------------
// splits a string at the location, not including that location
bool SplitString(const QString& input, size_t point, QString& out1, QString& out2);
//-------------------------------------------------------------------------
// takes an input double, and outputs it as a percentage
QString ToPercent(const double& insrc, const size_t& prec);
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
QString toLowercaseTurkishAzeri(QString input);
//-------------------------------------------------------
void SortStringList(QStringList* tosort, bool casesen);
//*****************************************************************************
