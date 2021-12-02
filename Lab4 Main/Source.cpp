#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include "TaskQueueHandler.h"
#include <chrono>

using namespace std;


/*
4. »зучение создани€ и использовани€ потоков и механизмов синхронизации.

- –азработать очередь заданий, в которую несколько потоков могут вставл€ть элементы атомарно.
- –азработать обработчик этой очереди, который извлекает из нее задани€ и раздает заданному количеству потоков.
- –азработать программу, котора€ использует очередь заданий и обработчик очереди дл€ сортировки строк в текстовом файле:
¬ходной поток читает файл в пам€ть, нарезает его на части и создает несколько заданий на сортировку (по числу сортирующих потоков),
которые помещает в очередь заданий. —ортирующие потоки извлекают задани€, сортируют свои части файла, отдают все результаты
выходному потоку. ¬ыходной поток дожидаетс€ всех сортированных частей и объедин€ет их методом сортирующего сли€ни€.

*/
#define INPUT_FILE "file.txt"
#define OUTPUT_FILE "file2.txt"

#define DEF_PROCESSOR_INFOS_SIZE 1
#define MIN_NUM_THREADS 1

vector<vector<string>*> parts;

void sortPart(vector<string>* strings) {
	sort(strings->begin(), strings->end());
}


int main()
{
	ifstream istream(INPUT_FILE);
	vector<string> strings;
	string line;
	while (getline(istream, line)) {
		strings.push_back(line);
	}
	istream.close();




	TaskQueue tasks;

	SYSTEM_LOGICAL_PROCESSOR_INFORMATION* processorsInfo = new SYSTEM_LOGICAL_PROCESSOR_INFORMATION[DEF_PROCESSOR_INFOS_SIZE];

	DWORD processorsInfoSize = 0;
	GetLogicalProcessorInformation(processorsInfo, &processorsInfoSize);
	delete[](processorsInfo);
	int tmp = sizeof(SYSTEM_LOGICAL_PROCESSOR_INFORMATION);

	processorsInfo = new SYSTEM_LOGICAL_PROCESSOR_INFORMATION[processorsInfoSize / sizeof(SYSTEM_LOGICAL_PROCESSOR_INFORMATION)]; //456/24
	GetLogicalProcessorInformation(processorsInfo, &processorsInfoSize);

	//PROCESSOR_RELATIONSHIP - 
	//RelationProcessorCore - 
	//mask - 

	int logicalProcessorsCount = 0;
	for (int i = 0; i < processorsInfoSize / sizeof(SYSTEM_LOGICAL_PROCESSOR_INFORMATION); i++) {
		if (processorsInfo[i].Relationship == RelationProcessorCore) {
			ULONG_PTR mask = processorsInfo[i].ProcessorMask;
			while (mask != 0) {
				if (mask & 1) logicalProcessorsCount++;
				mask = mask >> 1; //че тут
			}
		}
	}


	int numThreads = 0;
	while (numThreads > logicalProcessorsCount || numThreads < 1) {
		cout << "¬ведите количество потоков (макс.  " << logicalProcessorsCount << "): " << endl;
		cin >> numThreads;
		if (numThreads > logicalProcessorsCount) {
			cout << " оличество введенных потоков превышает максимальное! ( " << logicalProcessorsCount << ")" << endl;
		}
		if (numThreads < 1) {
			cout << "Min number of threads: " << MIN_NUM_THREADS << endl;
		}
	}

	chrono::system_clock::time_point start = chrono::system_clock::now();

	int stringsInPart = strings.size() / numThreads;
	int a = strings.size() - (stringsInPart * numThreads);


	for (int i = 0; i < numThreads; i++) {
		parts.push_back(new vector<string>());
		for (int j = 0; j < ((i < a) ? stringsInPart + 1 : stringsInPart); j++) {
			parts[i]->push_back(strings[i * stringsInPart + j]);
		}
		tasks.addTask([i] {sortPart(parts[i]); });
	}

	TaskQueueHandler handler(&tasks);
	handler.startTasks(numThreads);
	handler.waitForComplition();

	chrono::system_clock::time_point finish = chrono::system_clock::now();
	while (parts.size() > 1) {
		vector<string>* merged = new vector<string>;
		merge(parts[0]->begin(), parts[0]->end(), parts[1]->begin(), parts[1]->end(), back_inserter(*merged));


		delete(parts[0]);
		auto it = parts.begin();
		parts.erase(it);

		delete(parts[0]);
		it = parts.begin();
		parts.erase(it);

		parts.push_back(merged);
	}


	ofstream ostream(OUTPUT_FILE);
	for (int i = 0; i < parts[0]->size(); i++) {
		ostream << (*parts[0])[i] << endl;
	}
	ostream.close();

	delete(parts[0]);


	chrono::duration<double> elapsed = finish - start;

	cout << elapsed.count() << " seconds" << endl;
}