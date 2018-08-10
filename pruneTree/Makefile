make pruneTree:
	g++ -g -c pruneTree.c -o pruneTree.o `root-config --cflags --glibs`
	g++ pruneTree.o ../build/libremoll.so -o pruneTree `root-config --cflags --glibs` -L../build/ -lremoll -Wl,-R../build/
	rm pruneTree.o

clean:
	rm ./*.o
	rm ./fom
	rm hallRad; rm rad_dose; rm pruneTree; rm pruneTreeEnvelope;
