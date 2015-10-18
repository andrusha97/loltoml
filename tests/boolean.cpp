#include "common.hpp"

#include <sstream>


TEST(Boolean, ParseTrue) {
    std::istringstream input("true = true");
    events_aggregator_t handler;

    loltoml::parse(input, handler);

    std::vector<sax_event_t> expected_events = {
        {sax_event_t::start_document},
        {sax_event_t::key, "true"},
        {sax_event_t::boolean, true},
        {sax_event_t::finish_document}
    };

    EXPECT_EQ(expected_events, handler.events);
}

TEST(Boolean, ParseFalse) {
    std::istringstream input("false = false");
    events_aggregator_t handler;

    loltoml::parse(input, handler);

    std::vector<sax_event_t> expected_events = {
        {sax_event_t::start_document},
        {sax_event_t::key, "false"},
        {sax_event_t::boolean, false},
        {sax_event_t::finish_document}
    };

    EXPECT_EQ(expected_events, handler.events);
}
