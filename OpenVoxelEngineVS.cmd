@echo off
setlocal EnableExtensions EnableDelayedExpansion

set "ROOT=%~dp0"
set "ROOT=%ROOT:~0,-1%"
set "VSWHERE=%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe"

if not exist "%VSWHERE%" (
    echo Could not find vswhere.exe.
    echo Expected: "%VSWHERE%"
    pause
    exit /b 1
)

set "VSINSTALL="
set "VS_PRESET="
set "VS_BUILD_NAME="
for /f "usebackq tokens=*" %%I in (`"%VSWHERE%" -latest -version [18.0^,19.0^) -products * -requires Microsoft.Component.MSBuild Microsoft.VisualStudio.Component.CoreEditor -property installationPath`) do (
    set "VSINSTALL=%%I"
    set "VS_PRESET=windows-visual-studio-2026"
    set "VS_BUILD_NAME=windows-visual-studio-2026"
)
if "%VSINSTALL%"=="" for /f "usebackq tokens=*" %%I in (`"%VSWHERE%" -latest -version [17.0^,18.0^) -products * -requires Microsoft.Component.MSBuild Microsoft.VisualStudio.Component.CoreEditor -property installationPath`) do (
    set "VSINSTALL=%%I"
    set "VS_PRESET=windows-visual-studio"
    set "VS_BUILD_NAME=windows-visual-studio-2022"
)

if "%VSINSTALL%"=="" (
    echo Could not find Visual Studio 2022 or 2026 with MSBuild.
    pause
    exit /b 1
)

set "BUILD_DIR=%ROOT%\out\build\%VS_BUILD_NAME%-manifest"
set "SOLUTION_SLNX=%BUILD_DIR%\VoxelEngine.slnx"
set "SOLUTION_SLN=%BUILD_DIR%\VoxelEngine.sln"
set "SOLUTION="

set "DEVENV=%VSINSTALL%\Common7\IDE\devenv.exe"
set "CMAKE=%VSINSTALL%\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe"

if not exist "%DEVENV%" (
    echo Could not find devenv.exe.
    echo Expected: "%DEVENV%"
    pause
    exit /b 1
)

if not defined VCPKG_ROOT if defined VCPKG_INSTALLATION_ROOT set "VCPKG_ROOT=%VCPKG_INSTALLATION_ROOT%"
if not defined VCPKG_ROOT if exist "%USERPROFILE%\Developer\vcpkg\scripts\buildsystems\vcpkg.cmake" set "VCPKG_ROOT=%USERPROFILE%\Developer\vcpkg"
if not defined VCPKG_ROOT if exist "C:\vcpkg\scripts\buildsystems\vcpkg.cmake" set "VCPKG_ROOT=C:\vcpkg"
if not defined VCPKG_ROOT (
    echo Could not find vcpkg. Set VCPKG_ROOT to the vcpkg checkout.
    pause
    exit /b 1
)

if exist "%SOLUTION_SLNX%" set "SOLUTION=%SOLUTION_SLNX%"
if exist "%SOLUTION_SLN%" set "SOLUTION=%SOLUTION_SLN%"

if /I "%~1"=="--print-selection" (
    echo Visual Studio: "%VSINSTALL%"
    echo CMake preset: "%VS_PRESET%"
    echo Build directory: "%BUILD_DIR%"
    if not "!SOLUTION!"=="" echo Existing solution: "!SOLUTION!"
    exit /b 0
)

if "%SOLUTION%"=="" (
    if not exist "%CMAKE%" (
        echo Could not find Visual Studio CMake.
        echo Expected: "%CMAKE%"
        pause
        exit /b 1
    )

    echo Configuring VoxelEngine Visual Studio solution...
    pushd "%ROOT%" >nul
    "%CMAKE%" --preset "%VS_PRESET%"
    set "CMAKE_RESULT=!ERRORLEVEL!"
    popd >nul

    if not "!CMAKE_RESULT!"=="0" (
        echo CMake configure failed.
        pause
        exit /b !CMAKE_RESULT!
    )
)

if exist "%SOLUTION_SLNX%" set "SOLUTION=%SOLUTION_SLNX%"
if exist "%SOLUTION_SLN%" set "SOLUTION=%SOLUTION_SLN%"

if "%SOLUTION%"=="" (
    echo Solution was not generated:
    echo "%SOLUTION_SLN%"
    pause
    exit /b 1
)

echo Opening "%SOLUTION%"
start "" "%DEVENV%" "%SOLUTION%"
exit /b 0
