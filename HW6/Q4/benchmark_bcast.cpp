// benchmark_bcast.cpp
#include <mpi.h>
#include <vector>
#include <iostream>
#include <iomanip>
#include <cmath>
#include "my_broadcast.hpp"

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    const int root = 0;
    const int num_trials = 10;

    // 从 8B 开始，每次 *2，一直到 256MB
    std::vector<size_t> msg_sizes;
    for (size_t b = 8; b <= 256*1024*1024; b *= 2) {
        msg_sizes.push_back(b);
    }

    if (rank == 0) {
        std::cout << "# procs=" << size
                  << "  size[B]  bw_my[MB/s]  bw_mpi[MB/s]\n";
    }

    for (size_t bytes : msg_sizes) {
        int count = bytes / sizeof(double);  // 这里以 double 为例
        if (count == 0) count = 1;

        std::vector<double> buffer(count);
        if (rank == root) {
            // root 初始化数据
            for (int i = 0; i < count; ++i) buffer[i] = i * 1.0;
        }

        // 1) 测试 my_broadcast
        MPI_Barrier(MPI_COMM_WORLD);
        double t0 = MPI_Wtime();
        for (int t = 0; t < num_trials; ++t) {
            my_broadcast(buffer.data(), count, root, MPI_COMM_WORLD);
        }
        MPI_Barrier(MPI_COMM_WORLD);
        double t_my = (MPI_Wtime() - t0) / num_trials;

        // 2) 测试 MPI_Bcast
        MPI_Barrier(MPI_COMM_WORLD);
        t0 = MPI_Wtime();
        for (int t = 0; t < num_trials; ++t) {
            MPI_Bcast(
                buffer.data(),
                count,
                MPI_DOUBLE,
                root,
                MPI_COMM_WORLD
            );
        }
        MPI_Barrier(MPI_COMM_WORLD);
        double t_mpi = (MPI_Wtime() - t0) / num_trials;

        // 计算带宽：字节/秒 -> MB/s
        double bw_my  = (bytes / t_my)  / (1024*1024);
        double bw_mpi = (bytes / t_mpi) / (1024*1024);

        if (rank == 0) {
            std::cout << std::setw(9) << bytes
                      << std::fixed << std::setprecision(1)
                      << std::setw(12) << bw_my
                      << std::setw(12) << bw_mpi
                      << "\n";
        }
    }

    MPI_Finalize();
    return 0;
}