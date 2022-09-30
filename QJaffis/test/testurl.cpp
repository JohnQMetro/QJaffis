/*****************************************************************************
Name    : testurl.cpp
Basic   : URLs and any misc stuff for the test routines
Author  : John Q Metro
Started : March 18, 2018
Updated : April 17, 2022
******************************************************************************/
#ifndef TESTURL_H
    #include "testurl.h"
#endif // TESTURL_H
//-----------------------------
#ifndef FFNCATPARSER1_H
  #include "../fanficnet/threads/ffncatparser1.h"
#endif // FFNCATPARSER1_H
#ifndef FFNCATPARSER2_H
  #include "../fanficnet/threads/ffncatparser2.h"
#endif // FFNCATPARSER2_H
#ifndef FFNITEM_PARSER_H
  #include "../fanficnet/threads/ffnitem_parser.h"
#endif // FFNITEM_PARSER_H
#ifndef FFNPARSER_H
  #include "../ficdown/parsers/ffnparser.h"
#endif // FFNPARSER_H
#ifndef AO3CAT_PARSER_H
  #include "../fanfic/threads/ao3/ao3cat_parser.h"
#endif // AO3CAT_PARSER_H
#ifndef AO3ITEM_PARSER_H
  #include "../fanfic/threads/ao3/ao3item_parser.h"
#endif // AO3ITEM_PARSER_H
#ifndef AO3PARSER_H
  #include"../ficdown/parsers/ao3parser.h"
#endif // AO3PARSER_H
#ifndef FIMTAG_PARSE_H
    #include "../fanfic/threads/fim/fimtag_parse.h"
#endif // FIMTAG_PARSE_H
#ifndef FIMPARSE_COMPACT_H
  #include "../fanfic/threads/fim/fimparse_compact.h"
#endif // FIMPARSE_COMPACT_H
#ifndef FIMPARSE_ITEMS_H
  #include "../fanfic/threads/fim/fimparse_items.h"
#endif // FIMPARSE_ITEMS_H
#ifndef BASEFIM_H
  #include "../ficdown/parsers/basefimparser.h"
#endif // BASEFIM_H
#ifndef NEWFIMGROUP_PARSER_H
  #include "../fanfic/threads/fim/newfimgroup_parser.h"
#endif // NEWFIMGROUP_PARSER_H
#ifndef FIMGROUP_PARSER_H
  #include "../fanfic/threads/fim/fimgroup_parser.h"
#endif // FIMGROUP_PARSER_H
#ifndef MMOPARSER_H
  #include "../ficdown/parsers/mmoparser.h"
#endif // MMOPARSER_H
#ifndef CATGROUP_H
  #include "../fanficnet/categories/catgroup.h"
#endif // CATGROUP_H
#ifndef AO3_CATS_H_INCLUDED
  #include "../fanfic/data/ao3/ao3_cats.h"
#endif // AO3_CATS_H_INCLUDED
//-------------------------------------------------
#ifndef LOGGING_H_INCLUDED
  #include "../core/utils/logging.h"
#endif // LOGGING_H_INCLUDED
/*****************************************************************************/
const size_t test::count = 23;
const QString test::urls[test::count] = {
    /** Fanfiction.Net categories **/
    "https://www.fanfiction.net/cartoon/",
    "https://www.fanfiction.net/crossovers/movie/",
    "https://www.fanfiction.net/crossovers/Tangled/7755/",
    /** Fanfiction.Net listings **/
    "https://www.fanfiction.net/comic/X-Men/?&srt=2&r=10",
    "https://www.fanfiction.net/comic/X-Men/?&srt=2&r=10&p=4",
    "https://www.fanfiction.net/comic/Astounding-Wolf-Man/?&srt=2&r=10",
    "https://www.fanfiction.net/Sonic-the-Hedgehog-and-My-Little-Pony-Crossovers/253/621/?&srt=2&r=10",
    /** Fanfiction.Net fic pages **/
    "https://www.fanfiction.net/s/13380174/1/The-Devil-Wears-a-Suit-and-Tie",
    "https://www.fanfiction.net/s/13380174/2/The-Devil-Wears-a-Suit-and-Tie",
    /** Archive of Our Own stuff **/
    "https://archiveofourown.org/media/TV%20Shows/fandoms",
    "https://archiveofourown.org/works?utf8=%E2%9C%93&commit=Sort+and+Filter&work_search%5Bsort_column%5D=revised_at&work_search%5Bcategory_ids%5D%5B%5D=21&work_search%5Bother_tag_names%5D=&work_search%5Bquery%5D=&work_search%5Blanguage_id%5D=&work_search%5Bcomplete%5D=0&tag_id=The+A-Team+-+All+Media+Types",
    "https://archiveofourown.org/works/14055480?view_adult=true",
    "https://archiveofourown.org/works/8710672/chapters/19972105",
    "https://archiveofourown.org/works/8710672/chapters/19972252",
    /** Fimfiction.Net **/
    "https://www.fimfiction.net/stories?q=",
    "https://www.fimfiction.net/stories?order=latest&view_mode=2&q=%23adventure",
    "https://www.fimfiction.net/stories?order=latest&view_mode=0&q=%23adventure",
    "https://www.fimfiction.net/story/356136/digimon-a-new-legacy-of-digidestined",
    "https://www.fimfiction.net/story/356136/1/",
    "https://www.fimfiction.net/groups?q=twilight&order=date_created",
    "https://www.fimfiction.net/group/212215/twilight-sparkles-library",
    /** Others **/
    "https://www.mediaminer.org/fanfic/s/tenchi-muyo-fan-fiction/the-second-chance/159822",
    "https://www.mediaminer.org/fanfic/c/tenchi-muyo-fan-fiction/the-second-chance/159822/560497"

};
const QString test::labels[test::count] = {
    "Fanfiction.Net Section","Fanfiction.Net Crossover Section","Fanfiction.Net Crossover Index",
    "Fanfiction.Net Listing (Page 1)", "Fanfiction.Net Listing (Page 4)", "Fanfiction.Net Listing (Single Page)",
    "Fanfiction.Net Listing (Crossover)", "Fanfiction.Net Fic (Page 1)", "Fanfiction.Net Fic (Page 2)",
    "ArchiveOfOurOwn.org Fandom List","ArchiveOfOurOwn.org Listing (Gen)",
    "ArchiveOfOurOwn.org Fic (Single Page)", "ArchiveOfOurOwn.org Fic (First Page)","ArchiveOfOurOwn.org (Second Page)",
    "Fimfiction.Net Tag Source","Fimfiction.Net Compact Listing","Fimfiction.Net Full Listing",
    "Fimfiction.Net Fic Index","Fimfiction.Net Fic Part",
    "Fimfiction.Net Group Listing","Fimfiction.Net Group",
    "MediaMiner.org Fic Index","MediaMiner.org Fic Part"
};
// some of these indexes are not accurate, but they don't always have to be. */
const size_t test::pageindexes[test::count] = {
    3,7,1,  1,4,1,   1,1,2,    9,1,   1,1,2,   1,1,1,
    1,2,  1,1,  1,2
};
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
ParserList* test::makeParsers() {
    const QString fname = "test::makeParsers()";
    /**/JDEBUGLOG(fname,1)
    const jfFFN_Categories* catsource;
    jfFFNItemParser* cffn_parser;
    const jfFFN_CategoryCore* usecat;

    catsource = ffn_catorganizer::main_manager->GetHolder();

    ParserList* result = new ParserList();
    /**/JDEBUGLOG(fname,2)
    /*  0 */ result->push_back(new jfFFNSectionParser());
    /*  1 */ result->push_back(new jfFFNCrossoverSectionParser());
    /*  2 */ result->push_back(new jfFFNCrossoverParser());
    /**/JDEBUGLOG(fname,3)
    cffn_parser = new jfFFNItemParser();
    usecat = catsource->FindCatFromFinder("Comics;X-Men");
    cffn_parser->SetCategory(usecat);
    /*  3 */ result->push_back(cffn_parser);
    /*  4 */ result->push_back(cffn_parser);
    cffn_parser = new jfFFNItemParser();
    usecat = catsource->FindCatFromFinder("Comics;Astounding Wolf-Man");
    cffn_parser->SetCategory(usecat);
    /*  5 */ result->push_back(cffn_parser);
    /**/JDEBUGLOG(fname,4)
    cffn_parser = new jfFFNItemParser();
    usecat = catsource->FindCatFromFinder("Cartoons;My Little Pony;Games;Sonic the Hedgehog");
    cffn_parser->SetCategory(usecat);
    /*  6 */ result->push_back(cffn_parser);
    /**/JDEBUGLOG(fname,5)
    /*  7 */ result->push_back(new jfFFN_FicPartParser());
    /*  8 */ result->push_back(result->back());

    assert(ao3_catdata::ao3_catmanager != NULL);
    jfGen_CatHolder* aocat = ao3_catdata::ao3_catmanager->GetData();


    /*  9 */ result->push_back(new jfAO3CatParser());

    const jfGeneralCategory* dc = aocat->FindCatByName("Movies","The A-Team - All Media Types");
    assert(dc != NULL);
    const jfAO3_Category* usecatao3 = dynamic_cast<const jfAO3_Category*>(dc);
    jfAO3ItemParser* ao3_parser = new jfAO3ItemParser();
    ao3_parser->SetCategory(usecatao3);

    /* 10 */ result->push_back(ao3_parser);

    /* 11 */ result->push_back(new jfAO3_FicPartParser());
    /* 12 */ result->push_back(result->back());
    /* 13 */ result->push_back(result->back());
    /* 14 */  result->push_back(new jfFIMTag_Extractor());
    /* 15 */ result->push_back(new jfFIMCompactParser());
    /* 16 */ result->push_back(new jfFIMPageParser());
    /* 17 */ result->push_back(new jfFIM_FicPartParser());
    /* 18 */ result->push_back(result->back());
    /* 19 */ result->push_back(new jfNewFIMGroupParser());
    /* 20 */ result->push_back(new jfFIMGroupParser());
    /* 21 */ result->push_back(new jfMMO_FicPartParser());
    /* 22 */ result->push_back(result->back());

    return result;
}
//------------------------------------------
bool test::FreeParsers(ParserList* plist) {
    if (plist == NULL) return false;
    if ((plist->size()) != test::count)
    delete (*plist)[0];
    delete (*plist)[1];
    delete (*plist)[2];
    delete (*plist)[3];
    delete (*plist)[7];
    delete (*plist)[9];
    delete (*plist)[10];
    delete (*plist)[11];
    delete (*plist)[14];
    delete (*plist)[15];
    delete (*plist)[16];
    delete (*plist)[17];
    delete (*plist)[19];
    delete (*plist)[20];
    delete (*plist)[21];
    plist->clear();
    return true;
}
//-------------------------------------------
bool test::DeleteResults(size_t index, void* resultdata) {
    if (index >= test::count) return false;
    jfResultUnitVector* item_results;
    jfFicExtract* fextract;
    jfFicPart* fpart;
    jfFicPair* fpair;
    // Fanfiction.Net sections
    if (index == 0) {
        jfFFNSection* sdata1 = static_cast<jfFFNSection*>(resultdata);
        delete sdata1;
    }
    else if (index == 1) {
        jfFFN_CrossoverSection* sdata2 = static_cast<jfFFN_CrossoverSection*>(resultdata);
        delete sdata2;
    }
    else if (index == 2) {
        jfFFN_CrossoverGroup* sdata3 = static_cast<jfFFN_CrossoverGroup*>(resultdata);
        delete sdata3;
    }
    // index listings
    else if (index <= 6) {
        item_results = static_cast<jfResultUnitVector*>(resultdata);
        delete resultdata;
    }
    // Fanfiction.Net first page results
    else if (index == 7) {
        fpair = static_cast<jfFicPair*>(resultdata);
        delete (fpair->extract);
        delete (fpair->part_one);
    }
    // Fanfiction.Net non first page results
    else if (index == 8) {
        fpart = static_cast<jfFicPart*>(resultdata);
        delete fpart;
    }
    // ArchiveOfOurOwn.org fandom
    else if (index == 9) {
        jfAO3Section* catsection = static_cast<jfAO3Section*>(resultdata);
        delete catsection;
    }
    // index listings
    else if (index == 10) {
        item_results = static_cast<jfResultUnitVector*>(resultdata);
        delete resultdata;
    }
    // ArchiveOfOurOwn.org first page results
    else if (index <= 12) {
        fpair = static_cast<jfFicPair*>(resultdata);
        delete (fpair->extract);
        delete (fpair->part_one);
    }
    else if (index == 13) {
        fpart = static_cast<jfFicPart*>(resultdata);
        delete fpart;
    }
    // Fimfiction.Net tag data
    else if (index == 14) {
        jfFIMTag_Collection* tagc = static_cast<jfFIMTag_Collection*>(resultdata);
        delete tagc;
    }
    // compact fimfiction.net data
    else if (index == 15) {
        jfIDStringStore* sstore = static_cast<jfIDStringStore*>(resultdata);
        delete sstore;
    }
    // index listings
    else if (index == 16) {
        item_results = static_cast<jfResultUnitVector*>(resultdata);
        delete resultdata;
    }
    // fimfiction.net index page
    else if (index == 17) {
        fextract = static_cast<jfFicExtract*>(resultdata);
        delete fextract;
    }
    //fimfiction.net fic part page
    else if (index == 18) {
        fpart = static_cast<jfFicPart*>(resultdata);
        delete fpart;
    }
    // fim group data
    else if (index <= 20) {
        item_results = static_cast<jfResultUnitVector*>(resultdata);
        delete resultdata;
    }

    // fimfiction.net group parts
    else if (index <= 20) {
        item_results = static_cast<jfResultUnitVector*>(resultdata);
        delete resultdata;
    }

    // mediaminer.org fics are index, then separate parts, always.
    else if (index == 21) {
        fextract = static_cast<jfFicExtract*>(resultdata);
        delete fextract;
    }
    else if (index == 22) {
        fpart = static_cast<jfFicPart*>(resultdata);
        delete fpart;
    }
    else return false;
    // done
    return true;
}

/*****************************************************************************/
