/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%  Copyright 2014-2016 ImageMagick Studio LLC, a non-profit organization      %
%  dedicated to making software imaging solutions freely available.           %
%                                                                             %
%  You may not use this file except in compliance with the License.  You may  %
%  obtain a copy of the License at                                            %
%                                                                             %
%    http://www.imagemagick.org/script/license.php                            %
%                                                                             %
%  Unless required by applicable law or agreed to in writing, software        %
%  distributed under the License is distributed on an "AS IS" BASIS,          %
%  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.   %
%  See the License for the specific language governing permissions and        %
%  limitations under the License.                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
*/
#include "stdafx.h"
#include "Project.h"
#include "Shared.h"

wstring Project::configDefine() const
{
  return(_configDefine);
}

vector<wstring> &Project::defines()
{
  return(_defines);
}

vector<wstring> &Project::definesDll()
{
  return(_definesDll);
}

vector<wstring> &Project::dependencies()
{
  return(_dependencies);
}

vector<wstring> &Project::directories()
{
  return(_directories);
}

vector<wstring> &Project::excludes()
{
  return(_excludes);
}

vector<wstring> &Project::excludesX64()
{
  return(_excludesX64);
}

vector<wstring> &Project::excludesX86()
{
  return(_excludesX86);
}

vector<ProjectFile*> &Project::files()
{
  return(_files);
}

vector<wstring> &Project::includes()
{
  return(_includes);
}

bool Project::isConsole() const
{
  if (!isExe())
    return(false);
  return(_type != APPTYPE);
}

bool Project::isCom() const
{
  return(_type == COMTYPE);
}

bool Project::isDll() const
{
  return((_type == DLLTYPE) || (_type == DLLMODULETYPE) || (_type == COMTYPE));
}

bool Project::isExe() const
{
  return((_type == EXETYPE) || (_type == EXEMODULETYPE) || (_type == APPTYPE));
}

bool Project::isLib() const
{
  return((_type == STATICTYPE));
}

bool Project::isModule() const
{
  return((_type == DLLMODULETYPE) || (_type == EXEMODULETYPE));
}

vector<wstring> &Project::libraries()
{
  return(_libraries);
}

wstring Project::moduleDefinitionFile() const
{
  return(_moduleDefinitionFile);
}

wstring Project::name() const
{
  return(_name);
}

vector<wstring> &Project::references()
{
  return(_references);
}

bool Project::useUnicode() const
{
  return(_useUnicode);
}

int Project::warningLevel() const
{
  return(_warningLevel);
}

Project* Project::create(wstring name)
{
  wifstream
    config;

  config.open(L"..\\" + name + L"\\Config.txt");
  if (!config)
    return((Project *) NULL);

  Project* project = new Project(name);
  project->loadConfig(config);

  config.close();
  return(project);
}

bool Project::loadFiles(const ConfigureWizard &wizard)
{
  ProjectFile
    *projectFile;

  _files.clear();

  if (!_isEnabled)
    return(false);

  if (_hasIncompatibleLicense && !wizard.includeIncompatibleLicense())
    return(false);

  if (_isOptional && !wizard.includeOptional())
    return(false);

  switch(_type)
  {
    case COMTYPE:
    {
      projectFile=new ProjectFile(&wizard,this,L"COM",_name);
      _files.push_back(projectFile);
      break;
    }
    case DLLMODULETYPE:
    {
      if (wizard.solutionType() == DYNAMIC_MT)
        loadModules(wizard);
      else
      {
        projectFile=new ProjectFile(&wizard,this,L"CORE",_name);
        _files.push_back(projectFile);
      }
      break;
    }
    case DLLTYPE:
    {
      projectFile=new ProjectFile(&wizard,this,L"CORE",_name);
      _files.push_back(projectFile);
      break;
    }
    case APPTYPE:
    case EXETYPE:
    {
      projectFile=new ProjectFile(&wizard,this,L"UTIL",_name);
      _files.push_back(projectFile);
      break;
    }
    case EXEMODULETYPE:
    {
      loadModules(wizard);
      break;
    }
    case STATICTYPE:
    {
      projectFile=new ProjectFile(&wizard,this,L"CORE",_name);
      _files.push_back(projectFile);
      break;
    }
  }

  return(true);
}

Project::Project(wstring name)
{
  _name=name;

  _hasIncompatibleLicense=false;
  _isEnabled=true;
  _isOptional=false;
  _type=UNDEFINEDTYPE;
  _useUnicode=false;
  _warningLevel=0;
}

void Project::addLines(wifstream &config,wstring &value)
{
  wstring
    line;

  while (!config.eof())
  {
    getline(config,line);
    if (trim(line).empty())
      return;

    value+=line+L"\n";
  }
}

void Project::addLines(wifstream &config,vector<wstring> &container)
{
  wstring
    line;

  while (!config.eof())
  {
    line=readLine(config);
    if (line.empty())
      return;

    container.push_back(line);
  }
}

void Project::loadConfig(wifstream &config)
{
  wstring
    line;

  while (!config.eof())
  {
    line=readLine(config);
    if (line == L"[APP]")
      _type=APPTYPE;
    else if (line == L"[COM]")
      _type=COMTYPE;
    else if (line == L"[CONFIG_DEFINE]")
      addLines(config,_configDefine);
    else if (line == L"[DEFINES_DLL]")
      addLines(config,_definesDll);
    else if (line == L"[DEFINES]")
      addLines(config,_defines);
    else if (line == L"[DEPENDENCIES]")
      addLines(config,_dependencies);
    else if (line == L"[DIRECTORIES]")
      addLines(config,_directories);
    else if (line == L"[DISABLED]")
      _isEnabled=false;
    else if (line == L"[DLL]")
      _type=DLLTYPE;
    else if (line == L"[DLLMODULE]")
      _type=DLLMODULETYPE;
    else if (line == L"[EXE]")
      _type=EXETYPE;
    else if (line == L"[EXEMODULE]")
      _type=EXEMODULETYPE;
    else if (line == L"[EXCLUDES]")
      addLines(config,_excludes);
    else if (line == L"[EXCLUDES_X86]")
      addLines(config,_excludesX86);
    else if (line == L"[EXCLUDES_X64]")
      addLines(config,_excludesX64);
    else if (line == L"[INCLUDES]")
      addLines(config,_includes);
    else if (line == L"[INCOMPATIBLE_LICENSE]")
      _hasIncompatibleLicense=true;
    else if (line == L"[STATIC]")
      _type=STATICTYPE;
    else if (line == L"[LIBRARIES]")
      addLines(config,_libraries);
    else if (line == L"[MODULE_DEFINITION_FILE]")
      _moduleDefinitionFile=readLine(config);
    else if (line == L"[MODULE_PREFIX]")
      _modulePrefix=readLine(config);
    else if (line == L"[OPTIONAL]")
      _isOptional=true;
    else if (line == L"[REFERENCES]")
      addLines(config,_references);
    else if (line == L"[UNICODE]")
      _useUnicode=true;
    else if (line == L"[WARNING_LEVEL]")
      _warningLevel=stoi(readLine(config));
  }
}

void Project::loadModules(const ConfigureWizard &wizard)
{
  HANDLE
    fileHandle;

  ProjectFile
    *projectFile;

  wstring
    name;

  WIN32_FIND_DATA
    data;

  foreach (wstring,dir,_directories)
  {
    fileHandle=FindFirstFile((*dir + L"\\*.*").c_str(),&data);
    do
    {
      if (fileHandle == INVALID_HANDLE_VALUE)
        return;

      if ((data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY)
        continue;

      if (contains(_excludes,data.cFileName))
        continue;

      name=data.cFileName;

      if (!endsWith(name,L".c") && !endsWith(name,L".cpp"))
        continue;

      name=name.substr(0, name.find_last_of(L"."));
      projectFile=new ProjectFile(&wizard,this,_modulePrefix,name);
      _files.push_back(projectFile);

    } while (FindNextFile(fileHandle,&data));
  }
}