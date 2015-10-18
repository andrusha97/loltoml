#include "common.hpp"

#include <limits>
#include <sstream>


TEST(Integer, Zero) {
    std::istringstream input("key = 0");
    events_aggregator_t handler;

    loltoml::parse(input, handler);

    std::vector<sax_event_t> expected_events = {
        {sax_event_t::start_document},
        {sax_event_t::key, "key"},
        {sax_event_t::integer, 0},
        {sax_event_t::finish_document}
    };

    EXPECT_EQ(expected_events, handler.events);
}

TEST(Integer, PositiveZero) {
    std::istringstream input("key = +0");
    events_aggregator_t handler;

    loltoml::parse(input, handler);

    std::vector<sax_event_t> expected_events = {
        {sax_event_t::start_document},
        {sax_event_t::key, "key"},
        {sax_event_t::integer, 0},
        {sax_event_t::finish_document}
    };

    EXPECT_EQ(expected_events, handler.events);
}

TEST(Integer, NegativeZero) {
    std::istringstream input("key = -0");
    events_aggregator_t handler;

    loltoml::parse(input, handler);

    std::vector<sax_event_t> expected_events = {
        {sax_event_t::start_document},
        {sax_event_t::key, "key"},
        {sax_event_t::integer, 0},
        {sax_event_t::finish_document}
    };

    EXPECT_EQ(expected_events, handler.events);
}

TEST(Integer, OneDigit) {
    std::istringstream input("key = 1");
    events_aggregator_t handler;

    loltoml::parse(input, handler);

    std::vector<sax_event_t> expected_events = {
        {sax_event_t::start_document},
        {sax_event_t::key, "key"},
        {sax_event_t::integer, 1},
        {sax_event_t::finish_document}
    };

    EXPECT_EQ(expected_events, handler.events);
}

TEST(Integer, ManyDigits) {
    std::istringstream input("key = 430903");
    events_aggregator_t handler;

    loltoml::parse(input, handler);

    std::vector<sax_event_t> expected_events = {
        {sax_event_t::start_document},
        {sax_event_t::key, "key"},
        {sax_event_t::integer, 430903},
        {sax_event_t::finish_document}
    };

    EXPECT_EQ(expected_events, handler.events);
}

TEST(Integer, PositiveOneDigit) {
    std::istringstream input("key = +1");
    events_aggregator_t handler;

    loltoml::parse(input, handler);

    std::vector<sax_event_t> expected_events = {
        {sax_event_t::start_document},
        {sax_event_t::key, "key"},
        {sax_event_t::integer, 1},
        {sax_event_t::finish_document}
    };

    EXPECT_EQ(expected_events, handler.events);
}

TEST(Integer, PosititveManyDigits) {
    std::istringstream input("key = +430903");
    events_aggregator_t handler;

    loltoml::parse(input, handler);

    std::vector<sax_event_t> expected_events = {
        {sax_event_t::start_document},
        {sax_event_t::key, "key"},
        {sax_event_t::integer, 430903},
        {sax_event_t::finish_document}
    };

    EXPECT_EQ(expected_events, handler.events);
}

TEST(Integer, NegativeOneDigit) {
    std::istringstream input("key = -4");
    events_aggregator_t handler;

    loltoml::parse(input, handler);

    std::vector<sax_event_t> expected_events = {
        {sax_event_t::start_document},
        {sax_event_t::key, "key"},
        {sax_event_t::integer, -4},
        {sax_event_t::finish_document}
    };

    EXPECT_EQ(expected_events, handler.events);
}

TEST(Integer, NegativeManyDigits) {
    std::istringstream input("key = -9834");
    events_aggregator_t handler;

    loltoml::parse(input, handler);

    std::vector<sax_event_t> expected_events = {
        {sax_event_t::start_document},
        {sax_event_t::key, "key"},
        {sax_event_t::integer, -9834},
        {sax_event_t::finish_document}
    };

    EXPECT_EQ(expected_events, handler.events);
}

TEST(Integer, MaxPossibleNumber) {
    std::istringstream input("key = 9223372036854775807");
    events_aggregator_t handler;

    loltoml::parse(input, handler);

    std::vector<sax_event_t> expected_events = {
        {sax_event_t::start_document},
        {sax_event_t::key, "key"},
        {sax_event_t::integer, std::numeric_limits<int64_t>::max()},
        {sax_event_t::finish_document}
    };

    EXPECT_EQ(expected_events, handler.events);
}

TEST(Integer, MinPossibleNumber) {
    std::istringstream input("key = -9223372036854775808");
    events_aggregator_t handler;

    loltoml::parse(input, handler);

    std::vector<sax_event_t> expected_events = {
        {sax_event_t::start_document},
        {sax_event_t::key, "key"},
        {sax_event_t::integer, std::numeric_limits<int64_t>::min()},
        {sax_event_t::finish_document}
    };

    EXPECT_EQ(expected_events, handler.events);
}

TEST(Integer, PositiveOutOfRange1) {
    std::istringstream input("key = 9223372036854775808");
    events_aggregator_t handler;

    EXPECT_THROW(loltoml::parse(input, handler), loltoml::parser_error_t);
}

TEST(Integer, PositiveOutOfRange2) {
    std::istringstream input("key = 9223372036864775800");
    events_aggregator_t handler;

    EXPECT_THROW(loltoml::parse(input, handler), loltoml::parser_error_t);
}

TEST(Integer, PositiveOutOfRange3) {
    std::istringstream input("key = 19223372036854775808");
    events_aggregator_t handler;

    EXPECT_THROW(loltoml::parse(input, handler), loltoml::parser_error_t);
}

TEST(Integer, NegativeOutOfRange1) {
    std::istringstream input("key = -9223372036854775809");
    events_aggregator_t handler;

    EXPECT_THROW(loltoml::parse(input, handler), loltoml::parser_error_t);
}

TEST(Integer, NegativeOutOfRange2) {
    std::istringstream input("key = -9223372036864775807");
    events_aggregator_t handler;

    EXPECT_THROW(loltoml::parse(input, handler), loltoml::parser_error_t);
}

TEST(Integer, NegativeOutOfRange3) {
    std::istringstream input("key = -19223372036854775808");
    events_aggregator_t handler;

    EXPECT_THROW(loltoml::parse(input, handler), loltoml::parser_error_t);
}

TEST(Integer, LeadingZerosAreNotAllowed) {
    std::istringstream input("key = 01");
    events_aggregator_t handler;

    EXPECT_THROW(loltoml::parse(input, handler), loltoml::parser_error_t);
}

TEST(Integer, LeadingZerosAreNotAllowed2) {
    std::istringstream input("key = 00");
    events_aggregator_t handler;

    EXPECT_THROW(loltoml::parse(input, handler), loltoml::parser_error_t);
}

TEST(Integer, LeadingZerosAreNotAllowed3) {
    std::istringstream input("key = -01");
    events_aggregator_t handler;

    EXPECT_THROW(loltoml::parse(input, handler), loltoml::parser_error_t);
}
