#include "../inc/SymTable.hpp"

int SymTable::ifExists(string symbolName){
  for(int i = 0; i<tabela.size(); i++){
    if(tabela[i].name == symbolName) return i;
  }
  return -1;
}

void SymTable::addSymbol(int val, Bind b, int sekcija, string imE, Type tip){
  Sym* s = new Sym();
  s->name = imE;
  s->bind = b;
  s->NDX = sekcija;
  s->RBr = tabela.size();
  s->type = tip;
  s->value = val;
  tabela.push_back(*s); 
}

void SymTable::setSizeSekcije(string sec_name, int sec_size){
  for(int i = 0; i< tabela.size(); i++){
    if(tabela[i].name == sec_name){
      tabela[i].size = sec_size;
      return;
    }
  }
}


void SymTable::setGlobal(string ime){
  for(int i = 0; i< tabela.size(); i++){
    if(tabela[i].name == ime){
      tabela[i].bind = GLOBAL;
      return;
    }
  }
}

Sym* SymTable::getSymbol(string symbolName){
  
  for(int i = 0; i< tabela.size(); i++){
    if(tabela[i].name == symbolName) return &tabela[i];
  }
  return nullptr;
}

void SymTable::ispis(){
  cout << "SYMTABLE \t type : 0 - NOTYP 1 - SCTN \tbind : 0- L 1 - G \n"
       << "RBr\t" << "value\t" <<"size\t"
       << "type\t" <<  "bind\t" <<"NDX\t"
       << "name" << std::endl;
  for (const Sym& sym : tabela) {
      cout<< sym.RBr << "\t "
       <<  sym.value << "\t "
       <<  sym.size << "\t "
       << static_cast<int>(sym.type) << "\t "
       <<  static_cast<int>(sym.bind) << "\t "
       << sym.NDX << "\t "
       << sym.name
       << "\n  ";
    
    }
    cout << "\n}";
}

void SymTable::upisi(std::ofstream* outputFile){

  (*outputFile) << "SYMTABLE\n";
  for(int i = 0; i< tabela.size(); i++){
    (*outputFile) << tabela[i].RBr<< " " << tabela[i].value<< " " << tabela[i].size<< " "
    << static_cast<int>(tabela[i].type)<< " " << static_cast<int>(tabela[i].bind)<< " "
     << tabela[i].NDX<< " " << tabela[i].name<< "\n";
  }
}

void SymTable::addLine(int RBr, int val, int s, int tip, int b, int ndx, string ime){
  Sym* sym = new Sym();
  sym->RBr = RBr;
  sym->value = val;
  sym->size = s;

  if(tip == 0)sym->type = NOTYPE;
  else sym->type = SCTN;

  if(b == 0)sym->bind = LOCAL;
  else sym->bind = GLOBAL;
  
  sym->NDX = ndx;
  sym->name = ime;
  tabela.push_back(*sym);
}

std::vector<Sym>* SymTable::getSviSymbolNDX(std::vector<Sym>* pom,int ndx){
  
  for(int i = 0; i < tabela.size(); i++){
    if(tabela[i].NDX == ndx) pom->push_back(tabela[i]);
  } 

  return pom;
}

std::vector<Sym>* SymTable::getSviSymboli(std::vector<Sym>* pom){
  
  for(int i = 0; i < tabela.size(); i++){
    pom->push_back(tabela[i]);
  }
  return pom;
}

string SymTable::nazivSekcijeKojojPripada(string naziv_simbola){
  Sym* s = getSymbol(naziv_simbola);

  for(int i = 0; i < tabela.size(); i++){
    if(tabela[i].NDX == s->NDX && tabela[i].type==SCTN) return tabela[i].name;
  }
  cout<< "GRESKA UNUTAR TRAZENJA NAZIVA_SEKCIJE_KOJOJ_PRIPADA";
  exit(1);
  return "GRESKA UNUTAR TRAZENJA NAZIVA_SEKCIJE_KOJOJ_PRIPADA";
}

string SymTable::getSymbolNameByRBR(int RBR){
  for(auto a: tabela){
    if(a.RBr == RBR)return a.name;
  }
  cout << "GRESKA UNUTAR GETSYMBOLRBR";
  exit(1);
}