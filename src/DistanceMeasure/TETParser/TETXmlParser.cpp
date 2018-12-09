#include <TAPNParser/util.hpp>
#include <vector>
#include "TETXmlParser.hpp"

namespace DistanceMeasure{
    namespace TETXmlParser{
        std::vector<TimedEventTraces::TimedEventTrace>* TETXmlParser::parse(const std::string & filename)
        {
            const std::string contents = VerifyTAPN::readFile(filename);
            std::vector<char> charArray(contents.begin(), contents.end());
            charArray.push_back('\0');

            rapidxml::xml_document<> xmldoc;
            xmldoc.parse<0>(&charArray[0]); // we need a char* to the string, and contents.c_str() returns const char*

            rapidxml::xml_node<>* tel = xmldoc.first_node("TIMED_EVENT_LOG");
            if(tel == 0) throw std::string("invalid file.");

            auto timed_event_trace_vector = new std::vector<TimedEventTraces::TimedEventTrace>;

            xml_node<>* timed_event_trace = tel->first_node("TIMED_EVENT_TRACE");
            while(timed_event_trace != NULL){
                timed_event_trace_vector->emplace_back(parseTraces(*timed_event_trace));
                timed_event_trace = timed_event_trace->next_sibling("TIMED_EVENT_TRACE");
            }
            return timed_event_trace_vector;
        }

        TimedEventTraces::TimedEventTrace TETXmlParser::parseTraces(const xml_node<>& tet) {
            TimedEventTraces::TimedEventTrace trace(std::stoi(tet.first_attribute("id")->value()), TimedEventTraces::TimedEventTrace::timed_action_list());

            xml_node<>* event_node = tet.first_node("TIMED_EVENT");
            while(event_node != NULL){
                TimedEventTraces::TimedEventTrace::timed_action event = parseEvent(*event_node);
                trace.queueTimedAction(event);
                event_node = event_node->next_sibling("TIMED_EVENT");
            }
            return trace;

        }

        TimedEventTraces::TimedEventTrace::timed_action TETXmlParser::parseEvent(const xml_node<>& event_node)
        {
            const std::string label(event_node.first_node("LABEL")->value());
            TimedEventTraces::TimedEventTrace::id_t delay(static_cast<TimedEventTraces::TimedEventTrace::id_t>(std::stoi(event_node.first_node("DELAY")->value())));

            TimedEventTraces::TimedEventTrace::timed_action event (label, delay);

            return event;
        }

    }
}