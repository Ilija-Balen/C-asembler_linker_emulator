#ifndef SYM_TABLE_H
#define SYM_TABLE_H

#include <iostream>
#include <stdio.h>
#include <list>
#include <string>
#include <regex>
#include <fstream>
#include <vector>
using namespace std;

// elf64word st_name; unsigned char st_info st_other;
//elf64half st_shndx; elf64addr st_value; elf64_Xword st_size
enum Bind{LOCAL, GLOBAL};
enum Type{NOTYPE, SCTN};

struct Sym {
  int RBr;
  int value;
  int size;
  Type type;
  Bind bind;
  int NDX;
  string name;
};

class SymTable{
  std::vector<Sym> tabela;
  public:

  SymTable(){
    Sym* s = new Sym();
    s->name = "0";
    tabela.push_back(*s);
  }
  SymTable(int p){
    
  }
  Sym* getSymbol(string symbolName); //vraca 
  int ifExists(string symbolName);
  void addSymbol(int val, Bind b, int sekcija, string imE, Type tip);
  void setSizeSekcije(string sec_name, int sec_size);
  void setGlobal(string ime);
  
  void upisi(std::ofstream* outputFile);
  void ispis();

  void addLine(int RBr, int val, int s, int tip, int b, int ndx, string ime);
  std::vector<Sym>* getSviSymbolNDX(std::vector<Sym>* pom, int ndx);
  std::vector<Sym>* getSviSymboli(std::vector<Sym>* pom);
  string nazivSekcijeKojojPripada(string naziv_simbola);
  string getSymbolNameByRBR(int RBR);
};
#endif