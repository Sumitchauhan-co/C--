; =====================================================================
; Inno Setup Script for C-- Programming Language
; =====================================================================

[Setup]
AppId={{CMM-LANG-2026-SETUP-07}}
AppName=C-- Programming Language
AppVersion=0.7.0
AppPublisher=Sumit Chauhan
AppPublisherURL=https://github.com/Sumitchauhan-co
AppSupportURL=https://github.com/Sumitchauhan-co
AppUpdatesURL=https://github.com/Sumitchauhan-co
DefaultDirName={autopf}\C--
DefaultGroupName=C-- Language
OutputDir=output
OutputBaseFilename=cmm-setup-v0.7.0
Compression=lzma2/ultra64
SolidCompression=yes
WizardStyle=modern
PrivilegesRequired=admin

[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"

[Tasks]
Name: "addtopath"; Description: "Add C-- to system PATH (recommended for command line usage)"; Flags: checkedonce
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked

[Files]
; Source binary from build/cmm.exe
Source: "build\cmm.exe"; DestDir: "{app}"; Flags: ignoreversion

[Icons]
Name: "{group}\C-- CLI"; Filename: "{cmd}"; Parameters: "/K ""{app}\cmm.exe"" --version"
Name: "{group}\{cm:UninstallProgram,C-- Programming Language}"; Filename: "{uninstallexe}"
Name: "{autodesktop}\C-- CLI"; Filename: "{cmd}"; Parameters: "/K ""{app}\cmm.exe"" --version"; Tasks: desktopicon

[Run]
Description: "Test C-- in Terminal"; Filename: "{cmd}"; Parameters: "/K ""{app}\cmm.exe"""; Flags: postinstall nowait skipifsilent unchecked

; =====================================================================
; Pascal Scripting: Automatically Add/Remove C-- from System PATH
; =====================================================================
[Code]
const
  EnvironmentKey = 'SYSTEM\CurrentControlSet\Control\Session Manager\Environment';

procedure AddToPath(PathToAdd: String);
var
  OldPath: String;
begin
  if RegQueryStringValue(HKEY_LOCAL_MACHINE, EnvironmentKey, 'Path', OldPath) then
  begin
    // Check if path is already present
    if Pos(';' + Uppercase(PathToAdd) + ';', ';' + Uppercase(OldPath) + ';') = 0 then
    begin
      if (Length(OldPath) > 0) and (OldPath[Length(OldPath)] <> ';') then
        OldPath := OldPath + ';';
      RegWriteStringValue(HKEY_LOCAL_MACHINE, EnvironmentKey, 'Path', OldPath + PathToAdd);
    end;
  end;
end;

procedure RemoveFromPath(PathToRemove: String);
var
  OldPath, NewPath: String;
  P: Integer;
begin
  if RegQueryStringValue(HKEY_LOCAL_MACHINE, EnvironmentKey, 'Path', OldPath) then
  begin
    P := Pos(';' + Uppercase(PathToRemove) + ';', ';' + Uppercase(OldPath) + ';');
    if P > 0 then
    begin
      NewPath := OldPath;
      Delete(NewPath, P - 1, Length(PathToRemove) + 1);
      RegWriteStringValue(HKEY_LOCAL_MACHINE, EnvironmentKey, 'Path', NewPath);
    end;
  end;
end;

procedure CurStepChanged(CurStep: TSetupStep);
begin
  if (CurStep = ssPostInstall) and IsTaskSelected('addtopath') then
  begin
    AddToPath(ExpandConstant('{app}'));
  end;
end;

procedure CurUninstallStepChanged(CurUninstallStep: TUninstallStep);
begin
  if CurUninstallStep = usPostUninstall then
  begin
    RemoveFromPath(ExpandConstant('{app}'));
  end;
end;