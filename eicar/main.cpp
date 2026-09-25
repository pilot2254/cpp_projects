#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <climits>

//put it into virustotal and see for urself

#define EICAR R"(X5O!P%@AP[4\PZX54(P^)7CC)7}$EICAR-STANDARD-ANTIVIRUS-TEST-FILE!$H+H*)" //for uneducated: https://en.wikipedia.org/wiki/EICAR_test_file

int main() {
        const std::filesystem::path folder = "eicar";

        std::filesystem::remove_all(folder);
        std::filesystem::create_directory(folder);

        for (int i{}; i < INT_MAX; i++) {
                const std::filesystem::path file = folder / ("eicar_" + std::to_string(i) + ".txt");

                std::ofstream current_file(file);

                if (current_file) { current_file << EICAR; }
                else { return 1; }
        }

        return 0;
}
