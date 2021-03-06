#pragma once
#include "util.h"
struct SpMatOpt {
    int nRow;
    int nCol;
    int nNnz;
    __attribute__((aligned(ALIGNMENT))) int *ptr;
    __attribute__((aligned(ALIGNMENT))) int *idx;
    __attribute__((aligned(ALIGNMENT))) double *val;
};
struct VecOpt {
    int size;
    __attribute__((aligned(ALIGNMENT))) double *val;
};
void OptimizeProblem (const SpMat &A, const Vec &x, SpMatOpt &A_opt, VecOpt &x_opt);
extern "C" {
void SpMV (const SpMatOpt &A, const VecOpt &x, Vec &y);
}
