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
import sys
# ==================================================================
# if geckodriver is not in your path, you will need to modify this file
# DRIVER_PATH = r'/home/david/Programs/geckodriver'
# The line below ise used, define it correctly
# CHROME_PATH= r'<FIX HERE>\chrome.exe'
# the value below, if set to true, will try to insert a script into the webpages that will turn the webdriver flag off.
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
        self.mbrowser = undetected_chromedriver.Chrome(options=options,version_main=111,use_subprocess=True,browser_executable_path=CHROME_PATH)
        self.starting = False

    def applyCookieString(self, cookie_string:str):
        cookie_object = SimpleCookie()
        cookie_object.load(cookie_string)
        for key,value in cookie_object.items():
            self.mbrowser.add_cookies( { 'name':key , 'value':value } )
    

    # This is core Fetch customized for fanfiction.net and cloudflare
    def coreFetchFanfiction(self, url:str, waittime:int, cookie:Optional[str] ) -> bool:

        # -- CONDITIONS BASED ON PAGE RETURNED ---
        # page fetched okay
        goodPage = EC.presence_of_element_located((By.ID, "p_footer"))
        # fanfiction error (TODO)
        #fanficError = EC.presence_of_element_located((By.ID, "p_footer"))
        # fanfiction.net challenge button <input type="button" value="Verify you are human"
        challengeButton = EC.presence_of_element_located((By.XPATH, "//input[@value='Verify you are human']"))

        self.mbrowser.get(url)
        if UNDEFINE_WEBDRIVER:
            self.mbrowser.execute_script("Object.defineProperty(navigator, 'webdriver', {get: () => undefined})")
        # apply cookies after get?
        if (cookie != None) and (len(cookie) > 0):
            self.applyCookieString(cookie) 

        elem = WebDriverWait(self.mbrowser,waittime).until(EC.any_of(goodPage, challengeButton))

        # check what we get and take action
        if elem.get_attribute("id") == "p_footer":
            # page fetched okay
            return True
        elif elem.get_attribute("value") == "Verify you are human":
            # here, we wait, click the button, and then wait for a good page
            time.sleep(1.5)
            elem.click()
            elem = WebDriverWait(self.mbrowser,waittime).until(goodPage)
            return True
        else:
            return False

    # Trying to click the Cloudflare 'Verify you are human' checkbox
    def cloudflareVerifyCheck(self, waittime:int) -> bool:

        # looking for the frame first
        cframepath = "//iframe[@title='Widget containing a Cloudflare security challenge']"
        condition = EC.frame_to_be_available_and_switch_to_it((By.XPATH,cframepath))
        WebDriverWait(self.mbrowser,waittime).until(condition)

        # next, the checkbox 
        # <label class="ctp-checkbox-label"><input type="checkbox"><span class="mark"></span>Verify you are human</label>
        cloudflareCheckbox = EC.element_to_be_clickable((By.CSS_SELECTOR, ".ctp-checkbox-label"))
        elem = WebDriverWait(self.mbrowser,waittime).until(cloudflareCheckbox)
        
        # looking for the checkbox
        elementList = elem.find_elements(By.TAG_NAME,"input")
        if len(elementList) > 0:
            inputcb = elementList[0]
            if inputcb.get_attribute("type") == "checkbox":
                # found the checkbox.
                time.sleep(2)
                inputcb.click()
                self.mbrowser.switch_to.parent_frame()
                goodPage = EC.presence_of_element_located((By.ID, "p_footer"))
                # waiting for a good page means if we get a challenge twice in a row, we abort
                # (aborting will trigger re-making the browser object and trying again)
                elem = WebDriverWait(self.mbrowser,waittime).until(goodPage)
                return True
            else:
                return False
        else:
            return False

    # Special fetch with fallback. Note that we cannot wait for an item inside a frame directly.
    # so, the procedure will be to wait for a good page until we timeout, then try
    # to look for the Cloudflare Turnstile 'Verify you are human' checkbox.
    def fallbackFetch(self, url:str, mainwait:int, fallbackwait:int, cookie:Optional[str] ) -> bool:
        gotten = False
        try:
            # waiting for a good page
            gotten = self.coreFetchFanfiction(url, mainwait, cookie)
            return gotten
        except (TimeoutException,Exception) as tex:
            sys.stderr.write(repr(tex) + "\n")
            try:
                # looking for (and trying to click) the clouflare checkbox
                gotten = self.cloudflareVerifyCheck(fallbackwait)
                return gotten
            except (TimeoutException, Exception) as tex2:
                sys.stderr.write(repr(tex2) + "\n")
                return False

    # if this method finishes without throwing an exception, it succeeds
    # not currencty used
    def coreFetch(self, url:str, find_id:str, waittime:int, cookie:Optional[str] ):
        self.mbrowser.get(url)
        if UNDEFINE_WEBDRIVER:
            self.mbrowser.execute_script("Object.defineProperty(navigator, 'webdriver', {get: () => undefined})")
        # apply cookies after get?
        if (cookie != None) and (len(cookie) > 0):
            self.applyCookieString(cookie)   
        elem = WebDriverWait(self.mbrowser,waittime).until(EC.presence_of_element_located((By.ID, find_id)))



    # the primary fetch in a loop 
    def fetchURL(self, url:str, cookie:Optional[str]) -> bool:
        try:
            # fetching
            self.fetched_data = ''
            retry_count = 0
            gotten = False
            while retry_count < 20:
                gotten = False
                try:
                    gotten = self.fallbackFetch(url, 15 + 10*retry_count, 15 + 10*retry_count, cookie)
                    if gotten:
                       break
                except (TimeoutException,Exception) as tex:
                    sys.stderr.write(repr(tex) + "\n")
                # if we get here, we have failed, discard the session in the hopes that starting over will fix things
                self.closeSession()
                retry_count = retry_count + 1
                self.setupBrowser()
            # here, the fetch has been a success
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



