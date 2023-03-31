/////////////////////////////////////////////////////////////////////////////
// Name:        expr_match.h
// Author :     John Q Metro
// Purpose :    Declares functions that use the expression matches
// Created:     April 1, 2010
// Conversion to QT Started April 8, 2013
// Updated:     April 1, 2010
/////////////////////////////////////////////////////////////////////////////
// headers
#define JFEXPRMATCH

#ifndef JFEXPRSTRUCTS
#include "expr_structs.h"
#endif

#include "../objs/baseitem.h"

//***************************************************************************

// a matching function that uses everything: title, embedded filters, the works
bool FullItemExprMatch(const jfSearchResultItem* testelem, const jfElemArray* parsedinfo);

// a more limited matching function that works on strings only
bool StringExprMatch(const QString& teststr, const jfElemArray* parsedinfo);

//***************************************************************************

