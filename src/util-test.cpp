#include "util-test.hpp"

namespace dfc {
namespace test {
RawPattern createPattern(char const* const str) {
  return RawPattern(reinterpret_cast<byte const*>(str), std::strlen(str));
}
RawPattern twoBytePattern() { return createPattern("te"); }
RawPattern fiveBytePattern() { return createPattern("testx"); }
RawPattern createCaseInsensitivePattern(char const* const str) {
  return RawPattern(reinterpret_cast<byte const*>(str), std::strlen(str),
                    Pattern::CaseSensitivity::CaseInsensitive);
}

ImmutablePattern createImmutablePattern(Pid const pid, char const* const str) {
  return ImmutablePattern(pid, createPattern(str));
}
ImmutablePattern createCaseInsensitiveImmutablePattern(Pid const pid,
                                                       char const* const str) {
  return ImmutablePattern(pid, createCaseInsensitivePattern(str));
}
}  // namespace test
}  // namespace dfc
