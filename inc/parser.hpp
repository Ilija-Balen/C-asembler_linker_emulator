#ifndef PARSER_H
#define PARSER_H

#include <iostream>
#include <stdio.h>
#include <list>
#include <string>
#include <regex>
using namespace std;

enum Direktiv{global,exterN,section,word,skip,enD};
enum Inst{halt,Int,iret,call,ret,jmp,beq,bne,bgt,push,pop,xchg,add,sub,mul,diV,noT,anD,oR,xoR,shl,shr,ld,st, csrrd, csrwr};
enum Identifier{labela, direktiva, instrukcija, empty};
enum TipAdresiranja{vrednost, vrednost_mem, reg, mem_reg, mem_reg_pom}; 

struct Parsed{
  string labela;
  Direktiv direktiv;
  Inst inst;
  Identifier id;
  std::list<string> listaArg;
  TipAdresiranja tipadresiranja;
  bool labelaSaJos = false;
};

class Parser{
  

  public :
  Parsed parse(std::list <string> tokens);
};

#endif