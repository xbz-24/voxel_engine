@echo off
setlocal

set "ROOT=%~dp0"
set "ROOT=%ROOT:~0,-1%"
set "BUILD_DIR=%ROOT%\out\build\windows-visual-studio-manifest"
set "SOLUTION=%BUILD_DIR%\VoxelEngine.slnx"
set "VSWHERE=%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe"

if not exist "%VSWHERE%" (
    echo Could not find vswhere.exe.
    echo Expected: "%VSWHERE%"
    pause
    exit /b 1
)

set "VSINSTALL="
for /f "usebackq tokens=*" %%I in (`"%VSWHERE%" -latest -products * -requires Microsoft.Component.MSBuild -property installationPath`) do (
    set "VSINSTALL=%%I"
)

if "%VSINSTALL%"=="" (
    echo Could not find a Visual Studio installation with MSBuild.
    pause
    exit /b 1
)

set "DEVENV=%VSINSTALL%\Common7\IDE\devenv.exe"
set "CMAKE=%VSINSTALL%\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe"

if not exist "%DEVENV%" (
    echo Could not find devenv.exe.
    echo Expected: "%DEVENV%"
    pause
    exit /b 1
)

if not exist "%SOLUTION%" (
    if not exist "%CMAKE%" (
        echo Could not find Visual Studio CMake.
        echo Expected: "%CMAKE%"
        pause
        exit /b 1
    )

    echo Configuring VoxelEngine Visual Studio solution...
    pushd "%ROOT%" >nul
    "%CMAKE%" --preset windows-visual-studio
    set "CMAKE_RESULT=%ERRORLEVEL%"
    popd >nul

    if not "%CMAKE_RESULT%"=="0" (
        echo CMake configure failed.
        pause
        exit /b %CMAKE_RESULT%
    )
)

if not exist "%SOLUTION%" (
    echo Solution was not generated:
    echo "%SOLUTION%"
    pause
    exit /b 1
)

echo Opening "%SOLUTION%"
start "" "%DEVENV%" "%SOLUTION%"
exit /b 0
