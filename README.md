# QJaffis

QJaffis is a Graphical program that serves as a search and download tool for Fanfiction.Net, Fimfiction.Net, and Archiveofourown.org, with a few other additions. This program was developed as a personal tool that I am releasing in case somebody else might find it useful.

The folder QJaffis contains the source code, icon, and the Qt Creator project file.
The folder Subfolder contains other folders, scripts, and config files that must be placed in the working/executable folder of a built version.

Note that for Fanfiction.Net, the need to get past Cloudflare, combined with the lack of Selenium for C++, means the program needs to download by proxy, linking with a python script. The script I have written uses undetected chromedriver and Chromium to fetch pages, this likely need to be customized by the user.

It currently lacks proper documentation. 

I make no claims as to the elegance or usefulness of the program. If you wish to report bugs, you can do so, but do not necessarily expect them to be fixed (or even expect a response).

If you wish to build QJaffis, it is currently being developed using Qt Creator (an older version), with Qt 5.12 (also an older version). It has not yet been tested or built on anything except 64 bit Windows and 64 bit Linux). The Qt and SSL libraries are not included.
