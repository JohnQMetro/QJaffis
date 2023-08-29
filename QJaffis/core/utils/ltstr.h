/******************************************************************************
Name    :   ltstr.h
Author  :   John Q Metro
Purpose :   This string comparator is used in several places
Created :   April 9, 2023
Updated :   April 9, 2023
******************************************************************************/
#pragma once

#ifndef LTSTR_H
#define LTSTR_H
#endif // LTSTR_H

#include <QString>
/*****************************************************************************/

struct ltstr {
    bool operator()(const QString& s1, const QString& s2) const {
        int ri = s1.compare(s2);
        return (ri<0);
    }
};
/*****************************************************************************/

