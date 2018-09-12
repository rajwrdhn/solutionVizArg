all: proboToASP parseOutput pickRandomArg

proboToASP: proboToASP.cc Makefile
	g++ -O3 proboToASP.cc -o proboToASP
	
parseOutput: parseOutput.cc Makefile
	g++ -O3 parseOutput.cc -o parseOutput
	
pickRandomArg: pickRandomArg.cc Makefile
	g++ -O3 pickRandomArg.cc -o pickRandomArg

clean:
	rm -f parseOutput proboToASP pickRandomArg *~
