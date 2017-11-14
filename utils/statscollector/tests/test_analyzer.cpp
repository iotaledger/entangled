#include <cstdint>
#include <memory>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "iota/utils/common/iri.hpp"
#include "iota/utils/statscollector/analyzer.hpp"
#include "iota/utils/statscollector/stats/stats.hpp"

using namespace iota::utils;
using namespace iota::utils::statscollector;

namespace {

const std::string_view TX_payload(
    "FCTUIMMLIDJFOMVPHLWTIWVUYKJINKYIKAI9DUZGJPFSWEYHGQWREXTNJJEDMTYIOYFOTICXGB"
    "FNA9999 "
    "KPWCHICGJZXKE9GSUDXZYUAPLHAKAHYHDXNPHENTERYMMBQOPSQIDENXKLKCEYCPVTZQLEEJVY"
    "JZV9BWU 0 IQTNA9999999999999999999999 1509897914 0 1 "
    "STHTBDTVDYOFVOKVRPGRUVFCLFXEYUZYISBOFZZUEFVSOLDADEKQMRXDBEEXDYEGCCHXXDYUEQ"
    "QVTJSRW "
    "JKOMYKTPKTSUAUQJCRBJ99UEOUHEVSVMORJKCIILJHDPICXTAFQCULGYYFVRREXDJEJIGRGVML"
    "RZZ9999 "
    "VPZTEHURNXLNBNDLJTJCGLIQZWVIQSSFDL9C9GSSULPJZDKWTAHJNRIHRARWELJPLWLIBDQIIR"
    "EBA9999 1509897927055");

class MockStats : public TXStats {
 public:
  MOCK_METHOD1(trackNewTX, void(iri::TXMessage&));
  MOCK_METHOD0(trackReattachedTX, void());
  MOCK_METHOD0(trackNewBundle, void());
  MOCK_METHOD3(trackConfirmedBundle, void(int64_t, uint64_t, uint64_t));
};

TEST(AnalyzerTest, tracksUnknownTX) {
  using namespace testing;
  auto stats = std::make_shared<MockStats>();
  auto analyzer = std::make_shared<TXAnalyzer>(stats);
  auto txo = std::make_shared<iri::TXMessage>(TX_payload);

  EXPECT_CALL(*stats, trackNewTX(_)).Times(1);
  EXPECT_CALL(*stats, trackNewBundle()).Times(1);
  analyzer->newTransaction(txo);
}

}  // namespace
