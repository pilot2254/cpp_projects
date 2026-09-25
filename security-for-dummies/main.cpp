/*
//unprotected version

#include <windows.h>
#include <cstdio>

int main() {
        printf("PID: %lu\n", GetCurrentProcessId());
        return 0;
}
*/

#include <windows.h>
#include <cstdio>

#include "lazy_importer.hpp"
#include "skCrypter.h"

int main() {
        auto getpid = LI_FN(GetCurrentProcessId);

        printf(skCrypt("PID: %lu\n"), getpid());

        return 0;
}
