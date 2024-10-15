; Script generated by the Inno Setup Script Wizard.
; SEE THE DOCUMENTATION FOR DETAILS ON CREATING INNO SETUP SCRIPT FILES!

#define MyAppName "Tale for Tempest"
#define MyAppVersion "0.1.2"
#define MyAppBuildConfiguration "Release"
#define MyAppPublisher "Minye"
#define MyAppURL "www.myemc.net.cn"
#define MyAppExeName "main.exe"

[Setup]
; NOTE: The value of AppId uniquely identifies this application. Do not use the same AppId value in installers for other applications.
; (To generate a new GUID, click Tools | Generate GUID inside the IDE.)
AppId={{F6C44C73-8902-413F-9970-67CF6F660077}
AppName={#MyAppName}
AppVersion={#MyAppVersion}
;AppVerName={#MyAppName} {#MyAppVersion}
AppPublisher={#MyAppPublisher}
AppPublisherURL={#MyAppURL}
AppSupportURL={#MyAppURL}
AppUpdatesURL={#MyAppURL}
DefaultDirName={autopf}\{#MyAppName}
; "ArchitecturesAllowed=x64compatible" specifies that Setup cannot run
; on anything but x64 and Windows 11 on Arm.
ArchitecturesAllowed=x64compatible
; "ArchitecturesInstallIn64BitMode=x64compatible" requests that the
; install be done in "64-bit mode" on x64 or Windows 11 on Arm,
; meaning it should use the native 64-bit Program Files directory and
; the 64-bit view of the registry.
ArchitecturesInstallIn64BitMode=x64compatible
DisableProgramGroupPage=yes
; Uncomment the following line to run in non administrative install mode (install for current user only.)
;PrivilegesRequired=lowest
OutputDir=.\
OutputBaseFilename={#MyAppName}-{#MyAppVersion}-{#MyAppBuildConfiguration}-setup
SetupIconFile=.\main\resources\application.ico
Compression=lzma
SolidCompression=yes
WizardStyle=modern

[Languages]
; Name: "english"; MessagesFile: "compiler:Default.isl"
Name: "chinesesimplified"; MessagesFile: "compiler:ChineseSimplified.isl"

[Tasks]
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked

[Files]
Source: ".\build\Desktop_Qt_5_15_2_MSVC2019_64bit-release\main\release\main.exe"; DestDir: "{app}"; DestName: "{#MyAppExeName}"; Flags: ignoreversion; Check: IsRelease
; NOTE: Don't use "Flags: ignoreversion" on any shared system files

Source: ".\dist\*"; DestDir: "{app}\"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: ".\tool\python-3.8.10\*"; DestDir: "{app}\"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: ".\tool\scripts\logger.py"; DestDir: "{app}\"; Flags: ignoreversion
Source: ".\tool\scripts\meter.py"; DestDir: "{app}\"; Flags: ignoreversion

[Icons]
Name: "{autoprograms}\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}"
Name: "{autodesktop}\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}"; Tasks: desktopicon

[Run]
Filename: "{app}\{#MyAppExeName}"; Description: "{cm:LaunchProgram,{#StringChange(MyAppName, '&', '&&')}}"; Flags: nowait postinstall skipifsilent

[Code]
function IsProfile(): Boolean;
begin
  Result := (CompareText(ExpandConstant('{#MyAppBuildConfiguration}'), 'Profile') = 0);
end;

function IsRelease(): Boolean;
begin
  Result := (CompareText(ExpandConstant('{#MyAppBuildConfiguration}'), 'Release') = 0);
end;