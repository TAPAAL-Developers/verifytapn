/*
 * QueryVisitor.hpp
 *
 *  Created on: 21/03/2012
 *      Author: MathiasGS
 */

#ifndef WEIGHTQUERYVISITOR_HPP_
#define WEIGHTQUERYVISITOR_HPP_

#include "../../Core/QueryParser/Visitor.hpp"
#include "../DataStructures/NonStrictMarking.hpp"
#include "../../Core/QueryParser/AST.hpp"
#include <exception>
#include <limits.h>

namespace VerifyTAPN {
namespace DiscreteVerification {

using namespace AST;

class WeightQueryVisitor : public Visitor{
	public:
	WeightQueryVisitor(NonStrictMarkingBase& marking) : marking(marking){};
		virtual ~WeightQueryVisitor(){};

	public: // visitor methods
		virtual void visit(NotExpression& expr, Result& context);
		virtual void visit(OrExpression& expr, Result& context);
		virtual void visit(AndExpression& expr, Result& context);
		virtual void visit(AtomicProposition& expr, Result& context);
		virtual void visit(BoolExpression& expr, Result& context);
		virtual void visit(Query& query, Result& context);
                virtual void visit(DeadlockExpression& expr, Result& context);
                virtual void visit(NumberExpression& expr, Result& context);
                virtual void visit(IdentifierExpression& expr, Result& context);
                virtual void visit(MultiplyExpression& expr, Result& context);
                virtual void visit(MinusExpression& expr, Result& context);
                virtual void visit(SubtractExpression& expr, Result& context);
                virtual void visit(PlusExpression& expr, Result& context);
	private:
		int compare(int numberOfTokensInPlace, const std::string& op, int n) const;

	private:
		const NonStrictMarkingBase& marking;
};

} /* namespace DiscreteVerification */
} /* namespace VerifyTAPN */
#endif /* QUERYVISITOR_HPP_ */
