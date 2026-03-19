@echo off
echo ============================================
echo   LiteMagicMouse Driver - Install
echo   Lite Tecnologia
echo ============================================
echo.

net session >nul 2>&1
if errorlevel 1 (
    echo [ERRO] Execute como Administrador!
    pause
    exit /b 1
)

set DRIVER_DIR=C:\Users\dougl\Projects\LiteMagicMouseDriver\src\AmtPtpHidFilter\build\AmtPtpHidFilter\x64\Release
set PROJECT_DIR=C:\Users\dougl\Projects\LiteMagicMouseDriver
set SIGNTOOL="C:\Program Files (x86)\Windows Kits\10\bin\10.0.22621.0\x64\signtool.exe"
set INF2CAT="C:\Program Files (x86)\Windows Kits\10\bin\10.0.22621.0\x86\Inf2Cat.exe"
set THUMBPRINT_FILE=%TEMP%\litemm_thumbprint.txt

:: Verificar se test signing esta habilitado
echo [0/5] Verificando test signing...
bcdedit /enum {current} | findstr /i "testsigning" | findstr /i "Yes Sim" >nul 2>&1
if errorlevel 1 (
    echo.
    echo [AVISO] Test signing NAO esta habilitado!
    echo Execute primeiro: enable_testsigning.bat
    echo E REINICIE o PC.
    echo.
    pause
    exit /b 1
)
echo Test signing: OK

echo.
echo [1/5] Criando certificado de teste com PowerShell...

:: Remover certificado antigo se existir
powershell -Command "Get-ChildItem Cert:\LocalMachine\My | Where-Object {$_.Subject -eq 'CN=LiteMagicMouse Test'} | Remove-Item -Force" 2>nul
powershell -Command "Get-ChildItem Cert:\LocalMachine\Root | Where-Object {$_.Subject -eq 'CN=LiteMagicMouse Test'} | Remove-Item -Force" 2>nul
powershell -Command "Get-ChildItem Cert:\LocalMachine\TrustedPublisher | Where-Object {$_.Subject -eq 'CN=LiteMagicMouse Test'} | Remove-Item -Force" 2>nul

:: Criar certificado auto-assinado e salvar thumbprint em arquivo
powershell -Command ^
  "$cert = New-SelfSignedCertificate -Type CodeSigningCert -Subject 'CN=LiteMagicMouse Test' -CertStoreLocation 'Cert:\LocalMachine\My' -NotAfter (Get-Date).AddYears(5);" ^
  "$tp = $cert.Thumbprint;" ^
  "Write-Host \"Thumbprint: $tp\";" ^
  "$tp | Out-File -FilePath '%THUMBPRINT_FILE%' -Encoding ASCII -NoNewline;" ^
  "Export-Certificate -Cert $cert -FilePath '%PROJECT_DIR%\LiteMagicMouse.cer' -Force | Out-Null;" ^
  "Import-Certificate -FilePath '%PROJECT_DIR%\LiteMagicMouse.cer' -CertStoreLocation 'Cert:\LocalMachine\Root' | Out-Null;" ^
  "Import-Certificate -FilePath '%PROJECT_DIR%\LiteMagicMouse.cer' -CertStoreLocation 'Cert:\LocalMachine\TrustedPublisher' | Out-Null;" ^
  "Write-Host 'Certificado criado e instalado com sucesso.'"
if errorlevel 1 (
    echo [ERRO] Falha ao criar certificado!
    pause
    exit /b 1
)

:: Ler thumbprint do arquivo
set /p CERT_THUMB=<%THUMBPRINT_FILE%
echo Usando thumbprint: %CERT_THUMB%

echo.
echo [2/5] Gerando catalogo (.cat) com Inf2Cat...
%INF2CAT% /driver:"%DRIVER_DIR%" /os:10_X64 /uselocaltime
if errorlevel 1 (
    echo [ERRO] Inf2Cat falhou!
    pause
    exit /b 1
)

echo.
echo [3/5] Assinando driver (.sys)...
%SIGNTOOL% sign /fd SHA256 /sha1 %CERT_THUMB% "%DRIVER_DIR%\AmtPtpHidFilter.sys"
if errorlevel 1 (
    echo [AVISO] Falha com sha1, tentando com /a /s Root...
    %SIGNTOOL% sign /a /s Root /n "LiteMagicMouse Test" /fd SHA256 "%DRIVER_DIR%\AmtPtpHidFilter.sys"
    if errorlevel 1 (
        echo [ERRO] Falha ao assinar .sys!
        pause
        exit /b 1
    )
)
echo .sys assinado com sucesso.

echo.
echo [4/5] Assinando catalogo (.cat)...
if exist "%DRIVER_DIR%\AmtPtpHidFilter.cat" (
    %SIGNTOOL% sign /fd SHA256 /sha1 %CERT_THUMB% "%DRIVER_DIR%\AmtPtpHidFilter.cat"
    if errorlevel 1 (
        echo [AVISO] Falha com sha1, tentando com /a /s Root...
        %SIGNTOOL% sign /a /s Root /n "LiteMagicMouse Test" /fd SHA256 "%DRIVER_DIR%\AmtPtpHidFilter.cat"
    )
    echo .cat assinado com sucesso.
) else (
    echo [AVISO] Arquivo .cat nao encontrado, pulando...
)

echo.
echo [5/5] Instalando driver...
pnputil /add-driver "%DRIVER_DIR%\AmtPtpHidFilter.inf" /install
if errorlevel 1 (
    echo.
    echo [INFO] Se o erro persistir, tente:
    echo   1. Verifique se Test Signing esta ativo
    echo   2. Desative Secure Boot na BIOS/UEFI
    echo   3. Reinicie e tente novamente
)

:: Limpar arquivo temporario
del "%THUMBPRINT_FILE%" 2>nul

echo.
echo ============================================
echo   Instalacao concluida!
echo   Pareie o Magic Mouse via Bluetooth.
echo ============================================
pause
