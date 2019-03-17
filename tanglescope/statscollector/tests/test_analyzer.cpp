#include <cstdint>
#include <memory>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "tanglescope/common/iri.hpp"
#include "tanglescope/prometheus_collector/prometheus_collector.hpp"
#include "tanglescope/statscollector/analyzer.hpp"
#include "tanglescope/statscollector/stats/stats.hpp"

using namespace iota::tanglescope;
using namespace iota::tanglescope::statscollector;

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
  MOCK_METHOD2(trackNewTX, void(iri::TXMessage&, PrometheusCollector::CountersMap&));
  MOCK_METHOD1(trackReattachedTX, void(PrometheusCollector::CountersMap&));
  MOCK_METHOD1(trackNewBundle, void(PrometheusCollector::CountersMap&));
  MOCK_METHOD5(trackConfirmedBundle, void(int64_t, uint64_t, uint64_t, PrometheusCollector::CountersMap&,
                                          PrometheusCollector::HistogramsMap&));
};

TEST(AnalyzerTest, tracksUnknownTX) {
  using namespace testing;

  PrometheusCollector::CountersMap counters;
  PrometheusCollector::HistogramsMap histograms;
  auto stats = std::make_shared<MockStats>();
  auto analyzer = std::make_shared<TXAnalyzer>(counters, histograms, stats);
  auto txo = std::make_shared<iri::TXMessage>(TX_payload);

  EXPECT_CALL(*stats, trackNewTX(_, _)).Times(1);
  EXPECT_CALL(*stats, trackNewBundle(_)).Times(1);
  analyzer->newTransaction(txo);
}

}  // namespace
