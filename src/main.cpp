#include <iostream>
#include <vector>
#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cmath>
#ifdef _OPENMP
#include <omp.h>
#endif
/*
#include <ittnotify.h> // VTune Amplifier
*/
#include "opt.h"
#include "util.h"
#include "param.h"
using namespace std;
int main (int argc, char **argv) {
    srand(3);
    if (argc < 2) {
        printf("Usage: %s <matrix>\n", argv[0]);
        exit(1);
    }
    string matFile = argv[1];
    SpMat A;
    cerr << "Loading sparse matrix " << matFile << " ... ";
    LoadSparseMatrix(A, matFile);
    cerr << "done." << endl;
    int nRow = A.nRow;
    int nCol = A.nCol;
    int nNnz = A.nNnz;
    Vec x = CreateRandomVector(nCol);
    Vec y = CreateRandomVector(nRow);
    SpMatOpt A_opt;
    VecOpt x_opt;
    cerr << "Optimizing ... ";
    OptimizeProblem(A, x, A_opt, x_opt);
    cerr << "done." << endl;


#ifdef VERIFY
    for (int i = 0; i < 2; i++) {
        {
            extern vector<double> g_profile;
            g_profile = vector<double>(10);
            SpMV(A_opt, x_opt, y);
        }
        //ViewVec(y);

        cerr << "Verifying " << i << " ... ";
        if (!VerifyResult(A, x, y)) {
            printf("*** invalid result ***\n");
            exit(1);
        }
        cerr << "done." << endl;
    }
#endif

    int loop = 1;
    cerr << "Calculating SpMV ... ";
    {
        extern vector<double> g_profile;
        g_profile = vector<double>(10);
        double elapsedTime;
        elapsedTime = -GetTimeBySec();
        do {
            for (int i = 0; i < loop; i++) {
                SpMV(A_opt, x_opt, y);
            }
            loop *= 2;
        } while (GetTimeBySec() + elapsedTime < 1.0);
    }

    double minElapsedTime;
    /*
       __itt_domain *domain = __itt_domain_create("MySpMV.Region");
       __itt_string_handle *handle = __itt_string_handle_create("MySpMV.Handle");
       __itt_task_begin(domain, __itt_null, __itt_null, handle);
       */
    vector<double> g_best_profile;
    {
        const int nTry = 10;
        for (int t = 0; t < nTry; t++) {
            extern vector<double> g_profile;
            g_profile = vector<double>(10);
            double elapsedTime = -GetTimeBySec();
            for (int i = 0; i < loop; i++) {
                SpMV(A_opt, x_opt, y);
            }
            elapsedTime += GetTimeBySec();
            elapsedTime /= loop;
            if (t == 0) {
                minElapsedTime = elapsedTime;
                g_best_profile = g_profile;
            } else {
                if (minElapsedTime > elapsedTime) {
                    minElapsedTime = elapsedTime;
                    g_best_profile = g_profile;
                }
            }
            //minElapsedTime = t ? min(minElapsedTime, elapsedTime) : elapsedTime;
        }
    }
    /*
       __itt_task_end(domain);
       */
    cerr << "done." << endl;


    printf("++++++++++++++++++++++++++++++++++++++++\n");
    bool isDefinedArch = false;
#ifdef CPU
    printf("%25s\t%s\n", "Architecture", "CPU");
    isDefinedArch = true;
#endif
#ifdef MIC
    printf("%25s\t%s\n", "Architecture", "MIC");
    isDefinedArch = true;
#endif
#ifdef GPU
    printf("%25s\t%s\n", "Architecture", "GPU");
    isDefinedArch = true;
#endif
    assert(isDefinedArch == true);

    bool isDefinedFormat = false;
#ifdef OPT_CRS
    printf("%25s\t%s\n", "MatrixFormat", "CRS");
    isDefinedFormat = true;
#endif
#ifdef OPT_COO
    printf("%25s\t%s\n", "MatrixFormat", "COO");
    isDefinedFormat = true;
#endif
#ifdef OPT_ELL
    printf("%25s\t%s\n", "MatrixFormat", "ELL");
    isDefinedFormat = true;
#endif
#ifdef OPT_JDS
    printf("%25s\t%s\n", "MatrixFormat", "JDS");
    isDefinedFormat = true;
#endif
#ifdef OPT_DIA
    printf("%25s\t%s\n", "MatrixFormat", "DIA");
    isDefinedFormat = true;
#endif
#ifdef OPT_MKL
    printf("%25s\t%s\n", "MatrixFormat", "MKL");
    isDefinedFormat = true;
#endif
#ifdef OPT_CUSPARSE
    printf("%25s\t%s\n", "MatrixFormat", "CUSPARSE");
    isDefinedFormat = true;
#endif
    // SS
#ifdef OPT_SS
    printf("%25s\t%s\n", "MatrixFormat", "SS");
    extern vector<int> g_step_count;
    extern vector<double> g_step_time;
    printf("%25s\t%d\n", "nStep", int(g_step_count.size()));
    for (int i = 0; i < g_step_count.size(); i++) {
        printf("%22s-%02d\t%d\n", "StepCount", i, g_step_count[i]);
    }
#ifdef MEASURE_STEP_TIME
    for (int i = 0; i < g_step_count.size(); i++) {
        printf("%22s-%02d\t%lf\n", "StepTime", i, g_step_time[i]);
    }
#endif
    printf("%25s\t%d\n", "SEGMENT_WIDTH(byte)", int(SEGMENT_WIDTH*sizeof(double)));
#ifdef PADDING
    printf("%25s\t%d\n", "PADDING_WIDTH(byte)", int(PADDING_SIZE*sizeof(double)));
#endif
#ifdef PROFILING
    printf("%25s\t%lf\n", "MulPerf", nNnz*2/(g_best_profile[0]/loop)/1e9);
    printf("%25s\t%lf\n", "SumPerf", nNnz*2/(g_best_profile[1]/loop)/1e9);
#endif
    isDefinedFormat = true;
#endif
    // CSS
#ifdef OPT_CSS
    printf("%25s\t%s\n", "MatrixFormat", "CSS");
    printf("%25s\t%d\n", "N_BLOCK", N_BLOCK);
    printf("%25s\t%d\n", "SEGMENT_WIDTH(byte)", int(SEGMENT_WIDTH*sizeof(double)));
#ifdef PADDING
    printf("%25s\t%d\n", "PADDING_WIDTH(byte)", int(PADDING_SIZE*sizeof(double)));
#endif
#ifdef PROFILING
    printf("%25s\t%lf\n", "MulPerf(GFLOPS)", nNnz*2/(g_best_profile[0]/loop)/1e9);
    printf("%25s\t%lf\n", "SumPerf(GFLOPS)", nNnz*2/(g_best_profile[1]/loop)/1e9);
#endif
    isDefinedFormat = true;
#endif
    // COMMON
    assert(isDefinedFormat == true);
    printf("%25s\t%s\n", "Matrix", GetBasename(matFile).c_str());
    printf("%25s\t%s\n", "MatrixPath", matFile.c_str());
    printf("%25s\t%lf\n", "Performance(GFLOPS)", nNnz*2/minElapsedTime/1e9);
    printf("%25s\t%d\n", "nRow", nRow);
    printf("%25s\t%d\n", "nCol", nCol);
    printf("%25s\t%d\n", "nNnz", nNnz);
#pragma omp parallel
    {
#ifdef _OPENMP
#pragma omp master
        printf("%25s\t%d\n", "nThread", omp_get_num_threads());
#endif
    }
    printf("----------------------------------------\n");
    return 0;
}
