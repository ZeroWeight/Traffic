#include <iostream>
#include <string>
#include <vector>
#include <list>
#include <iterator>
#include <cmath>
#include <algorithm>
int main (int argc, char ** argv) {
	double MaxV = 16;
	double MaxA = 3;
	std::string foldername (argv[1]);
	std::string filename1 = foldername + "/1.csv";
	std::string filename2 = foldername + "/0.csv";
	FILE* file1 = fopen (filename1.data (), "r");
	FILE* file2 = fopen (filename2.data (), "r");
	std::vector<std::pair<std::pair<double, double>, double>> Q1;
	std::vector<std::pair<std::pair<double, double>, double>> Q2;
	double buffer[2];
	double temp;
	while (!feof (file1)) {
		fscanf (file1, "%lf,%lf\n", buffer, buffer + 1);
		if ((MaxV * MaxV - buffer[0] * buffer[0]) / 2 / MaxA >= buffer[1])
			temp = (sqrt (2 * MaxA * buffer[1] +
				buffer[0] * buffer[0])
				- buffer[0] * buffer[0]) / MaxA;
		else
			temp = (MaxV - buffer[0]) / MaxA +
			(buffer[1] - (MaxV * MaxV - buffer[0] *
				buffer[0]) / 2 / MaxA) / MaxV;
		Q1.insert (Q1.end (), std::pair<std::pair<double, double>, double>
			(std::pair<double, double> (buffer[0], buffer[1]), temp));
	}
	while (!feof (file2)) {
		fscanf (file2, "%lf,%lf\n", buffer, buffer + 1);
		if ((MaxV * MaxV - buffer[0] * buffer[0]) / 2 / MaxA >= buffer[1])
			temp = (sqrt (2 * MaxA * buffer[1] +
				buffer[0] * buffer[0])
				- buffer[0] * buffer[0]) / MaxA;
		else
			temp = (MaxV - buffer[0]) / MaxA +
			(buffer[1] - (MaxV * MaxV - buffer[0] *
				buffer[0]) / 2 / MaxA) / MaxV;
		Q2.insert (Q2.end (), std::pair<std::pair<double, double>, double>
			(std::pair<double, double> (buffer[0], buffer[1]), temp));
	}

	const double Dt1 = 3;
	const double Dt2 = 5;
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
		if (leaf.begin ()->rank_Q1 == Q1.size () - 1
			&& leaf.begin ()->rank_Q2 == Q2.size () - 1) break;
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
				leaf.begin ()->Q1_pass->time = std::fmax (t1, Q1[leaf.begin ()->Q1_pass->rank_Q1].second);
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
				leaf.begin ()->Q2_pass->time = std::fmax (t2, Q2[leaf.begin ()->Q2_pass->rank_Q2].second);
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
	std::cout << leaf.begin ()->time << '\t' << leaf.begin ()->rank_Q1 << '\t' << leaf.begin ()->rank_Q2 <<
		'\t' << Q1.back ().second << '\t' << Q2.back ().second << std::endl;
	node * p = &leaf[0];
	while (p->parent != nullptr) {
		std::cout << p->passing << '\t' << p->rank_Q1 << '\t' << p->rank_Q2 << '\t' << p->time << std::endl;
		p = p->parent;
	}
	return 0;
}