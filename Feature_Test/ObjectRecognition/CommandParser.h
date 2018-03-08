//***********************************************************************
// File Name		: CommandParser.h
// Last Modified By : raorao
// Last Modified On : 03-08-2018
// Description      : 读取命令行数据
//***********************************************************************
#pragma once

class CommandParser
{
	char		**argList;		//point to hold argument list
	int			numArgs;		// no of arguments in the list
public:
	CommandParser(void);
	CommandParser(int, char**);
	~CommandParser(void);
	char* GetParameter(const char *key);
};

