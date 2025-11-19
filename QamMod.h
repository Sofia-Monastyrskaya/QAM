#pragma once
#include <vector>
#include <string>
#include <stdexcept> 

class QamMod {
private:
    int fc, fs;
    int M;
    int sps;

    double I, Q;
    double real, imag;
public:
    std::vector<double> ts;
    std::vector<double> I_symbols, Q_symbols;
    std::vector<double> I_rep, Q_rep;
    std::vector<double> s_t;

    QamMod(int fs, int fc, int M, int sps);

    void modulate(std::vector<std::string> data);
};