#include "TimedEventTrace.hpp"

namespace DistanceMeasure{
    namespace TimedEventTraces{

        TimedEventTrace::id_t & TimedEventTrace::getId() const
        {
            return id;
        }

        void TimedEventTrace::print(std::ostream& out) const{
            out << "Timed event trace " << getId() << ": ";

            for(TimedEventTrace::timed_action_list::const_iterator it = timed_event_trace.begin(); it != timed_event_trace.end(); ++it){
                out << "(" << it->first << "," << it->second << ") ";
            }
            out << "\n";
        }
    }
}
