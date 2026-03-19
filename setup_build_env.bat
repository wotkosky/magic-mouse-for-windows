@echo off
echo ============================================
echo   LiteMagicMouse Driver - Build Environment Setup
echo   Lite Tecnologia
echo ============================================
echo.
echo This will install:
echo   1. Visual Studio 2022 Build Tools (with C++ and WDK support)
echo   2. Windows Driver Kit (WDK) for Windows 10/11
echo.
echo This requires ~8GB of disk space and admin privileges.
echo.
pause

echo.
echo [1/3] Downloading Visual Studio 2022 Build Tools...
powershell -Command "Invoke-WebRequest -Uri 'https://aka.ms/vs/17/release/vs_BuildTools.exe' -OutFile '%TEMP%\vs_BuildTools.exe'"

echo.
echo [2/3] Installing Visual Studio Build Tools with C++ and WDK components...
echo (This will take several minutes...)
"%TEMP%\vs_BuildTools.exe" --quiet --wait --norestart ^
    --add Microsoft.VisualStudio.Workload.VCTools ^
    --add Microsoft.VisualStudio.Component.VC.Tools.x86.x64 ^
    --add Microsoft.VisualStudio.Component.Windows11SDK.22621 ^
    --add Microsoft.VisualStudio.Component.VC.ATL ^
    --add Microsoft.VisualStudio.Component.VC.Runtimes.x86.x64.Spectre ^
    --includeRecommended

echo.
echo [3/3] Downloading and installing Windows Driver Kit...
powershell -Command "Invoke-WebRequest -Uri 'https://go.microsoft.com/fwlink/?linkid=2249371' -OutFile '%TEMP%\wdksetup.exe'"
"%TEMP%\wdksetup.exe" /quiet /norestart

echo.
echo ============================================
echo   Installation complete!
echo.
echo   To enable test signing for driver development:
echo     bcdedit /set testsigning on
echo     (then reboot)
echo.
echo   To build the driver:
echo     Open "Developer Command Prompt for VS 2022"
echo     cd C:\Users\dougl\Projects\LiteMagicMouseDriver
echo     msbuild src\AmtPtpHidFilter\AmtPtpHidFilter.vcxproj /p:Configuration=Release /p:Platform=x64
echo ============================================
pause
