#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <ctime>
#include <cstdlib>
#include <complex>
#include "QamMod.h"
#include "AWGN.h"
#include "QamDemod.h"

#define PI 3.1415926535

std::vector<std::string> generateRandomBits(int num_symbols, int bits_per_symbol) {
    std::vector<std::string> bits;
    for (int i = 0; i < num_symbols; i++) {
        std::string symbol_bits = "";
        for (int j = 0; j < bits_per_symbol; j++) {
            symbol_bits += (std::rand() % 2) ? '1' : '0';
        }
        bits.push_back(symbol_bits);
    }
    return bits;
}

double calculateBER(const std::vector<std::string>& original, const std::vector<std::string>& received) {
    int total_bits = 0;
    int error_bits = 0;

    for (int i = 0; i < original.size(); i++) {
        if (i >= received.size() || original[i].length() != received[i].length()) {
            continue;
        }
        for (int j = 0; j < original[i].length(); j++) {
            total_bits++;
            if (original[i][j] != received[i][j]) {
                error_bits++;
            }
        }
    }

    if (total_bits == 0) return 1.0;
    if (error_bits == 0) return 0.0;

    return static_cast<double>(error_bits) / total_bits;
}

int main() {
    setlocale(LC_ALL, "Russian");
    std::srand(std::time(0));

    int fs = 2000000;
    int fc = 200000;
    int sps = 20;
    int num_symbols = 50000;
    const int num_experiments = 10;

    std::vector<double> snr_db_values = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25};
    std::vector<int> modulation_types = { 4, 16, 64 };
    std::vector<int> bits_per_symbols = { 2, 4, 6 };

    std::remove("qpsk_ber.txt");
    std::remove("qam16_ber.txt");
    std::remove("qam64_ber.txt");

    std::ofstream file_qam4("qpsk_ber.txt");
    std::ofstream file_qam16("qam16_ber.txt");
    std::ofstream file_qam64("qam64_ber.txt");

    file_qam4 << "# SNR_dB BER" << std::endl;
    file_qam16 << "# SNR_dB BER" << std::endl;
    file_qam64 << "# SNR_dB BER" << std::endl;

    for (int mod_idx = 0; mod_idx < modulation_types.size(); mod_idx++) {
        int M = modulation_types[mod_idx];
        int bps = bits_per_symbols[mod_idx];

        std::ofstream* output_file;
        switch (M) {
        case 4: output_file = &file_qam4; break;
        case 16: output_file = &file_qam16; break;
        case 64: output_file = &file_qam64; break;
        default: output_file = &file_qam4;
        }

        for (int j = 0; j < snr_db_values.size(); j++) {
            double snr_db = snr_db_values[j];
            double snr_linear = std::pow(10.0, snr_db / 10.0);

            double total_ber = 0.0;
            int total_errors = 0;
            int total_bits = 0;

            for (int exp = 0; exp < num_experiments; exp++) {
                auto original_bits = generateRandomBits(num_symbols, bps);

                QamMod qam(fs, fc, M, sps);
                qam.modulate(original_bits);
                double total_energy = 0.0;
                for (const auto& s : qam.s_t) {
                    total_energy += std::norm(s);
                }

                double Es = total_energy / num_symbols;

                if (Es > 0) {
                    double scale = std::sqrt(1.0 / Es);
                    for (auto& s : qam.s_t) {
                        s *= scale;
                    }
                }

                double k = std::log2(M);
                double EbN0_linear = std::pow(10.0, snr_db / 10.0);
                double sigma = std::sqrt(1.0 / (2.0 * k * EbN0_linear));

                AWGN awgn(0.0, sigma);
                auto noisy_signal = qam.s_t;
                awgn.noise(noisy_signal);

                QamDemod demod(fs, fc, M, sps);
                auto demod_bits = demod.demodulate(noisy_signal, qam.ts);

                long long current_errors = 0;
                for (int i = 0; i < original_bits.size() && i < demod_bits.size(); ++i) {
                    for (int j = 0; j < original_bits[i].size() && j < demod_bits[i].size(); ++j) {
                        if (original_bits[i][j] != demod_bits[i][j])
                            current_errors++;
                    }
                }
                long long current_bits = (long long)num_symbols * bps;

                total_errors += current_errors;
                total_bits += current_bits;
                double ber = current_bits > 0 ? (double)current_errors / current_bits : 0.0;
                std::cout << "QAM-" << M << ", SNR = " << snr_db << " dB, BER = " << ber
                    << " (" << current_errors << "/" << current_bits << " bits)" << std::endl;
            }

            double average_ber = static_cast<double>(total_errors) / total_bits;

            if (average_ber == 0.0) {
                *output_file << snr_db << " " << 1e-8 << std::endl;
            }
            else {
                *output_file << snr_db << " " << average_ber << std::endl;
            }
        }
        std::cout << std::endl;
    }

    //gnuplot
    file_qam4.close();
    file_qam16.close();
    file_qam64.close();

    std::string gnuplot_cmd =
        "gnuplot -p -e \""
        "set logscale y; set grid; "
        "set xlabel 'SNR (dB)'; set ylabel 'BER'; "
        "set title 'BER vs SNR for QAM Modulations'; "
        "set yrange [1e-6:1]; "
        "plot 'qpsk_ber.txt' using 1:2 w lp lw 2 pt 7 ps 1 title 'QPSK', "
        "'qam16_ber.txt' using 1:2 w lp lw 2 pt 7 ps 1 title '16-QAM', "
        "'qam64_ber.txt' using 1:2 w lp lw 2 pt 7 ps 1 title '64-QAM'\"";

    system(gnuplot_cmd.c_str());
}