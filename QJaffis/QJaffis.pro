#-------------------------------------------------
#
# Project created by QtCreator 2016-07-14T08:35:06
#
#-------------------------------------------------

QT       += core gui network widgets concurrent
 CONFIG += console

TARGET = QJaffis
TEMPLATE = app

SOURCES += main.cpp\
    core/filters/ao3/ao3_percent_tags_filter.cpp \
    core/tagfilter/base_tagfilters.cpp \
    core/tagfilter/multimatchmake.cpp \
    core/tagfilter/stringcounter.cpp \
    core/tagfilter/taglist.cpp \
    core/tagfilter/tagmatch.cpp \
    defaultpaths.cpp \
    fanfic/data/ao3/ao3_lookvals.cpp \
    fanfic/data/ao3/ao3_urlmake.cpp \
    fanfic/data/displayhtmlspec.cpp \
    fanfic/interface/ao3/ao3_searchfilter_panel.cpp \
    fanfic/interface/ao3/ao3_tagparams.cpp \
    fanficnet/threads/ffncatparser_exp1.cpp \
    fetching/core/fetchbase.cpp \
    fetching/core/fetchpython.cpp \
    fetching/core/fetchqt.cpp \
    fetching/core/fetchwrapbase.cpp \
    fetching/core/pagefetch.cpp \
    fetching/core/pythonpaths.cpp \
    fetching/core/testswrapper.cpp \
    fetching/downbase.cpp \
    fetching/logbase.cpp \
    fetching/loopget/downloadroot.cpp \
    fetching/loopget/downloadroot2.cpp \
    fetching/loopget/getpage.cpp \
    fetching/loopget/multicatroot.cpp \
    fetchparse/parsefetch_packagemaker.cpp \
    ficdown/threads/update_fetcher.cpp \
    globalsettings.cpp \
    interface/controls/charo_editor.cpp \
    interface/controls/tag_widgets.cpp \
    interface/controls/tag_widgets2.cpp \
    interface/controls/widgets1.cpp \
    interface/controls/widgets2.cpp \
    interface/controls/widgets3.cpp \
    interface/controls/widgets4.cpp \
    interface/filters/ao3/ao3_list_percent_filter_editor.cpp \
    interface/filters/tagfilter/filter_match.cpp \
    interface/filters/tagfilter/multimatch.cpp \
    interface/filters/tagfilter/tag_match_filter_edits.cpp \
        mainwindow.cpp \
    initend.cpp \
    core/express/expr_structs.cpp \
    core/express/expr_parse.cpp \
    core/express/expr_match.cpp \
    core/express/expr_base.cpp \
    core/filters/ao3/ao3_stringfilters.cpp \
    core/filters/ao3/ao3_specials3.cpp \
    core/filters/ao3/ao3_specials2.cpp \
    core/filters/ao3/ao3_specials1.cpp \
    core/filters/base/sexp_filters.cpp \
    core/filters/base/filterbase.cpp \
    core/filters/base/expression.cpp \
    core/filters/extended/pairfilter_tools.cpp \
    core/filters/extended/pairfilter.cpp \
    core/filters/extended/morefilters1.cpp \
    core/filters/extended/minmax.cpp \
    core/filters/extended/extended_sexp.cpp \
    core/filters/extended/daterange.cpp \
    core/filters/ffn/ffn_special1.cpp \
    core/filters/fim/fimfilters2.cpp \
    core/filters/fim/fimfilters1.cpp \
    core/filters/filter_groups.cpp \
    core/filters/filter_globals.cpp \
    core/objs/skelstore.cpp \
    core/objs/searchcore.cpp \
    core/objs/misc_types1.cpp \
    core/objs/baseobj.cpp \
    core/structs/vectors.cpp \
    core/structs/skelcoll.cpp \
    core/structs/resultcollect.cpp \
    core/structs/pagetracker.cpp \
    core/structs/itemcollect.cpp \
    core/structs/categories.cpp \
    core/structs/basecollect.cpp \
    core/utils/utils3.cpp \
    core/utils/utils2.cpp \
    core/utils/utils1.cpp \
    core/utils/stringparser.cpp \
    core/utils/specialfuncs.cpp \
    core/utils/logging.cpp \
    core/utils/htmlskelton.cpp \
    core/utils/htmlparse.cpp \
    fanfic/data/ao3/ao3_ficobj.cpp \
    fanfic/data/ao3/ao3_consts.cpp \
    fanfic/data/ao3/ao3_cats.cpp \
    fanfic/data/cats/gen_cat.cpp \
    fanfic/data/cats/cat_misc.cpp \
    fanfic/data/cats/cat_coll.cpp \
    fanfic/data/fim/fim_search.cpp \
    fanfic/data/fim/fim_groupsearch.cpp \
    fanfic/data/fim/fim_groupobj.cpp \
    fanfic/data/fim/fim_ficobj.cpp \
    fanfic/data/fim/fim_constants.cpp \
    fanfic/data/fim/fim_coll.cpp \
    fanfic/data/ficbase.cpp \
    fanfic/interface/fim/fimgroup_panel1.cpp \
    fanfic/interface/fim/fimgroup_mainpanel1.cpp \
    fanfic/interface/fim/fim_panel1.cpp \
    fanfic/interface/fim/fim_mainpanel1.cpp \
    fanfic/threads/ao3/ao3cat_parser.cpp \
    fanfic/threads/fim/fimparse_items.cpp \
    fanfic/threads/fim/fimparse_compact.cpp \
    fanfic/threads/fim/fimitem_thread.cpp \
    fanfic/threads/fim/fimgroup_thread.cpp \
    fanfic/threads/fim/fimgroup_parser.cpp \
    fanfic/threads/gencat_thread.cpp \
    fanficnet/categories/ffn_catcore.cpp \
    fanficnet/data/ffn_ficobj.cpp \
    fetching/panel/progress_panel2.cpp \
    fetching/panel/progress_panel.cpp \
    fetching/threadbase.cpp \
    fetching/pagegetter.cpp \
    fetching/fetchandparse.cpp \
    fetching/connect_helpers.cpp \
    fetching/baseparse.cpp \
    ficdown/data/ficurl_parsing.cpp \
    ficdown/data/ficpart.cpp \
    ficdown/data/ficextract.cpp \
    ficdown/data/ficdown_data.cpp \
    ficdown/interface/ffndown_dialog.cpp \
    ficdown/parsers/mmoparser.cpp \
    ficdown/parsers/makeparser.cpp \
    ficdown/parsers/ffnparser.cpp \
    ficdown/parsers/basefimparser.cpp \
    ficdown/parsers/ao3parser.cpp \
    ficdown/threads/ficdown_tbase.cpp \
    ficdown/threads/ficdown_single.cpp \
    interface/display/typed_display.cpp \
    interface/display/search_panels.cpp \
    interface/display/result_panel.cpp \
    interface/display/mainsearch.cpp \
    interface/display/html_display.cpp \
    interface/display/displayindex.cpp \
    interface/display/display_parts.cpp \
    interface/filters/ao3/ao3_special3.cpp \
    interface/filters/ao3/ao3_special2.cpp \
    interface/filters/ao3/ao3_special1.cpp \
    interface/filters/ao3/ao3_sexpeditors.cpp \
    interface/filters/base/urlfilter_editor.cpp \
    interface/filters/base/sfilt_edit.cpp \
    interface/filters/base/sexpr_edit.cpp \
    interface/filters/base/expr_editor.cpp \
    interface/filters/base/bfilt_edit.cpp \
    interface/filters/category/category_listedit.cpp \
    interface/filters/category/category_editor.cpp \
    interface/filters/extended/sexp_extended.cpp \
    interface/filters/extended/pairs_edit.cpp \
    interface/filters/extended/pairf_editor.cpp \
    interface/filters/extended/minmax_editor.cpp \
    interface/filters/extended/filters_edit1.cpp \
    interface/filters/extended/drange_edit.cpp \
    interface/filters/ffn/ffn_filteredit1.cpp \
    interface/filters/fim/fim2_filedit.cpp \
    interface/filters/fim/fim1_filedit.cpp \
    interface/filters/filter_utils.cpp \
    interface/filters/filter_picker.cpp \
    interface/filters/filter_editor.cpp \
    interface/misc/skeledit.cpp \
    interface/misc/menu.cpp \
    interface/misc/global_skel.cpp \
    interface/misc/defaultdir_dialog.cpp \
    fanfic/threads/ao3/ao3cat_thread.cpp \
    fanfic/interface/gcat_picker.cpp \
    fanfic/interface/gcat_dialog.cpp \
    fanfic/interface/ao3/ao3_catdialog.cpp \
    core/structs/result_multi.cpp \
    fanfic/threads/ao3/ao3item_parser.cpp \
    fanfic/data/ao3/ao3_coll.cpp \
    fanfic/threads/ao3/ao3item_thread.cpp \
    fanfic/data/ao3/ao3_search.cpp \
    fanfic/interface/ao3/ao3_panel1.cpp \
    fanfic/interface/ao3/ao3_mainpanel.cpp \
    fanficnet/categories/cat_collections.cpp \
    fanficnet/categories/crossover_groups.cpp \
    fanficnet/threads/ffncatparser1.cpp \
    fanficnet/threads/ffncatparser2.cpp \
    fanficnet/categories/ffn_catpdata.cpp \
    fanficnet/categories/catgroup.cpp \
    fanficnet/threads/ffncatthread.cpp \
    fanficnet/interface/categories_downdialog.cpp \
    fanficnet/data/ffn_search.cpp \
    fanficnet/data/ffn_linkdata.cpp \
    fanficnet/threads/ffnitem_parser.cpp \
    fanficnet/data/ffn_itemcoll.cpp \
    fanficnet/data/ffn_mulcoll.cpp \
    fanficnet/threads/ffnitems_thread.cpp \
    fanficnet/interface/ffn_catpick2.cpp \
    fanficnet/interface/ffn_catpick1.cpp \
    fanficnet/interface/ffn_panel1.cpp \
    fanficnet/interface/ffn_mainpanel.cpp \
    ficdown/threads/ficupdate_thread.cpp \
    ficdown/interface/ficupdate_panel.cpp \
    core/utils/dirupdate.cpp \
    ficdown/threads/dirupdate_thread.cpp \
    ficdown/interface/dirupdate_dialog.cpp \
    ffnupdate/data/upd_types.cpp \
    ffnupdate/data/auth_coll.cpp \
    ffnupdate/threads/author_pageparser.cpp \
    ffnupdate/data/res_bundle.cpp \
    fanfic/data/cats/catgroupx.cpp \
    interface/defcolors.cpp \
    fanfic/data/fim/fim_tags.cpp \
    fetching/onepage.cpp \
    fanfic/threads/fim/fimtag_parse.cpp \
    fanfic/interface/fim/fimtagdown_dialog.cpp \
    fanfic/threads/fim/newfimgroup_parser.cpp \
    core/filters/fim/fimfilters3.cpp \
    interface/filters/fim/fim3_filedit.cpp \
    test/testfetch.cpp \
    test/testparse.cpp \
    test/testurl.cpp \
    test/testthread.cpp \
    test/testpanel.cpp

HEADERS  += mainwindow.h \
    core/filters/ao3/ao3_percent_tags_filter.h \
    core/tagfilter/base_tagfilters.h \
    core/tagfilter/multimatchmake.h \
    core/tagfilter/stringcounter.h \
    core/tagfilter/taglist.h \
    core/tagfilter/tagmatch.h \
    defaultpaths.h \
    fanfic/data/ao3/ao3_lookvals.h \
    fanfic/data/displayhtmlspec.h \
    fanfic/interface/ao3/ao3_searchfilter_panel.h \
    fanfic/interface/ao3/ao3_tagparams.h \
    fanficnet/threads/ffncatparser_exp1.h \
    fetching/core/fetchbase.h \
    fetching/core/fetchpython.h \
    fetching/core/fetchqt.h \
    fetching/core/fetchwrapbase.h \
    fetching/core/pagefetch.h \
    fetching/core/pythonpaths.h \
    fetching/core/testswrapper.h \
    fetching/downbase.h \
    fetching/logbase.h \
    fetching/loopget/downloadroot.h \
    fetching/loopget/downloadroot2.h \
    fetching/loopget/getpage.h \
    fetching/loopget/multicatroot.h \
    fetchparse/parsefetch_packagemaker.h \
    ficdown/threads/update_fetcher.h \
    globalsettings.h \
    initend.h \
    core/express/expr_structs.h \
    core/express/expr_parse.h \
    core/express/expr_match.h \
    core/express/expr_base.h \
    core/filters/ao3/ao3_stringfilters.h \
    core/filters/ao3/ao3_specials3.h \
    core/filters/ao3/ao3_specials2.h \
    core/filters/ao3/ao3_specials1.h \
    core/filters/base/sexp_filters.h \
    core/filters/base/filterbase.h \
    core/filters/base/expression.h \
    core/filters/extended/pairfilter_tools.h \
    core/filters/extended/pairfilter.h \
    core/filters/extended/morefilters1.h \
    core/filters/extended/minmax.h \
    core/filters/extended/extended_sexp.h \
    core/filters/extended/daterange.h \
    core/filters/ffn/ffn_special1.h \
    core/filters/fim/fimfilters2.h \
    core/filters/fim/fimfilters1.h \
    core/filters/filter_groups.h \
    core/filters/filter_globals.h \
    core/objs/skelstore.h \
    core/objs/searchcore.h \
    core/objs/misc_types1.h \
    core/objs/baseobj.h \
    core/structs/vectors.h \
    core/structs/typedcollect.h \
    core/structs/skelcoll.h \
    core/structs/resultcollect.h \
    core/structs/pagetracker.h \
    core/structs/itemcollect.h \
    core/structs/categories.h \
    core/structs/basecollect.h \
    core/utils/utils3.h \
    core/utils/utils2.h \
    core/utils/utils1.h \
    core/utils/stringparser.h \
    core/utils/specialfuncs.h \
    core/utils/logging.h \
    core/utils/htmlskelton.h \
    core/utils/htmlparse.h \
    fanfic/data/ao3/ao3_ficobj.h \
    fanfic/data/ao3/ao3_consts.h \
    fanfic/data/ao3/ao3_cats.h \
    fanfic/data/cats/gen_cat.h \
    fanfic/data/cats/cat_misc.h \
    fanfic/data/cats/cat_coll.h \
    fanfic/data/fim/fim_search.h \
    fanfic/data/fim/fim_groupsearch.h \
    fanfic/data/fim/fim_groupobj.h \
    fanfic/data/fim/fim_ficobj.h \
    fanfic/data/fim/fim_constants.h \
    fanfic/data/fim/fim_coll.h \
    fanfic/data/ficbase.h \
    fanfic/interface/fim/fimgroup_panel1.h \
    fanfic/interface/fim/fimgroup_mainpanel1.h \
    fanfic/interface/fim/fim_panel1.h \
    fanfic/interface/fim/fim_mainpanel1.h \
    fanfic/threads/ao3/ao3cat_parser.h \
    fanfic/threads/fim/fimparse_items.h \
    fanfic/threads/fim/fimparse_compact.h \
    fanfic/threads/fim/fimitem_thread.h \
    fanfic/threads/fim/fimgroup_thread.h \
    fanfic/threads/fim/fimgroup_parser.h \
    fanfic/threads/gencat_thread.h \
    fanficnet/categories/ffn_catcore.h \
    fanficnet/data/ffn_ficobj.h \
    fetching/panel/progress_panel2.h \
    fetching/panel/progress_panel.h \
    fetching/threadbase.h \
    fetching/pagegetter.h \
    fetching/fetchandparse.h \
    fetching/connect_helpers.h \
    fetching/baseparse.h \
    ficdown/data/ficurl_parsing.h \
    ficdown/data/ficpart.h \
    ficdown/data/ficextract.h \
    ficdown/data/ffndown_data.h \
    ficdown/interface/ffndown_dialog.h \
    ficdown/parsers/mmoparser.h \
    ficdown/parsers/makeparser.h \
    ficdown/parsers/ffnparser.h \
    ficdown/parsers/basefimparser.h \
    ficdown/parsers/ao3parser.h \
    ficdown/threads/ficdown_tbase.h \
    ficdown/threads/ficdown_single.h \
    interface/controls/charo_editor.h \
    interface/controls/tag_widgets.h \
    interface/controls/tag_widgets2.h \
    interface/controls/widgets1.h \
    interface/controls/widgets2.h \
    interface/controls/widgets3.h \
    interface/controls/widgets4.h \
    interface/display/typed_display.h \
    interface/display/search_panels.h \
    interface/display/result_panel.h \
    interface/display/mainsearch.h \
    interface/display/html_display.h \
    interface/display/displayindex.h \
    interface/display/display_parts.h \
    interface/filters/ao3/ao3_list_percent_filter_editor.h \
    interface/filters/ao3/ao3_special3.h \
    interface/filters/ao3/ao3_special2.h \
    interface/filters/ao3/ao3_special1.h \
    interface/filters/ao3/ao3_sexpeditors.h \
    interface/filters/base/urlfilter_editor.h \
    interface/filters/base/sfilt_edit.h \
    interface/filters/base/sexpr_edit.h \
    interface/filters/base/expr_editor.h \
    interface/filters/base/bfilt_edit.h \
    interface/filters/category/category_listedit.h \
    interface/filters/category/category_editor.h \
    interface/filters/extended/sexp_extended.h \
    interface/filters/extended/pairs_edit.h \
    interface/filters/extended/pairf_editor.h \
    interface/filters/extended/minmax_editor.h \
    interface/filters/extended/filters_edit1.h \
    interface/filters/extended/drange_edit.h \
    interface/filters/ffn/ffn_filteredit1.h \
    interface/filters/fim/fim2_filedit.h \
    interface/filters/fim/fim1_filedit.h \
    interface/filters/filter_utils.h \
    interface/filters/filter_picker.h \
    interface/filters/filter_editor.h \
    interface/filters/tagfilter/filter_match.h \
    interface/filters/tagfilter/multimatch.h \
    interface/filters/tagfilter/tag_match_filter_edits.h \
    interface/misc/skeledit.h \
    interface/misc/menu.h \
    interface/misc/global_skel.h \
    interface/misc/defaultdir_dialog.h \
    mw_includes.h \
    fanfic/threads/ao3/ao3cat_thread.h \
    ficdown/interface/ffndown_dialog.h \
    fanfic/interface/gcat_picker.h \
    fanfic/interface/gcat_dialog.h \
    fanfic/interface/ao3/ao3_catdialog.h \
    core/structs/result_multi.h \
    fanfic/threads/ao3/ao3item_parser.h \
    fanfic/data/ao3/ao3_coll.h \
    fanfic/threads/ao3/ao3item_thread.h \
    fanfic/data/ao3/ao3_search.h \
    fanfic/interface/ao3/ao3_panel1.h \
    fanfic/interface/ao3/ao3_mainpanel.h \
    fanficnet/categories/cat_collections.h \
    fanficnet/categories/crossover_groups.h \
    fanficnet/threads/ffncatparser1.h \
    fanficnet/threads/ffncatparser2.h \
    fanficnet/categories/ffn_catpdata.h \
    fanficnet/threads/ffncatthread.h \
    fanficnet/interface/categories_downdialog.h \
    fanficnet/data/ffn_search.h \
    fanficnet/data/ffn_linkdata.h \
    fanficnet/threads/ffnitem_parser.h \
    fanficnet/data/ffn_itemcoll.h \
    fanficnet/data/ffn_mulcoll.h \
    fanficnet/threads/ffnitems_thread.h \
    fanficnet/interface/ffn_catpick2.h \
    fanficnet/interface/ffn_catpick1.h \
    fanficnet/interface/ffn_panel1.h \
    fanficnet/interface/ffn_mainpanel.h \
    ficdown/threads/ficupdate_thread.h \
    ficdown/interface/ficupdate_panel.h \
    core/utils/dirupdate.h \
    ficdown/threads/dirupdate_thread.h \
    ficdown/interface/dirupdate_dialog.h \
    ffnupdate/data/upd_types.h \
    ffnupdate/data/auth_coll.h \
    ffnupdate/threads/author_pageparser.h \
    ffnupdate/data/res_bundle.h \
    ffnupdate/threads/loadold_thread.h \
    fanfic/data/cats/catgroupx.h \
    interface/defcolors.h \
    fanfic/data/fim/fim_tags.h \
    fetching/onepage.h \
    fanfic/threads/fim/fimtag_parse.h \
    fanfic/interface/fim/fimtagdown_dialog.h \
    fanfic/threads/fim/newfimgroup_parser.h \
    core/filters/fim/fimfilters3.h \
    interface/filters/fim/fim3_filedit.h \
    test/testfetch.h \
    test/testparse.h \
    test/testurl.h \
    test/testthread.h \
    test/testpanel.h \
    fanficnet/categories/catgroup.h

RC_FILE = qjaffis.rc

OTHER_FILES += \
    todo.txt

DISTFILES += \
    interface/filters/tagfilter/ydhjdty.txt
