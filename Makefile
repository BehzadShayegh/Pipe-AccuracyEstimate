CC = g++ -std=c++11

all: classifier.out EnsembleClassifier.out voter.out

classifier.out: classifier.cpp
	${CC} classifier.cpp -o classifier.out

voter.out: voter.cpp
	${CC} voter.cpp -o voter.out

EnsembleClassifier.out: main.cpp
	${CC} main.cpp -o EnsembleClassifier.out

clean : clean_out clean_result

clean_result:
	rm *.csv

clean_out:
	rm *.out