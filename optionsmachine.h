/*
 *	选项处理机
 *	例子，一个类Unix的长短选项 'ls' 命令：
 *		#define OPTM_DEFAULTOUTPUT printf("Unknow command. Please input '--help' or '-h' .\n")
 *		#include <optionsmachine.h>
 *		void defaultfunction() {
 *			system("dir");
 *		}
 *		void help() {
 *			print("It does not have other options.\n");
 *		}
 *		#define OPTNUM 2
 *		int main(int argc, char *argv[]) {
 *			struct optmArgs args = { argc,argv };
 *			OPTM_BUILD_SALOPTIONS("-h", "--help", help)
 *			struct optmSelectingPack pack[OPTNUM] = {
 *				optm_l_help_l,
 *				optm_l_help_s
 *			};
 *			struct optmOptions options = {
 *				defaultfunction,
 *				OPTNUM-1,
 *				pack
 *			};
 *			omachine_optionsmachine(args, options);
 *			return 0;
 *		}
 */
#ifndef OPTIONSMACHINE_H_
#define OPTIONSMACHINE_H_

#include <arg.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef OPTM_DEBUG
//TODO
#endif

// 默认输出，当选项无法被匹配时调用
#ifndef OPTM_DEFAULTOUTPUT
#define OPTM_DEFAULTOUTPUT printf("Unknow command '%s' . Please input '-h'\n", arg)
#endif

// 构建长短命令 参数：短命令，长命令，命令函数
#ifndef OPTM_ENABLE_SAVE_SALOPTIONS
#define OPTM_BUILD_SALOPTIONS(x, y, func)                                           \
	char *_##func##spack_l_eq[1] = {y};                                             \
	struct optmSelectingPack optm_l_##func##_s = {x, func, 1, _##func##spack_l_eq}; \
	char *_##func##spack_s_eq[1] = {x};                                             \
	struct optmSelectingPack optm_l_##func##_l = {y, func, 1, _##func##spack_s_eq};
#else
#ifndef OPTM_SAVE_SALOPTIONS_NUMBER
char *optmSALOptionsSaver[1][32];
#else
char *optmSALOptionsSaver[1][OPTM_SAVE_SALOPTIONS_NUMBER];
#endif
int optmSALOptionsSaverLastIndex = 0;
#define OPTM_BUILD_SALOPTIONS(x, y, func)                                           \
	char *_##func##spack_l_eq[1] = {y};                                             \
	struct optmSelectingPack optm_l_##func##_s = {x, func, 1, _##func##spack_l_eq}; \
	char *_##func##spack_s_eq[1] = {x};                                             \
	struct optmSelectingPack optm_l_##func##_l = {y, func, 1, _##func##spack_s_eq}; \
	optmSALOptionsSaver[optmSALOptionsSaverLastIndex][0] = x;                       \
	optmSALOptionsSaver[optmSALOptionsSaverLastIndex][1] = y;                       \
	optmSALOptionsSaverLastIndex++;
#endif

#ifndef OPTM_DEFAULTCOMMANDCACHE
#define OPTM_DEFAULTCOMMANDCACHE 32
#endif

char *_optmEarlierCommandsList[OPTM_DEFAULTCOMMANDCACHE];
int _optmEarlierCommandsLastIndex = 0;

// 获得上一个输入的命令
// 如果使用 Alias ，则输出配置的最后一个
char *optmGetLastCommand()
{
	return _optmEarlierCommandsList[_optmEarlierCommandsLastIndex - 1];
}
// It may to return: NULL
char *optmGetLastShortCommand()
{
	for (int i = 0; i < _optmEarlierCommandsLastIndex; i++)
	{
		if (_optmEarlierCommandsList[i - 1][0] == '-' && _optmEarlierCommandsList[i - 1][1] != '-')
			return _optmEarlierCommandsList[i - 1];
	}
	return NULL;
}
char *optmGetLastLongCommand()
{
	for (int i = 0; i < _optmEarlierCommandsLastIndex; i++)
	{
		if (_optmEarlierCommandsList[i - 1][0] == '-' && _optmEarlierCommandsList[i - 1][1] == '-' && _optmEarlierCommandsList[i - 1][2] != '-')
			return _optmEarlierCommandsList[i - 1];
	}
	return NULL;
}

// Short: -1 Other: 0 Long: 1 
int optmLastCommandType() {
	if(_optmEarlierCommandsList[_optmEarlierCommandsLastIndex - 1][0] == '-' && _optmEarlierCommandsList[_optmEarlierCommandsLastIndex - 1][1] != '-') return -1;
	if(_optmEarlierCommandsList[_optmEarlierCommandsLastIndex - 1][0] == '-' && _optmEarlierCommandsList[_optmEarlierCommandsLastIndex - 1][1] == '-' && _optmEarlierCommandsList[_optmEarlierCommandsLastIndex - 1][2] != '-') return 1;
	return 0;
}

#ifdef OPTM_ENABLE_SAVE_SALOPTIONS
int* optmSreachSALSaver(char *command) {
	for (int i = 0; i <= optmSALOptionsSaverLastIndex - 1; i++)
	{
		if(!strcmp(command,optmSALOptionsSaver[0][i])) {
			int* temp = malloc(2);
			temp[0]=0;
			temp[1]=optmSALOptionsSaverLastIndex;
			return temp;
		}
	}
	for (int i = 0; i <= optmSALOptionsSaverLastIndex - 1; i++)
	{
		if(!strcmp(command,optmSALOptionsSaver[1][i])) {
			int* temp = malloc(2);
			temp[0]=1;
			temp[1]=optmSALOptionsSaverLastIndex;
			return temp;
		}
	}
	return NULL;
	
}
char **optmGetLastSALOption() {
	for (int i = 0; i < _optmEarlierCommandsLastIndex - 1; i++)
	{
		if (_optmEarlierCommandsList[i][0] == '-') {
			int * temp = optmSreachSALSaver(_optmEarlierCommandsList[i]);
			if(temp == NULL) continue;
			char **tempstr;
			tempstr[0] = optmSALOptionsSaver[0][temp[1]];
			tempstr[1] = optmSALOptionsSaver[1][temp[1]];
			return tempstr;
		}
	}
	return NULL;
}
char **optmGetLastSALOptionForLastOption() {
	if (_optmEarlierCommandsList[_optmEarlierCommandsLastIndex - 1][0] == '-') {
		int * temp = optmSreachSALSaver(_optmEarlierCommandsList[_optmEarlierCommandsLastIndex - 1]);
		if(temp == NULL) return NULL;
		char **tempstr;
		tempstr[0] = optmSALOptionsSaver[0][temp[1]];
			tempstr[1] = optmSALOptionsSaver[1][temp[1]];
			return tempstr;
	}
	return NULL;
}
#endif

// 选项包，一个命令选项参数
struct optmSelectingPack
{
	char *name;					// 选项名， 例如“-h”
	void (*func)(void);			// 命令函数
	int equivalencyCommandNum;	// 等价命令数量，从1计，一般地: NULL
	char **equivalencyCommands; // 等价命令数组，一般地: NULL
};
// 程序参数包，一般是程序抓取的参数列表
struct optmArgs
{
	int argc;
	char **argv;
};
// 参数包，所有可匹配参数的列表
struct optmOptions
{
	void (*defaultFunction)(void);	// 默认函数，当没有任何参数时调用
	int selectingPackNum;			// 选项数。从0计
	struct optmSelectingPack *pack; // 选项包列表
};
void omachine_optionsmachine(struct optmArgs args, struct optmOptions machineOptions)
{
	if (machineOptions.pack == NULL)
	{
		if(machineOptions.defaultFunction == NULL) return;
		machineOptions.defaultFunction();
		return;
	}
	int argc = args.argc;
	char **argv = args.argv;
	int argcIndex = 0;
	for (argcIndex; argcIndex < argc; argcIndex++)
	{
		if (argcIndex == 0)
			continue;
		int flagHasElement = 0;
		for (int element = 0; element < _optmEarlierCommandsLastIndex; element++)
		{
			if (argcIndex == 1)
				continue;
			if (!strcmp(_optmEarlierCommandsList[element], argv[argcIndex]))
			{
				flagHasElement = 1;
				break;
			}
		}
		if (flagHasElement)
			continue;
		int flagHasOption = 0;
		for (int optionIndex = 0; optionIndex <= machineOptions.selectingPackNum; optionIndex++)
		{
			if (!strcmp(argv[argcIndex], machineOptions.pack[optionIndex].name))
			{
				flagHasOption = 1;
				_optmEarlierCommandsList[_optmEarlierCommandsLastIndex] = argv[argcIndex];
				if (machineOptions.pack[optionIndex].equivalencyCommandNum)
					for (int equivalencyCommand = 0; equivalencyCommand < machineOptions.pack[optionIndex].equivalencyCommandNum; equivalencyCommand++)
						_optmEarlierCommandsList[_optmEarlierCommandsLastIndex + machineOptions.pack[optionIndex].equivalencyCommandNum + 1] = machineOptions.pack[optionIndex].equivalencyCommands[equivalencyCommand];
				_optmEarlierCommandsLastIndex += machineOptions.pack[optionIndex].equivalencyCommandNum + 2;
				machineOptions.pack[optionIndex].func();
			}
		}
		if (!flagHasOption)
		{
			char *arg = args.argv[argcIndex];
			OPTM_DEFAULTOUTPUT;
		}
	}
	if (argcIndex == 1 && machineOptions.defaultFunction != NULL)
		machineOptions.defaultFunction();
}

#endif
