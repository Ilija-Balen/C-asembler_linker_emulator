#ifndef ASEMBLER_H
#define ASEMBLER_H

#include <iostream>
#include <stdio.h>
#include <list>
#include <string>
#include <regex>
#include <fstream>
#include "lexer.hpp"
#include "parser.hpp"
#include "SymTable.hpp"
#include "SectionTable.hpp"
#include "RelTable.hpp"
using namespace std;

class Asembler{
  SectionTable* sectiontable = new SectionTable();
  Section* sekcija = nullptr;
  std::list<RelTable*> allRelTables;
  RelTable* currRelTable;
  int currSection = 0;
  int locCounter = 0;
  Lexer *lexer = new Lexer();
  Parser* parser = new Parser();
  SymTable* symtable = new SymTable();
  const long long NO_LITERAL = std::numeric_limits<long long>::min();
  public:

  void asemble(string ulaz, string izlaz);//komentar
  bool prviProlaz(string line);
  bool drugiProlaz(string line);
  void newSection(string arg);
  bool isDecimalLiteral(const std::string & str);
  bool isHexadecimalLiteral(const std::string& str);
  int extractRegNumb(string s);
  string intToHexString(int br);
  void addLiteralTableProvera(long long  key, int val, string ime);
  void ifHexDecaddLitProvera(string arg);
  void popuniLitTable(int oldLocCounter);
  void popuniSadrzajInstr(string opCode, int regA, int regB, int regC, string pomeraj);
  int addLiteralTableSymbol(long long key, string ime);
  int unsignedToSigned12Bit(int unsignedValue);
  int getPostavljenLocLitOrSymb(string arg2);
  void addRelZapisInstr(int lokacija, string simbol);
  void addRelZapisLiteral(int lokacija);
};

#endif