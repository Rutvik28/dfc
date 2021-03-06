#ifndef DFC_SEGMENT_EXTENDER_HPP
#define DFC_SEGMENT_EXTENDER_HPP

#include <array>
#include <cstring>
#include <limits>
#include <optional>
#include <vector>

#include "byte.hpp"
#include "pattern.hpp"

namespace dfc {
template <typename SegmentType>
class SegmentExtender {
  static int const segmentSize = sizeof(SegmentType);
  // if we allow 2 bytes, we're already at 16k permutations
  static int const allowedBytesToExtend = 1;

  static constexpr int permutationCount() noexcept {
    return characterValueCount();
  }

  static constexpr int characterValueCount() noexcept {
    return std::numeric_limits<uint8_t>::max() + 1;
  }

 public:
  using SegmentPermutation = std::array<byte, segmentSize>;
  using PermutationCollection =
      std::array<SegmentPermutation, permutationCount()>;

  struct TooManyPermutationsException : public std::exception {
    char const *what() const throw() {
      return "The supplied pattern requires a segment extension of more than 1 "
             "byte";
    }
  };

  PermutationCollection extend(Pattern const &pattern) const {
    if (!isAllowed(pattern)) {
      throw TooManyPermutationsException();
    }

    auto permutations =
        initializePermutationContainer(pattern.data(), pattern.size());

    extendPermutationsAtIndex(pattern.size(), permutations);

    return permutations;
  }

  PermutationCollection extend(byte const *const prefix, int const index) {
    auto permutations = initializePermutationContainer(prefix, index);

    extendPermutationsAtIndex(index, permutations);

    return permutations;
  }

 private:
  bool isAllowed(Pattern const &pattern) const noexcept {
    return segmentSize - pattern.size() <= allowedBytesToExtend;
  }

  PermutationCollection initializePermutationContainer(
      byte const *const prefix, int const prefixSize) const {
    SegmentPermutation permutation;
    std::memcpy(permutation.data(), prefix, prefixSize);

    PermutationCollection permutations;
    std::fill(std::begin(permutations), std::end(permutations), permutation);

    return permutations;
  }

  void extendPermutationsAtIndex(int const index,
                                 PermutationCollection &permutations) const
      noexcept {
    auto it = std::begin(permutations);
    int val = 0;
    while (it != std::cend(permutations)) {
      it->at(index) = val;
      ++it;
      ++val;
    }
  }
};
}  // namespace dfc

#endif
