#include "common.hpp"

#include <sstream>


TEST(Key, BareKey) {
    std::istringstream input("key_-23_- = 123");
    events_aggregator_t handler;

    loltoml::parse(input, handler);

    std::vector<sax_event_t> expected_events = {
        {sax_event_t::start_document},
        {sax_event_t::key, "key_-23_-"},
        {sax_event_t::integer, 123},
        {sax_event_t::finish_document}
    };

    EXPECT_EQ(expected_events, handler.events);
}

TEST(Key, NumericKey) {
    std::istringstream input("123 = 123");
    events_aggregator_t handler;

    loltoml::parse(input, handler);

    std::vector<sax_event_t> expected_events = {
        {sax_event_t::start_document},
        {sax_event_t::key, "123"},
        {sax_event_t::integer, 123},
        {sax_event_t::finish_document}
    };

    EXPECT_EQ(expected_events, handler.events);
}

TEST(Key, QuotedKey) {
    std::istringstream input("\"key\\n\\t123\" = 123");
    events_aggregator_t handler;

    loltoml::parse(input, handler);

    std::vector<sax_event_t> expected_events = {
        {sax_event_t::start_document},
        {sax_event_t::key, "key\n\t123"},
        {sax_event_t::integer, 123},
        {sax_event_t::finish_document}
    };

    EXPECT_EQ(expected_events, handler.events);
}

TEST(Key, SpacesAroundKey) {
    std::istringstream input("  \n \r\n\n\r\n   key  \t = 123  #comment\n\n");
    events_aggregator_t handler;

    loltoml::parse(input, handler);

    std::vector<sax_event_t> expected_events = {
        {sax_event_t::start_document},
        {sax_event_t::key, "key"},
        {sax_event_t::integer, 123},
        {sax_event_t::comment, "comment"},
        {sax_event_t::finish_document}
    };

    EXPECT_EQ(expected_events, handler.events);
}

TEST(Key, NewLinesAreNotAllowed) {
    std::istringstream input("key\n = 123");
    events_aggregator_t handler;

    try {
        loltoml::parse(input, handler);
        FAIL();
    } catch (const loltoml::parser_error_t &e) {
        EXPECT_EQ(3, e.offset());
    }
}

TEST(Key, NewLinesAreNotAllowed2) {
    std::istringstream input("key \n= 123");
    events_aggregator_t handler;

    try {
        loltoml::parse(input, handler);
        FAIL();
    } catch (const loltoml::parser_error_t &e) {
        EXPECT_EQ(4, e.offset());
    }
}

TEST(Key, NewLinesAreNotAllowed3) {
    std::istringstream input("key =\n 123");
    events_aggregator_t handler;

    try {
        loltoml::parse(input, handler);
        FAIL();
    } catch (const loltoml::parser_error_t &e) {
        EXPECT_EQ(5, e.offset());
    }
}

TEST(Key, NewLinesAreNotAllowed4) {
    std::istringstream input("key = \n123");
    events_aggregator_t handler;

    try {
        loltoml::parse(input, handler);
        FAIL();
    } catch (const loltoml::parser_error_t &e) {
        EXPECT_EQ(6, e.offset());
    }
}

TEST(Key, NewLinesAreNotAllowed5) {
    std::istringstream input("\"key\nkey\" = 123");
    events_aggregator_t handler;

    try {
        loltoml::parse(input, handler);
        FAIL();
    } catch (const loltoml::parser_error_t &e) {
        EXPECT_EQ(4, e.offset());
    }
}
