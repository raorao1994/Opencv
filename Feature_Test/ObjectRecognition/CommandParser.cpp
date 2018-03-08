//***********************************************************************
// File Name		: CommandParser.cpp
// Last Modified By : raorao
// Last Modified On : 03-08-2018
// Description      : 读取命令行数据
//***********************************************************************
#include "StdAfx.h"
#include "string.h"
#include "CommandParser.h"

// default constucter
CommandParser::CommandParser(void)
{
	numArgs = 0;
	argList = NULL;
}

CommandParser::~CommandParser(void)
{
}
//constructer to initialize with number of argument and argument list
CommandParser::CommandParser(int _numArgs, char** _argList)
{
	numArgs = _numArgs;
	argList = _argList;
}

//return argument that curresponds to the key
char* CommandParser::GetParameter(const char *key)
{
	for (int currArg = 0; currArg < numArgs; currArg++)
	{
		if (strcmp(key, argList[currArg]) == 0)
			return argList[currArg + 1];
	}

	return NULL;
}