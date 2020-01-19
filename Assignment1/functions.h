#include "library.h"
#include "defs.h"

void LUdecomposition(int n, vvi &A, vvi &P, vvi &L, vvi &U);
void LUopenMP(int num_threads, int n, vvi &A, vvi &P, vvi &L, vvi &U);