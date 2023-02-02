#include <stdio.h>
#include <map>
#include <list>
#include <string>
#include <Parse.h>


/*
[Project]
FileName=Sockets.dev
Name=Sockets
UnitCount=82
Type=2
Ver=1
ObjFiles=
Includes=C:\OpenSSL\include
Libs=C:\OpenSSL\lib\MinGW
PrivateResource=
ResourceIncludes=
MakeIncludes=
Compiler=
CppCompiler=-D_VERSION='"2.1.9"'_@@_-D__CYGWIN___@@__@@_
Linker=../../../../OpenSSL/lib/MinGW/ssleay32.a_@@_../../../../OpenSSL/lib/MinGW/libeay32.a_@@_
IsCpp=1
Icon=
ExeOutput=..\..\lib
ObjectOutput=
OverrideOutput=0
OverrideOutputName=Sockets.a
HostApplication=
Folders="Asynchronous DNS","Basic Sockets","File handling","HTTP Sockets",Internal,"Log help classes",SMTP,Threading,"Timer Events",Utilities,"Webserver framework"
CommandLine=
UseCustomMakefile=0
CustomMakefile=
IncludeVersionInfo=0
SupportXPThemes=0
CompilerSet=0
CompilerSettings=0000000000000000000000

[Unit1]
FileName=..\Base64.cpp
CompileCpp=1
Folder=Utilities
Compile=1
Link=1
Priority=1000
OverrideBuildCmd=0
BuildCmd=

*/
int main(int argc, char *argv[])
{
	std::map<std::string, std::list<std::string> > units;
	std::map<std::string, std::list<std::string> > folder;
	if (argc < 2)
		return -1;
	FILE *fil = fopen(argv[1], "rt");
	if (!fil)
		return -1;
	bool folder_found = false;
	std::string section;
	char slask[1000];
	fgets(slask, 1000, fil);
	while (!feof(fil))
	{
		while (strlen(slask) && (slask[strlen(slask) - 1] == 13 || slask[strlen(slask) - 1] == 10))
			slask[strlen(slask) - 1] = 0;
		if (!*slask) // end of section
		{
			if (!folder_found)
				fprintf(stderr, "No folder in: %s\n", section.c_str());
			section = "";
		}
		else
		if (*slask == '[') // section start
		{
			section = slask;
			folder_found = false;
		}
		else
		if ( !section.empty() )
		{
			units[section].push_back(slask);
			Parse pa(slask, "=");
			std::string key = pa.getword();
			std::string value = pa.getrest();
			if (key == "Folder")
			{
				folder[value].push_back(section);
				folder_found = true;
			}
		}
		//
		fgets(slask, 1000, fil);
	}
	fclose(fil);

	// print 'project'
	{
		std::string unit_name = "[Project]";
		std::list<std::string>& ref = units[unit_name];
		printf("%s\n", unit_name.c_str());
		for (std::list<std::string>::iterator it = ref.begin(); it != ref.end(); it++)
		{
			Parse pa(*it, "=");
			std::string key = pa.getword();
			std::string value = pa.getrest();
			if (key == "UnitCount")
			{
				printf("UnitCount=%d\n", units.size() - 2);
			}
			else
			{
				printf("%s\n", (*it).c_str());
			}
		}
		printf("\n");
	}

	// print 'VersionInfo'
	{
		std::string unit_name = "[VersionInfo]";
		std::list<std::string>& ref = units[unit_name];
		printf("%s\n", unit_name.c_str());
		for (std::list<std::string>::iterator it = ref.begin(); it != ref.end(); it++)
		{
			Parse pa(*it, "=");
			std::string key = pa.getword();
			std::string value = pa.getrest();
			{
				printf("%s\n", (*it).c_str());
			}
		}
		printf("\n");
	}

	// units...
	int nr = 1;
	for (std::map<std::string, std::list<std::string> >::iterator it = folder.begin(); it != folder.end(); it++)
	{
		std::list<std::string>& ref = it -> second;
		for (std::list<std::string>::iterator it = ref.begin(); it != ref.end(); it++)
		{
			std::string unit_name = *it;
			std::list<std::string>& ref = units[unit_name];
			printf("[Unit%d]\n", nr++);
			for (std::list<std::string>::iterator it = ref.begin(); it != ref.end(); it++)
			{
				Parse pa(*it, "=");
				std::string key = pa.getword();
				std::string value = pa.getrest();
				{
					printf("%s\n", (*it).c_str());
				}
			}
			printf("\n");
		}
	}

}


