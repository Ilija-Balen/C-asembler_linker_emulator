#ifndef SECTION_TABLE_H
#define SECTION_TABLE_H

#include <iostream>
#include <stdio.h>
#include <list>
#include <string>
#include <regex>
#include <fstream>
#include <vector>
#include <unordered_map>
#include <fstream>
#include <iomanip>
#include <limits>
#include "SymTable.hpp"
using namespace std;

struct Literal{
  string name;
  long long key;

  //value
  long long adress;
};

struct Section{
  int NDX;
  int base;
  int ofset;
  std::list <string> hex_sadrzaj;
  string name;
  std::vector<Literal*> tabela_literala;
  long long getValueLiteral(long long kljuc, string ime){
    for(int i= 0; i< tabela_literala.size(); i++)
      if(tabela_literala[i]->key == kljuc && tabela_literala[i]->name == ime)
        return tabela_literala[i]->adress;
    
    return std::numeric_limits<long long>::min();//ako ne nadje vraca ovo
  }
  void addLiteral(long long kljuc, long long vr, string ime){
    Literal * lit = new Literal();
    lit->key = kljuc; lit->adress = vr; lit->name = ime;
    tabela_literala.push_back(lit);
  }
};

class SectionTable{
  public:
  std::vector<Section*> tabela;
  
  void addSection(Section* nova_sekcija);
  void addDecimal(Section* sec, int decimalValue);
  void addString(Section* sec, string hexString);
  void addString2(Section* sec, long long value);
  void skip(Section* sec,int  decimalValue);
  Section* getSection(string sec_name);
  string dopuniString (string hex);
  void dodajTabeleLiteralaUSadrzaj();
  void setSize(SymTable*symtable);

  void upisi(std::ofstream* outputFile);
  void ispis();

  //ovo koristi linker za punjenje podataka
  void addLine(Section* sec, int ndx, string sec_name, int base, int ofset);
  void addLineHex(Section* sec, string first_4byte, string second_4byte);
  void addLineLiteralTable(Section* sec,long long key, long long value, string arg);
  std::vector<string>* getHeaderNames(std::vector<string>* pom);
  std::vector<int>* getHeaderNDX(std::vector<int>* pom);
};

#endif