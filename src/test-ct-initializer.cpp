#include "catch.hpp"

#include "ct-initializer.hpp"

namespace {
std::vector<dfc::Pid> matchedPids;
struct TestOnMatcher : public dfc::OnMatcher<TestOnMatcher> {
  void onMatch(dfc::Pattern const& pattern) const {
    matchedPids.emplace_back(pattern.pid());
  }
};

dfc::RawPattern pattern(char const* val) {
  return dfc::RawPattern(reinterpret_cast<byte const*>(val), std::strlen(val));
}

TEST_CASE("CT") {
  matchedPids.clear();
  auto patterns = std::make_shared<std::vector<dfc::Pattern>>();

  int const ctSize = 0x100;
  dfc::CompactTableInitializer<uint8_t, 1, ctSize> initializer;

  SECTION("Is empty by default") {
    auto const ct = initializer.ct<TestOnMatcher>(patterns);

    for (int i = 0; i < ctSize; ++i) {
      byte in = i;
      ct.exactMatching(&in, 1);
    }

    REQUIRE(matchedPids.size() == 0);
  }

  SECTION("Matches if added and equal") {
    auto patternValue = "x";

    dfc::Pid const pid = 1;
    patterns->emplace_back(pid, pattern(patternValue));
    int const patternIndex = 0;
    initializer.addPattern(patternIndex, patterns->at(patternIndex));

    auto const ct = initializer.ct<TestOnMatcher>(patterns);
    ct.exactMatching(patternValue, 1);

    REQUIRE(matchedPids.size() == 1);
    REQUIRE(matchedPids[0] == pid);
  }

  SECTION("Does not match if remaining characters are fewer than segment") {
    auto patternValue = "x";

    dfc::Pid const pid = 1;
    patterns->emplace_back(pid, pattern(patternValue));
    int const patternIndex = 0;
    initializer.addPattern(patternIndex, patterns->at(patternIndex));

    auto const ct = initializer.ct<TestOnMatcher>(patterns);
    ct.exactMatching(patternValue, 0);

    REQUIRE(matchedPids.size() == 0);
  }

  SECTION("Multiple patterns") {
    dfc::Pid const firstPatternPid = 1, secondPatternPid = 2;
    SECTION("Can match multiple equal segments") {
      patterns->emplace_back(firstPatternPid, pattern("x"));
      patterns->emplace_back(secondPatternPid, pattern("x"));
      initializer.addPattern(0, patterns->at(0));
      initializer.addPattern(1, patterns->at(1));

      auto const ct = initializer.ct<TestOnMatcher>(patterns);
      ct.exactMatching("x", 1);

      REQUIRE(matchedPids.size() == 2);
      REQUIRE(matchedPids[0] == firstPatternPid);
      REQUIRE(matchedPids[1] == secondPatternPid);
    }

    SECTION("Can match multiple different segments") {
      patterns->emplace_back(firstPatternPid, pattern("x"));
      patterns->emplace_back(secondPatternPid, pattern("y"));
      initializer.addPattern(0, patterns->at(0));
      initializer.addPattern(1, patterns->at(1));

      auto const ct = initializer.ct<TestOnMatcher>(patterns);
      ct.exactMatching("x", 1);
      ct.exactMatching("y", 1);

      REQUIRE(matchedPids.size() == 2);
      REQUIRE(matchedPids[0] == firstPatternPid);
      REQUIRE(matchedPids[1] == secondPatternPid);
    }
  }
}
}  // namespace