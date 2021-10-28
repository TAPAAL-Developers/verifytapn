#ifndef AST_HPP_
#define AST_HPP_

#include <string>
#include <iostream>
#include "Visitor.hpp"

namespace VerifyTAPN{
	namespace AST {

		class Visitable
		{
		public:
			virtual void accept(Visitor& visitor, Result& context) const = 0;
		};

		class Expression : public Visitable
		{
		public:
			virtual ~Expression() { };
			virtual Expression* clone() const = 0;
		};

		class NotExpression : public Expression
		{
		public:
			explicit NotExpression(Expression* expr) : expr(expr) { };
			NotExpression(const NotExpression& other) : expr(other.expr->clone()) { };
			NotExpression& operator=(const NotExpression& other)
			{
				if(&other != this){
					delete expr;
					expr = other.expr->clone();
				}

				return *this;
			}

			virtual ~NotExpression(){
				if( expr ) delete expr;
			};

			virtual NotExpression* clone() const;
			virtual void accept(Visitor& visitor, Result& context) const;

			const Expression& getChild() const { return *expr; }
		private:
			Expression* expr;
		};
                
                class DeadlockExpression : public Expression
                {
                    public:
			explicit DeadlockExpression() {};
			virtual ~DeadlockExpression() { };

			virtual DeadlockExpression* clone() const;
			virtual void accept(Visitor& visitor, Result& context) const;
                };

		class BoolExpression : public Expression
		{
		public:
			explicit BoolExpression(bool value) : value(value) {};
			virtual ~BoolExpression() { };

			virtual BoolExpression* clone() const;
			virtual void accept(Visitor& visitor, Result& context) const;

			bool getValue() const { return value; };
		private:
			bool value;
		};

		class AtomicProposition : public Expression
		{
		public:
			AtomicProposition(int placeIndex, std::string* op, int n) : place(placeIndex), op(op->begin(), op->end()), n(n){};
			AtomicProposition(const AtomicProposition& other) : place(other.place), op(other.op), n(other.n) { };
			AtomicProposition& operator=(const AtomicProposition& other)
			{
				if(&other != this){
					place = other.place;
					op = other.op;
					n = other.n;
				}
				return *this;
			}

			virtual ~AtomicProposition() { };

			virtual AtomicProposition* clone() const;
			virtual void accept(Visitor& visitor, Result& context) const;

			const int getPlace() const { return place; }
			const std::string& getOperator() const { return op; }
			int getNumberOfTokens() const { return n; }
		private:
			int place;
			std::string op;
			int n;
		};

		class AndExpression : public Expression
		{
		public:
			AndExpression(Expression* left, Expression* right) : left(left), right(right) { };
			AndExpression(const AndExpression& other) : left(other.left->clone()), right(other.right->clone()) {};
			AndExpression& operator=(const AndExpression& other)
			{
				if(&other != this){
					delete left;
					delete right;

					left = other.left->clone();
					right = other.right->clone();
				}
				return *this;
			}

			virtual ~AndExpression() {
				if( left ) delete left;
				if( right ) delete right;
			}

			virtual AndExpression* clone() const;
			void accept(Visitor& visitor, Result& context) const;

			const Expression& getLeft() const { return *left; }
			const Expression& getRight() const { return *right; }
		private:
			Expression* left;
			Expression* right;
		};

		class OrExpression : public Expression
		{
		public:
			OrExpression(Expression* left, Expression* right) : left(left), right(right) { };
			OrExpression(const OrExpression& other) : left(other.left->clone()), right(other.right->clone()) { };
			OrExpression& operator=(const OrExpression& other)
			{
				if(&other != this){
					delete left;
					delete right;

					left = other.left->clone();
					right = other.right->clone();
				}
				return *this;
			}


			virtual ~OrExpression(){
				if( left ) delete left;
				if( right ) delete right;
			};



			virtual OrExpression* clone() const;
			virtual void accept(Visitor& visitor, Result& context) const;

			const Expression& getLeft() const { return *left; }
			const Expression& getRight() const { return *right; }
		private:
			Expression* left;
			Expression* right;
		};

		class ParExpression : public Expression
		{
		public:
			explicit ParExpression(Expression* expr) : expr(expr) { };
			ParExpression(const ParExpression& other) : expr(other.expr->clone()) { };
			ParExpression& operator=(const ParExpression& other)
			{
				if(&other != this){
					delete expr;
					expr = other.expr->clone();
				}

				return *this;
			}

			virtual ~ParExpression(){
				if( expr ) delete expr;
			};

			virtual ParExpression* clone() const;
			virtual void accept(Visitor& visitor, Result& context) const;

			const Expression& getChild() const { return *expr; }
		private:
			Expression* expr;
		};

		enum Quantifier { EF, AG, EG, AF};

		class Query : public Visitable
		{
		public:
			Query(Quantifier quantifier, Expression* expr) : quantifier(quantifier), expr(expr) { };
			Query(const Query& other) : quantifier(other.quantifier), expr(other.expr->clone()) { };
			Query& operator=(const Query& other)
			{
				if(&other != this){
					delete expr;
					expr = other.expr->clone();
				}
				return *this;
			}

			virtual ~Query() { if( expr ) delete expr; }

			virtual Query* clone() const;
			virtual void accept(Visitor& visitor, Result& context) const;

			Quantifier getQuantifier() const { return quantifier; }
			const Expression& getChild() const { return *expr; }
		private:
			Quantifier quantifier;
			Expression* expr;
		};
	}
}

#endif /* AST_HPP_ */
