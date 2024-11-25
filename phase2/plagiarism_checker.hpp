#include "structures.hpp"
// -----------------------------------------------------------------------------

#include <thread>
#include <mutex>
#include <condition_variable>
#include <vector>
#include <map>
#include <queue>
#include <chrono>

// You are free to add any STL includes above this comment, below the --line--.
// DO NOT add "using namespace std;" or include any other files/libraries.
// Also DO NOT add the include "bits/stdc++.h"

// OPTIONAL: Add your helper functions and classes here

class plagiarism_checker_t {
    // You should NOT modify the public interface of this class.
public:
    plagiarism_checker_t(void);
    plagiarism_checker_t(std::vector<std::shared_ptr<submission_t>> 
                            __submissions);
    ~plagiarism_checker_t(void);
    void add_submission(std::shared_ptr<submission_t> __submission);

protected:
    // TODO: Add members and function signatures here
    void worker_thread_function();

    std::vector<std::shared_ptr<submission_t>> base_submissions;
    std::map<long, std::chrono::time_point<std::chrono::high_resolution_clock>> timestamps;
    std::queue<std::shared_ptr<submission_t>> submission_queue;
    std::mutex mtx;
    std::condition_variable cv;
    bool stop_thread = false;
    std::thread worker_thread;
    // End TODO
};