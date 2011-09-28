libURL="http://ob.sr.unh.edu/svn/WSAG/branches/Balazs/snapshots/2009-12-18 NASA-IDS"
mdlURL="http://ob.sr.unh.edu/svn/WSAG/branches/Balazs/snapshots/2009-12-18 NASA-IDS"

all: CMlib_target MFlib_target WBMplus_target

CMlib_target:   CMlib
	svn update CMlib
	make -C CMlib all

MFlib_target:   MFlib
	svn update MFlib
	make -C MFlib all

WBMplus_target: WBMplus
	svn update WBMplus
	make -C WBMplus all

CMlib:
	svn checkout $(libURL)/CMlib

MFlib:
	svn checkout $(libURL)/MFlib

WBMplus:
	svn checkout $(mdlURL)/WBMplus
