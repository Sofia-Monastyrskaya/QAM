#include "QamMod.h"
#include <iostream>
#include <fstream>
#include <stdexcept> 

#define PI 3.1415926535

QamMod::QamMod(int fs, int fc, int M, int sps) 
    : fs(fs), fc(fc), M(M), sps(sps), I(0.0), Q(0.0), real(0.0), imag(0.0) {
    if (M != 4 && M != 16 && M != 64) {
        throw std::invalid_argument("Modulation order must be 4, 16 or 64");
    }
}

void QamMod::modulate(std::vector<std::string> data) {
    I_symbols.clear();
    Q_symbols.clear();
    I_rep.clear();
    Q_rep.clear();
    s_t.clear();

    for (int i = 0; i < data.size(); i++) {
        switch (M) {
            //QPSK
            case 4: {
                int I_bin = (data[i][0] - '0');
                int Q_bin = (data[i][1] - '0');

                int I_gray = I_bin;
                int Q_gray = Q_bin;

                I = (I_gray == 0) ? -1.0 / sqrt(2.0) : 1.0 / sqrt(2.0);
                Q = (Q_gray == 0) ? -1.0 / sqrt(2.0) : 1.0 / sqrt(2.0);
                break;
            }
            //QAM-16
            case 16: {
                int I_bin = (data[i][0] - '0') * 2 + (data[i][1] - '0');
                int Q_bin = (data[i][2] - '0') * 2 + (data[i][3] - '0');

                int I_idx = I_bin ^ (I_bin >> 1);
                int Q_idx = Q_bin ^ (Q_bin >> 1);

                I = (-3 + 2 * I_idx) / sqrt(10.0);
                Q = (-3 + 2 * Q_idx) / sqrt(10.0);
                break;
            }
            //QAM-64
            case 64: {
                int I_bin = (data[i][0] - '0') * 4 + (data[i][1] - '0') * 2 + (data[i][2] - '0');
                int Q_bin = (data[i][3] - '0') * 4 + (data[i][4] - '0') * 2 + (data[i][5] - '0');

                int I_idx = I_bin ^ (I_bin >> 1);
                I_idx ^= I_idx >> 2;
                int Q_idx = Q_bin ^ (Q_bin >> 1);
                Q_idx ^= Q_idx >> 2;

                I = (-7 + 2 * I_idx) / sqrt(42.0);
                Q = (-7 + 2 * Q_idx) / sqrt(42.0);
                break;
            }
        }
        I_symbols.push_back(I);
        Q_symbols.push_back(Q);
    }

    for (int i = 0; i < I_symbols.size(); i++) {
        double I_val = I_symbols[i];
        double Q_val = Q_symbols[i];
        for (int j = 0; j < sps; j++) {
            I_rep.push_back(I_val);
            Q_rep.push_back(Q_val);
        }
    }

    ts.clear();
    ts.resize(I_rep.size());
    for (int i = 0; i < ts.size(); i++) {
        ts[i] = i / double(fs);
    }

    for (int i = 0; i < I_rep.size(); i++) {
        real = I_rep[i] * cos(2 * PI * fc * ts[i]);
        imag = Q_rep[i] * sin(2 * PI * fc * ts[i]);
        s_t.push_back(real + imag);
    }
}