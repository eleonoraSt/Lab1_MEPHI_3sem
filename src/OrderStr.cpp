#include "..\include\OrderStr.h"

bool equal(const char* str1, const char* str2) {
    for (int index = 0; index < ORDER_LEN; index++) {
        if (str1[index] != str2[index]) return false;
    }
    return true;
}
