
//<developer>
//    <name>linapex 曹一峰</name>
//    <email>linapex@163.com</email>
//    <wx>superexc</wx>
//    <qqgroup>128148617</qqgroup>
//    <url>https://jsq.ink</url>
//    <role>pku engineer</role>
//    <date>2019-03-16 19:44:16</date>
//</624457046409875456>

#include "Inline/BasicTypes.h"
#include "CLI.h"
#include "WAST/WAST.h"
#include "WASM/WASM.h"

int commandMain(int argc,char** argv)
{
	if(argc != 3)
	{
		std::cerr <<  "Usage: Disassemble in.wasm out.wast" << std::endl;
		return EXIT_FAILURE;
	}
	const char* inputFilename = argv[1];
	const char* outputFilename = argv[2];
	
//加载WASM文件。
	IR::Module module;
	if(!loadBinaryModule(inputFilename,module)) { return EXIT_FAILURE; }

//将模块打印到WAST。
	const std::string wastString = WAST::print(module);
	
//将序列化数据写入输出文件。
	std::ofstream outputStream(outputFilename);
	outputStream.write(wastString.data(),wastString.size());
	outputStream.close();

	return EXIT_SUCCESS;
}

