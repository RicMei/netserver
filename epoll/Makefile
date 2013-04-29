TryMake : main.o fileA.o fileStcLib.a
	g++ -o TryMake main.o fileA.o -L -lfileStcLib.a
main.o fileA.o : main.cpp fileA.cpp
	g++ -c main.cpp fileA.cpp
#fileA.o : fileA.cpp
#	g++ -c  fileA.cpp
clean  : 
	rm -rf TryMake *.o core*
