assembler: src/lexer.cpp src/parser.cpp src/asembler.cpp src/SectionTable.cpp src/SymTable.cpp src/RelTable.cpp
	g++ -fdiagnostics-color=always -g -o ${@} ${^}
linker: src/SectionTable.cpp src/SymTable.cpp src/RelTable.cpp src/linker.cpp
	g++ -fdiagnostics-color=always -g -o ${@} ${^}
emulator: src/emulator.cpp
	g++ -fdiagnostics-color=always -g -o ${@} ${^}