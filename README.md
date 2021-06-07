# verifytapn

VerifyTAPN is a stand-alone verification engine for timed-arc Petri nets, developed for the verification tool TAPAAL. 


```
--------------------------------------------------------------------------------
1. Setup project in Eclipse
--------------------------------------------------------------------------------
This will detail how to setup a project in Eclipse CDT 
(http://www.eclipse.org/cdt/) or Eclipse IDE for C/C++ developers.

Preliminaries:
a. Install gcc/g++ (we use version 4.4.5)
b. Install boost libraries (we used 1.40 but any version that is backward 
   compatible with 1.40 should work)
	- On ubuntu you can install it through the software center
	- Otherwise, there is a guide here: 
	  http://www.boost.org/doc/libs/1_40_0/more/getting_started/unix-variants.html
   On Mac run: brew install automake
c. Download UPPAAL DBM library (we use version 2.0.7)
	- unpack the downloaded file
	- go to the "modules" folder
	- run "./setup.sh"
		1. press enter when it asks for "Build directory"
		2. press enter when it asks for install path
		3. choose g++ as compiler
		4. Choose compilation options: write 2 and press enter
		5. Choose configuration options: write 5 and press enter
	- run "sudo make install"
d. Install google-sparsehash library.
	- On Ubuntu you can install this from the software center, otherwise you have
	  to get it from http://code.google.com/p/google-sparsehash/

1. Create a branch of the code, e.g. "bzr branch lp:verifytapn" (replace with 
   the branch you want to checkout)
2. Open Eclipse and create a new "empty C++ project", choose "linux GCC" as 
   toolchain, uncheck "use default location" and point eclipse to the folder in 
   which you checked out the branch.
3. Press finish
4. Right-click on your new project and choose properties
5. go to "C/C++ Build" -> "Settings"
2. Setup configuration "Debug":
	1. Choose "Debug" in the "Configuration" dropdown list
	2. under "GCC C++ compiler" -> "Directories" add the path to the UPPAAL 
	   DBM library's "include" folder. If you followed the instructions above
	   then you need to add the path "/usr/local/uppaal/include"
		- If boost is not installed in the default folders (/usr/include
		  or /usr/local/include) you must also add a path to the the 
		  folder containing the boost include files (.h files)
		- The same applies to the google-sparsehash header files.
	3. under "GCC C++ linker" -> "Libraries":
		- add a path to the DBM library's "lib" folder (per the instructions
		  above, you need to add the path "/usr/local/uppaal/lib") to the 
		  "Library search path"
		- The same MAY apply to the google-sparsehash code, depending on whether
		  it compiles to a library or its a header-only library.
4. Setup configuration "Release": Same as "Debug".
6. The project should now be compilable in both debug and release versions.

--------------------------------------------------------------------------------
2. Generating Query parser
--------------------------------------------------------------------------------
If you need to change the Query parser, you'll need to modify the flex.ll and
grammar.yy files. Make sure you have flex and bison installed.

To recompile the query parser follow these steps:
1. go to src/Core/QueryParser/
2. run flex -o Generated/lexer.cpp flex.ll
3. run bison -o Generated/parser.cpp grammar.yy
```
