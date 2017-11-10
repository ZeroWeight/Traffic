#include <iostream>
#include <string>
#include <vector>
#include <list>
#include <iterator>
#include <cmath>
#include <algorithm>
#include <iomanip>
std::vector<std::pair<int, int>> SeqOpt (int offset = 0);
double _main_ (std::string filename1, std::string filename2);
std::vector<std::pair<std::pair<double, double>, std::pair<double, double>>> Q1;
std::vector<std::pair<std::pair<double, double>, std::pair<double, double>>> Q2;
const double Dt1 = 1;
const double Dt2 = 1.5;
const double Dt3 = 10;
int main (int argc, char** argv) {
	int count = 0;
	double ave = 0;
	double pre;
	std::string foldername = "result_2000_full/";
	std::string subfoldername;
	std::string file1;
	std::string file2;
	int start = 20;
	for (int i = start; i < start + 15; i++) {
		subfoldername = foldername + std::to_string (i) + '/';
		for (int j = 0; j < 4; j++) {
			file1 = subfoldername + std::to_string (j) + ".csv";
			for (int k = 0; k < 4; k++) {
				file2 = subfoldername + std::to_string (k) + ".csv";
				pre = _main_ (file1, file2);
				ave = (ave * count + pre) / (count + 1);
				count += 1;
				std::cout << ave << std::endl;
			}
		}
	}
}
double _main_ (std::string filename1, std::string filename2) {
	Q1.clear ();
	Q2.clear ();
	const double MaxV = 16;
	const double MaxA = 1;
	const double MinV = 7;
	FILE* file1 = fopen (filename1.data (), "r");
	FILE* file2 = fopen (filename2.data (), "r");
	double buffer[2];
	double temp1;
	double temp2;
	while (!feof (file1)) {
		fscanf (file1, "%lf,%lf\n", buffer, buffer + 1);
		if ((MaxV * MaxV - buffer[0] * buffer[0]) / 2 / MaxA >= buffer[1])
			temp1 = (sqrt (2 * MaxA * buffer[1] + buffer[0] * buffer[0]) - buffer[0]) / MaxA;
		else
			temp1 = (MaxV - buffer[0]) / MaxA +
			(buffer[1] - (MaxV * MaxV - buffer[0] * buffer[0]) / 2 / MaxA) / MaxV;
		if ((buffer[0] * buffer[0] - MinV * MinV) / 2 / MaxA >= buffer[1])
			temp2 = (buffer[0] - sqrt (buffer[0] * buffer[0] - 2 * MaxA * buffer[1]) / MaxA);
		else
			temp2 = (buffer[0] - MinV) / MaxA +
			(buffer[1] - (buffer[0] * buffer[0] - MinV * MinV) / 2 / MaxA) / MinV;
		Q1.insert (Q1.end (), std::pair<std::pair<double, double>, std::pair<double, double>>
			(std::pair<double, double> (buffer[0], buffer[1]), std::pair<double, double> (temp1, temp2)));
	}
	while (!feof (file2)) {
		fscanf (file2, "%lf,%lf\n", buffer, buffer + 1);
		if ((MaxV * MaxV - buffer[0] * buffer[0]) / 2 / MaxA >= buffer[1])
			temp1 = (sqrt (2 * MaxA * buffer[1] + buffer[0] * buffer[0]) - buffer[0]) / MaxA;
		else
			temp1 = (MaxV - buffer[0]) / MaxA +
			(buffer[1] - (MaxV * MaxV - buffer[0] * buffer[0]) / 2 / MaxA) / MaxV;
		if ((buffer[0] * buffer[0] - MinV * MinV) / 2 / MaxA >= buffer[1])
			temp2 = (buffer[0] - sqrt (buffer[0] * buffer[0] - 2 * MaxA * buffer[1]) / MaxA);
		else
			temp2 = (buffer[0] - MinV) / MaxA +
			(buffer[1] - (buffer[0] * buffer[0] - MinV * MinV) / 2 / MaxA) / MinV;
		Q2.insert (Q2.end (), std::pair<std::pair<double, double>, std::pair<double, double>>
			(std::pair<double, double> (buffer[0], buffer[1]), std::pair<double, double> (temp1, temp2)));
	}
	std::vector<std::pair<int, int>> Seq1 = SeqOpt ();
	std::vector<std::pair<int, int>> Seq2 = SeqOpt (1);
	int count = 0;
	std::vector<std::pair<int, int>>::iterator it1 = Seq1.end () - 1;
	std::vector<std::pair<int, int>>::iterator it2 = Seq2.end () - 1;
	for (; it1 >= Seq1.begin () && it2 >= Seq2.begin (); --it1, --it2) {
		if (it1->first == it2->first && it1->second == it2->second)
			count++;
		else
			break;
	}
	std::cout << count << '\t' << Seq1.size () << '\t' << Seq2.size () << '\t'
		<< std::setprecision (4) << double (count) / Seq2.size () << '\t';
	return double (count) / Seq2.size ();
}

std::vector<std::pair<int, int>> SeqOpt (int offset) {
	struct node {
		node * parent = nullptr;
		node * Q1_pass;
		node * Q2_pass;
		node * self;
		double time = 0;
		int rank_Q1 = -1;
		int rank_Q2 = -1;
		int passing = 0;
		bool ok = true;
	}root;
	root.self = &root;
	std::vector<node> leaf;
	leaf.insert (leaf.end (), root);
	std::make_heap (leaf.begin (), leaf.end (), [](const node& T1, const node & T2)->bool {
		return T1.time > T2.time;
	});
	double *** arr = new double**[Q1.size () + 1];
	for (int i = 0; i < Q1.size () + 1; i++) {
		arr[i] = new double*[Q2.size () + 1];
		for (int j = 0; j < Q2.size () + 1; j++) {
			arr[i][j] = new double[3];
			arr[i][j][0] = arr[i][j][1] = arr[i][j][2] = 1e10;
		}
	}
	while (true) {
		int rank1 = leaf.begin ()->rank_Q1;
		int rank2 = leaf.begin ()->rank_Q2;
		if (leaf.begin ()->rank_Q1 == Q1.size () - 1 - offset
			&& leaf.begin ()->rank_Q2 == Q2.size () - 1 - offset) break;
		if (arr[leaf.begin ()->rank_Q1 + 1][leaf.begin ()->rank_Q2 + 1]
			[leaf.begin ()->passing] + 0.01 < leaf.begin ()->time)
			leaf.begin ()->ok = false;
		if (leaf.begin ()->ok) {
			double t1 = leaf.begin ()->time;
			double t2 = leaf.begin ()->time;
			switch (leaf.begin ()->passing) {
			case 0:t1 += Dt1; t2 += Dt1; break;
			case 1:t1 += Dt1; t2 += Dt2; break;
			case 2:t1 += Dt2; t2 += Dt1; break;
			default: t1 += Dt2; t2 += Dt2; break;
			}
			if (leaf.begin ()->rank_Q1 < (int)Q1.size () - 1) {
				leaf.begin ()->Q1_pass = (node*)malloc (sizeof (node));
				leaf.begin ()->Q1_pass->self = leaf.begin ()->Q1_pass;
				leaf.begin ()->Q1_pass->parent = leaf.begin ()->self;
				leaf.begin ()->Q1_pass->rank_Q1 = rank1 + 1;
				leaf.begin ()->Q1_pass->rank_Q2 = rank2;
				leaf.begin ()->Q1_pass->passing = 1;
				leaf.begin ()->Q1_pass->ok = true;
				leaf.begin ()->Q1_pass->time = std::fmax (t1, Q1[leaf.begin ()->Q1_pass->rank_Q1].second.first);
				if (leaf.begin ()->Q1_pass->time > Q1[leaf.begin ()->Q1_pass->rank_Q1].second.second)
					leaf.begin ()->Q1_pass->time += Dt3;

				if (arr[leaf.begin ()->Q1_pass->rank_Q1 + 1][leaf.begin ()->Q1_pass->rank_Q2 + 1]
					[leaf.begin ()->Q1_pass->passing] > leaf.begin ()->Q1_pass->time) {
					arr[leaf.begin ()->Q1_pass->rank_Q1 + 1][leaf.begin ()->Q1_pass->rank_Q2 + 1]
						[leaf.begin ()->Q1_pass->passing] = leaf.begin ()->Q1_pass->time;
					leaf.push_back (*(leaf.begin ()->Q1_pass));
					std::push_heap (leaf.begin (), leaf.end (), [](const node& T1, const node & T2)->bool {
						return T1.time > T2.time;
					});
				}
			}
			if (leaf.begin ()->rank_Q2 < (int)Q2.size () - 1) {
				leaf.begin ()->Q2_pass = (node*)malloc (sizeof (node));
				leaf.begin ()->Q2_pass->self = leaf.begin ()->Q2_pass;
				leaf.begin ()->Q2_pass->parent = leaf.begin ()->self;
				leaf.begin ()->Q2_pass->rank_Q1 = rank1;
				leaf.begin ()->Q2_pass->rank_Q2 = rank2 + 1;
				leaf.begin ()->Q2_pass->passing = 2;
				leaf.begin ()->Q2_pass->ok = true;
				leaf.begin ()->Q2_pass->time = std::fmax (t2, Q2[leaf.begin ()->Q2_pass->rank_Q2].second.first);
				if (leaf.begin ()->Q2_pass->time > Q2[leaf.begin ()->Q2_pass->rank_Q2].second.second)
					leaf.begin ()->Q2_pass->time += Dt3;
				if (arr[leaf.begin ()->Q2_pass->rank_Q1 + 1][leaf.begin ()->Q2_pass->rank_Q2 + 1]
					[leaf.begin ()->Q2_pass->passing] > leaf.begin ()->Q2_pass->time) {
					arr[leaf.begin ()->Q2_pass->rank_Q1 + 1][leaf.begin ()->Q2_pass->rank_Q2 + 1]
						[leaf.begin ()->Q2_pass->passing] = leaf.begin ()->Q2_pass->time;
					leaf.push_back (*(leaf.begin ()->Q2_pass));
					std::push_heap (leaf.begin (), leaf.end (), [](const node& T1, const node & T2)->bool {
						return T1.time > T2.time;
					});
				}
			}
		}
		std::pop_heap (leaf.begin (), leaf.end (), [](const node& T1, const node & T2)->bool {
			return T1.time > T2.time;
		});
		leaf.pop_back ();
	}
	for (int i = 0; i < Q1.size () + 1; i++) {
		for (int j = 0; j < Q2.size () + 1; j++) {
			delete arr[i][j];
		}
		delete arr[i];
	}
	delete arr;
	node * p = &leaf[0];
	std::vector<std::pair<int, int>> Seq1;
	while (p->parent != nullptr) {
		Seq1.push_back (std::pair<int, int> (p->rank_Q1, p->rank_Q2));
		p = p->parent;
	}
	return Seq1;
}