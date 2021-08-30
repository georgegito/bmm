/* -------------------------------------------------------------------------- */
/*                                  main.cpp                                  */
/* -------------------------------------------------------------------------- */

#include <iostream>
#include <cstdlib>
#include <cstdbool>
#include <iostream>
#include <fstream>
#include <sys/time.h>
#include <unistd.h>
#include <bits/stdc++.h>
#include <mpi.h>

#include <headers.hpp>
#include <bmm.cpp>
#include <blocking.cpp>
#include <block-bmm.cpp>
#include <masked-block-bmm.cpp>
// #include <parallel-masked-block-bmm.cpp>
#include <distributed-block-bmm.cpp>
#include <utils.cpp>
#include <reader.cpp>

int main(int argc, char **argv)
{
    /* -------------------------------------------------------------------------- */
    /*                        OpenMPI bmm distribution test                       */
    /* -------------------------------------------------------------------------- */

    int numProcesses, pId;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &numProcesses);
    MPI_Comm_rank(MPI_COMM_WORLD, &pId);
    MPI_Status stat;

    if(pId == 1) {
        
        struct timeval timer;
        double t = -1;

        /* ------------------------------- read matrix ------------------------------ */

        int n;
        int nnz;

        std::string graph = "com-Youtube.mtx";
        std::string file = "graphs/" + graph;

        readMtxValues(file, n, nnz);

        coo M;
        util::initCoo(M, n, nnz);

        openMtxFile(file, M.col, M.row, M.n, M.nnz);

        csr A;
        util::initCsr(A, n, nnz);
        csc B;
        util::initCsc(B, n, nnz);

        // prt::cooMat(M);

        coo2csr(A.rowPtr, A.colInd, M.row, M.col, A.nnz, A.n, 0);
        coo2csr(B.colPtr, B.rowInd, M.col, M.row, B.nnz, B.n, 0);

        util::delCoo(M);

        // prt::csrMat(A);
        // prt::cscMat(B);

        std::cout << "\nMatrix read successfully\nn = " << A.n << ", nnz = " << A.nnz << std::endl;

        /* ----------------------------------- s12 ---------------------------------- */

        // int b = 2;
        // int b = 3;
        // int b = 4;
        // int b = 6;

        /* ------------------------------- com-Youtube ------------------------------ */

        // int b = 226978;
        int b = 113489;
        
        /* -------------------------------- dblp-2010 ------------------------------- */

        // int b = 46598;
        // int b = 23299;
        // int b = 14182;
        // int b = 7091;
        // int b = 2026;
        // int b = 1013;

        /* ------------------------------- as-Skitter ------------------------------- */

        // int b = 242345;
        // int b = 89285;
        // int b = 48469;
        // int b = 17857;
        // int b = 12755;
        // int b = 2551;

        /* ------------------------------- belgium_osm ------------------------------ */

        // int b = 62665;

        /* --------------------------- bcsr blocking test --------------------------- */
        
        timer = util::tic();
        int numBlocks = (n / b) * (n / b);
        int LL_bRowPtrSize = numBlocks * (b + 1);

        bcsr blA;
        blA.n = A.n;
        blA.b = b;

        // init Low-Level CSR
        blA.LL_bRowPtr = new int[LL_bRowPtrSize]();
        blA.LL_bColInd = new int[nnz]();

        // blocking
        ret _ret = csr2bcsr(A, blA);

        blA.HL_bRowPtr = _ret.ret1;
        blA.HL_bColInd = _ret.ret2;
        blA.nzBlockIndex = _ret.ret3;
        blA.blockNnzCounter = _ret.ret4;

        t = util::toc(timer);
        std::cout << "\nBlocking A in B-CSR completed\n" << "Blocking time = " << t << " seconds" << std::endl;

        /* --------------------------- bcsc blocking test --------------------------- */

        // std::cout << "\nBlocking B in B-CSC...\n";

        timer = util::tic();

        int LL_bColPtrSize = numBlocks * (b + 1);

        bcsc blB;
        blB.n = A.n;
        blB.b = b;

        // init Low-Level CSC
        blB.LL_bColPtr = new int[LL_bColPtrSize]();
        blB.LL_bRowInd = new int[nnz]();

        // blocking
        _ret = csr2bcsc(B, blB);

        blB.HL_bColPtr = _ret.ret1;
        blB.HL_bRowInd = _ret.ret2;
        blB.nzBlockIndex = _ret.ret3;
        blB.blockNnzCounter = _ret.ret4;

        t = util::toc(timer);
        std::cout << "\nBlocking B in B-CSC completed\n" << "Blocking time = " << t << " seconds" << std::endl;

        
    }

    MPI_Finalize();

    /* ----------------------------- block bmm test ----------------------------- */

    // timer = util::tic();

    // std::multimap<int, int> C;

    // // blockBmm(blA, blB, C);
    // maskedBlockBmm(blA, blA, blB, C);
    // // ret2 ans = parallelMaskedBlockBmm(blA, blA, blB);

    // t = util::toc(timer);
    // std::cout << "\nBlock-BMM completed\n" << "Block-BMM time = " << t << " seconds" << std::endl;

    // std::vector<std::pair<int, int>> vecC;

    // for (const auto& x : C) {
    //   vecC.push_back(std::pair<int, int> (x.first, x.second));
    // }
    // std::sort(vecC.begin(), vecC.end());

    // // prt::vec(vecC);

    // /* ------------------------------ check result ------------------------------ */

    // if (util::checkRes(graph, vecC)) {
    //   std::cout << "\nTest passed\n";
    // }
    // else {
    //   std::cout << "\nTest failed\n";
    // }

    // /* ------------------------------- free memory ------------------------------ */

    // util::delCsr(A);
    // util::delCsc(B);
    // util::delBcsr(blA); 
    // util::delBcsc(blB);

  return 0;
}