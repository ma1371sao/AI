/**
the first two lines may be disappeared in linux
name: Shijie Ma
date: 01/26/2017
**/

extern "C" {
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
}

#include<iostream>
#include<vector>
#include<string>
#include<fstream>
#include<stack>
#include<unordered_map>
using namespace std;

class NB {
public:
	int ne;
	int np;
	double pp;
	double pe;
	FILE* trainset;
	FILE* testset;
	vector<string> train;
	vector<string> test;
	vector<unordered_map<char, double>> pset;
	vector<unordered_map<char, double>> eset;
	vector<unordered_map<char, double>> pAttribute;
	NB(char* trainfile, char* testfile);
	void getTrainSet();
	void getTestSet();
	void training();
	void testing();
};

void NB::testing() {
	double accuracy=0;
	char res;
	for (int i = 0; i < test.size(); i++) {
		double is_p = 1;
		double is_e = 1;
		for (int j = 1; j < 23; j++) {
			is_p *= pset[j][test[i][j]];
			is_e *= eset[j][test[i][j]];
		}
		is_p *= pp;
		is_e *= pe;
		cout << "The probability of edible : " << is_e / (is_e + is_p) << endl;
		cout << "The probability of poisonous : " << is_p / (is_e + is_p) << endl;
		if (is_p >= is_e)	res = 'p';
		else res = 'e';
		if (res == test[i][0])	accuracy++;
	}
	cout << "The accuracy is : " << accuracy / test.size() << endl;
}

void NB::training() {
	for (int i = 0; i < train.size(); i++) {
		for (int j = 1; j < 23; j++) {
			if (train[i][0] == 'p')
				pset[j][train[i][j]]++;
			else if (train[i][0] == 'e')
				eset[j][train[i][j]]++;
			pAttribute[j][train[i][j]]++;
		}
	}
	for (int i = 1; i < 23; i++) {
		for (unordered_map<char, double>::iterator it = pset[i].begin(); it != pset[i].end(); it++)
			it->second = it->second / np;
	}
	for (int i = 1; i < 23; i++) {
		for (unordered_map<char, double>::iterator it = eset[i].begin(); it != eset[i].end(); it++)
			it->second = it->second / ne;
	}
	for (int i = 1; i < 23; i++) {
		for (unordered_map<char, double>::iterator it = pAttribute[i].begin(); it != pAttribute[i].end(); it++)
			it->second = it->second / train.size();
	}
	pp = np*1.0 / train.size();
	pe = ne*1.0 / train.size();
}

void NB::getTrainSet() {
	char buf[1024];
	while (fgets(buf, 1023, trainset)) {
		string s;
		for (int i = 0; i < strlen(buf); i += 2)
			s.push_back(buf[i]);
		if(s.length()==23)
			train.push_back(s);
		if (s[0] == 'p')	np++;
		if (s[0] == 'e')	ne++;
	}
}

void NB::getTestSet() {
	char buf[1024];
	while (fgets(buf, 1023, testset)) {
		string s;
		for (int i = 0; i < strlen(buf); i += 2)
			s.push_back(buf[i]);
		if(s.length()==23)
			test.push_back(s);
	}
}

NB::NB(char* trainfile, char* testfile) {
	ne = 0;
	np = 0;
	trainset = fopen(trainfile, "r");
	testset = fopen(testfile, "r");
	if (trainset == NULL) {
		fprintf(stderr, "Error opening training file %s, exiting\n", trainfile);
		exit(1);
	}
	if (testset == NULL) {
		fprintf(stderr, "Error opening testing file %s, exiting\n", testfile);
		exit(1);
	}
	for (int i = 0; i < 23; i++) {
		unordered_map<char, double> hash1;
		unordered_map<char, double> hash2;
		unordered_map<char, double> hash3;
		pset.push_back(hash1);
		eset.push_back(hash2);
		pAttribute.push_back(hash3);
	}
	getTrainSet();
	getTestSet();
}


int main(int argc, char **argv) {
	NB* classify;
	if (argc == 3) {
		classify = new NB(argv[1], argv[2]);
	}
	else {
		fprintf(stderr, "Usage: ./a.out <TrainingSetFile> <TestingSetFile> > <OutputFile>\n");
		exit(1);
	}
	classify->training();
	classify->testing();
	delete classify;
	return 0;
}