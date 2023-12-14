@echo off
echo Hello, this is a Windows batch file!

@REM for loop
for %%i in (1 2 3 4 5 6 7 8 9 10 11 12 13) do (
    echo sample%%i.pgm
    call sample.exe sample%%i.pgm data\sample%%i_bi.pgm
)
