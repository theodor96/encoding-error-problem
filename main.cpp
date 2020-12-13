#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <exception>
#include <fstream>
#include <iostream>
#include <iterator>
#include <stdexcept>
#include <unordered_set>
#include <vector>

namespace
{
    constexpr const char* INPUT_FILE_NAME = "input.txt";
    constexpr const std::size_t PREAMBLE_SIZE = 25;

    using Input = std::uint64_t;
    using InputCache = std::unordered_set<Input>;
    using InputReference = std::pair<Input, InputCache::const_iterator>;
    using InputReferenceList = std::vector<InputReference>;

    InputCache inputCache(PREAMBLE_SIZE);

    InputReferenceList inputReferenceList{};
}

bool isInputComputableAsCacheSum(Input input)
{
    return inputReferenceList.cend() !=
                   std::find_if(std::prev(inputReferenceList.cend(), PREAMBLE_SIZE),
                                inputReferenceList.cend(),
                                [&input](const auto& inputReference)
                                {
                                    if (inputReference.first * 2 == input)
                                    {
                                        return false;
                                    }

                                    return inputCache.find(input - inputReference.first) != inputCache.cend();
                                });
}

bool isPreambleComplete()
{
    return inputCache.size() == PREAMBLE_SIZE;
}

auto getUpdatedNextInputCacheLocation(Input nextInput)
{
    auto extractedCacheHandle = inputCache.extract(std::prev(inputReferenceList.cend(), PREAMBLE_SIZE)->second);
    extractedCacheHandle.value() = nextInput;

    return inputCache.insert(std::move(extractedCacheHandle)).position;
}

Input findWeakInput(std::ifstream& inStream)
{
    Input weakInput{};
    Input nextInput{};

    inputReferenceList.reserve(PREAMBLE_SIZE);
    while (inStream >> nextInput)
    {
        if (!weakInput && isPreambleComplete() && !isInputComputableAsCacheSum(nextInput))
        {
            weakInput = nextInput;
        }

        inputReferenceList.emplace_back(nextInput,
                                        isPreambleComplete() ? getUpdatedNextInputCacheLocation(nextInput)
                                                             : inputCache.emplace(nextInput).first);
    }

    if (!weakInput)
    {
        throw std::runtime_error{"Unable to find any weak input"};
    }

    return weakInput;
}

Input findMinMaxSum(Input target)
{
    auto leftItr = inputReferenceList.cbegin();
    auto rightItr = inputReferenceList.cbegin();

    Input sum{};
    while (rightItr != inputReferenceList.cend())
    {
        while (sum < target && rightItr != inputReferenceList.cend())
        {
            sum += rightItr++->first;
        }

        while (sum > target && leftItr != rightItr)
        {
            sum -= leftItr++->first;
        }

        if (sum == target)
        {
            const auto [minItr, maxItr] = std::minmax_element(leftItr,
                                                              rightItr,
                                                              [](const auto& lhs, const auto& rhs)
                                                              {
                                                                  return lhs.first < rhs.first;
                                                              });
            return minItr->first + maxItr->first;
        }
    }

    throw std::runtime_error{"Unable to find weak sum for given target"};
}

int main()
{
    try
    {
        std::ifstream inStream{INPUT_FILE_NAME};

        const auto weakInput = findWeakInput(inStream);
        std::cout << "Weak input = " << weakInput << '\n';
        inStream.close();

        std::cout << "Minmax sum = " << findMinMaxSum(weakInput) << '\n';
    }
    catch (const std::exception& ex)
    {
        std::cerr << "Fatal error: " << ex.what() << '\n';
        return -1;
    }

    return 0;
}
