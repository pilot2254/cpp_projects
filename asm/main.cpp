#include <iostream>

// basics
extern "C" long long asm_add(long long a, long long b);
extern "C" long long asm_sub(long long a, long long b);
extern "C" long long asm_max(long long a, long long b);
extern "C" long long asm_sum(long long n);
extern "C" long long asm_factorial(long long n);

// arrays
extern "C" long long asm_array_sum(long long array[], long long length);
extern "C" long long asm_array_double(long long array[], long long length);
extern "C" long long asm_array_max(long long array[], long long length);
extern "C" long long asm_array_min(long long array[], long long length);
extern "C" long long asm_array_reverse(long long array[], long long length);
extern "C" long long asm_array_contains(long long array[], long long length, long long what);
extern "C" long long asm_array_count_even(long long array[], long long length);
extern "C" long long asm_array_bubble_sort(long long array[], long long length);

int main() {
        

        std::cout << "ASM Add: " << asm_add(5, 10) << '\n';
        std::cout << "ASM Sub: " << asm_sub(5, 10) << '\n';

        std::cout << "ASM Max: " << asm_max(5, 10) << '\n';
        std::cout << "ASM Sum: " << asm_sum(5) << '\n';
        std::cout << "ASM Fac: " << asm_factorial(5) << '\n';

        long long arr[] = {1, 4, 2, 3, 5};
        std::cout << "ASM Arr Sum: " << asm_array_sum(arr, (long long)sizeof(arr) / sizeof(arr[0])) << '\n';

        std::cout << "ASM Arr Doubled: ";
        asm_array_double(arr, (long long)sizeof(arr) / sizeof(arr[0]));
        for (long long x : arr) std::cout << x << ' ';
        std::cout << '\n';

        std::cout << "ASM Max: " << asm_array_max(arr, (long long)sizeof(arr) / sizeof(arr[0])) << '\n';
        std::cout << "ASM Min: " << asm_array_min(arr, (long long)sizeof(arr) / sizeof(arr[0])) << '\n';

        std::cout << "ASM Arr Reversed: ";
        asm_array_reverse(arr, (long long)sizeof(arr) / sizeof(arr[0]));
        for (long long x : arr) std::cout << x << ' ';
        std::cout << '\n';

        std::cout << "ASM Arr Contains: ";
        std::string bContains = (asm_array_contains(arr, (long long)sizeof(arr)/sizeof(arr[0]), 10)) ? "YES\n" : "NO\n";
        std::cout << bContains;

        std::cout << "ASM Arr Even Count: " << asm_array_count_even(arr, (long long)sizeof(arr) / sizeof(arr[0])) << '\n';

        std::cout << "ASM Arr Sorted: ";
        asm_array_bubble_sort(arr, (long long)sizeof(arr) / sizeof(arr[0]));
        for (long long x : arr) std::cout << x << ' ';
        std::cout << '\n';

        std::cout << "\nPress any key to exit";

        std::cin.get();
        return 0;
}
