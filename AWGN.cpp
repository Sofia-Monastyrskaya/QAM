#include "AWGN.h"
#include <random>
#include <vector>

AWGN::AWGN(double mean, double stddev) : mean(mean), stddev(stddev){
	std::random_device rd;
	generator.seed(rd());
}

void AWGN::noise(std::vector<double>&s_t){
	std::normal_distribution<double> distr(mean, stddev);
	for (int i = 0; i < s_t.size(); i++) {
		s_t[i] += distr(generator);
	}
}