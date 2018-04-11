@echo off
if "%1"=="--help" GOTO help
if "%1"=="A" (
	if "%2"=="" GOTO publishAll
	if "%~x2"==".cpp" GOTO publishOne
	GOTO publishAll
)
if "%1"=="G" (
	if "%2"=="" GOTO publishGAll
	if "%~x2"==".cpp" GOTO publishGOne
	GOTO publishGAll
)
if "%1"=="C" GOTO clear
echo Unknown argument. Exiting
GOTO :eof

:publishGAll
echo Publishing all for debugging.
g++ -c -g %2*.cpp
g++ -g *.o
GOTO finish

:publishAll
echo Publishing all.
g++ -c %2*.cpp
g++ *.o
GOTO finish

:publishOne
echo Publishing %2 %3.
IF "%3"=="" (
	g++ -c "%2.cpp"
) ELSE (
	g++ -c "%2.cpp" "%3.cpp"
)
g++ *.o
GOTO finish

:publishGOne
echo Publishing %2 %3 for debugging.
IF "%3"=="" (
	g++ -c -g "%2.cpp"
) ELSE (
	g++ -c -g "%2.cpp" "%3.cpp"
)
g++ -g *.o
GOTO finish

:clear
del *.o a.exe *.pdf *.ps function.* *.dot
GOTO finish

:finish
echo Done!
GOTO :eof

:help
echo builder for c++ projects.
echo usage
echo 	build [option] [directory/file]
echo commands:
echo 	A	builds all if no file is given, otherwise builds file.
echo 	G	builds all for debugging if no file is given, otherwise build file for debugging.
echo 	C	deletes all .o, .exe, .pdf, .ps and function. files
echo files need to be named .cpp to be build
GOTO :eof


