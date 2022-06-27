#pragma once
#include <windows.h>
#include <fstream>

extern std::fstream LogCOM;
extern long Seq;

#define SEQ LONG __Seq=InterlockedIncrement(&Seq)
#define LOG(x,y) LogCOM<<__Seq<<":"<<(x)<<(y)<<"\n"


extern std::fstream LogHT;
extern long SeqHT;


//#define SEQHT LONG __SeqHT=InterlockedIncrement(&SeqHT)
//#define LOGHT(x,y) LogHT<<__SeqHT<<":"<<(x)<<(y)<<std::endl

#define SEQHT LONG __SeqHT=SeqHT

#define LOGHT(x,y) {\
	std::ofstream fout(".logs\\IHT_log.txt",std::ios::app);\
	fout << InterlockedIncrement(&SeqHT) << ":" << (x) << (y) << std::endl;\
	fout.close();\
}