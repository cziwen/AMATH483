#ifndef MY_BROADCAST_HPP
#define MY_BROADCAST_HPP

#include <mpi.h>

/**
 * @brief 用 MPI 点对点通信实现 broadcast
 * @tparam T      数据元素类型
 * @param data    待广播的数据缓冲区（根进程保存原始，其他进程接收后放入此缓冲区）
 * @param count   元素个数
 * @param root    广播源 rank
 * @param comm    MPI communicator
 */
template<typename T>
void my_broadcast(T* data, int count, int root, MPI_Comm comm) {
    int rank, size;
    MPI_Comm_rank(comm, &rank);
    MPI_Comm_size(comm, &size);
    const int TAG = 0;

    if (rank == root) {
        // 根进程：向所有非 root 的进程发送
        for (int dest = 0; dest < size; ++dest) {
            if (dest == root) continue;
            MPI_Send(
                static_cast<void*>(data),
                count * sizeof(T),
                MPI_BYTE,
                dest,
                TAG,
                comm
            );
        }
    } else {
        // 非根进程：从 root 接收
        MPI_Status status;
        MPI_Recv(
            static_cast<void*>(data),
            count * sizeof(T),
            MPI_BYTE,
            root,
            TAG,
            comm,
            &status
        );
    }
}

#endif // MY_BROADCAST_HPP