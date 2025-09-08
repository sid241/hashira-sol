#include <iostream>
#include <vector>
#include <string>
#include <stdexcept>
#include <limits>

#include "BigInt.hpp"

struct SharePoint {
    BigInt x_val;
    BigInt y_val;
};

class SecretReconstructor {
private:
    std::vector<SharePoint> shares_list;

    BigInt parseToDecimal(const std::string& number_str, int base_val) {
        BigInt result_dec = 0;
        BigInt base_power = 1;

        for (int idx = number_str.length() - 1; idx >= 0; idx--) {
            int digit_num;
            char ch = number_str[idx];

            if (ch >= '0' && ch <= '9') {
                digit_num = ch - '0';
            } else if (ch >= 'a' && ch <= 'z') {
                digit_num = ch - 'a' + 10;
            } else {
                throw std::invalid_argument("Invalid character in number string.");
            }

            if (digit_num >= base_val) {
                throw std::invalid_argument("Digit exceeds provided base.");
            }

            result_dec += digit_num * base_power;
            base_power *= base_val;
        }
        return result_dec;
    }

public:
    void insertShare(const BigInt& x_input, const std::string& y_input_str, int y_input_base) {
        SharePoint sp;
        sp.x_val = x_input;
        sp.y_val = parseToDecimal(y_input_str, y_input_base);
        shares_list.push_back(sp);
    }

    BigInt reconstructSecret() {
        BigInt secret_val = 0;
        int num_shares = shares_list.size();

        if (num_shares == 0) {
            std::cerr << "Warning: No shares provided!" << std::endl;
            return 0;
        }

        for (int j = 0; j < num_shares; j++) {
            BigInt numerator = 1;
            BigInt denominator = 1;

            for (int i = 0; i < num_shares; i++) {
                if (i == j) continue;
                numerator *= shares_list[i].x_val;
                denominator *= (shares_list[i].x_val - shares_list[j].x_val);
            }

            BigInt term = shares_list[j].y_val * numerator / denominator;
            secret_val += term;
        }
        return secret_val;
    }

    void resetShares() {
        shares_list.clear();
    }
};

int main() {
    SecretReconstructor recon;

    std::cout << "Processing First Test..." << std::endl;
    recon.insertShare(1, "4", 10);
    recon.insertShare(2, "111", 2);
    recon.insertShare(3, "12", 10);

    BigInt secret1 = recon.reconstructSecret();
    std::cout << "Recovered Secret (Test 1): " << secret1 << std::endl;
    std::cout << "---------------------------------" << std::endl;

    recon.resetShares();

    std::cout << "Processing Second Test..." << std::endl;
    recon.insertShare(1, "13444211440455345511", 6);
    recon.insertShare(2, "aed7015a346d635", 15);
    recon.insertShare(3, "6aeeb69631c227c", 15);
    recon.insertShare(4, "e1b5e05623d881f", 16);
    recon.insertShare(5, "316034514573652620673", 8);
    recon.insertShare(6, "2122212201122002221120200210011020220200", 3);
    recon.insertShare(7, "20120221122211000100210021102001201112121", 3);

    BigInt secret2 = recon.reconstructSecret();
    std::cout << "Recovered Secret (Test 2): " << secret2 << std::endl;

    return 0;
}
