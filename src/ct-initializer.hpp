#ifndef DFC_CT_INITIALIZER_HPP
#define DFC_CT_INITIALIZER_HPP

#include <array>
#include <limits>
#include <vector>

#include "byte.hpp"
#include "ct.hpp"
#include "pattern-range.hpp"

namespace dfc {
template <typename PatternRange, typename SegmentType, SegmentType Hash,
          int Size>
class CompactTableInitializer {
  static_assert(std::is_integral<SegmentType>::value,
                "SegmentType must be integral");

  using Table = std::array<std::vector<CompactTableEntry<SegmentType>>, Size>;
  Table table_;

  CompactTableIndexer<SegmentType, Hash, Size - 1> const indexer_{};
  Segmenter<SegmentType> const segmenter_{};

  PatternRange const patternRange_{};

 public:
  // TODO: If case insensitive, create all permutations of segment
  void addPattern(PidIndex const pidIndex, Pattern const& pattern) noexcept {
    if (patternRange_.includes(pattern)) {
      auto const segment = segmenter_.segment(pattern);
      auto const bucketIndex = indexer_.index(segment);

      auto& bucket = table_[bucketIndex];

      auto entry = std::begin(bucket);
      auto const end = std::cend(bucket);
      bool found = false;
      while (entry != end && !found) {
        if (entry->segment == segment) {
          entry->pids.emplace_back(pidIndex);
          found = true;
        }
        ++entry;
      }

      if (!found) {
        bucket.emplace_back(segment, pidIndex);
      }
    }
  }

  template <typename OnMatcher, typename Matcher = MemcmpMatcher>
  auto ct(std::shared_ptr<OnMatcher> onMatcher,
          std::shared_ptr<std::vector<Pattern> const> patterns) const {
    static_assert(std::is_base_of<dfc::OnMatcher, OnMatcher>::value,
                  "OnMatcher template parameter is not an OnMatcher");
    static_assert(std::is_base_of<dfc::Matcher, Matcher>::value,
                  "Matcher template parameter is not a Matcher");

    return CompactTable<SegmentType, Hash, Size, OnMatcher, Matcher>(
        table_, std::move(onMatcher), std::move(patterns));
  }
};
}  // namespace dfc

#endif
