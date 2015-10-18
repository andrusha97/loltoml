#include "common.hpp"

#include <sstream>


namespace {
    void test_parsing(const std::string &str) {
        std::string scope = "parse datetime '" + str + "'";
        SCOPED_TRACE(scope);

        std::istringstream input("key = " + str);
        events_aggregator_t handler;

        loltoml::parse(input, handler);

        std::vector<sax_event_t> expected_events = {
            {sax_event_t::start_document},
            {sax_event_t::key, "key"},
            {sax_event_t::datetime, str},
            {sax_event_t::finish_document}
        };

        EXPECT_EQ(expected_events, handler.events);
    }

    void test_error(const std::string &str) {
        std::string scope = "test error when parse '" + str + "'";
        SCOPED_TRACE(scope);

        std::istringstream input("key = " + str);
        events_aggregator_t handler;

        EXPECT_THROW(loltoml::parse(input, handler), loltoml::parser_error_t);
    }
}


TEST(Datetime, UTC) {
    test_parsing("1111-11-11T00:11:01Z");
    test_parsing("0014-00-00t03:21:91Z");
    test_parsing("1971-11-11t00:11:01z");
    test_parsing("1971-01-11t00:11:01Z");
}

TEST(Datetime, Timezone) {
    test_parsing("1111-11-11T00:11:01+00:00");
    test_parsing("0014-00-00t03:21:91-01:12");
    test_parsing("1971-11-11t00:11:01+32:33");
}

TEST(Datetime, FractionalSeconds) {
    test_parsing("1111-11-11T00:11:01.0+00:00");
    test_parsing("0014-00-00t03:21:91.2323-01:12");
    test_parsing("1971-11-11t00:11:01.090239+32:33");
    test_parsing("1971-11-11t00:11:01.090239z");
}

TEST(Datetime, TimezoneCannotBeOmitted) {
    test_error("1111-11-11T00:11:01.0");
    test_error("0014-00-00t03:21:91");
    test_error("1971-11-11t00:11:01.090239");
    test_error("1971-11-11t00:11:01");
}

TEST(Datetime, TimeCannotBeOmitted) {
    test_error("1111-11-11");
    test_error("0014-00-00t");
    test_error("1971-11-11t+00:01");
}

TEST(Datetime, DateCannotBeOmitted) {
    test_error("00:11:01.0+00:00");
    test_error("t03:21:91.2323-01:12");
    test_error("T00:11:01.090239+32:33");
    test_error("00:11:01.090239z");
    test_error("t00:11:01.090239z");
    test_error("00:11:01");
    test_error("00:11:01z");
}

TEST(Datetime, TCannotBeOmitted) {
    test_error("1111-11-1100:11:01.0+00:00");
    test_error("0014-00-0003:21:91.2323-01:12");
    test_error("1971-11-1100:11:01.090239+32:33");
    test_error("1971-11-1100:11:01.090239z");

    test_error("1111-11-11 00:11:01.0+00:00");
    test_error("0014-00-00 03:21:91.2323-01:12");
    test_error("1971-11-11 00:11:01.090239+32:33");
    test_error("1971-11-11 00:11:01.090239z");
}
