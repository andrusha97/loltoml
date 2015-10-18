#include "common.hpp"

#include <sstream>


TEST(Table, Simple) {
    std::istringstream input("[table]");
    events_aggregator_t handler;

    loltoml::parse(input, handler);

    std::vector<sax_event_t> expected_events = {
        {sax_event_t::start_document},
        {sax_event_t::table, {"table"}},
        {sax_event_t::finish_document}
    };

    EXPECT_EQ(expected_events, handler.events);
}

TEST(Table, NumericKey) {
    std::istringstream input("[123]");
    events_aggregator_t handler;

    loltoml::parse(input, handler);

    std::vector<sax_event_t> expected_events = {
        {sax_event_t::start_document},
        {sax_event_t::table, {"123"}},
        {sax_event_t::finish_document}
    };

    EXPECT_EQ(expected_events, handler.events);
}

TEST(Table, QuotedKey) {
    std::istringstream input("[\"table\\ntable\"]");
    events_aggregator_t handler;

    loltoml::parse(input, handler);

    std::vector<sax_event_t> expected_events = {
        {sax_event_t::start_document},
        {sax_event_t::table, {"table\ntable"}},
        {sax_event_t::finish_document}
    };

    EXPECT_EQ(expected_events, handler.events);
}

TEST(Table, MultipleKeys) {
    std::istringstream input("[table.123.\"table\\ntable\"]");
    events_aggregator_t handler;

    loltoml::parse(input, handler);

    std::vector<sax_event_t> expected_events = {
        {sax_event_t::start_document},
        {sax_event_t::table, {"table", "123", "table\ntable"}},
        {sax_event_t::finish_document}
    };

    EXPECT_EQ(expected_events, handler.events);
}

TEST(Table, TableCanBeWrappedWithSpacesAndComments) {
    std::istringstream input("\r\n   \n  \t    [table.123.\"table\\ntable\"]   # comment\r\n\n");
    events_aggregator_t handler;

    loltoml::parse(input, handler);

    std::vector<sax_event_t> expected_events = {
        {sax_event_t::start_document},
        {sax_event_t::table, {"table", "123", "table\ntable"}},
        {sax_event_t::comment, " comment"},
        {sax_event_t::finish_document}
    };

    EXPECT_EQ(expected_events, handler.events);
}

TEST(Table, TableCanContainSpacesInside) {
    std::istringstream input("[ table.  123.\t \"table\\ntable\" \t  ]");
    events_aggregator_t handler;

    loltoml::parse(input, handler);

    std::vector<sax_event_t> expected_events = {
        {sax_event_t::start_document},
        {sax_event_t::table, {"table", "123", "table\ntable"}},
        {sax_event_t::finish_document}
    };

    EXPECT_EQ(expected_events, handler.events);
}

TEST(Table, TableCannotBeMultiline) {
    std::istringstream input("[\ntable.123.\"table\\ntable\"]");
    events_aggregator_t handler;

    try {
        loltoml::parse(input, handler);
        FAIL();
    } catch (const loltoml::parser_error_t &e) {
        EXPECT_EQ(1, e.offset());
    }
}

TEST(Table, TableCannotBeMultiline2) {
    std::istringstream input("[table.\n123.\"table\\ntable\"]");
    events_aggregator_t handler;

    try {
        loltoml::parse(input, handler);
        FAIL();
    } catch (const loltoml::parser_error_t &e) {
        EXPECT_EQ(7, e.offset());
    }
}

TEST(Table, TableCannotBeMultiline3) {
    std::istringstream input("[table.123\n.\"table\\ntable\"]");
    events_aggregator_t handler;

    try {
        loltoml::parse(input, handler);
        FAIL();
    } catch (const loltoml::parser_error_t &e) {
        EXPECT_EQ(10, e.offset());
    }
}

TEST(Table, TableMustHaveAtLeastOneKey) {
    std::istringstream input("[  ]");
    events_aggregator_t handler;

    try {
        loltoml::parse(input, handler);
        FAIL();
    } catch (const loltoml::parser_error_t &e) {
        EXPECT_EQ(3, e.offset());
    }
}
