==============================
Custom Themes for Graffik
==============================


You may create custom themes for Graffik through the use of Qt Style Sheets.  Each theme should
exist as a subdirectory inside of the themes/ directory.

A custom theme must have, at a minimum, a theme.ini file, the contents of this file should contain
the following keys:

name : Theme Name
author : Theme Author
source : Theme Source (URL, company name, etc.)

If there is not a valid theme.ini file in the directory, the directory will be ignored.

==============================
Theme QSS Files
==============================

Each themeable item has at least one Qt Style Sheet file per item.  The names of the files shall
be <item>.qss, and all names are case-sensitive.  e.g.: main.qss

The following are the allowed themable items:

> Core Elements

main		The main window
about		The about window
global_opts	The Global Options Dialog
axis_opts	The Axis Options Dialog
net_base	The "Devices" base dialog
net_man		The "Device Manager" sub-window
add_device	The "Add Device" dialog
add_net		The "Add Bus" dialog
delete_device	The "Delete Device" dialog
delete_net	The "Delete Bus" dialog
error		The error dialog
confirm		The confirm dialog

> Film Workflow Elements

film		The Film workflow sub-window
camera_opts	The Camera Control dialog
jog		The jog plugin panel
motionbase	The motion area widget, and its children

> Slim Scripting Workflow Elements

slim		The Slim Scripting sub-window

=============================
OS-Specific Theme Files
=============================

As each OS may have slightly different QSS requirements, you can also specify a theme file
for each object, for each OS that requires specific instructions.  To do so, append the OS
tag to the file name, before the extension.  The following OS tags are supported:

_osx		Apple OSX
_win		Windows
_linux		Linux
_elinux		Embedded Linux

For example: main_osx.qss

=============================
Disk-based Resource Files
=============================

You can include disk-based resource files, such as images, direct from disk.  To do so,
make sure to use the relative path, from the root application directory.  e.g.:

border-image: url(themes/DP-Blue/img/set-but-normal.png);

