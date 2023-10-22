#ifndef EMULATOR_H 
#define EMULATOR_H


#include <iostream>
#include <stdio.h>
#include <list>
#include <string>
#include <regex>
#include <fstream>
#include <vector>
#include <sstream>
#include <map>

using namespace std;

class Emulator{
  std::map <long long, string> memorija;
  long long r1, r2, r3, r4, r5, r6, r7, r8, r9, r10, r11, r12, r13, r14, r15;
  long long r0 = 0;
  long long status, handler, cause;
public:
  Emulator(){
    r1 = r2 = r3 = r4 = r5 = r6 =r7 = r8 = r9 = r10 = r11 = r12 = r13 = r14 =0;
    r15 = 0x40000000;
    status = handler = cause = 0;
  }

  void ucitaj_memoriju(string naziv_datoteke);
  void emuliraj();
  long long* odrediRegistar(char bitoviReg);
  int odrediPomeraj(string thirdByte,string fourthByte);
  long long  dohvati4ByteIzMemorije(long long vr);
  void staviUMem(long long vr, long long gpr);
  void pozoviHalt();

  string intToHexString(long long value);
  string dopuniString (string hex);
};

#endif