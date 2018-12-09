#ifndef SW9_KODE_DISTANCE_H
#define SW9_KODE_DISTANCE_H

#include <iostream>
#include <unordered_set>
#include <Generator.h>
#include "TET/TimedEventTrace.hpp"
#include "../Core/TAPN/TAPN.hpp"
#include "../DiscreteVerification/DataStructures/NonStrictMarkingBase.hpp"

namespace DistanceMeasure{

    class Distance{
    public: // typedefs
        typedef uint32_t pen_t; // TODO: REMOVE THIS
        typedef std::pair<VerifyTAPN::DiscreteVerification::NonStrictMarkingBase&, TimedEventTraces::TimedEventTrace*> marking_penalty_pair;

        //TODO: maybe we can drop the large penalty values and just increment one now that we separate them
    public: // construction/destruction
        Distance(VerifyTAPN::TAPN::TimedArcPetriNet& tapn): bfs_successful(false), tapn(tapn), current_trace(
                nullptr){ };
        virtual ~Distance(){ };


    public: // modifiers

    public: // inspectors
        void print(std::ostream& out) const;

    public: // public functions
        TimedEventTraces::TimedEventTrace::Vector telDistance(std::vector<int> &initialPlacement,
                                         std::vector<TimedEventTraces::TimedEventTrace> *tel);

    private: // private functions
        pen_t tetDistance(VerifyTAPN::DiscreteVerification::Generator &generator,
                          VerifyTAPN::DiscreteVerification::NonStrictMarkingBase &nsmb,
                          TimedEventTraces::TimedEventTrace trace);

        std::list<VerifyTAPN::DiscreteVerification::NonStrictMarkingBase> tauBfs(VerifyTAPN::DiscreteVerification::Generator &generator,
               VerifyTAPN::DiscreteVerification::NonStrictMarkingBase start_marking,
               std::string target_label);

        VerifyTAPN::DiscreteVerification::NonStrictMarkingBase calculateDistance(
                VerifyTAPN::DiscreteVerification::Generator &generator,
                std::list<VerifyTAPN::DiscreteVerification::NonStrictMarkingBase> marking_candidates,
                TimedEventTraces::TimedEventTrace::id_t d, std::string a);

        marking_penalty_pair delayDistance(VerifyTAPN::DiscreteVerification::Generator &generator,
                                           VerifyTAPN::DiscreteVerification::NonStrictMarkingBase &marking, uint32_t delay,
                                           TimedEventTraces::TimedEventTrace* in_trace);

        marking_penalty_pair firingDistance(VerifyTAPN::DiscreteVerification::Generator &generator,
                                            VerifyTAPN::DiscreteVerification::NonStrictMarkingBase &marking,
                                            std::string label, TimedEventTraces::TimedEventTrace* in_trace);

        marking_penalty_pair enableTransition(VerifyTAPN::DiscreteVerification::Generator &generator,
                                              VerifyTAPN::DiscreteVerification::NonStrictMarkingBase &marking,
                                              VerifyTAPN::TAPN::TimedTransition &timed_transition,
                                              TimedEventTraces::TimedEventTrace* in_trace);

        marking_penalty_pair modifyTokenAges(VerifyTAPN::TAPN::TimedPlace &place,
                                             VerifyTAPN::DiscreteVerification::NonStrictMarkingBase &marking,
                                             VerifyTAPN::TAPN::TimeInterval &interval, int tokens_to_spawn,
                                             TimedEventTraces::TimedEventTrace* in_trace);

    private: // data
        DistanceMeasure::TimedEventTraces::TimedEventTrace* current_trace;
        std::unordered_set<std::string> alphabet;
        bool bfs_successful;
        VerifyTAPN::TAPN::TimedArcPetriNet& tapn;

    };

    inline std::ostream& operator<<(std::ostream& out, const Distance& distance) {
        distance.print(out);
        return out;
    }

}

#endif //SW9_KODE_DISTANCE_H
