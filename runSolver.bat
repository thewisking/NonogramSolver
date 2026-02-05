@echo off
setlocal EnableExtensions EnableDelayedExpansion

REM Run from the folder this .bat is in (project root)
cd /d "%~dp0"

set "CXX=g++"
set "CXXFLAGS=-std=c++17 -Wall -Wextra -pedantic -O0 -g"
set "INCLUDES=-Iinclude"
set "LIBS=-lgdi32"
set "OUT=app.exe"
set "MAIN=app/app.cpp"

REM Collect all .cpp under src\ (recursive) + test.cpp
set "SRCS=%MAIN%"
for /r "src" %%F in (*.cpp) do (
  set "SRCS=!SRCS! "%%F""
)

echo Building...
%CXX% %CXXFLAGS% %INCLUDES% %SRCS% -o "%OUT%" %LIBS%
if errorlevel 1 (
  echo.
  echo Build failed.
  exit /b 1
)

echo.
echo Running...
"%OUT%"
