#pragma once
#include <random>
#include <vector>

class AWGN {
private:
	double mean, stddev;
	std::default_random_engine generator;
	std::normal_distribution<double> distribution;
public:
	AWGN(double mean = 0.0, double stddev = 0.0);

	void noise(std::vector<double> &s_t);
};
