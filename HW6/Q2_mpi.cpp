#include <mpi.h>
#include <cmath>
#include <vector>
#include <iostream>
#include <cstdlib>
#include <iomanip>
#include <chrono>


double f_prime(double x) {
    return 1.0 / x - 0.25 * x;
}

double integrand(double x) {
    double fp = f_prime(x);
    return std::sqrt(1 + fp * fp);
}

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);

    int world_rank, world_size;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    if (argc != 2) {
        if (world_rank == 0)
            std::cerr << "Usage: " << argv[0] << " <num_points>" << std::endl;
        MPI_Finalize();
        return 1;
    }

    long long n = atoll(argv[1]);  // Number of points
    double a = 1.0, b = 6.0;
    double h = (b - a) / n;

    // Start timing (only on root, but barrier to sync)
    MPI_Barrier(MPI_COMM_WORLD);
    auto t0 = std::chrono::high_resolution_clock::now();

    // Determine local range
    long long local_n = n / world_size;
    long long start = world_rank * local_n;
    long long end = (world_rank == world_size - 1) ? n : start + local_n;

    // Compute local Riemann sum
    double local_sum = 0.0;
    for (long long i = start; i < end; ++i) {
        double x = a + (i + 0.5) * h;
        local_sum += integrand(x) * h;
    }

    // Reduce all partial results to the root
    double total_sum = 0.0;
    MPI_Reduce(&local_sum, &total_sum, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

    MPI_Barrier(MPI_COMM_WORLD);
    auto t1 = std::chrono::high_resolution_clock::now();
    double elapsed = std::chrono::duration<double>(t1 - t0).count();

    if (world_rank == 0) {
        const double exact = 6.16676;
        double error = std::abs(total_sum - exact);
        double log_error = std::log10(error);

        std::cout << std::fixed << std::setprecision(8);
        std::cout << "Length ≈ " << total_sum << "\n";
        std::cout << "Time   = " << elapsed << " s\n";
        std::cout << "Error  = " << error << "\n";
        std::cout << "log10(error) = " << log_error << "\n";
    }

    MPI_Finalize();
    return 0;
}