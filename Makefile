build:
	riscv64-unknown-linux-gnu-g++ -static -o bintrans main.cpp ../fadec/decode.o
debug:
	g++ -static -g -o bintransdebug main.cpp ../fadec/decodedebug.o
