# NorsynObjects Intune Deployment Guide

<overview>
This guide covers company-wide deployment of NorsynObjects (NSText, NSMText entities) via Microsoft Intune. The deployment includes:
- DBX file (object enabler)
- COM wrapper DLL (Properties Palette integration)
- Registry keys (demand loading)
</overview>

<deployment-components>
## Components to Deploy

| Component | Purpose | Target Location |
|-----------|---------|-----------------|
| NSNorsynObjects.dbx | Custom entity definitions | `X:\AutoCAD DRI - 01 Civil 3D\Appload\2025\` |
| comNorsynObjects.dll | COM wrapper for Properties Palette | Same location or `%ProgramFiles%\Norsyn\` |
| Registry keys | Demand loading configuration | HKLM\SOFTWARE\Autodesk\ObjectDBX |
</deployment-components>

<win32-app-packaging>
## Win32 App Packaging

<step name="prepare-files">
### 1. Prepare Installation Files

Create a deployment folder with:
```
NorsynObjects_Deploy\
├── NSNorsynObjects.dbx
├── comNorsynObjects.dll
├── install.ps1
├── uninstall.ps1
└── detection.ps1
```
</step>

<step name="install-script">
### 2. Install Script (install.ps1)

```powershell
# install.ps1 - NorsynObjects Installation
param(
    [string]$InstallPath = "X:\AutoCAD DRI - 01 Civil 3D\Appload\2025"
)

$ErrorActionPreference = "Stop"

# Create install directory if needed
if (!(Test-Path $InstallPath)) {
    New-Item -ItemType Directory -Path $InstallPath -Force | Out-Null
}

# Copy DBX and COM DLL
$ScriptDir = Split-Path -Parent $MyInvocation.MyCommand.Path
Copy-Item "$ScriptDir\NSNorsynObjects.dbx" -Destination $InstallPath -Force
Copy-Item "$ScriptDir\comNorsynObjects.dll" -Destination $InstallPath -Force

# Register COM DLL
$ComDll = Join-Path $InstallPath "comNorsynObjects.dll"
Start-Process -FilePath "regsvr32.exe" -ArgumentList "/s `"$ComDll`"" -Wait -NoNewWindow

# Add demand loading registry keys (AutoCAD 2025 - ObjectDBX R25.0)
$RegPath = "HKLM:\SOFTWARE\Autodesk\ObjectDBX\R25.0\Applications\NSNORSYNOBJECTSAPP"
if (!(Test-Path $RegPath)) {
    New-Item -Path $RegPath -Force | Out-Null
}

$DbxPath = Join-Path $InstallPath "NSNorsynObjects.dbx"
Set-ItemProperty -Path $RegPath -Name "DESCRIPTION" -Value "Norsyn Custom Objects"
Set-ItemProperty -Path $RegPath -Name "LOADER" -Value $DbxPath
Set-ItemProperty -Path $RegPath -Name "LOADCTRLS" -Value 0x0D -Type DWord

Write-Host "NorsynObjects installed successfully"
exit 0
```
</step>

<step name="uninstall-script">
### 3. Uninstall Script (uninstall.ps1)

```powershell
# uninstall.ps1 - NorsynObjects Removal
param(
    [string]$InstallPath = "X:\AutoCAD DRI - 01 Civil 3D\Appload\2025"
)

$ErrorActionPreference = "SilentlyContinue"

# Unregister COM DLL
$ComDll = Join-Path $InstallPath "comNorsynObjects.dll"
if (Test-Path $ComDll) {
    Start-Process -FilePath "regsvr32.exe" -ArgumentList "/s /u `"$ComDll`"" -Wait -NoNewWindow
}

# Remove files
Remove-Item "$InstallPath\NSNorsynObjects.dbx" -Force
Remove-Item "$InstallPath\comNorsynObjects.dll" -Force

# Remove demand loading registry keys
$RegPath = "HKLM:\SOFTWARE\Autodesk\ObjectDBX\R25.0\Applications\NSNORSYNOBJECTSAPP"
if (Test-Path $RegPath) {
    Remove-Item -Path $RegPath -Recurse -Force
}

Write-Host "NorsynObjects uninstalled"
exit 0
```
</step>

<step name="detection-script">
### 4. Detection Script (detection.ps1)

```powershell
# detection.ps1 - Check if NorsynObjects is installed
$InstallPath = "X:\AutoCAD DRI - 01 Civil 3D\Appload\2025"
$DbxPath = Join-Path $InstallPath "NSNorsynObjects.dbx"
$RegPath = "HKLM:\SOFTWARE\Autodesk\ObjectDBX\R25.0\Applications\NSNORSYNOBJECTSAPP"

# Check DBX file exists
if (!(Test-Path $DbxPath)) {
    exit 1
}

# Check registry key exists
if (!(Test-Path $RegPath)) {
    exit 1
}

# Installed
Write-Host "NorsynObjects detected"
exit 0
```
</step>

<step name="create-intunewin">
### 5. Create .intunewin Package

1. Download Microsoft Win32 Content Prep Tool:
   https://github.com/microsoft/Microsoft-Win32-Content-Prep-Tool

2. Run the packaging tool:
```cmd
IntuneWinAppUtil.exe -c "C:\Deploy\NorsynObjects_Deploy" -s "install.ps1" -o "C:\Deploy\Output"
```

3. This creates `install.intunewin` for upload to Intune.
</step>
</win32-app-packaging>

<intune-configuration>
## Intune App Configuration

<app-settings>
### App Information
| Setting | Value |
|---------|-------|
| Name | NorsynObjects for AutoCAD 2025 |
| Description | Custom entity support (NSText, NSMText) with Properties Palette integration |
| Publisher | Norsyn |
| App Version | 1.0.0 |
</app-settings>

<program-settings>
### Program Settings
| Setting | Value |
|---------|-------|
| Install command | `powershell.exe -ExecutionPolicy Bypass -File install.ps1` |
| Uninstall command | `powershell.exe -ExecutionPolicy Bypass -File uninstall.ps1` |
| Install behavior | System |
| Device restart behavior | No specific action |
</program-settings>

<requirements>
### Requirements
| Setting | Value |
|---------|-------|
| OS Architecture | 64-bit |
| Minimum OS | Windows 10 1903 |
| Additional requirements | AutoCAD 2025 installed (optional check) |
</requirements>

<detection-rules>
### Detection Rules
Use custom detection script:
- Script file: `detection.ps1`
- Run script as 32-bit process: No
- Enforce script signature check: No (or sign your scripts)
</detection-rules>
</intune-configuration>

<multi-version-support>
## Supporting Multiple AutoCAD Versions

<version-mapping>
### ObjectDBX Version Mapping
| AutoCAD Version | ObjectDBX Version | Registry Path |
|-----------------|-------------------|---------------|
| 2025 | R25.0 | `ObjectDBX\R25.0` |
| 2024 | R24.0 | `ObjectDBX\R24.0` |
| 2023 | R24.0 | `ObjectDBX\R24.0` |
</version-mapping>

<multi-version-install>
### Multi-Version Install Script

```powershell
# For multiple AutoCAD versions, add registry keys for each:
$Versions = @(
    @{ Version = "R25.0"; Year = "2025" },
    @{ Version = "R24.0"; Year = "2024" }
)

foreach ($Ver in $Versions) {
    $RegPath = "HKLM:\SOFTWARE\Autodesk\ObjectDBX\$($Ver.Version)\Applications\NSNORSYNOBJECTSAPP"
    if (Test-Path "HKLM:\SOFTWARE\Autodesk\ObjectDBX\$($Ver.Version)") {
        if (!(Test-Path $RegPath)) {
            New-Item -Path $RegPath -Force | Out-Null
        }
        Set-ItemProperty -Path $RegPath -Name "DESCRIPTION" -Value "Norsyn Custom Objects"
        Set-ItemProperty -Path $RegPath -Name "LOADER" -Value $DbxPath
        Set-ItemProperty -Path $RegPath -Name "LOADCTRLS" -Value 0x0D -Type DWord
    }
}
```
</multi-version-install>
</multi-version-support>

<troubleshooting>
## Troubleshooting

<issue name="Objects show as proxy">
### Custom objects still show as proxy
1. Verify DEMANDLOAD system variable is 1 or 3 in AutoCAD
2. Check registry: `regedit` > navigate to ObjectDBX path
3. Verify LOADER path points to existing DBX file
4. Close and reopen AutoCAD after installation
</issue>

<issue name="Properties Palette shows wrong name">
### Properties Palette shows "Text" instead of "NSText"
1. Verify comNorsynObjects.dll is registered: `regsvr32 /s comNorsynObjects.dll`
2. Check COM registration in registry under HKCR\CLSID
3. Restart AutoCAD after COM registration
</issue>

<issue name="Intune install fails">
### Intune installation fails
1. Check Intune Management Extension logs: `%ProgramData%\Microsoft\IntuneManagementExtension\Logs`
2. Verify install script runs successfully manually (as SYSTEM)
3. Check network path accessibility (X: drive must be mapped)
</issue>

<issue name="Network path considerations">
### Network Path (X: drive) Considerations
If using network paths:
- Ensure drive mapping exists for SYSTEM account
- Consider using UNC paths instead: `\\server\share\path`
- Or install to local path: `%ProgramFiles%\Norsyn\`
</issue>
</troubleshooting>

<security-considerations>
## Security Considerations

<secureload>
### AutoCAD SECURELOAD
AutoCAD may block loading untrusted DLLs. Options:
1. Set SECURELOAD=0 (not recommended for production)
2. Add install path to TrustedPaths in AutoCAD options
3. Digitally sign your DBX/DLL files (recommended)
</secureload>

<trusted-paths>
### Adding Trusted Paths via Registry
```powershell
# Add trusted path for AutoCAD 2025
$AcadRegPath = "HKCU:\SOFTWARE\Autodesk\AutoCAD\R25.0\ACAD-8001:409\Profiles\<<Unnamed Profile>>\Dialogs\Security"
# Note: Profile path varies - may need to enumerate
```
</trusted-paths>
</security-considerations>

<references>
## References
- [Intune Win32 App Management](https://learn.microsoft.com/en-us/mem/intune/apps/apps-win32-app-management)
- [Win32 Content Prep Tool](https://github.com/microsoft/Microsoft-Win32-Content-Prep-Tool)
- [AutoCAD Demand Loading](../objectarx/demand-loading.md)
- [COM Wrapper Guide](../objectarx/com-wrapper.md)
</references>
