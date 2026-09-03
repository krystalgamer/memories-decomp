#include "../types.h"

extern int Campaign_TestStoryFlag(int);
extern void Library_UpdateCardUsedFlag(int);
int func_8002CD48(int value) {
    int result = Campaign_TestStoryFlag(value);
    if (result == 0) Library_UpdateCardUsedFlag(value);
    return result;
}
