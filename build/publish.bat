@echo off
if "%1" == "--help" GOTO help
if "%1"=="L" GOTO latex
if "%2"=="L" GOTO latex
if "%1"=="D" GOTO dot
if "%2"=="D" GOTO dot

if exist "function.tex"	(
	pdflatex function.tex
	start function.pdf
)
if exist "tree.dot" (
	dot -Tps2 tree.dot -o tree.ps
	ps2pdf tree.ps
	del tree.ps
	start tree.pdf
)
GOTO :eof

:latex
pdflatex function.tex
start function.pdf
if "%2"=="D" GOTO dot
GOTO :eof

:dot
dot -Tps2 tree.dot -o tree.ps
ps2pdf tree.ps
del tree.ps
start tree.pdf
if "%2"=="L" GOTO latex
GOTO :eof

:help
echo published for differentiator.
echo usage
echo 	publish [option1] [option2]
echo 	publish
echo commands:
echo 	L	publishes latex file
echo 	D	publishes dot file
echo 	options published both files.
echo files need to be named function.tex and tree.dot
GOTO :eof