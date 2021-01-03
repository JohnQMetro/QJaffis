#!/usr/bin/python
import sys
import cloudscraper
import codecs

# getting stuff from the command line
def ParseArguments():
    numarg = len(sys.argv)
    if numarg < 3:
        return False
    # param 0 is the script name, we ignore
    # param 1 is some sort of special flag
    # params 2+ are urls
    global invals
    invals = (sys.argv[1],sys.argv[2:])
    return True
    
def MakeBrowser():
    return {
        'browser': 'chrome',
        'platform': 'windows',
        'desktop': True
    }
    
# flag a: get the tokens
def PrintTokens():
    separat = "#~~~___~~~#"
    first_url = invals[1][0];
    # unfortunatly, get_tokens and get_cookie_string are defective
    # scraper = cloudscraper.create_scraper(browser=MakeBrowser(),debug=True);
    token_tuple = cloudscraper.get_tokens(first_url, browser = MakeBrowser(), debug=True)
    # cookie_value, user_agent = cloudscraper.get_cookie_string('https://www.fanfiction.net/crossovers/Myth-Adventures/5143/', browser = MakeBrowser())
    print(separat)
    # user-agent first
    print(token_tuple[1])    
    print(separat)
    # print(cookie_value)
    # the cookies (tokens)
    for cookie_key in token_tuple[0]:
        print(cookie_key)
        print(token_tuple[0][cookie_key])
        print(separat)

# flag b: get the urls
def PrintURLContents():
    scraper = cloudscraper.create_scraper(browser=MakeBrowser(), debug = True);
    separat = "#~~~___~~~#"
    file = codecs.open("transfer.txt", "w", encoding="utf-8")
    for curl in invals[1]:
        file.write(separat)
        file.write("\n")
        file.write(curl)
        file.write(separat)
        file.write("\n")
        down_data = scraper.get(curl).text
        file.write(down_data);
    file.close()    

# Main

if ParseArguments():
    if (invals[0] == 'a'):
        PrintTokens()
    else:
        PrintURLContents()
else:
    print("Arguments bad!")


