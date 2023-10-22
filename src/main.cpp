#include <iostream>
#include "../inc/lexer.hpp"
#include "../inc/parser.hpp"
#include "../inc/asembler.hpp"
#include "../inc/linker.hpp"
#include "../inc/emulator.hpp"

int main(){

  //string moj = ".section math\nmathAdd:\nld $d, %4";
  std::list<string> treci = {"ld [%sp + 0x08], %r1", "ret", "div %r2, %r1", "pop %r2"};


  Asembler* asembler = new Asembler();

  asembler->asemble("ulaz.txt", "izlaz.txt");
   //komentar
  Linker* linker = new Linker();
  std::vector<string> vektorDatoteka;
  vektorDatoteka.push_back("izlaz.txt");
  // vektorDatoteka.push_back("izlaz1.txt");
  // vektorDatoteka.push_back("izlaz2.txt");
  linker->ucitaj(vektorDatoteka);
  
  std::vector<Placeholder>placeholder; 
  Placeholder p1;// p1.ime_sekcije = "isr"; p1.hex_adresa= "0x4000F000";
  Placeholder p2; //p2.ime_sekcije = "my_data"; p2.hex_adresa= "0x40000000";
  //placeholder.push_back(p1); placeholder.push_back(p2);
  for(string s: vektorDatoteka)
    linker->datoteke.push_back(s);
  linker->mapiranje(&placeholder);
  linker->odredjivanje();
  linker->razresavanje();
  linker->upis("izlazLinker.txt");

  Emulator* em = new Emulator();
  em->ucitaj_memoriju("izlazLinker.txt");
  em->emuliraj();
  return 0;
}