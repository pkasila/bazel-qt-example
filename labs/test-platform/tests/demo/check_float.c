#include <ctype.h>
#include <errno.h>
#include <math.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

__attribute__((export_name("answer_score")))
int answer_score(const char* answer, const char* userdata) {
    long double ans_ld = strtold(answer, NULL);
    long double correct_ld = strtold(userdata, NULL);
    if(fabsl(ans_ld - correct_ld) / fmaxl(1, fabsl(correct_ld)) <= 1e-6) {
        return 10;
    }
    return 0;
}

__attribute__((export_name("answer_valid")))
bool answer_valid(const char* answer, const char* userdata) {
    char* endptr;
    errno = 0;
    long double ans_ld = strtold(answer, &endptr);
    return errno == 0 && *endptr == '\0';
}

