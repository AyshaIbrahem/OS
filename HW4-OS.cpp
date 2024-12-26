#include <iostream>
#include <vector>
#include <algorithm>
#include <fstream>
#include <string>
#include <sstream>
#include <queue>
#include <map>
#include <climits>

using namespace std;

// Process control block
struct pcb {
    string name;
    int arrival_time, burst_time, start_time, finish_time;
    int original_burst_time; // Preserve the original burst time for calculations
};

int processes_count, context_switch_time, time_quantum;
const int SLEEPING_INTERVAL = 0;
const string CONTEXT_SWITCH = "CS", IDLE = "Idle";


void processes_input(vector<pcb> &processes) {
    ifstream input("processes.txt");
    if (!input) {
        cerr << "Error: Unable to open input file.\n";
        exit(1);
    }

    string line;
    while (getline(input, line)) {
        if (line.substr(0, 20) == "Number of processes:") {
            stringstream stream(line.substr(20));
            stream >> processes_count;

            if (processes_count <= 0) {
                cerr << "Invalid number of processes.\n";
                exit(1);
            }

            for (int i = 0; i < processes_count; i++) {
                pcb p;
                p.name = "P" + to_string(i + 1);
                processes.push_back(p);
            }
        }

        if (line.substr(0, 14) == "Arrival times:") {
            stringstream stream(line.substr(14));
            for (int i = 0; i < processes_count; i++)
                stream >> processes[i].arrival_time;
        }

        if (line.substr(0, 11) == "CPU bursts:") {
            stringstream stream(line.substr(11));
            for (int i = 0; i < processes_count; i++) {
                stream >> processes[i].burst_time;
                processes[i].original_burst_time = processes[i].burst_time;

                if (processes[i].burst_time <= 0) {
                    cerr << "Invalid CPU burst time for process " << processes[i].name << ".\n";
                    exit(1);
                }
            }
        }

        if (line.substr(0, 35) == "Context switch time (milliseconds):") {
            stringstream stream(line.substr(35));
            stream >> context_switch_time;

            if (context_switch_time < 0) {
                cerr << "Invalid context switch time.\n";
                exit(1);
            }
        }

        if (line.substr(0, 29) == "Time quantum for Round Robin:") {
            stringstream stream(line.substr(29));
            stream >> time_quantum;

            if (time_quantum <= 0) {
                cerr << "Invalid time quantum.\n";
                exit(1);
            }
        }
    }
}

bool by_arrival_time(const pcb &process1, const pcb &process2) {
    return process1.arrival_time < process2.arrival_time;
}

bool by_name(const pcb &process1, const pcb &process2) {
    return process1.name < process2.name;
}

void context_switch(int &time, vector<string> &timeline) {
    for (int interval = 0; interval < context_switch_time; interval++) {
        time++;
        timeline.push_back(CONTEXT_SWITCH);
    }
}

vector<string> get_first_come_first_served_timeline(vector<pcb> processes) {
    vector<string> timeline;
    queue<int> ready_queue;

    for (int time = 0, index = 0;; time++) {
        if (index < processes_count && time >= processes[index].arrival_time) {
            ready_queue.push(index);
            index++;
        }

        if (ready_queue.empty()) {
            if (index >= processes_count) break;
            timeline.push_back(IDLE);
            continue;
        }

        processes[ready_queue.front()].burst_time--;
        timeline.push_back(processes[ready_queue.front()].name);

        if (processes[ready_queue.front()].burst_time == 0) {
            ready_queue.pop();

            if (index == processes_count && ready_queue.empty()) break;
            context_switch(time, timeline);
        }
    }

    return timeline;
}

void set_start_and_finish_time(vector<pcb> &processes, vector<string> timeline) {
    for (int i = 0; i < processes_count; i++) {
        for (int j = 0; j < timeline.size(); j++) {
            if (processes[i].name == timeline[j]) {
                processes[i].start_time = j;
                break;
            }
        }

        for (int j = timeline.size() - 1; j >= 0; j--) {
            if (processes[i].name == timeline[j]) {
                processes[i].finish_time = j + 1;
                break;
            }
        }
    }
}

void print_gantt_chart(vector<string> timeline) {
    cout << "Gantt Chart:\n|";
    for (const string &event : timeline) {
        cout << " " << event << " |";
    }
    cout << "\n\n";
}

void calculate_and_print_results(vector<pcb> processes, vector<string> timeline) {
    set_start_and_finish_time(processes, timeline);
    sort(processes.begin(), processes.end(), by_name);

    print_gantt_chart(timeline);

    cout << "Finish time for each process:\n";
    for (const auto &process : processes)
        cout << process.name << ": " << process.finish_time << endl;

    cout << "\nWaiting time for each process:\n";
    for (const auto &process : processes)
        cout << process.name << ": " << process.finish_time - process.original_burst_time - process.arrival_time << endl;

    cout << "\nTurnaround time for each process:\n";
    for (const auto &process : processes)
        cout << process.name << ": " << process.finish_time - process.arrival_time << endl;

    cout << "\n*****************************************************\n";
}

int main() {
    vector<pcb> processes;
    processes_input(processes);
    sort(processes.begin(), processes.end(), by_arrival_time);

    vector<string> fcfs_timeline = get_first_come_first_served_timeline(processes);
    calculate_and_print_results(processes, fcfs_timeline);

    // Additional functions for RR and SRT can be added here if required.
    return 0;
}
