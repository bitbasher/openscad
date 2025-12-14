# design intent - resources inventory for OpenSCAD

OpenSCAD has an existing feature for inserting a template, which is a snippet of text generally intended, to be code in an .scad script.

The QScintilla editor in OpenSCAD reponds to a right click with a small menu of options one of which is "insert template". The user selects a template from a popup list and the text is insert at the current cursor position. There may be a position marker in the text of the template, "^$^" i believe, that the editor's insertion point should be moved to once the text of the template is pasted in. This is a convience for the user who is thus shown were they may wish to start filling expected values into the template

templates are json files that may have only one template per file and only one text string per template. They are read from the "templates" folder in the installation and from a folder in the user's space, which is a per platform location.

another existing feature is that dropping certain types of file into the open app will cause a new editor session to open with the code for the expression, function call, or module that would take the filename as an argument. So a .png file dropped in becomes a call to the surface module to create a hight map surface, a json file creates an assignment expression where the LHS is a variable to take the object returned from import()

what is desired is a merger of this file insertion behaviour with the planned introduction of the snippet facility of Visual Studio Code ported to C++ and Qt.

the components of the solution are

1. knowledge of the platform and system
2. the location of the app's installation folder
3. knowledge of the file types that OpenSCAD can use, import and export
4. knowlege of the resources that openscad uses and needs
5. a facilty for reading and writing json files used for configuration, settings and resources
6. knowledge of the syntax of the modules and functions that deal with files (ref #3)
7. the concept of "resource inventory" exists but is underdefined
8. method of intigration with the UI and the QScintilla editor
9.

# Platform

The app is built as a complete, statically linked app for a particular platform. There is no dynamic linking. At time of writing there are build targets for Windows using Msys2 Mingw64 OR Msys2 UCRT, Mac OS X, and Linux (which is taken to mean Posix ... mostly).

The GUI is written in C++ using Qt and CMAKE (not qmake). There is some use of Boost library so if any of its features are needed it is already in the app, but not in the GUI.

Thus the running app "knows" which platform it is runnng on. Given the platform, it knows the likely folders for the installation and the users home folder. These locations give us the resource folder locations for the installation and user tiers. In the context of this project we need a way to categorize resource locations so we say that there are three tiers, installation, machine, and user

installation - the installation directory has sub-folders for the resources needed by the running app.

user - there are no resources installed in the user space by the app, but there are locations in the user's space where resources will be discovered and inventoried by the app as it starts up. The most typical location across the three platfomes is a folder "Documents/OpenSCAD", but specifically for Windows it could be ~/AppData/Local/OpenSCAD.

This project will add a third tier that we will call machine tier, for resources that should be common to all users of the machine. The common location for this in Mac OS and Posix is a folder "openscad" in /usr/share or /usr/local/share. These folders can also exist in a Windows file system, but the more usual place for app reasources outside of user space is C:/ProgramData/OpenSCAD

The list and nature of these resources is captured in the resource inventory project, but our main interests are the inventories of openSCAD example and test scripts, json template (aka snippets) and color scheme files,
