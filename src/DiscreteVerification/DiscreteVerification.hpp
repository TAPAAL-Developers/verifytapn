/*
 * DiscreteVerification.hpp
 *
 *  Created on: 23 Feb 2012
 *      Author: jakob
 */

#ifndef DISCRETEVERIFICATION_HPP_
#define DISCRETEVERIFICATION_HPP_

#include <iostream>
#include "../Core/TAPN/TAPN.hpp"
#include "../Core/QueryParser/AST.hpp"
#include "../Core/VerificationOptions.hpp"
#include "DataStructures/NonStrictMarking.hpp"
#include "../../lib/rapidxml-1.13/rapidxml.hpp"
#include "../../lib/rapidxml-1.13/rapidxml_print.hpp"
#include "SearchStrategies/SearchStrategy.hpp"
#include "../Core/TAPNParser/util.hpp"
#include <stack>
#include "VerificationTypes/Verification.hpp"
#include "VerificationTypes/LivenessSearch.hpp"
#include "VerificationTypes/ReachabilitySearch.hpp"
#include "VerificationTypes/TimeDartReachabilitySearch.hpp"
#include "VerificationTypes/TimeDartLiveness.hpp"
#include "SearchStrategies/SearchFactory.h"
#include "DataStructures/PTrie.h"


namespace VerifyTAPN {

namespace DiscreteVerification {

class DiscreteVerification {
public:
	DiscreteVerification();
	virtual ~DiscreteVerification();
	static int run(TAPN::TimedArcPetriNet& tapn, std::vector<int> initialPlacement, AST::Query* query, VerificationOptions& options);
private:
	static void printHumanTrace(bool result, NonStrictMarking* m, std::stack<NonStrictMarking*>& stack, AST::Quantifier query);
	static void printXMLTrace(bool result, NonStrictMarking* m, std::stack<NonStrictMarking*>& stack, AST::Quantifier query);
	static rapidxml::xml_node<>* createTransitionNode(NonStrictMarking* old, NonStrictMarking* current, rapidxml::xml_document<>& doc);
	static rapidxml::xml_node<>* createTokenNode(rapidxml::xml_document<>& doc, const TAPN::TimedPlace& place, const Token& token);
	static void createTransitionSubNodes(NonStrictMarking* old, NonStrictMarking* current, rapidxml::xml_document<>& doc, rapidxml::xml_node<>* transitionNode, const TAPN::TimedPlace& place, const TAPN::TimeInterval& interval, const int weight);
	static void generateTraceStack(NonStrictMarking* m, std::stack<NonStrictMarking*>* stack, std::stack<NonStrictMarking*>* liveness = NULL);
};

}

}

#endif /* DISCRETEVERIFICATION_HPP_ */
