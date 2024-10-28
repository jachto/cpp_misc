#include <iostream>
#include <vector>
#include <cmath>
#include <thread>
#include <future>
#include <mutex>
#include <algorithm>

struct WorkerResult {
    std::vector<long long> factors;
    long long remaining_n;
};

// Mutex for synchronizing access to shared resources
std::mutex factors_mutex;

// Worker function that will check a specific range of factors
WorkerResult factorize_worker(long long n, long long start, long long end) {
    WorkerResult result;
    result.remaining_n = n;

    long long sqrt_n = static_cast<long long>(std::sqrt(n));

    // Check factors in our assigned range
    for (long long i = start; i <= end && i <= sqrt_n; i += 2) {
      // std::cout << "res.n = " << result.remaining_n << ", i = " << i << std::endl;
        while (result.remaining_n % i == 0) {
            result.factors.push_back(i);
            result.remaining_n /= i;
        }
    }
    if ((end >= sqrt_n) && (result.remaining_n > sqrt_n)) {
      result.factors.push_back(result.remaining_n);
    }
    return result;
}

// Function to merge and sort all factors
std::vector<long long> process_number(long long n, int num_workers) {
    std::vector<long long> final_factors;

    long long n_orig = n;

    // Handle negative numbers first
    if (n < 0) {
        final_factors.push_back(-1);
        n = -n;
    }

    // Handle 2 as a special case first
    while (n % 2 == 0) {
        final_factors.push_back(2);
        n = n / 2;
    }

    if (n == 1) return final_factors;

    // Calculate the square root once
    long long sqrt_n = static_cast<long long>(std::sqrt(n));

    // Calculate range size for each worker
    long long range_size = (sqrt_n + 1) / num_workers;

    // Make range_size even as we step by 2 in the division test.
    range_size = (range_size % 2 == 0) ? range_size : range_size + 1;

    // Create promises and futures for each worker
    std::vector<std::promise<WorkerResult>> promises(num_workers);
    std::vector<std::future<WorkerResult>> futures;
    std::vector<std::thread> workers;

    // std::cout << " n = " << n << std::endl;
    // std::cout << " range_size = " << range_size << std::endl;
    // Launch workers
    for (int i = 0; i < num_workers; ++i) {
        long long start = 3 + i * range_size;
        long long end = (i == num_workers - 1) ? (sqrt_n + 1) : start + range_size - 2;

        // std::cout << " start = " << start << ", end = " << end << std::endl;
        futures.push_back(promises[i].get_future());

        workers.emplace_back([n, start, end, &promise = promises[i]]() {
            try {
                WorkerResult result = factorize_worker(n, start, end);
                promise.set_value(result);
            } catch (...) {
                promise.set_exception(std::current_exception());
            }
        });
    }

    // Collect results
    //    long long remaining_n = n;
    for (int i = 0; i < num_workers; ++i) {
        WorkerResult result = futures[i].get();
        final_factors.insert(final_factors.end(),
                           result.factors.begin(),
                           result.factors.end());
        //        remaining_n = std::min(remaining_n, result.remaining_n);
    }

    // Join all worker threads
    for (auto& worker : workers) {
        worker.join();
    }

    // Sort the factors
    std::sort(final_factors.begin(), final_factors.end());

    long long result = 1;
    for (uint i = 0; i < final_factors.size(); i++) {
      // std::cout << i << " factor is = " << final_factors[i];
      if (result == n_orig) {
        final_factors.resize(i);
        // std::cout << std::endl;
        break;
      }
      result *= final_factors[i];
      // std::cout << ", res = " << result << std::endl;
    }
    return final_factors;
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <integer> <num_workers>" << std::endl;
        return 1;
    }

    try {
        long long number = std::stoll(argv[1]);
        int num_workers = std::stoi(argv[2]);

        if (num_workers < 1) {
            std::cerr << "Number of workers must be at least 1" << std::endl;
            return 1;
        }

        std::cout << "Starting factorization with " << num_workers << " workers..." << std::endl;

        // Process the number with multiple workers
        std::vector<long long> factors = process_number(number, num_workers);

        // Print the result
        std::cout << number << " = ";
        for (size_t i = 0; i < factors.size(); ++i) {
            std::cout << factors[i];
            if (i < factors.size() - 1) {
                std::cout << " × ";
            }
        }
        std::cout << std::endl;


    } catch (const std::invalid_argument& e) {
        std::cerr << "Error: Please provide a valid integer" << std::endl;
        return 1;
    } catch (const std::out_of_range& e) {
        std::cerr << "Error: Number out of range" << std::endl;
        return 1;
    } catch (const std::exception& e) {
        std::cerr << "Error during factorization: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
