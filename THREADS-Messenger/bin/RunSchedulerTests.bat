
@echo off
 
set "testPrefix=SchedulerTest"

REM Edit this list to change which tests run
set "testNumbers=00 01 02 03 04 05 06 07 08 09 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 31"

for %%a in (%testNumbers%) do (
    %testPrefix%%%a
)
