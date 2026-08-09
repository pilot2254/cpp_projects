#include <iostream>

extern "C" long long asm_add(long long a, long long b);
extern "C" long long asm_sub(long long a, long long b);

extern "C" long long asm_max(long long a, long long b);

extern "C" long long asm_sum(long long n);

extern "C" long long asm_factorial(long long n);

extern "C" long long asm_array_sum(long long array[], long long length);
extern "C" long long asm_array_double(long long array[], long long length);

extern "C" long long asm_array_max(long long array[], long long length);
extern "C" long long asm_array_min(long long array[], long long length);

extern "C" long long asm_array_reverse(long long array[], long long length);

int main() {
        

        std::cout << asm_add(5, 10)     << '\n';
        std::cout << asm_sub(5, 10)     << '\n';

        std::cout << asm_max(5, 10)     << '\n';
        std::cout << asm_sum(5)         << '\n';
        std::cout << asm_factorial(5)   << '\n';

        long long arr[] = {5, 2, 3, 4, 1};
        std::cout << asm_array_sum(arr, (long long)sizeof(arr)/sizeof(arr[0])) << '\n';

        asm_array_double(arr, (long long)sizeof(arr)/sizeof(arr[0]));
        for (long long x : arr) std::cout << x << ' ';
        std::cout << '\n';

        std::cout << asm_array_max(arr, (long long)sizeof(arr)/sizeof(arr[0])) << '\n';
        std::cout << asm_array_min(arr, (long long)sizeof(arr) / sizeof(arr[0])) << '\n';

        asm_array_reverse(arr, (long long)sizeof(arr) / sizeof(arr[0]));
        for (long long x : arr) std::cout << x << ' ';
        std::cout << '\n';

        std::cin.get();
        return 0;
}
