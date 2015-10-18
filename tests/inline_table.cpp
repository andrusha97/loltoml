#include "common.hpp"

#include <sstream>


TEST(InlineTable, Empty) {
    std::istringstream input("key = {}");
    events_aggregator_t handler;

    loltoml::parse(input, handler);

    std::vector<sax_event_t> expected_events = {
        {sax_event_t::start_document},
        {sax_event_t::key, "key"},
        {sax_event_t::start_inline_table},
        {sax_event_t::finish_inline_table, 0},
        {sax_event_t::finish_document}
    };

    EXPECT_EQ(expected_events, handler.events);
}

TEST(InlineTable, EmptyWithSpaces) {
    std::istringstream input("key = {  \t  \t\t  } ");
    events_aggregator_t handler;

    loltoml::parse(input, handler);

    std::vector<sax_event_t> expected_events = {
        {sax_event_t::start_document},
        {sax_event_t::key, "key"},
        {sax_event_t::start_inline_table},
        {sax_event_t::finish_inline_table, 0},
        {sax_event_t::finish_document}
    };

    EXPECT_EQ(expected_events, handler.events);
}

TEST(InlineTable, WithBareKey) {
    std::istringstream input("key = {key=123} ");
    events_aggregator_t handler;

    loltoml::parse(input, handler);

    std::vector<sax_event_t> expected_events = {
        {sax_event_t::start_document},
        {sax_event_t::key, "key"},
        {sax_event_t::start_inline_table},
        {sax_event_t::key, "key"},
        {sax_event_t::integer, 123},
        {sax_event_t::finish_inline_table, 1},
        {sax_event_t::finish_document}
    };

    EXPECT_EQ(expected_events, handler.events);
}

TEST(InlineTable, WithNumericKey) {
    std::istringstream input("key = { 123=\t123   } ");
    events_aggregator_t handler;

    loltoml::parse(input, handler);

    std::vector<sax_event_t> expected_events = {
        {sax_event_t::start_document},
        {sax_event_t::key, "key"},
        {sax_event_t::start_inline_table},
        {sax_event_t::key, "123"},
        {sax_event_t::integer, 123},
        {sax_event_t::finish_inline_table, 1},
        {sax_event_t::finish_document}
    };

    EXPECT_EQ(expected_events, handler.events);
}

TEST(InlineTable, WithQuotedKey) {
    std::istringstream input("key = { \"key\\nkey\"  =\t123   } ");
    events_aggregator_t handler;

    loltoml::parse(input, handler);

    std::vector<sax_event_t> expected_events = {
        {sax_event_t::start_document},
        {sax_event_t::key, "key"},
        {sax_event_t::start_inline_table},
        {sax_event_t::key, "key\nkey"},
        {sax_event_t::integer, 123},
        {sax_event_t::finish_inline_table, 1},
        {sax_event_t::finish_document}
    };

    EXPECT_EQ(expected_events, handler.events);
}

TEST(InlineTable, WithMultipleKeys) {
    std::istringstream input("key = {\"key\"=123,\tkey=345 , 123 = \t'value' } ");
    events_aggregator_t handler;

    loltoml::parse(input, handler);

    std::vector<sax_event_t> expected_events = {
        {sax_event_t::start_document},
        {sax_event_t::key, "key"},
        {sax_event_t::start_inline_table},
        {sax_event_t::key, "key"},
        {sax_event_t::integer, 123},
        {sax_event_t::key, "key"},
        {sax_event_t::integer, 345},
        {sax_event_t::key, "123"},
        {sax_event_t::string, "value"},
        {sax_event_t::finish_inline_table, 3},
        {sax_event_t::finish_document}
    };

    EXPECT_EQ(expected_events, handler.events);
}

TEST(InlineTable, WithArray) {
    std::istringstream input("key = {\"key\"=123,\tkey= [1, \n2\r\n  ,3] } ");
    events_aggregator_t handler;

    loltoml::parse(input, handler);

    std::vector<sax_event_t> expected_events = {
        {sax_event_t::start_document},
        {sax_event_t::key, "key"},
        {sax_event_t::start_inline_table},
        {sax_event_t::key, "key"},
        {sax_event_t::integer, 123},
        {sax_event_t::key, "key"},
        {sax_event_t::start_array},
        {sax_event_t::integer, 1},
        {sax_event_t::integer, 2},
        {sax_event_t::integer, 3},
        {sax_event_t::finish_array, 3},
        {sax_event_t::finish_inline_table, 2},
        {sax_event_t::finish_document}
    };

    EXPECT_EQ(expected_events, handler.events);
}

TEST(InlineTable, WithMultilineString) {
    std::istringstream input("key = {\"key\"=123,\tkey= '''\n one line\r\nanother''' } ");
    events_aggregator_t handler;

    loltoml::parse(input, handler);

    std::vector<sax_event_t> expected_events = {
        {sax_event_t::start_document},
        {sax_event_t::key, "key"},
        {sax_event_t::start_inline_table},
        {sax_event_t::key, "key"},
        {sax_event_t::integer, 123},
        {sax_event_t::key, "key"},
        {sax_event_t::string, " one line\nanother"},
        {sax_event_t::finish_inline_table, 2},
        {sax_event_t::finish_document}
    };

    EXPECT_EQ(expected_events, handler.events);
}

TEST(InlineTable, MustBeOneLine1) {
    std::istringstream input("key = {\"key\" \n=123,\tkey=345 , 123 = \t'value' } ");
    events_aggregator_t handler;

    try {
        loltoml::parse(input, handler);
        FAIL();
    } catch (const loltoml::parser_error_t &e) {
        EXPECT_EQ(13, e.offset());
    }
}

TEST(InlineTable, MustBeOneLine2) {
    std::istringstream input("key = {\"key\" =\n123,\tkey=345 , 123 = \t'value' } ");
    events_aggregator_t handler;

    try {
        loltoml::parse(input, handler);
        FAIL();
    } catch (const loltoml::parser_error_t &e) {
        EXPECT_EQ(14, e.offset());
    }
}

TEST(InlineTable, MustBeOneLine3) {
    std::istringstream input("key = {\"key\" =123\n,\tkey=345 , 123 = \t'value' } ");
    events_aggregator_t handler;

    try {
        loltoml::parse(input, handler);
        FAIL();
    } catch (const loltoml::parser_error_t &e) {
        EXPECT_EQ(17, e.offset());
    }
}

TEST(InlineTable, MustBeOneLine4) {
    std::istringstream input("key = {\"key\" =123,\n\tkey=345 , 123 = \t'value' } ");
    events_aggregator_t handler;

    try {
        loltoml::parse(input, handler);
        FAIL();
    } catch (const loltoml::parser_error_t &e) {
        EXPECT_EQ(18, e.offset());
    }
}

TEST(InlineTable, TrailingCommaNotAllowed) {
    std::istringstream input("key = {key=345,} ");
    events_aggregator_t handler;

    EXPECT_THROW(loltoml::parse(input, handler), loltoml::parser_error_t);
}
