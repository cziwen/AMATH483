// multiple rider case attempt
/*
Passenger List: elevator maintains list of passengers (pairs of person_id and destination floor)
Batch Processing: elevator picks up as many passengers as possible from the queue if they are on the same floor, respecting the maximum occupancy
Route Management: elevator manages passengers' routes, moving to each passenger's destination floor and logging the necessary information
Complete Logging: detailed logs for each step including entering and exiting the elevator are added to provide comprehensive traceability
also
Occupancy Declaration: occupancy variable initialized to 0 at the start of the elevator function
Occupancy Management: increment occupancy each time a passenger enters the elevator and decrement each time a passenger exits.
*/
#ifndef ELEVATOR_HPP
#define ELEVATOR_HPP

#include <iostream>
#include <thread>
#include <mutex>
#include <queue>
#include <chrono>
#include <random>
#include <atomic>
#include <vector>
#include <condition_variable>
#include <tuple>

using namespace std;

// HELLO grader, I changed these global variables to test edge cases.
// But I forgot what was them before, I tried my best to fill them with previous values.
// SORRY for the inconvenience.

const int NUM_FLOORS = 50;
const int NUM_ELEVATORS = 6;
const int MAX_OCCUPANCY = 5;
const int MAX_WAIT_TIME = 5000; // milliseconds

mutex cout_mtx;
mutex queue_mtx;
condition_variable cv;
queue<tuple<int, int, int>> global_queue; // person_id, start_floor, dest_floor
vector<int> elevator_positions(NUM_ELEVATORS, 0);
vector<atomic<int>> elevator_occupancies(NUM_ELEVATORS); // current load per elevator
vector<int> elevator_directions(NUM_ELEVATORS, 0);       // current direction per elevator
atomic<int> num_people_serviced(0);
vector<int> global_passengers_serviced(NUM_ELEVATORS, 0);
int npeople;

void elevator(int id)
{
    int occupancy = 0;                               // Initialize occupancy for each elevator
    vector<pair<int, int>> passengers;               // pairs of (person_id, dest_floor)
    vector<tuple<int, int, int>> pending_passengers; // tuple of (person_id, start_floor, dest_floor)
    int direction = 0;                               // 0: idle, 1: up, -1: down
    int target_floor = -1;

    // initialize global occupancy and direction
    elevator_occupancies[id] = occupancy;
    elevator_directions[id] = direction;

    // 外层循环：等待或结束
    while (true)
    {
        unique_lock<mutex> lock(queue_mtx);
        cv.wait(lock, []
                { return num_people_serviced == npeople || !global_queue.empty(); });
        lock.unlock();

        if (num_people_serviced == npeople)
        {
            cv.notify_all(); // 通知其他电梯线程退出
            break;
        }

        // 内层循环：服务循环
        while (!global_queue.empty() || !passengers.empty() || !pending_passengers.empty())
        {
            // 移动到目标楼层
            if (target_floor != -1)
            {
                // 移动到目标楼层
                this_thread::sleep_for(chrono::milliseconds(rand() % MAX_WAIT_TIME));
                elevator_positions[id] = target_floor;

                // 卸客
                for (auto it = passengers.begin(); it != passengers.end();)
                {
                    auto [pid, dest] = *it;

                    // {
                    //     lock_guard<mutex> cout_lock(cout_mtx);
                    //      cout << "🚨[DEBUG] At floor " << target_floor << ", trying to drop off pid " << pid << " with dest " << dest << endl;
                    // }

                    if (dest == target_floor)
                    {
                        occupancy--;
                        elevator_occupancies[id] = occupancy;
                        {
                            lock_guard<mutex> cout_lock(cout_mtx);
                            cout << "[Elevator " << id
                                 << "] dropped off person " << pid
                                 << " at floor " << dest
                                 << " ==[occupancy: " << occupancy << "]=="
                                 << endl;
                        }
                        global_passengers_serviced[id]++;
                        num_people_serviced++;
                        it = passengers.erase(it);
                    }
                    else
                        ++it;
                }
                cv.notify_all(); // 通知其他电梯线程

                // 装客
                for (auto it = pending_passengers.begin(); it != pending_passengers.end();)
                {
                    auto [pid, start, dest] = *it;
                    // {
                    //     lock_guard<mutex> cout_lock(cout_mtx);
                    //     cout << "[[[--" << target_floor << start << "--]]]" << endl;
                    // }
                    if (start == target_floor && occupancy < MAX_OCCUPANCY)
                    {
                        occupancy++;
                        elevator_occupancies[id] = occupancy;
                        {
                            lock_guard<mutex> cout_lock(cout_mtx);
                            cout << "Person " << pid
                                 << " entered [elevator " << id
                                 << "] at floor " << start
                                 << " (dest: " << dest << ")"
                                 << " ==[occupancy: " << occupancy << "]=="
                                 << endl;
                        }
                        passengers.emplace_back(pid, dest);
                        it = pending_passengers.erase(it);
                    }
                    else
                        ++it;
                }
                cv.notify_all(); // 通知其他电梯线程
            }

            // 从 global_queue 中挑选 pending passenger
            {
                // {
                //     lock_guard<mutex> cout_lock(cout_mtx);
                //     cout << "=====[Elevator " << id << "]===== is processing the queue." << endl;
                // }
                lock_guard<mutex> qlock(queue_mtx);
                int processed = 0;
                int original_size = global_queue.size();
                queue<tuple<int, int, int>> tempq;

                while (processed < original_size && occupancy < MAX_OCCUPANCY && !global_queue.empty())
                {
                    auto req = global_queue.front();
                    global_queue.pop();
                    processed++;
                    auto [pid, start, dest] = req;
                    bool aligned = (start - elevator_positions[id]) * direction >= 0;
                    if (aligned)
                    {
                        // 在同方向电梯中，先载客最少，其次距离最近
                        int bestE = -1;
                        int bestOcc = MAX_OCCUPANCY + 1;
                        int bestDist = NUM_FLOORS + 1;
                        for (int e = 0; e < NUM_ELEVATORS; e++)
                        {
                            if ((start - elevator_positions[e]) * elevator_directions[e] < 0)
                                continue;
                            int occ = elevator_occupancies[e].load();
                            int dist = abs(elevator_positions[e] - start);
                            if (occ < bestOcc || (occ == bestOcc && dist < bestDist))
                            {
                                bestOcc = occ;
                                bestDist = dist;
                                bestE = e;
                            }
                        }
                        if (bestE == id)
                            pending_passengers.push_back(req);
                        else
                            tempq.push(req);
                    }
                    else
                        tempq.push(req);
                }
                while (!tempq.empty())
                {
                    global_queue.push(tempq.front());
                    tempq.pop();
                }
                // {
                //     lock_guard<mutex> cout_lock(cout_mtx);
                //     cout << "=====[Elevator " << id << "]===== exit the queue." << endl;
                // }
            }

            // 选择下一个目标楼层
            int curr_floor = elevator_positions[id];
            int min_distance = NUM_FLOORS + 1;
            target_floor = -1;
            for (auto &pr : passengers) // 乘客列表
            {
                auto [pid, dest] = pr;
                int dist = abs(curr_floor - dest);
                if (dist < min_distance)
                {
                    min_distance = dist;
                    target_floor = dest;
                }
            }

            if (occupancy < MAX_OCCUPANCY)
            { // 如果电梯未满才考虑 pending_passengers
                for (auto &tpl : pending_passengers)
                {
                    auto [pid, start, dest] = tpl;
                    int dist = abs(curr_floor - start);
                    if (dist < min_distance)
                    {
                        min_distance = dist;
                        target_floor = start;
                    }
                }
            }

            // 更新方向并写回全局
            if (target_floor == -1)
            {
                direction = 0;
            }
            else
            {
                if (target_floor > curr_floor)
                    direction = 1;
                else if (target_floor < curr_floor)
                    direction = -1;
                else
                    direction = 0;
            }

            elevator_directions[id] = direction;
        }
    }

    // 完成服务，打印并退出
    {
        lock_guard<mutex> cout_lock(cout_mtx);
        cout << "=====[Elevator " << id << "]===== has finished servicing all people." << endl;
        cout << "=====[Elevator " << id << "]===== serviced "
             << global_passengers_serviced[id] << " passengers." << endl;
    }
}

void person(int id)
{
    int curr_floor = rand() % NUM_FLOORS;
    int dest_floor = rand() % NUM_FLOORS;
    while (dest_floor == curr_floor)
        dest_floor = rand() % NUM_FLOORS;

    {
        lock_guard<mutex> lock(cout_mtx);
        cout << "Person " << id << " wants to go from floor "
             << curr_floor << " to floor " << dest_floor << endl;
    }
    {
        lock_guard<mutex> qlock(queue_mtx);
        global_queue.emplace(id, curr_floor, dest_floor);
    }
    cv.notify_all();
}

#endif // ELEVATOR_HPP
