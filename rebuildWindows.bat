@echo off
REM Remove existing build directory
if exist build (
    echo Removing existing build directory...
    rmdir /s /q build
)

REM Create build directory
echo Creating build directory...
mkdir build

REM Enter build directory
cd build

REM Record start time
for /f "tokens=1-4 delims=:.," %%a in ("%TIME%") do (
    set startHour=%%a
    set startMin=%%b
    set startSec=%%c
    set startCentisec=%%d
)

REM Run CMake with Ninja generator
echo Running CMake...
cmake -G Ninja ..

REM Build with Ninja
echo Building project...
ninja

REM Record end time
for /f "tokens=1-4 delims=:.," %%a in ("%TIME%") do (
    set endHour=%%a
    set endMin=%%b
    set endSec=%%c
    set endCentisec=%%d
)

REM Calculate elapsed time in seconds (approximate)
set /a startTotal=(%startHour%*3600 + %startMin%*60 + %startSec%)
set /a endTotal=(%endHour%*3600 + %endMin%*60 + %endSec%)
set /a elapsed=endTotal-startTotal
if %elapsed% LSS 0 set /a elapsed+=86400

echo Build and project generation took %elapsed% seconds.

REM Run the executable
dsaViz.exe

echo Done!
pause
