#include "common.hpp"

#include <sstream>

TEST(ValidParse, EmptyDocument) {
    std::istringstream input("");
    events_aggregator_t handler;

    loltoml::parse(input, handler);

    std::vector<sax_event_t> expected_events = {
        {sax_event_t::start_document},
        {sax_event_t::finish_document}
    };

    EXPECT_EQ(expected_events, handler.events);
}

TEST(ValidParse, MultilineEmptyDocument) {
    std::istringstream input("\n\r\n\n");
    events_aggregator_t handler;

    loltoml::parse(input, handler);

    std::vector<sax_event_t> expected_events = {
        {sax_event_t::start_document},
        {sax_event_t::finish_document}
    };

    EXPECT_EQ(expected_events, handler.events);
}
