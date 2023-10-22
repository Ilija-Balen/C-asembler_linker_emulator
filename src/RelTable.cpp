#include "../inc/RelTable.hpp"

void RelTable::dodajZapis(Rela* zapis){
  ulazi.push_back(zapis);
}

void RelTable::setName(string name){
  sectionName = name;
}

void RelTable::ispis(){
  cout << "Section: " << sectionName << std::endl;
    cout << "Relocation entries:" << std::endl;
    for (auto entry : ulazi) {
        cout << "Offset: " << entry->r_offset << ", "
            << "Type: " << static_cast<int>(entry->r_type) << ", "
            << "Addend: " << entry->r_addend << ", "
            << "RBr(SymTable): " << entry->s_index << std::endl;
    }
}

void RelTable::upis(std::ofstream* outputFile){
  (*outputFile) << "RELTABLE\n";
  (*outputFile) << sectionName <<"\n";
  for(auto entry: ulazi){
    (*outputFile) << entry->r_offset << " " <<static_cast<int>(entry->r_type)<< " "
    << entry->r_addend << " " << entry->s_index << "\n"; 
  }
  if(ulazi.size()==0)(*outputFile) << std::endl;
}

void RelTable::addLine(int ofset, int type, int addend, int rbr){
  Rela* pom = new Rela();
  pom->r_offset = ofset;
  
  if(type == 0)pom->r_type = ABS;
  else pom->r_type = REL;

  pom->r_addend = addend;
  pom->s_index = rbr;
  ulazi.push_back(pom);
}