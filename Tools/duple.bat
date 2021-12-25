@echo off
echo This batch file is part of the TFS library
pause
set /p Input= Enter new file name:
copy /b 1kblank.txt %Input%
REM the next line will create a 64k file
FOR /L %%y IN (0, 1, 1023) DO COPY /b %Input%+1kblank.txt %Input%
echo Done!