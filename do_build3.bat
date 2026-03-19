@echo off
"C:\Program Files\Microsoft Visual Studio\2022\Community\MSBuild\Current\Bin\amd64\MSBuild.exe" "C:\Users\dougl\Projects\LiteMagicMouseDriver\src\AmtPtpHidFilter\AmtPtpHidFilter.vcxproj" /p:Configuration=Release /p:Platform=x64 /p:TreatWarningAsError=false /v:minimal
echo EXIT CODE: %ERRORLEVEL%
