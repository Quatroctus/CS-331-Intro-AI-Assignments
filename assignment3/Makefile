CC = g++ -std=c++11 -g -O3
EXE_FILE = naive_bayes

$(EXE_FILE): main.cpp naive_bayes.cpp
	$(CC) -o $@ $^

debug:
	$(CC) -DDEBUG -o $(EXE_FILE) main.cpp naive_bayes.cpp

zip: tayloja5_assignment3.zip

tayloja5_assignment3.zip: Makefile main.cpp naive_bayes.cpp naive_bayes.h results.txt trainingSet.txt testSet.txt
	zip $@ $^
