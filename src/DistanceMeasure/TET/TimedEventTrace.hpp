#ifndef SW9_KODE_TRACE_H
#define SW9_KODE_TRACE_H

#include <stdint-gcc.h>
#include <list>
#include <utility>
#include <string>
#include <iostream>
#include <vector>

namespace DistanceMeasure{
    namespace TimedEventTraces{

        class TimedEventTrace{
        public: // typedefs
            typedef const uint32_t id_t;
            typedef const uint32_t delay_t;
            typedef uint32_t pen_t; // TODO: define a common type for both dist and TET header files
            typedef std::pair<const std::string, delay_t> timed_action;
            typedef std::list<timed_action> timed_action_list; // TODO: Make sure the delays in this vector are in ascending order(Add some validation/checks in the trace XML parser)
            typedef std::vector<TimedEventTrace> Vector; //TODO: make to pointers

        public: // construction/destruction
            TimedEventTrace(id_t& id, timed_action_list timed_event_trace): id(id), timed_event_trace(timed_event_trace),
                                                                            token_distance(0), non_replayable_distance(0), delay_distance(0) { };
            virtual ~TimedEventTrace(){ };


        public: // modifiers
            inline void dequeueTimedAction() { timed_event_trace.pop_front(); };
            inline void queueTimedAction(std::pair<const std::string, const uint64_t > timed_action) { timed_event_trace.emplace_back(timed_action); };
            inline void addTokenPenalty(pen_t penalty) { token_distance += penalty; };
            inline void addNonReplayablePenalty(pen_t penalty) { non_replayable_distance += penalty; }; //TODO: Consider makings this an increment
            inline void addDelayPenalty(pen_t penalty) { delay_distance += penalty; };
            inline void setTokenPenalty(pen_t penalty) { token_distance = penalty; };
            inline void setNonReplayablePenalty(pen_t penalty) { non_replayable_distance = penalty; };
            inline void setDelayPenalty(pen_t penalty) { delay_distance = penalty; };

        public: // inspectors
            id_t & getId() const;
            inline int getTraceLength() const { return timed_event_trace.size(); };
            inline timed_action_list& getTrace() { return timed_event_trace; };
            inline pen_t getTokenDistance() const { return token_distance; };
            inline pen_t getNonReplayableDistance() const { return non_replayable_distance; };
            inline pen_t getDelayDistance() const { return delay_distance; };
            inline pen_t getTotalDistance() const { return token_distance + 1000 * delay_distance + 10000 * non_replayable_distance; };
            void print(std::ostream& out) const;

            //TODO: Overwrite equals operator for custom comparison
            /*TimedEventTrace& operator=(const TimedEventTrace& rhs){
                return *this;
            }*/

        private: // data
            id_t id;
            timed_action_list timed_event_trace;
            pen_t token_distance;
            pen_t non_replayable_distance;
            pen_t delay_distance;
        };

        inline std::ostream& operator<<(std::ostream& out, const TimedEventTrace& trace)
        {
            trace.print(out);
            return out;
        }
    }
}
#endif //SW9_KODE_TRACE_H
