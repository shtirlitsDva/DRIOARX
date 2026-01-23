# ObjectARX Demand Loading

<overview>
Demand loading allows AutoCAD to automatically load DBX/ARX modules when needed, rather than requiring manual loading via APPLOAD or NETLOAD. This is essential for custom entities - when a drawing containing your custom objects is opened, AutoCAD will automatically load the object enabler (DBX) to display them properly instead of showing proxy objects.
</overview>

<system-variable>
## DEMANDLOAD System Variable

Controls when AutoCAD demand-loads third-party applications.

| Property | Value |
|----------|-------|
| Type | Integer |
| Saved in | Registry |
| Default | 3 |

<values>
| Value | Description |
|-------|-------------|
| 0 | Demand-loading disabled (breaks many commands) |
| 1 | Load on proxy detection only (custom objects in drawing) |
| 2 | Load on command invocation only |
| 3 | Load on both proxy detection AND command invocation (recommended) |
</values>
</system-variable>

<registry-configuration>
## Registry Setup for Demand Loading

<registry-path>
### Registry Location

For ObjectDBX applications (DBX files):
```
HKEY_LOCAL_MACHINE\SOFTWARE\Autodesk\ObjectDBX\R<VERSION>\Applications\<APP_NAME>
```

For ObjectARX applications (ARX files):
```
HKEY_LOCAL_MACHINE\SOFTWARE\Autodesk\AutoCAD\R<VERSION>\ACAD-<ID>:<LANG>\Applications\<APP_NAME>
```

Version mapping:
- AutoCAD 2025 = R25.0
- AutoCAD 2024 = R24.0
- AutoCAD 2023 = R24.0
</registry-path>

<registry-values>
### Required Registry Values

| Value Name | Type | Description |
|------------|------|-------------|
| DESCRIPTION | REG_SZ | Human-readable application name |
| LOADER | REG_SZ | Full path to the DBX/ARX file |
| LOADCTRLS | REG_DWORD | Bit flags controlling when to load |
</registry-values>

<important>
### Critical: APP_NAME Must Match ACRX_DXF_DEFINE_MEMBERS

The `<APP_NAME>` in the registry path must exactly match the application name argument in your `ACRX_DXF_DEFINE_MEMBERS` macro:

```cpp
ACRX_DXF_DEFINE_MEMBERS(
    NSText, AcDbText,
    AcDb::kDHL_CURRENT, AcDb::kMReleaseCurrent,
    AcDbProxyEntity::kNoOperation, NSTEXT,
    NSNORSYNOBJECTSAPP  // <-- THIS is your APP_NAME
    | Product Desc : ...
)
```

Registry key must be: `...\Applications\NSNORSYNOBJECTSAPP`
</important>
</registry-configuration>

<loadctrls-flags>
## LOADCTRLS Flag Values

Defined in `rxdlinkr.h` as `AcadApp::LoadReasons`:

| Flag | Value | Hex | Description |
|------|-------|-----|-------------|
| kOnProxyDetection | 1 | 0x01 | Load when proxy/custom object detected |
| kOnAutoCADStartup | 2 | 0x02 | Load when AutoCAD starts |
| kOnCommandInvocation | 4 | 0x04 | Load when registered command invoked |
| kOnLoadRequest | 8 | 0x08 | Load on explicit acrxLoadModule() request |
| kLoadDisabled | 16 | 0x10 | Demand loading disabled |
| kTransparentlyLoadable | 32 | 0x20 | Can load during transparent command |
| kOnIdleLoad | 64 | 0x40 | Load during idle time |

<common-combinations>
### Common Flag Combinations

| Use Case | Value | Flags Combined |
|----------|-------|----------------|
| DBX Object Enabler | 0x01 | kOnProxyDetection |
| DBX with load request | 0x09 | kOnProxyDetection + kOnLoadRequest |
| ARX with commands | 0x0C | kOnCommandInvocation + kOnLoadRequest |
| Full demand loading | 0x0D | kOnProxyDetection + kOnCommandInvocation + kOnLoadRequest |
</common-combinations>
</loadctrls-flags>

<example-registry-file>
## Example .reg File

For NSNorsynObjects.dbx targeting AutoCAD 2025:

```reg
Windows Registry Editor Version 5.00

[HKEY_LOCAL_MACHINE\SOFTWARE\Autodesk\ObjectDBX\R25.0\Applications\NSNORSYNOBJECTSAPP]
"DESCRIPTION"="Norsyn Custom Objects"
"LOADER"="C:\\Program Files\\Norsyn\\NSNorsynObjects.dbx"
"LOADCTRLS"=dword:00000001
```

<notes>
- Use double backslashes in paths
- LOADCTRLS is DWORD (hexadecimal)
- Run as Administrator to import
</notes>
</example-registry-file>

<programmatic-registration>
## Programmatic Registration

You can register demand loading from code during module initialization:

```cpp
// In acrxEntryPoint.cpp, during kInitAppMsg

#include "rxdlinkr.h"

// Register for demand loading
acrxRegisterApp(
    AcadApp::kOnProxyDetection,  // LoadReasons
    _T("NSNORSYNOBJECTSAPP"),    // Logical name (must match ACRX_DXF_DEFINE_MEMBERS)
    _T("C:\\Path\\To\\NSNorsynObjects.dbx")  // Optional: explicit path
);
```

Or using AcadAppInfo class for more control:

```cpp
AcadAppInfo appInfo;
appInfo.setAppName(_T("NSNORSYNOBJECTSAPP"));
appInfo.setModuleName(_T("C:\\Path\\To\\NSNorsynObjects.dbx"));
appInfo.setAppDesc(_T("Norsyn Custom Objects"));
appInfo.setLoadReason(AcadApp::kOnProxyDetection);
appInfo.writeToRegistry();
```
</programmatic-registration>

<troubleshooting>
## Troubleshooting

<issue name="Objects still show as proxy">
1. Verify DEMANDLOAD system variable is 1 or 3
2. Check registry key name matches ACRX_DXF_DEFINE_MEMBERS app name exactly
3. Verify LOADER path is correct and file exists
4. Check AutoCAD version matches registry path (R25.0 for 2025)
</issue>

<issue name="Registry changes not taking effect">
1. Close all AutoCAD instances
2. Run `regedit` as Administrator to verify entries
3. Try HKEY_CURRENT_USER if HKEY_LOCAL_MACHINE has permission issues
</issue>

<issue name="DBX loads but ARX commands don't work">
If ARX depends on DBX, use delay loading in Visual Studio:
- Linker > Input > Delay Loaded DLLs: add your DBX name
- Load DBX explicitly in ARX initialization with acrxLoadModule()
</issue>
</troubleshooting>

<references>
## References

- [Autodesk: Demand Loading](https://help.autodesk.com/cloudhelp/2022/ENU/OARX-DevGuide/files/GUID-DC378416-7BFD-46A9-9C84-5E4EABACBB92.htm)
- [Autodesk: Demand Loading on Proxy Detection](https://help.autodesk.com/cloudhelp/2017/ENU/OARXMAC-DevGuide/files/GUID-46EBE6D1-1521-4D36-9F9D-EB140FC29CB0.htm)
- [Kean Walmsley: Automatic Loading](https://www.keanw.com/2009/05/creating-demand-loading-entries-automatically-for-your-autocad-application-using-net.html)
- ObjectARX SDK: `inc/rxdlinkr.h` - LoadReasons enum
</references>
