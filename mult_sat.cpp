#include <iostream>
#include <string>
#include <vector>
#include <iomanip>

// Structure to represent a single digit multiplication step
struct DigitMultiplication {
    int digit1;         // digit from first number
    int digit2;         // digit from second number
    int product;        // result of digit1 * digit2
    int carry;          // carry value for next position
    int position;       // position in the final result
};

// Structure to hold the complete multiplication state
struct MultiplicationState {
    std::vector<int> number1_digits;  // digits of first number
    std::vector<int> number2_digits;  // digits of second number
    std::vector<DigitMultiplication> steps;  // individual multiplication steps
    std::vector<int> result_digits;   // digits of final result
    std::vector<int> carries;         // carries between positions
};

// Convert a number to vector of digits
std::vector<int> numberToDigits(int number) {
    std::vector<int> digits;
    if (number == 0) {
        digits.push_back(0);
        return digits;
    }
    while (number > 0) {
        digits.insert(digits.begin(), number % 10); // insert at the beginning
        number /= 10;
    }
    return digits;
}

// Perform multiplication of two digits and handle carry
DigitMultiplication multiplyDigits(int d1, int d2, int position) {
    DigitMultiplication step;
    step.digit1 = d1;
    step.digit2 = d2;
    step.product = d1 * d2; // modulo 10 ?
    step.carry = step.product / 10;
    step.position = position;
    return step;
}

// Print the multiplication state
void printMultiplicationState(const MultiplicationState& state) {
    std::cout << "\nMultiplication Steps:\n";
    std::cout << "First number:  ";
    for (int d : state.number1_digits) std::cout << d;
    std::cout << "\nSecond number: ";
    for (int d : state.number2_digits) std::cout << d;
    std::cout << "\n\nSteps:\n";

    for (const auto& step : state.steps) {
        std::cout << "Position " << std::setw(2) << step.position << ": "
                  << step.digit1 << " × " << step.digit2 << " = " << step.product
                  << " (carry: " << step.carry << ")\n";
    }

    std::cout << "\nResult: ";
    bool leadingZero = true;
    for (int d : state.result_digits) {
        if (d != 0 || !leadingZero) {
            std::cout << d;
            leadingZero = false;
        }
    }
    if (leadingZero) std::cout << "0";
    std::cout << std::endl;
}

int main(int argc, char* argv[]) {
    // Check if exactly one argument is provided
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <integer>" << std::endl;
        return 1;
    }

    // Convert string argument to integer
    try {
        int number = std::stoi(argv[1]);

        // Create multiplication state
        MultiplicationState state;
        state.number1_digits = numberToDigits(number);
        state.number2_digits = numberToDigits(number); // Multiply by itself for now

        // Initialize result vectors
        int max_position = state.number1_digits.size() + state.number2_digits.size();
        state.carries.resize(max_position, 0);
        state.result_digits.resize(max_position, 0);

        // Perform digit-by-digit multiplication
        for (uint i = 0; i <= state.number1_digits.size() - 1; i++) {
            for (uint j = 0; j <= state.number2_digits.size() - 1; j++) {
                int position = i + j + 1;

                // Multiply digits and store step
                DigitMultiplication step = multiplyDigits(
                    state.number1_digits[i],
                    state.number2_digits[j],
                    position
                );
                state.steps.push_back(step);

                // Add result and carry to position
                int current = state.result_digits[position] + (step.product % 10) + state.carries[position];
                state.result_digits[position] = current % 10;
                state.carries[position - 1] += step.carry + (current / 10);
            }
        }

        // Handle final carries
        for (uint i = max_position - 1; i > 0; i--) {
            if (state.carries[i] > 0) {
                state.result_digits[i] += state.carries[i] % 10;
                state.carries[i-1] += state.carries[i] / 10;
            }
        }
        state.result_digits[0] += state.carries[0];

        // Print the multiplication process
        printMultiplicationState(state);

        // Verify the result
        long long calculated_result = 0;
        for (uint i = 0; i < state.result_digits.size(); i++) {
            calculated_result = calculated_result * 10 + state.result_digits[i];
        }
        long long expected_result = static_cast<long long>(number) * number;

        std::cout << "\nVerification:" << std::endl;
        std::cout << "Calculated result: " << calculated_result << std::endl;
        std::cout << "Expected result:   " << expected_result << std::endl;
        if (calculated_result == expected_result) {
            std::cout << "✓ Results match!" << std::endl;
        } else {
            std::cout << "✗ Results don't match!" << std::endl;
        }

    } catch (const std::invalid_argument& e) {
        std::cerr << "Error: Please provide a valid integer" << std::endl;
        return 1;
    } catch (const std::out_of_range& e) {
        std::cerr << "Error: Number out of range" << std::endl;
        return 1;
    }

    return 0;
}
