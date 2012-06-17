@echo off
echo Windows Deployment Cleanup Batch
echo Got Dir %1
del /Q %1\*.o
del /Q %1\*.cpp

