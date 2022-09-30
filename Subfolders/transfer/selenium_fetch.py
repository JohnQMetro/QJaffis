# selenium
from selenium import webdriver
from selenium.webdriver.support.ui import WebDriverWait
from selenium.webdriver.common.by import By
from selenium.webdriver.support import expected_conditions as EC
from selenium.common.exceptions import TimeoutException

# from selenium.webdriver.firefox.options import Options
# from selenium.webdriver.firefox.service import Service

import undetected_chromedriver
from http.cookies import SimpleCookie
from typing import Optional
import time
# ==================================================================

DRIVER_PATH = r'/home/david/Programs/geckodriver'
UNDEFINE_WEBDRIVER = False


class SeleniumFetcher:

    def __init__(self,proxy:Optional[str], user_agent:Optional[str]):
        self.proxy = proxy
        self.user_agent = user_agent
        self.mbrowser = None

        self.starting:bool = True
        self.cloudcookie:bool = False
        self.fetches_done:int = 0
        self.fetched_data:str = ''
        self.fetch_error:str = ''
        
    def setupBrowser(self):
        options = undetected_chromedriver.ChromeOptions();
        options.headless = False
        options.add_argument("--start-maximized")
        options.add_argument("--dns-prefetch-disable")
        # driverService = Service(DRIVER_PATH)
        self.mbrowser = undetected_chromedriver.Chrome(options=options,version_main=105)
        self.starting = False

    def applyCookieString(self, cookie_string:str):
        cookie_object = SimpleCookie()
        cookie_object.load(cookie_string)
        for key,value in cookie_object.items():
            self.mbrowser.add_cookies( { 'name':key , 'value':value } )

    # if this method finishes without throwing an exception, it succeeds
    def coreFetch(self, url:str, find_id:str, waittime:int, cookie:Optional[str] ):
        self.mbrowser.get(url)
        if UNDEFINE_WEBDRIVER:
            self.mbrowser.execute_script("Object.defineProperty(navigator, 'webdriver', {get: () => undefined})")
        # apply cookies after get?
        if (cookie != None) and (len(cookie) > 0):
            self.applyCookieString(cookie)   
        elem = WebDriverWait(self.mbrowser,waittime).until(EC.presence_of_element_located((By.ID, find_id)))


    def fetchURL(self, url:str, cookie:Optional[str]) -> bool:
        try:
            # fetching
            self.fetched_data = ''
            retry_count = 0
            while retry_count < 10:
                gotten = False
                try:
                    self.coreFetch(url, "p_footer", 40 + 10*retry_count, cookie)
                    gotten = True
                    break
                except (TimeoutException,Exception) as tex:
                    sys.stderr.write(repr(error_message) + "\n")
                    self.closeSession()
                    retry_count = retry_count + 1
                    self.setupBrowser()
            if gotten:
                time.sleep(2)
                down_data = self.mbrowser.page_source
                # saving
                self.fetched_data = down_data
                self.fetches_done = self.fetches_done + 1
                # deleting cookies sometimes throws exceptions
                try:
                    self.mbrowser.delete_all_cookies()
                    self.cloudcookie = False
                except Exception as cex:
                    pass
                return True
            else:
                self.fetch_error = "Failed to get page after " + retry_count + " tries"
                return False
                
        except Exception as ex:
            # selenium never returns status codes or errors like that
            self.fetched_data = self.mbrowser.page_source
            self.fetch_error = repr(ex)
            return False

    def closeSession(self):
        self.mbrowser.close()
        self.mbrowser = None
        self.starting = True



