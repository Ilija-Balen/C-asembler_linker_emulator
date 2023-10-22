#ifndef LINKER_H
#define LINKER_H

#include <iostream>
#include <stdio.h>
#include <list>
#include <string>
#include <regex>
#include <fstream>
#include <vector>
#include <sstream>
#include <map>
#include "RelTable.hpp"
#include "SectionTable.hpp"
#include "SymTable.hpp"
using namespace std;

struct Placeholder{
  string ime_sekcije;
  string hex_adresa;
};

struct Podatak{
  string name;
  int input_number;
  int size;
};

struct NizPodataka{
  std::vector<Podatak *> niz;
};

struct allRelT{
  string fileName;
  std::list<RelTable*> table;
};

struct allSecT{
  string fileName;
   SectionTable* sectiontable = new SectionTable();
};

struct allSymT{
  string fileName;
  SymTable* symtable = new SymTable(1);
};

class Linker
{
  std::vector<allRelT* > allRelTables;
  std::vector<allSecT* > allSecTables;//trebaju nam vrednosti novih mesta gde se nalaze tabele sekcija
  std::vector<allSymT* > allSymTables;//MORAM DA CUVAM SVE SYM TABLES DA BIH ODREDIO VREDNOST SVIH (NEDEFINISANIH) SIMBOLA
  
  std::vector<Podatak* > undefinedSymbolNames;
  std::vector<string> istoimeneSekcije;
  public:  
  std::vector<string > datoteke;
  private:
  std::list<RelTable*> finalRelTable;
  SectionTable* finalSectionTable = new SectionTable();
  SymTable* finalSymTable = new SymTable();

  std::map<long long, string> memorija;
  std::map<long long, NizPodataka*> mapirano;

  long long pocetnaAdresa =  0;
  public :
  void mapiranje(std::vector<Placeholder>* placeholder);
  void odredjivanje();
  void razresavanje();
  void upis(string ime_datoteke);

  void ucitaj(std::vector<string> vektorDatoteka);
  int mapiraj_podatke_vezane_za_sekciju(long long adresa, int ndx, int rbr_input, string sec_name);
  long long mapiraj_Mapped(string sec_name, int rbr_input);
  long long mapiraj_InPlaceholder(string sec_name, int rbr_input, std::vector<Placeholder>* placeholder);
  void dodajUndSibole(int rbr_input);
  long long pronadji_poslednji_mapirani(int rbr_input);
  void proveri_preklapanje();
  void getAllSHDRNamesNoRepetition(std::vector<string>* all_sec_header_names);
  void napuni_memoriju();
  long long  pronadji_u_mapiranom(string name, int rbr_input);
  void napuni_sekcije_sa_ponavljanjem();
  void dopuniDoDeljivosti();

  bool isSectionMapped(string sec_name);
  bool isInPlaceholder(string sec_name,std::vector<Placeholder>* placeholder);
};

#endif