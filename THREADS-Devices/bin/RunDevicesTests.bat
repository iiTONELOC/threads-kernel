
@echo off
 
set "testPrefix=DevicesTest"

REM Edit this list to change which tests run
set "testNumbers=00 01 02 03 04 05 06 07 08 09 10 11 12 13"

for %%a in (%testNumbers%) do (
    %testPrefix%%%a
)
