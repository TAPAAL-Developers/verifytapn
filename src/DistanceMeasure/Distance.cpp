#include "Distance.hpp"
#include <queue>

// TODO: It looks like declaring vars inside a for loop is good practice due to scoping. So do this.
// TODO: See if i use a penalty variable locally when i might as well use marking_penalty_pair type second member.

using namespace VerifyTAPN;
namespace DistanceMeasure {

    std::list<DiscreteVerification::NonStrictMarkingBase>
    Distance::tauBfs(DiscreteVerification::Generator &generator,
                     DiscreteVerification::NonStrictMarkingBase start_marking,
                     const std::string target_label) {

        // List of already visited and queued markings
        std::vector<DiscreteVerification::NonStrictMarkingBase> visited{}; //TODO: Make use of this when NonStrictBaseMarking gets an == operator
        std::queue<DiscreteVerification::NonStrictMarkingBase> queue{};
        std::list<DiscreteVerification::NonStrictMarkingBase> found_markings{};
        uint32_t depth = 0;
        uint32_t time_to_depth_increase = 1;
        bool candidate_found = false, first_marking = true;
        auto current_marking = start_marking;

        queue.push(current_marking);

        while (!queue.empty()) {
            bool already_visited = false;
            current_marking = queue.front();

            for (std::vector<DiscreteVerification::NonStrictMarkingBase>::const_iterator it_vs = visited.begin(); it_vs != visited.end(); it_vs++){
                if (!current_marking.cmp(*it_vs) && !first_marking){
                    already_visited = true;
                    break;
                }
            }

            if (!already_visited) {
                generator.from_marking(&current_marking);
                //TODO: avoid unnecessarily traversing place if candidate is found
                // Iterate over all places in the current marking
                for (vector<DiscreteVerification::Place>::const_iterator it = current_marking.getPlaceList().begin();
                     it != current_marking.getPlaceList().end(); it++) {

                    auto &transportArcs = it->place->getTransportArcs();
                    auto &inputArcs = it->place->getInputArcs();

                    //Iterate over input arcs
                    for (auto it_ia = inputArcs.begin();
                         it_ia != inputArcs.end(); it_ia++) {

                        if ((*it_ia)->getOutputTransition().getName().find(target_label + "_") != string::npos) {
                            candidate_found = true;
                            found_markings.push_back(current_marking);
                            break; // TODO: can this BFS handle a place which has a postset with two transitions with the same label?? the break here can't, right?
                        } else if ((*it_ia)->getOutputTransition().getName().find("Tau_") !=
                                   string::npos) { //TODO: use std::search() with custom predicate to make this not case sensitive
                            generator.only_transition(&(*it_ia)->getOutputTransition());

                            while (auto successor = std::unique_ptr<DiscreteVerification::NonStrictMarkingBase>(
                                    generator.next(false))) {
                                queue.emplace(*successor);
                            }
                            generator.from_marking(&current_marking);
                        }
                    }
                    // Iterate over transport arcs
                    for (auto it_ta = transportArcs.begin();
                         it_ta != transportArcs.end(); it_ta++) {

                        if ((*it_ta)->getTransition().getName().find(target_label + "_") != string::npos) {
                            candidate_found = true;
                            found_markings.push_back(current_marking);
                            break;
                        } else if ((*it_ta)->getTransition().getName().find("Tau_") !=
                                   string::npos) { //TODO: see todo about std::search()
                            generator.only_transition(
                                    &(*it_ta)->getTransition()); //TODO: Could we not set only_trans and then get all successors (we cannot have a place with both tau and act trans in postset due to invariant requireemtn, so it would work)

                            while (auto successor = std::unique_ptr<DiscreteVerification::NonStrictMarkingBase>(
                                    generator.next(false))) {
                                queue.emplace(*successor);
                            }
                            generator.from_marking(&current_marking);
                        }
                    }
                }
                visited.push_back(current_marking);
            }
            queue.pop();
            first_marking = false;

            time_to_depth_increase--;
            if (time_to_depth_increase == 0) {
                depth++;
                time_to_depth_increase = queue.size();
                if (candidate_found)
                    break;
            }

        }

        // We can safely set this to true as it is overwritten in the next if statement if the bfs was not successful
        bfs_successful = true;

        // If we did not find a tau sequence to the desired transition, return the current marking for future token spawning
        if (found_markings.empty()){
            found_markings.push_back(start_marking);
            bfs_successful = false;
        }

        return found_markings;
    }

    Distance::marking_penalty_pair Distance::modifyTokenAges(TAPN::TimedPlace &place,
                                                             DiscreteVerification::NonStrictMarkingBase &marking,
                                                             TAPN::TimeInterval &interval, int tokens_to_spawn, TimedEventTraces::TimedEventTrace* in_trace) {

        DiscreteVerification::Token token(0, 1);
        bool do_increment = false;
        pen_t diff = std::numeric_limits<pen_t>::max();
        marking_penalty_pair res(marking, in_trace);

        while (tokens_to_spawn > 0){
            for (auto it = marking.getTokenList(place.getIndex()).begin(); it != marking.getTokenList(place.getIndex()).end(); it++){
                if (diff > (*it).getAge() - interval.getUpperBound() && (*it).getAge() - interval.getUpperBound() > 0){
                    token = *it;
                    do_increment = false;
                    diff = static_cast<pen_t>((*it).getAge() - interval.getUpperBound());
                } else if (diff > interval.getLowerBound() - (*it).getAge() && interval.getLowerBound() - (*it).getAge() > 0){
                    token = *it;
                    do_increment = true;
                    diff = static_cast<pen_t>(interval.getLowerBound() - (*it).getAge());
                }
            }

            if (token.getCount() > 1){
                int modify_count = 0;
                if (tokens_to_spawn >= token.getCount()){
                    modify_count = token.getCount();
                    tokens_to_spawn = tokens_to_spawn - token.getCount();

                    for (int i = modify_count; i > 0; i--){
                        marking.removeToken(place.getIndex(), token.getAge());
                        res.second->addDelayPenalty(diff);
                        if (do_increment){
                            marking.addTokenInPlace(place, interval.getLowerBound());
                        }else {
                            marking.addTokenInPlace(place, interval.getUpperBound());
                        }
                    }

                }else{ // tokens_to_spawn < token.getCount
                    modify_count = tokens_to_spawn;
                    tokens_to_spawn = 0;

                    for (int i = modify_count; i > 0; i--){
                        marking.removeToken(place.getIndex(), token.getAge());
                        res.second->addDelayPenalty(diff);
                        if (do_increment){
                            marking.addTokenInPlace(place, interval.getLowerBound());
                        }else {
                            marking.addTokenInPlace(place, interval.getUpperBound());
                        }
                    }
                }
            }else{
                marking.removeToken(place.getIndex(), token.getAge());
                res.second->addDelayPenalty(diff);
                tokens_to_spawn--;
                if (do_increment){
                    marking.addTokenInPlace(place, interval.getLowerBound());
                }else {
                    marking.addTokenInPlace(place, interval.getUpperBound());
                }
            }
            diff = std::numeric_limits<pen_t>::max();
        }

        return res;
    }

    Distance::marking_penalty_pair Distance::enableTransition(DiscreteVerification::Generator &generator,
                                                              DiscreteVerification::NonStrictMarkingBase &marking,
                                                              TAPN::TimedTransition &timed_transition, TimedEventTraces::TimedEventTrace* in_trace) {
        marking_penalty_pair res(marking, in_trace);

        for (TAPN::TimedInputArc::Vector::const_iterator it = timed_transition.getPreset().begin();
             it != timed_transition.getPreset().end(); it++){

            auto guard = (*it)->getInterval();

            int c1 = 0; // Number of tokens which satisfy guard
            int c2 = marking.numberOfTokensInPlace((*it)->getInputPlace().getIndex());  // Number of tokens in place p
            int c3 = (*it)->getWeight();

            for (auto it_t = marking.getTokenList((*it)->getInputPlace().getIndex()).begin();
                    it_t != marking.getTokenList((*it)->getInputPlace().getIndex()).end(); it_t++){
                if ((*it_t).getAge() > guard.getLowerBound() && (*it_t).getAge() < guard.getUpperBound())
                    c1 += (*it_t).getCount();
            }
            if (c1 < c3){
                if (c3 <= c2){
                    auto temp_res = modifyTokenAges((*it)->getInputPlace(), marking, guard, c3 - c1, in_trace); // TODO: how  many tokens to age modify?
                    res.first = temp_res.first;
                    res.second = temp_res.second;
                }else {
                    in_trace->addTokenPenalty((pen_t)c3 - c2);

                    // Spawn c3-c2 tokens with age 0
                    for (int i = c3 - c2; i > 0; i--) {
                        marking.addTokenInPlace((*it)->getInputPlace(), guard.getLowerBound());
                    }
                    auto temp_res = modifyTokenAges((*it)->getInputPlace(), marking, guard, c2 - c1, in_trace); // TODO: how  many tokens to age modify?
                    res.first = temp_res.first;
                    res.second = temp_res.second;
                }
            }
        }
        return res;
    }

    Distance::marking_penalty_pair Distance::firingDistance(DiscreteVerification::Generator &generator,
                                                            DiscreteVerification::NonStrictMarkingBase &marking,
                                                            std::string label, TimedEventTraces::TimedEventTrace* in_trace) {
        std::vector<TAPN::TimedTransition*> transition_matches {};
        generator.from_marking(&marking);
        marking_penalty_pair res (marking, in_trace);
        bool zero_candidate_found = false;
        bool duplicate_transition_found = false, first_enabledTransition = true; // TODO: The duplicate_transition_found variable may disable the examination of multiple transitions with the same name for a given place. Make sure this is not the case.
        auto placeholder_trace = *in_trace;

        // Find all transitions with the correct label --- assuming that no place has multiple transitions with the same label in its postset
        if (bfs_successful) { // We found a transitions with label in BFS adn we therefore only examine transitions/places in our input marking
            for (auto it = marking.getPlaceList().begin();
                 it != marking.getPlaceList().end(); it++) {
                for (auto it_ia = it->place->getInputArcs().begin();
                     it_ia != it->place->getInputArcs().end(); it_ia++) {

                /*if (duplicate_transition_found){ //TODO: remove this since we can have postset with many a labels
                    break;
                }*/
                if (generator.is_enabled(&(*it_ia)->getOutputTransition()) &&
                        (*it_ia)->getOutputTransition().getName().find(label + "_") != string::npos){

                        generator.only_transition(&(*it_ia)->getOutputTransition());
                        while (auto successor = std::unique_ptr<DiscreteVerification::NonStrictMarkingBase>(
                                generator.next(false))) {
                            res.first = *successor;
                        } //TODO: Make sure this firing results in the correct succ and not fire t succ AND delay 1 succ

                        zero_candidate_found = true;
                        break;
                    } else if ((*it_ia)->getOutputTransition().getName().find(label + "_") != string::npos) {
                        // First, find all transitions with correct label as places can have intersecting postsets
                        for (auto it_tt = transition_matches.begin(); it_tt != transition_matches.end(); it_tt++) {
                            if ((*it_tt)->getName() == (*it_ia)->getOutputTransition().getName()) {
                                duplicate_transition_found = true;
                                break;
                            }
                        }
                        if (!duplicate_transition_found)
                            transition_matches.emplace_back(&(*it_ia)->getOutputTransition());
                    }
                }

                for (auto it_ta = it->place->getTransportArcs().begin();
                     it_ta != it->place->getTransportArcs().end(); it_ta++) {

                    if (zero_candidate_found) //(duplicate_transition_found or zero_candidate_found)//TODO: remove duplicates & handle same label transitions (ensure this works)
                        break;

                    if (generator.is_enabled(&(*it_ta)->getTransition()) &&
                        (*it_ta)->getTransition().getName().find(label + "_") != string::npos) {
                        while (auto successor = std::unique_ptr<DiscreteVerification::NonStrictMarkingBase>(
                                generator.next(false))) {
                            res.first = *successor;
                        }//TODO: Make sure this firing results in the correct succ and not fire t succ AND delay 1 succ

                        zero_candidate_found = true;
                        break;
                    } else if ((*it_ta)->getTransition().getName().find(label + "_") != string::npos) {
                        for (auto it_tt = transition_matches.begin(); it_tt != transition_matches.end(); it_tt++) {
                            if ((*it_tt)->getName() != (*it_ta)->getTransition().getName()) {
                                duplicate_transition_found = true;
                                break;
                            }
                        }
                        if (!duplicate_transition_found)
                            transition_matches.emplace_back(&(*it_ta)->getTransition());
                    }
                }

                if (zero_candidate_found) {
                    break;
                }
                duplicate_transition_found = false;
            }
        }else { // Go through all tapn transitions and find ones with label
            for (auto it = tapn.getTransitions().begin(); it != tapn.getTransitions().end(); it++){
                if ((*it)->getName().find(label + "_") != std::string::npos){
                    transition_matches.emplace_back(*it);
                }
            }
        }


        //TODO: Should we throw an error if transition_matches.size()>1?? Consider the impact of our PT sub class restrictions here
        // For all found transitions calculate firing penalty
        if(!zero_candidate_found){
            for (TAPN::TimedTransition::Vector::const_iterator it_tt = transition_matches.begin(); it_tt != transition_matches.end(); it_tt++){
                //TODO: make trace tmpvar
                auto tmp_trace = placeholder_trace;
                auto enabled_transition = enableTransition(generator, marking, *(*it_tt), &tmp_trace);

                if (enabled_transition.second->getTotalDistance() < res.second->getTotalDistance() or first_enabledTransition){
                    first_enabledTransition = false;
                    generator.from_marking(&enabled_transition.first);
                    generator.only_transition(*it_tt);
                    while (auto successor = std::unique_ptr<DiscreteVerification::NonStrictMarkingBase>(generator.next(false))) {
                        res.first = *successor;
                        res.second->setDelayPenalty(enabled_transition.second->getDelayDistance());
                        res.second->setNonReplayablePenalty(enabled_transition.second->getNonReplayableDistance());
                        res.second->setTokenPenalty(enabled_transition.second->getTokenDistance());
                    }
                }
            }
        }
        return res;
    }

    Distance::marking_penalty_pair Distance::delayDistance(DiscreteVerification::Generator &generator,
                                                           DiscreteVerification::NonStrictMarkingBase &marking,
                                                           uint32_t delay, TimedEventTraces::TimedEventTrace* in_trace) {

        marking_penalty_pair res (marking, in_trace);
        generator.from_marking(&marking);

        for (auto it = marking.getPlaceList().begin(); it != marking.getPlaceList().end(); it++){
            auto p_inv = it->place->getInvariant().getBound();

            // Penalize tokens in place p
            for (auto it_t = it->tokens.begin(); it_t != it->tokens.end(); it_t++){
                if (it_t->getAge() + delay > p_inv){ //TODO: are urgent transitions instantiated with inv = 0??
                    res.second->addDelayPenalty((it_t->getAge() + delay - p_inv) * it_t->getCount());
                    it_t->incrementAge(p_inv - it_t->getAge());
                }else{
                    it->incrementAge(delay);
                }
            }

            // Penalize for urgent transitions of p
            for (auto it_ia = it->place->getInputArcs().begin(); it_ia != it->place->getInputArcs().end(); it_ia++){
                if ((*it_ia)->getOutputTransition().isUrgent() && generator.is_enabled(&(*it_ia)->getOutputTransition())){
                    res.second->addDelayPenalty(delay);
                }
            }
            for (auto it_ta = it->place->getTransportArcs().begin(); it_ta != it->place->getTransportArcs().end(); it_ta++){
                if ((*it_ta)->getTransition().isUrgent() && generator.is_enabled(&(*it_ta)->getTransition())){
                    res.second->addDelayPenalty(delay);
                }
            }
        }

        return res;
    }

    DiscreteVerification::NonStrictMarkingBase Distance::calculateDistance(
            DiscreteVerification::Generator &generator,
            std::list<DiscreteVerification::NonStrictMarkingBase> marking_candidates,
            TimedEventTraces::TimedEventTrace::id_t d, const std::string a) {

        auto tr_tmp = *current_trace;
        DiscreteVerification::NonStrictMarkingBase best_marking = marking_candidates.front();
        bool first = true;

        // Calculate timed and un-timed tetDistance and select minimal candidate
        for (auto it = marking_candidates.begin(); it != marking_candidates.end(); it++) {
            auto now_trace = tr_tmp;

            auto m_pp = delayDistance(generator, *it, d, &now_trace);

            auto m_ppp = firingDistance(generator, m_pp.first, a, m_pp.second);

            if (first or m_ppp.second->getTotalDistance() < current_trace->getTotalDistance()){
                first = false;
                best_marking = m_ppp.first;
                current_trace->setTokenPenalty(m_ppp.second->getTokenDistance());
                current_trace->setDelayPenalty(m_ppp.second->getDelayDistance());
            }
        }


        /*auto cmp = [](const Distance::marking_penalty_pair & l, const Distance::marking_penalty_pair & r)
        {
            return l.second->getTotalDistance() < r.second->getTotalDistance();
        };
        std::sort(marking_candidate_penalized->begin(), marking_candidate_penalized->end(), cmp); // TODO: Consider how to choose between marking candidates with the same penalty
*/
        //TODO: Consider how to manage local timed event penalty and sum_total
        //penalty += marking_candidate_penalized->front().second;

        //TODO: ensure that global trace's values are set correctly!!! DO NOT ALTER CURRENT TRACE BEFORE WE HAVE EXHAUSTED ALL CANDIDATES
        //auto result = marking_candidate_penalized->front().first;
        //current_trace->setTokenPenalty(marking_candidate_penalized->front().second->getTokenDistance());
        //current_trace->setDelayPenalty(marking_candidate_penalized->front().second->getDelayDistance());
        //delete marking_candidate_penalized;

        return best_marking;
    }

    Distance::pen_t Distance::tetDistance(VerifyTAPN::DiscreteVerification::Generator &generator,
                                          VerifyTAPN::DiscreteVerification::NonStrictMarkingBase &nsmb,
                                          TimedEventTraces::TimedEventTrace trace) {

        id_t previous_delay = 0;
        //TODO: Make copy of trace param to alter and remember to store distance vals in the correct trace object!!

        //continue for each trace element
        while (!trace.getTrace().empty()) {
            if (alphabet.find(trace.getTrace().front().first + "_") != alphabet.end()) { //TODO: Decide when and how we postfix labels with _ both for logs and in TAPAAL model
                // Run BFS for given trace element
                auto found_markings = tauBfs(generator, nsmb, trace.getTrace().front().first);

                // Find minimal dist for d/label
                auto final_marking = calculateDistance(generator, found_markings,
                                                           trace.getTrace().front().second - previous_delay,
                                                           trace.getTrace().front().first);

                nsmb = final_marking;
            }else {
                trace.addNonReplayablePenalty(1);
                current_trace->addNonReplayablePenalty(1);
            }

            previous_delay = trace.getTrace().front().second;
            trace.dequeueTimedAction();
        }
        return 0;
    }

    //TODO: move initialPlacement as part of constructor and make this void
    TimedEventTraces::TimedEventTrace::Vector Distance::telDistance(std::vector<int> &initialPlacement,
                                                                    std::vector<TimedEventTraces::TimedEventTrace> *tel) {

        DiscreteVerification::NonStrictMarkingBase nsmb(tapn, initialPlacement);
        AST::Query* query = nullptr; // For now we do not use query for anything
        DiscreteVerification::Generator generator = DiscreteVerification::Generator(tapn, query);

        auto tapn_transitions = tapn.getTransitions();

        // Construct our labels set
        for (auto it = tapn_transitions.begin(); it != tapn_transitions.end(); it++){
            std::string delimiter = "_";
            std::string sub_str = (*it)->getId();
            for (int i = 0; i < 2; i++){
                auto next = sub_str.find(delimiter);
                sub_str = sub_str.substr(next + 1);
            }
            alphabet.emplace(sub_str);
        }

        TimedEventTraces::TimedEventTrace::Vector dist_list{};

        // Replay all timed-event traces in the log
        for (auto it = tel->begin(); it != tel->end(); it++){
            auto tmp_nsmb = nsmb;
            current_trace = &(*it);
            tetDistance(generator, tmp_nsmb, *it);
            dist_list.emplace_back(*it);
        }

        // TODO: Sort the traces according to their distance
        /*auto cmp = [](const TimedEventTraces::TimedEventTrace & a, const TimedEventTraces::TimedEventTrace & b)
        {
            return a.getTotalDistance() < b.getTotalDistance();
        };
        std::sort(dist_list.begin(), dist_list.end(), cmp);*/

        return dist_list;
    }
}
    