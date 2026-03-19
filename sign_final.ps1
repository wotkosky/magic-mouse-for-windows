# Final signing and install — targeted fix
$DriverDir = "C:\Users\dougl\Projects\LiteMagicMouseDriver\src\AmtPtpHidFilter\build\AmtPtpHidFilter\x64\Release"
$sysFile = "$DriverDir\AmtPtpHidFilter.sys"
$catFile = "$DriverDir\amtptphidfilter.cat"

Write-Host "=== LiteMagicMouse — Fix Signing ===" -ForegroundColor Cyan

# List all code signing certs
Write-Host ""
Write-Host "Code signing certs in LocalMachine\My:" -ForegroundColor Yellow
$certs = Get-ChildItem Cert:\LocalMachine\My -CodeSigningCert
$certs | ForEach-Object { Write-Host "  $($_.Thumbprint) — $($_.Subject) — Expires: $($_.NotAfter)" }

if ($certs.Count -eq 0) {
    Write-Host "  No code signing certs found! Creating one..." -ForegroundColor Red
    $cert = New-SelfSignedCertificate -Type CodeSigningCert -Subject "CN=LiteMagicMouse Test" `
        -CertStoreLocation "Cert:\LocalMachine\My" -NotAfter (Get-Date).AddYears(10) -HashAlgorithm SHA256
    $cerFile = "C:\Users\dougl\Projects\LiteMagicMouseDriver\LiteMagicMouse.cer"
    Export-Certificate -Cert $cert -FilePath $cerFile -Force | Out-Null
    Import-Certificate -FilePath $cerFile -CertStoreLocation "Cert:\LocalMachine\Root" | Out-Null
    Import-Certificate -FilePath $cerFile -CertStoreLocation "Cert:\LocalMachine\TrustedPublisher" | Out-Null
    $certs = @($cert)
}

$cert = $certs[0]
Write-Host ""
Write-Host "Using cert: $($cert.Thumbprint)" -ForegroundColor Green

# Sign .sys with Set-AuthenticodeSignature (PowerShell native, no signtool needed)
Write-Host ""
Write-Host "Signing .sys..." -ForegroundColor Yellow
$result = Set-AuthenticodeSignature -FilePath $sysFile -Certificate $cert -HashAlgorithm SHA256
Write-Host "  Status: $($result.Status)" -ForegroundColor $(if($result.Status -eq "Valid"){"Green"}else{"Red"})
Write-Host "  Message: $($result.StatusMessage)"

if ($result.Status -ne "Valid") {
    Write-Host ""
    Write-Host "Trying with -Force..." -ForegroundColor Yellow
    # Some .sys files need force
    $result2 = Set-AuthenticodeSignature -FilePath $sysFile -Certificate $cert -HashAlgorithm SHA256 -Force
    Write-Host "  Status: $($result2.Status)"

    if ($result2.Status -ne "Valid") {
        Write-Host ""
        Write-Host "Trying signtool directly..." -ForegroundColor Yellow
        $signTool = (Get-ChildItem "C:\Program Files (x86)\Windows Kits\10\bin\*\x64\signtool.exe" 2>$null | Sort-Object FullName -Descending | Select-Object -First 1).FullName
        if ($signTool) {
            Write-Host "  Using: $signTool"
            # Use /s My to search in the My store
            & $signTool sign /v /fd SHA256 /s My /n "LiteMagicMouse Test" $sysFile 2>&1
            Write-Host ""
            & $signTool verify /pa /v $sysFile 2>&1
        }
    }
}

# Verify final state
Write-Host ""
Write-Host "=== Verification ===" -ForegroundColor Cyan
$verify = Get-AuthenticodeSignature $sysFile
Write-Host ".sys signed: $($verify.Status)"
if ($verify.SignerCertificate) {
    Write-Host "  Signer: $($verify.SignerCertificate.Subject)"
}

if ($verify.Status -eq "Valid" -or $verify.Status -eq "UnknownError") {
    # Reinstall
    Write-Host ""
    Write-Host "=== Reinstalling driver ===" -ForegroundColor Yellow

    # Generate new catalog
    $inf2cat = (Get-ChildItem "C:\Program Files (x86)\Windows Kits\10\bin\*\x86\Inf2Cat.exe" 2>$null | Sort-Object FullName -Descending | Select-Object -First 1).FullName
    if ($inf2cat) {
        & $inf2cat /driver:"$DriverDir" /os:10_X64 /uselocaltime 2>&1
        if (Test-Path $catFile) {
            Set-AuthenticodeSignature -FilePath $catFile -Certificate $cert -HashAlgorithm SHA256 | Out-Null
        }
    }

    # Remove old
    $drvList = pnputil /enum-drivers 2>&1 | Out-String
    if ($drvList -match "(oem\d+\.inf)[\s\S]*?AmtPtpHidFilter") {
        pnputil /delete-driver $Matches[1] /force /uninstall 2>&1
    }

    # Install
    pnputil /add-driver "$DriverDir\AmtPtpHidFilter.inf" /install 2>&1

    # Restart device
    Write-Host ""
    Write-Host "Restarting Magic Mouse Col01..." -ForegroundColor Yellow
    $devId = "HID\{00001124-0000-1000-8000-00805F9B34FB}_VID&0001004C_PID&0269&COL01\8&1EB157E&3&0000"
    $dev = Get-PnpDevice -InstanceId $devId -ErrorAction SilentlyContinue
    if ($dev) {
        Disable-PnpDevice -InstanceId $devId -Confirm:$false -ErrorAction SilentlyContinue 2>&1
        Start-Sleep 2
        Enable-PnpDevice -InstanceId $devId -Confirm:$false -ErrorAction SilentlyContinue 2>&1
        Start-Sleep 3
    }

    # Check result
    Write-Host ""
    $devProps = Get-PnpDeviceProperty -InstanceId $devId -KeyName 'DEVPKEY_Device_DriverInfPath','DEVPKEY_Device_Service','DEVPKEY_Device_ProblemCode' -ErrorAction SilentlyContinue
    $devProps | Format-Table KeyName, Data -AutoSize

    $svc = Get-WmiObject Win32_SystemDriver | Where-Object { $_.Name -eq "AmtPtpHidFilter" }
    if ($svc) {
        Write-Host "DRIVER LOADED: $($svc.State)" -ForegroundColor Green
    } else {
        Write-Host "Driver not loaded yet." -ForegroundColor Yellow
    }
}

Read-Host "Pressione Enter"
