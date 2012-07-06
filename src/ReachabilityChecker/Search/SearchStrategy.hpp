#ifndef SEARCHSTRATEGY_HPP_
#define SEARCHSTRATEGY_HPP_

#include "../PassedWaitingList/WaitingList.hpp"
#include "../PassedWaitingList/PWList.hpp"
#include "../QueryChecker/QueryChecker.hpp"
#include "../../Core/VerificationOptions.hpp"
#include "../Trace/TraceStore.hpp"
#include "../SuccessorGenerator.hpp"

namespace VerifyTAPN
{
	class PassedWaitingList;
	class SymbolicMarking;

	namespace TAPN{
		class TimedArcPetriNet;
	}

	namespace AST{
		class Query;
	}

	class SearchStrategy
	{
	public:
		virtual ~SearchStrategy() { };
		virtual void Init() = 0;
		virtual bool Verify() = 0;
		virtual unsigned int MaxUsedTokens() const = 0;
		virtual Stats GetStats() const = 0;
		virtual void PrintTraceIfAny(bool result) const = 0;
		virtual void PrintTransitionStatistics() const = 0;
	};



	class DefaultSearchStrategy : public SearchStrategy
	{
	public:
		DefaultSearchStrategy(
			const VerifyTAPN::TAPN::TimedArcPetriNet& tapn,
			SymbolicMarking* initialMarking,
			const AST::Query* query,
			const VerificationOptions& options,
			MarkingFactory* factory
		);
		virtual ~DefaultSearchStrategy() { delete pwList; delete[] maxConstantsArray; };
		virtual void Init()
		{
			pwList = new PWList(CreateWaitingList(), factory);
		}

		virtual bool Verify();
		virtual unsigned int MaxUsedTokens() const { return succGen.MaxUsedTokens(); };
		virtual Stats GetStats() const;
		virtual void PrintTraceIfAny(bool result) const;
		virtual void PrintTransitionStatistics() const;
	protected:
		virtual WaitingList* CreateWaitingList() const = 0;
	private:
		virtual bool CheckQuery(const SymbolicMarking& marking) const;
		virtual void CreateLastInvariant(const SymbolicMarking& marking, std::vector<TraceInfo::Invariant>& invariants) const;
		void PrintDiagnostics(size_t successors) const;
		void UpdateMaxConstantsArray(const SymbolicMarking& marking);
	private:
		PassedWaitingList* pwList;
		const VerifyTAPN::TAPN::TimedArcPetriNet& tapn;
		SymbolicMarking* initialMarking;
		const QueryChecker checker;
		VerificationOptions options;
		SuccessorGenerator succGen;
		MarkingFactory* factory;
		TraceStore traceStore;
		int* maxConstantsArray;
	};
}

#endif /* SEARCHSTRATEGY_HPP_ */
