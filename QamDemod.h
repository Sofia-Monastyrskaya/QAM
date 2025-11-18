#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <complex>

class QamDemod {
private:
	double fs, fc;
	int M, sps;
public:
	QamDemod(double fs, double fc, int M, int sps);

	std::vector<std::string> demodulate(std::vector<double> modulated, std::vector<double> ts);

};