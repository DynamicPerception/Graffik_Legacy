Qt SDK Version
--------------

Graffik is known to work with version 4.8.1 of the SDK, older versions have bugs which will cause problems on some OSes

General
-------

* Build from Graffik.pro
* 'Re-build All' when changing contents in the libs/ folder (or your changes won't take)

Compiling on Windows
--------------------

If you get errors during help file compilation on windows, delete your build directory before building again. This is caused by a problem with the exists function of QMake on Windows.  (It just can't seem to detect that a directory already exists, no matter how hard you try.)

Compiling QextSerialPort is a bit of a pain on windows, that is because it uses qwineventnotifier_p.h from the private include directory, which Qt no longer includes in the normal SDK packages.  You need to download the source for Qt 4.8.1 SDK (use the Qt SDK Package Manager to do that), and then copy the file as indicated here: [Unable to compile...](http://qt-project.org/forums/viewthread/13232/#76850) 



