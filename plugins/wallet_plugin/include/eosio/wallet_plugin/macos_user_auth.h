
//<developer>
//    <name>linapex 曹一峰</name>
//    <email>linapex@163.com</email>
//    <wx>superexc</wx>
//    <qqgroup>128148617</qqgroup>
//    <url>https://jsq.ink</url>
//    <role>pku engineer</role>
//    <date>2019-03-16 19:44:18</date>
//</624457054047703040>

#pragma once

#include <CoreFoundation/CoreFoundation.h>

//请求用户身份验证，并在强制后使用true/false调用回调。**请注意，回调
//将在单独的线程中完成**
extern "C" void macos_user_auth(void(*cb)(int, void*), void* cb_userdata, CFStringRef message);

