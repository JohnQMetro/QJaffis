#!/usr/bin/python
import sys
import codecs
import os

from selenium_fetch import *
# ------------------------------------------------------------------------------------------
DRIVER_PATH = r'/home/david/Programs/geckodriver'
PROFILE_PATH = r'/home/david/.mozilla/firefox/ivkmfl84.default'
USE_PROFILE = False
# RANDOM_USER_ID = False
UNDEFINE_WEBDRIVER = True
# EXTENSION_PATH = r''
# USE_PROXY = False


class PageFetchWrapper:
    def __init__(self,filename:str):
        self.fetcher = None
        self.filename = filename
        
    def setupFetcher(self):
        self.fetcher = SeleniumFetcher(None,None)
        self.fetcher.setupBrowser()
        
    def parseInput(self,inline):
        cookie = None
        parts = inline.split(" ||| ")
        url = parts[0].strip()
        if len(parts) > 1:
            rawcookie = parts[1].strip()
            if len(rawcookie) > 0:
                cookie = rawcookie
        return url,cookie
        
    def onError(self, code, error_message, page_data):
        sys.stderr.write(error_message + "\n")
        file = codecs.open(self.filename, "w", encoding="utf-8")
        file.write(str(code) + "\n")
        file.write(error_message + "\n")
        file.write("\n ~~~ \n\n")
        file.write(page_data)
        file.close()
        print('N');
    
        
    def fetchURLLoop(self):
        while True:
            try:
                curl = input()
                if (curl[:5] == "close"):
                    break
                # preparing to fetch
                url, cookie = self.parseInput(curl)
                sys.stderr.write("URL: " + url + "\n")
                sys.stderr.write("Cookie: " + str(cookie) + "\n")
                if len(url) == 0:
                    print('Empty URL')
                    break
                if os.path.exists(self.filename):
                    os.remove(self.filename)
                    
                # fetching
                fetched:bool = self.fetcher.fetchURL(url,cookie)
                sys.stderr.write("Fetched: " + repr(fetched) + "\n")
                if fetched:
                    down_data = self.fetcher.fetched_data
                    # saving to a file
                    file = codecs.open(self.filename, "w", encoding="utf-8")
                    file.write(down_data)
                    file.close()
                    print('Y')
                else:
                    self.onError(0, self.fetcher.fetch_error, self.fetcher.fetched_data)

            except Exception as ex:
                self.onError(0, repr(ex), self.fetcher.fetched_data)
                
        self.fetcher.closeSession();
        
# -----------------------------------------------------
def GetTransferFilename():
    numarg = len(sys.argv)
    if numarg < 2:
        return False
    # param 0 is the script name, we ignore
    else:
        return sys.argv[1]


# Main
argResult = GetTransferFilename()
if argResult != False:
    print("Starting...\n")
    fetch = PageFetchWrapper(argResult)
    fetch.setupFetcher()
    fetch.fetchURLLoop();
    print("Over\n");
else:
    print("Arguments bad!")


