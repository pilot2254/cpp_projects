#include <iostream>

extern "C" long long asm_add(long long a, long long b);

int main() {
        long long result = asm_add(5, 10);
        std::cout << result;
        std::cin.get();
        return 0;
}
