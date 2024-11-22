#include "plagiarism_checker.hpp"
// You should NOT add ANY other includes to this file.
// Do NOT add "using namespace std;".

// TODO: Implement the methods of the plagiarism_checker_t class

plagiarism_checker_t::plagiarism_checker_t() : stop_thread(false) {
    worker_thread = std::thread(&plagiarism_checker_t::worker_thread_function, this);
}

plagiarism_checker_t::plagiarism_checker_t(std::vector<std::shared_ptr<submission_t>> __submissions)
    : plagiarism_checker_t() {
    base_submissions = std::move(__submissions);
    for (const auto& submission : base_submissions) {
        timestamps[submission->id] = std::chrono::time_point<std::chrono::high_resolution_clock>();
    }
}

plagiarism_checker_t::~plagiarism_checker_t() {
    {
        std::lock_guard<std::mutex> lock(mtx);
        stop_thread = true;
    }
    cv.notify_all();
    if (worker_thread.joinable())
        worker_thread.join();
}

void plagiarism_checker_t::add_submission(std::shared_ptr<submission_t> __submission) {
    auto timestamp = std::chrono::high_resolution_clock::now();
    {
        std::lock_guard<std::mutex> lock(mtx);
        submission_queue.push(__submission);
        timestamps[__submission->id] = timestamp;
    }
    cv.notify_one();
}

void plagiarism_checker_t::worker_thread_function() {
    while (true) {
        std::shared_ptr<submission_t> submission;
        {
            std::unique_lock<std::mutex> lock(mtx);
            cv.wait(lock, [this] { return !submission_queue.empty() || stop_thread; });
            if (stop_thread && submission_queue.empty())
                return;

            submission = submission_queue.front();
            submission_queue.pop();
        }

        // Tokenize the new submission
        tokenizer_t tokenizer(submission->codefile);
        std::vector<int> new_tokens = tokenizer.get_tokens();

        // Compare with pre-existing submissions
        for (const auto& existing_submission : base_submissions) {
            tokenizer_t existing_tokenizer(existing_submission->codefile);
            std::vector<int> existing_tokens = existing_tokenizer.get_tokens();

            // Detect plagiarism
            int matching_tokens = 0;
            int matching_patterns = 0;

            // Implement token matching logic here
            // For example, compare tokens from both vectors
            auto new_it = new_tokens.begin();
            auto existing_it = existing_tokens.begin();
            while (new_it != new_tokens.end() && existing_it != existing_tokens.end()) {
                if (*new_it == *existing_it) {
                    matching_tokens++;
                    // Assume "patterns" are sequences of matching tokens
                    if ((new_it + 1 != new_tokens.end() && existing_it + 1 != existing_tokens.end()) &&
                        *(new_it + 1) == *(existing_it + 1)) {
                        matching_patterns++;
                    }
                }
                ++new_it;
                ++existing_it;
            }

            if (matching_tokens >= 75 && matching_patterns >= 10) {
                submission->student->flag_student(submission);
                submission->professor->flag_professor(submission);
                break;
            }
        }
    }
}


// End TODO