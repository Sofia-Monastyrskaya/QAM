// QamDemod.cpp
#include "QamDemod.h"
#include <iostream>
#include <vector>
#include <string>
#include <cmath>
#include <stdexcept>

#define PI 3.1415926535

QamDemod::QamDemod(double fs, double fc, int M, int sps)
    : fs(fs), fc(fc), M(M), sps(sps) {
    if (M != 4 && M != 16 && M != 64)
        throw std::invalid_argument("M must be 4, 16 or 64");
}

std::vector<std::string>QamDemod :: demodulate(std::vector<double> modulated, std::vector<double> ts) {

    std::vector<double> I_demod;
    std::vector<double> Q_demod;
    std::vector<double> I_sym;
    std::vector<double> Q_sym;

    I_sym.clear();
    Q_sym.clear();

    for (int i = 0; i < modulated.size(); i++) {
        double real = 2.0 * modulated[i] * cos(2 * PI * ts[i] * fc);
        double imag = 2.0 * modulated[i] * sin(2 * PI * ts[i] * fc);
        I_demod.push_back(real);
        Q_demod.push_back(imag);
    }

    int num_symbols = I_demod.size() / sps;
    for (int i = 0; i < num_symbols; i++) {
        double I_sum = 0.0;
        double Q_sum = 0.0;
        for (int j = 0; j < sps; j++) {
            int idx = i * sps + j;
            if (idx < I_demod.size()) {
                I_sum += I_demod[idx];
                Q_sum += Q_demod[idx];
            }
        }
        double I_avg = I_sum / sps;
        double Q_avg = Q_sum / sps;
        I_sym.push_back(I_avg);
        Q_sym.push_back(Q_avg);
    }

    std::vector<std::string> result;
    result.reserve(I_sym.size());
    for (int i = 0; i < I_sym.size(); i++) {
        std::string bits;
        switch (M) {
            case 4: {
                double I_lvl = I_sym[i] * sqrt(2.0);
                double Q_lvl = Q_sym[i] * sqrt(2.0);

                int I_idx = round((I_lvl + 1) / 2.0);
                int Q_idx = round((Q_lvl + 1) / 2.0);

                I_idx = std::max(0, std::min(1, I_idx));
                Q_idx = std::max(0, std::min(1, Q_idx));

                const std::vector<std::string> gray_code_4 = { "0", "1" };

                std::string I_bits = gray_code_4[I_idx];
                std::string Q_bits = gray_code_4[Q_idx];

                bits = I_bits + Q_bits;
                break;
            }
            case 16: {
                double I_lvl = I_sym[i] * sqrt(10.0);
                double Q_lvl = Q_sym[i] * sqrt(10.0);

                int I_idx = round((I_lvl + 3) / 2.0);
                int Q_idx = round((Q_lvl + 3) / 2.0);

                I_idx = std::max(0, std::min(3, I_idx));
                Q_idx = std::max(0, std::min(3, Q_idx));

                const std::vector<std::string> gray_code_16 = { "00", "01", "11", "10" };

                std::string I_bits = gray_code_16[I_idx];
                std::string Q_bits = gray_code_16[Q_idx];

                bits = I_bits + Q_bits;
                break;
            }
            case 64: {
                double I_lvl = I_sym[i] * sqrt(42.0);
                double Q_lvl = Q_sym[i] * sqrt(42.0);

                int I_idx = round((I_lvl + 7) / 2.0);
                int Q_idx = round((Q_lvl + 7) / 2.0);

                I_idx = std::max(0, std::min(7, I_idx));
                Q_idx = std::max(0, std::min(7, Q_idx));

                const std::vector<std::string> gray_code_64 = { "000", "001", "011", "010",
                                                  "110", "111", "101", "100" };

                std::string I_bits = gray_code_64[I_idx];
                std::string Q_bits = gray_code_64[Q_idx];

                bits = I_bits + Q_bits;
                break;
            }
        }
        result.push_back(bits);
    }
    return result;
}
