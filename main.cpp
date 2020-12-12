#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <exception>
#include <fstream>
#include <iostream>
#include <iterator>
#include <limits>
#include <stdexcept>
#include <unordered_set>
#include <vector>

namespace
{
	constexpr const char* INPUT_FILE_NAME = "input.txt";
	constexpr const std::size_t PREAMBLE_SIZE = 25;

	using Input = std::uint64_t;
	using InputList = std::vector<Input>;
	using InputCache = std::unordered_set<Input>;
	using InputReference = std::pair<Input, InputCache::const_iterator>;
	using InputReferenceCache = std::vector<InputReference>;
	using InputReferenceCachePosition = InputReferenceCache::size_type;

	InputList inputList{};

	InputCache inputCache(PREAMBLE_SIZE);

	InputReferenceCache inputReferenceCache(PREAMBLE_SIZE);
}

bool isInputComputableAsCacheSum(Input input)
{
	return inputReferenceCache.cend() != std::find_if(inputReferenceCache.cbegin(),
		                                              inputReferenceCache.cend(),
		                                              [&input](const auto& inputReference)
		                                              {
		                                                  const auto differenceItr = inputCache.find(input - inputReference.first);
		                                                  return differenceItr != inputCache.cend() && differenceItr != inputReference.second;
		                                              });
}

bool isPreambleComplete()
{
	return inputCache.size() == PREAMBLE_SIZE;
}

auto& getNextInputReferenceLocation(Input nextInput)
{
	static InputReferenceCachePosition nextInputPosition{};

	auto& nextInputReference = inputReferenceCache[nextInputPosition];

	++nextInputPosition;
	if (nextInputPosition == PREAMBLE_SIZE)
	{
		nextInputPosition = 0;
	}

	nextInputReference.first = nextInput;
	return nextInputReference.second;
}

Input findWeakInput(std::ifstream& inStream)
{
	// perhaps much more, but at least this much
	//
	inputList.reserve(PREAMBLE_SIZE);

	Input weakInput{};
	Input nextInput{};

	while (inStream >> nextInput)
	{
		inputList.emplace_back(nextInput);

		auto& nextInputReferenceLocation = getNextInputReferenceLocation(nextInput);
        if (!isPreambleComplete())
        {
        	nextInputReferenceLocation = inputCache.emplace(nextInput).first;
		}
        else
        {
        	if (!weakInput && !isInputComputableAsCacheSum(nextInput))
        	{
        		weakInput = nextInput;
        	}

        	auto extractedCacheHandle = inputCache.extract(nextInputReferenceLocation);
			extractedCacheHandle.value() = nextInput;

			nextInputReferenceLocation = inputCache.insert(std::move(extractedCacheHandle)).position;
        }
	}

	if (!weakInput)
	{
		throw std::runtime_error{"Unable to find any weak input"};
	}

	return weakInput;
}

std::pair<Input, Input> findMinMax(InputList::const_iterator begin, InputList::const_iterator end)
{
	Input max{};
	Input min = std::numeric_limits<Input>::max();

	while (begin != end)
	{
		if (*begin < min)
		{
			min = *begin;
		}

		if (*begin > max)
		{
			max = *begin;
		}

		++begin;
	}

	return std::make_pair(min, max);
}

Input findWeakMinMaxSum(Input target)
{	
	Input sum{};
	for (auto rightItr = inputList.cbegin(), leftItr = rightItr; rightItr != inputList.cend();)
	{
		if (sum < target)
		{
			sum += *rightItr++;
		}
		else
		{
			while (sum > target && leftItr != rightItr)
			{
				sum -= *leftItr++;
			}

			if (sum == target)
			{
				const auto [min, max] = findMinMax(leftItr, rightItr);
				return min + max;
			}
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

		const auto weakMinMaxSum = findWeakMinMaxSum(weakInput);
		std::cout << "Weak minmax sum = " << weakMinMaxSum << '\n';
	}
	catch (const std::exception& ex)
	{
		std::cerr << "Fatal error: " << ex.what() << '\n';
		return -1;
	}

	return 0;
}
