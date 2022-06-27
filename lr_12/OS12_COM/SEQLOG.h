#pragma once
#include <windows.h>
#include <fstream>

extern std::fstream LogCOM;
extern LONG Seq;

#define SEQ LONG __Seq=InterlockedIncrement(&Seq)
#define LOG(x,y) LogCOM<<__Seq<<":"<<(x)<<(y)<<"\n"
