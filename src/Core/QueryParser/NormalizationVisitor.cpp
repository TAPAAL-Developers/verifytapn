#include "NormalizationVisitor.hpp"
#include "AST.hpp"

namespace VerifyTAPN
{
	namespace AST
	{

		void NormalizationVisitor::visit(NotExpression& expr, Result& context)
		{
			Tuple& tuple = static_cast<Tuple&>(context);
			Tuple any = Tuple(!tuple.negate, NULL);
			expr.getChild().accept(*this, any);
			tuple.returnExpr = static_cast<Tuple&>(any).returnExpr;
		}

		void NormalizationVisitor::visit(OrExpression& expr, Result& context)
		{
			Tuple& tuple = static_cast<Tuple&>(context);
			Tuple left = Tuple(tuple.negate, NULL), right = Tuple(tuple.negate, NULL);
			if(tuple.negate){
				expr.getLeft().accept(*this, left);
				expr.getRight().accept(*this, right);
				tuple.returnExpr = new AndExpression(static_cast<Tuple&>(left).returnExpr, static_cast<Tuple&>(right).returnExpr);
			}else{
				expr.getLeft().accept(*this, left);
				expr.getRight().accept(*this, right);
				tuple.returnExpr = new OrExpression(static_cast<Tuple&>(left).returnExpr, static_cast<Tuple&>(right).returnExpr);
			}
		}

		void NormalizationVisitor::visit(AndExpression& expr, Result& context)
		{
			Tuple& tuple = static_cast<Tuple&>(context);
			Tuple left = Tuple(tuple.negate, NULL), right = Tuple(tuple.negate, NULL);
			if(tuple.negate){
				expr.getLeft().accept(*this, left);
				expr.getRight().accept(*this, right);
				tuple.returnExpr = new OrExpression(static_cast<Tuple&>(left).returnExpr, static_cast<Tuple&>(right).returnExpr);
			}else{
				expr.getLeft().accept(*this, left);
				expr.getRight().accept(*this, right);
				tuple.returnExpr = new AndExpression(static_cast<Tuple&>(left).returnExpr, static_cast<Tuple&>(right).returnExpr);
			}
		}

		void NormalizationVisitor::visit(AtomicProposition& expr, Result& context)
		{
			Tuple& tuple = static_cast<Tuple&>(context);
			std::string op;
			if(tuple.negate){
				op = negateOperator(expr.getOperator());
			}else{
				op = expr.getOperator();
			}
			tuple.returnExpr = new AtomicProposition(&expr.getLeft(), &op, &expr.getRight());// dont visit arithmetics for now
		}
                
                void NormalizationVisitor::visit(NumberExpression& expr, Result& context){
                }
                
                void NormalizationVisitor::visit(IdentifierExpression& expr, Result& context){
                }
                
                void NormalizationVisitor::visit(MultiplyExpression& expr, Result& context){
                }
                
                void NormalizationVisitor::visit(MinusExpression& expr, Result& context){
                }
                
                void NormalizationVisitor::visit(SubtractExpression& expr, Result& context){
                }
                
                void NormalizationVisitor::visit(PlusExpression& expr, Result& context){
                }
                
                void NormalizationVisitor::visit(DeadlockExpression& expr, Result& context) {
			Tuple& tuple = static_cast<Tuple&>(context);
			tuple.returnExpr = new DeadlockExpression();                  
                }
                
		void NormalizationVisitor::visit(BoolExpression& expr, Result& context)
		{
			Tuple& tuple = static_cast<Tuple&>(context);
			if(tuple.negate){
				tuple.returnExpr = new BoolExpression(!expr.getValue());
			}else{
				tuple.returnExpr = new BoolExpression(expr.getValue());
			}
		}

		void NormalizationVisitor::visit(Query& query, Result& context)
		{
			Tuple any = Tuple(false, NULL);
			query.getChild()->accept(*this, any);

			normalizedQuery = new AST::Query(query.getQuantifier(), static_cast<Tuple&>(any).returnExpr);
		}

		std::string NormalizationVisitor::negateOperator(const std::string& op) const{
			if(op == "=" || op == "=="){
				return "!=";
			}else if(op == ">"){
				return "<=";
			}else if(op == "<"){
				return ">=";
			}else if(op == ">="){
				return "<";
			}else if(op == "<="){
				return ">";
			}else{
				std::cout << "Unknown operator";
				throw new std::exception();
			}
		}
	}
}
