#include "../inc/SectionTable.hpp"

void SectionTable::addSection(Section* nova_sekcija){
  tabela.push_back(nova_sekcija);
}

void SectionTable::setSize(SymTable*symtable){
  for(int i = 0; i < tabela.size(); i++)
    symtable->setSizeSekcije(tabela[i]->name, tabela[i]->hex_sadrzaj.size());
}

void SectionTable::addDecimal(Section* sec, int decimalValue)
{
  std::stringstream ss;
  ss << std::hex << decimalValue;
  std::string hexString = ss.str();

  if(hexString.size() != 8) hexString = dopuniString(hexString);
  for(int i = hexString.size()-1; i > 0; i-=2){
    int j = i-1;
    string pom = "";
    pom += hexString[j];
    pom+= hexString[i];
    sec->hex_sadrzaj.push_back(pom);
  }
}

void SectionTable::skip(Section* sec,int  decimalValue){
  for(int i = 0; i < decimalValue; i++)
    sec->hex_sadrzaj.push_back("00");
}

Section* SectionTable::getSection(string sec_name){
  for(int i = 0; i < tabela.size(); i++){
    if(tabela[i]->name == sec_name) return tabela[i]; 
  }

  std::cout<<"NE POSTOJI SEKCIJA getSection vraca nullptr";
  return nullptr;
}

string SectionTable::dopuniString (string hex){
  string povratni;
  switch (hex.size())
  {
  case 1:povratni ="0000000" + hex;
    /* code */
    break;
  case 2:povratni ="000000" + hex;
    /* code */
    break;
  case 3:povratni ="00000" + hex;
    /* code */
    break;
  case 4:povratni ="0000" + hex;
    /* code */
    break;
  case 5:povratni="000" + hex;
    /* code */
    break;
  case 6:povratni="00" + hex;
    /* code */
    break;
  case 7:povratni = "0" + hex;
    /* code */
    break;
  case 8:povratni = hex;
    break;
  default:
    break;
  }

  return povratni;
}

void SectionTable::addString(Section* sec, string hexString){
  if(hexString.size() != 8) hexString = dopuniString(hexString);
  for(int i = hexString.size()-1; i > 0; i-=2){
    int j = i-1;
    string pom = "";
    pom += hexString[j];
    pom+= hexString[i];
    sec->hex_sadrzaj.push_back(pom);
  }
}

void SectionTable::addString2(Section* sec, long long value){
  // Convert to unsigned long long to handle negative values and truncation
    unsigned long long uValue = static_cast<unsigned long long>(value);

    // Truncate the value to 32 bits to fit within 8 hexadecimal characters
    uValue &= 0xFFFFFFFF;

    std::stringstream ss;
    ss << std::hex << std::setw(8) << std::setfill('0') << uValue;

    std::string hexString = ss.str();
    
    for(int i = hexString.size()-1; i > 0; i-=2){
    int j = i-1;
    string pom = "";
    pom += hexString[j];
    pom+= hexString[i];
    sec->hex_sadrzaj.push_back(pom);
  }
}

void SectionTable::dodajTabeleLiteralaUSadrzaj(){
  for(int i = 0; i < tabela.size(); i++){
    for(auto entry : tabela[i]->tabela_literala){
      addString2(tabela[i], entry->key);
    }
  }
}

void SectionTable::ispis(){
  std::cout << "SectionTable:" << std::endl;
  std::cout<< "NDX||"<<"Section_name||" <<"base||" <<"ofset\n";
      for (Section* section : tabela) {
        std::cout<< section->NDX << " " <<section->name << " "
        << section->base << " " << section->ofset << std::endl; 
        std::cout << "hex_sadrzaj :\n"; 
      int brojac = 0;
      for (const std::string& hex : section->hex_sadrzaj) {
          std::cout << hex;
          brojac++;
          if((brojac % 4) == 0) std::cout<< " ";
          if((brojac % 8) == 0) std::cout<< std::endl;   
      }

      std::cout << "\n tabela_litera: {\n";
      for(auto entry: section->tabela_literala){
        std::cout << entry->key <<": "<< entry->adress <<" "<<entry->name<<std::endl;
      }

      std::cout << "}\n";
    }
    std::cout << "\n}";
}

void SectionTable::upisi(std::ofstream* outputFile){
  
  
  for (Section* section : tabela) {
        (*outputFile) << "SECTABLE\n";
        (*outputFile)<< section->NDX << " " <<section->name << " "
        << section->base << " " << section->ofset << std::endl; 
        (*outputFile) << "hex_sadrzaj\n"; 
      int brojac = 0;
      for (const std::string& hex : section->hex_sadrzaj) {
          (*outputFile) << hex;
          brojac++;
          if((brojac % 4) == 0) (*outputFile)<< " ";
          if((brojac % 8) == 0) (*outputFile)<< std::endl;   
      }
      if((brojac % 8)!=0)(*outputFile)<<std::endl;
      if(section->hex_sadrzaj.size()==0) (*outputFile) << std::endl;
      (*outputFile) << "tabela_literala\n";
      for (auto entry : section->tabela_literala) {
        (*outputFile) << entry->key << " "<< entry->adress<< " "<<entry->name<<std::endl;
      }
      if(section->tabela_literala.size()==0) (*outputFile) << std::endl;
  }
}

void SectionTable::addLine(Section* sec,int ndx, string sec_name, int base, int ofset){
  
  sec->NDX = ndx;
  sec->name = sec_name;
  sec->ofset = ofset;
  tabela.push_back(sec);
}

void SectionTable::addLineHex(Section* sec, string first_4byte, string second_4byte){
  for(int i = 0; i < first_4byte.size(); i+=2){
    int j = i+1;
    string pom = "";
    pom += first_4byte[i];
    pom+= first_4byte[j];
    sec->hex_sadrzaj.push_back(pom);
  }
  for(int i = 0; i < second_4byte.size(); i+=2){
    int j = i+1;
    string pom = "";
    pom += second_4byte[i];
    pom+= second_4byte[j];
    sec->hex_sadrzaj.push_back(pom);
  }
}

void SectionTable::addLineLiteralTable(Section* sec,long long key, long long value, string arg){
  sec->addLiteral(key, value, arg);
}

std::vector<string>* SectionTable::getHeaderNames(std::vector<string>* pom){
  
  for(int i = 0; i < tabela.size(); i++) pom->push_back(tabela[i]->name);

  return pom;
}

std::vector<int>* SectionTable::getHeaderNDX(std::vector<int>* pom){
  
  
  for(int i = 0; i < tabela.size(); i++) pom->push_back(tabela[i]->NDX);

  return pom;
}
