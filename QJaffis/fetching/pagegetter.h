/*****************************************************************************
Name    : pagegetter.h
Basic   : Declares my own html page fetching class (new for Qt)
Author  : John Q Metro
Started : March 15, 2013
Updated : March 30, 2018

******************************************************************************/
#ifndef PAGEGETTER_H
#define PAGEGETTER_H
#endif // PAGEGETTER_H
//---------------------------------
#include <QString>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUrl>
#include <QObject>
//*****************************************************************************
// an enumerated type for downloader errors, simpler than Qt Errors
/* updated Oct 18, 2015, to reflect a split between fetching and parsing, the
 * following is now the error after a single attempt at fetching. */
enum jfFETCH_ERROR {jff_NOERROR,      // no error
                    jff_TRYAGAIN,     // some sort of error, but not serious
                    jff_REDIRECTION,  // redirection, change the url and try again
                    jff_MISSING,      // 404 not found (or some equivalent). Don't try again
                    jff_FALIURE,      // an error that means trying again is futile
                    jff_HALTED
                   };


//=============================================================================
class jfPageParserBase; // forward declaration
/* Page fetching class.
-Generally, one calls SetParams(,,) to setup basic info for a site, before doing a loop
  that fetches multiple pages from that site.
-For each loop, SetURL(,) should be called first, and the result should be checked.
-Then one calls StartDownload().
-Optionally, once can do something else, because the download is asynchronous
-After, call PrepareResults() to try and get the page. If there is an error, we set
 the error and return false. If we get the page, we convert it to a string, and
 then use the TestMissing and TestIncomplete fucntions, if either one is false, we
 set the error, discard the page, and return false. If everything goes fine, the
 page is stored in thepage and we return true.
-If the download was successful, call GetResult() to get the page and reset the results.
-If the download failed and one wants to try again, call Reset() before doing so.

Note : if the function pointer input tests are NULL, we assume the tests always pass
*/
class jfFetchPage : public QObject {
  public:
      bool check_clientred; // set to true if we want to check for client-side redirection
    // constructor
      jfFetchPage();
    // setting inputs
      bool SetURL(const QString& inurl, const size_t& url_index);
      bool SetParams(jfPageParserBase* in_testsDelegate);
    // main functional methods
      bool StartDownload();  // starts the download, true if the url is valid and no download has been started
      bool PrepareResults(); // tries to get the data and convert it to a string. false if en error happened.
      bool Abort(); // only used between calling the above two, halts download and clears download objects
    // status info
      bool ValidUrl() const;      // true if the url exists and is valid
      bool IsDownloading() const; // true inbetween a succesful call to StartDownload(), and calling PrepareResults()
      bool AfterDownload() const; // true after calling PrepareResults(), before Reset()
      size_t GetErrorCount(jfFETCH_ERROR pick_which);
    // result info
      bool HasResult() const; // true if thepage (result) is not NULL, we have a string
      QString* GetResult();   // if successful, returns the page and Resets the Results
      jfFETCH_ERROR GetError() const;  // returns the error value
      QString GetRedirection() const;  // returns the redirection result, which may be empty
      QString GetRedirectTarget() const; // like GetRedirection, but resolves the redirection first
      bool Reset(); // after calling PrepareResults(), one can call this to discard results
    // destructor
      ~jfFetchPage();
  protected:
    // internal methods
      bool MakeError();  // sets theerror based upon QNetworkReply::NetworkError
      bool ProcessResult(); // read and convert the BytesRead, and check for redirection, etc.
      bool ParseForClient(); // parses the result for client side redirection
      bool ClearObjects(); // used to delete the qt objects after download
    // flags
      bool isdownloading; // flag, are we downloading?
      bool afterdownload; // flag, do we have results (error or no)
    // stored parameters
      QUrl fetch_this;    // url to fetch the webpage
      // fp : tests if the result is just a message about missing fics/data
      jfPageParserBase* testsDelegate;
    // Qt objects used for the download
      QNetworkRequest* req;                 // combined with url and cookie to fetch
      QNetworkAccessManager* intermediary; // uses req to make raw_result
      QNetworkReply* raw_result;           // the IO device from which we get the data or errors
    // results
      QString* thepage;       // the result of the download, if things go well
      QString redirectto;     // if there is a redirection, we store it here
      jfFETCH_ERROR theerror; // we store our simplified error here
    // misc extra stuff
      size_t errlog[7];       // counting all the errors for debugging

};

//*****************************************************************************
