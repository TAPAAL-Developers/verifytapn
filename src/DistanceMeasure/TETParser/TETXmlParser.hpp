#ifndef SW9_KODE_TETPARSER_HPP
#define SW9_KODE_TETPARSER_HPP

#include "../../../lib/rapidxml-1.13/rapidxml.hpp"
#include "../TET/TimedEventTrace.hpp"

namespace DistanceMeasure {
    namespace TETXmlParser {
        using namespace rapidxml;

        class TETXmlParser {
        public: // construction
            explicit TETXmlParser(){};
            virtual ~TETXmlParser() { /* empty */ };

            std::vector<DistanceMeasure::TimedEventTraces::TimedEventTrace>* parse(const std::string &filename);
        private:
            DistanceMeasure::TimedEventTraces::TimedEventTrace parseTraces(const xml_node<>& root);
            TimedEventTraces::TimedEventTrace::timed_action parseEvent(const xml_node<>& event_node);
        };

    }
}

#endif //SW9_KODE_TETPARSER_HPP
