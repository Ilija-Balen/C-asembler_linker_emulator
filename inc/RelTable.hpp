#ifndef REL_TABLE_H
#define REL_TABLE_H
#include <iostream>
#include <stdio.h>
#include <list>
#include <string>
#include <regex>
#include <fstream>
using namespace std;

enum TypeRel{ABS, REL};

struct Rela{
  int r_offset;
  TypeRel r_type;
  int r_addend;
  int s_index;
};

class RelTable{
  public:
  string sectionName;
  std::list<Rela*> ulazi;
  
  void dodajZapis(Rela* zapis);
  void setName(string name);

  void upis(std::ofstream* outputFile);
  void ispis();

  void addLine(int ofset, int type, int addend, int rbr);
};

#endif