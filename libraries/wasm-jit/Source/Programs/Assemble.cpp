
//<developer>
//    <name>linapex 曹一峰</name>
//    <email>linapex@163.com</email>
//    <wx>superexc</wx>
//    <qqgroup>128148617</qqgroup>
//    <url>https://jsq.ink</url>
//    <role>pku engineer</role>
//    <date>2019-03-16 19:44:16</date>
//</624457046292434944>

#include "Inline/BasicTypes.h"
#include "CLI.h"
#include "WAST/WAST.h"
#include "WASM/WASM.h"

int commandMain(int argc,char** argv)
{
	if(argc < 3)
	{
		std::cerr << "Usage: eosio-wast2wasm in.wast out.wasm [switches]" << std::endl;
		std::cerr << "  -n|--omit-names\t\tOmits WAST function and local names from the output" << std::endl;
		return EXIT_FAILURE;
	}
	const char* inputFilename = argv[1];
	const char* outputFilename = argv[2];
	bool omitNames = false;
	if(argc > 3)
	{
		for(Iptr argumentIndex = 3;argumentIndex < argc;++argumentIndex)
		{
			if(!strcmp(argv[argumentIndex],"-n") || !strcmp(argv[argumentIndex],"--omit-names"))
			{
				omitNames = true;
			}
			else
			{
				std::cerr << "Unrecognized argument: " << argv[argumentIndex] << std::endl;
				return EXIT_FAILURE;
			}
		}
	}
	
//加载WAST模块。
	IR::Module module;
	if(!loadTextModule(inputFilename,module)) { return EXIT_FAILURE; }

//如果指定了省略名称的命令行开关，则删除名称部分。
	if(omitNames)
	{
		for(auto sectionIt = module.userSections.begin();sectionIt != module.userSections.end();++sectionIt)
		{
			if(sectionIt->name == "name") { module.userSections.erase(sectionIt); break; }
		}
	}

//写入二进制模块。
	if(!saveBinaryModule(outputFilename,module)) { return EXIT_FAILURE; }

	return EXIT_SUCCESS;
}

