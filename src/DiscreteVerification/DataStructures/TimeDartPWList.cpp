/*
 * PWList.cpp
 *
 *  Created on: 01/03/2012
 *      Author: MathiasGS
 */

#include "TimeDartPWList.hpp"

namespace VerifyTAPN {
namespace DiscreteVerification {
TimeDartPWHashMap::~TimeDartPWHashMap() {
            // We don't care, it is deallocated on program execution done
        }
    
bool TimeDartPWHashMap::add(NonStrictMarkingBase* marking, int youngest, WaitingDart* parent, int upper, int start){
	discoveredMarkings++;
	TimeDartList& m = markings_storage[marking->getHashKey()];
	for(TimeDartList::const_iterator iter = m.begin();
			iter != m.end();
			iter++){
		if((*iter)->getBase()->equals(*marking)){
				bool inWaiting = (*iter)->getWaiting() < (*iter)->getPassed();

				(*iter)->setWaiting(min((*iter)->getWaiting(),youngest));

				if((*iter)->getWaiting() < (*iter)->getPassed() && !inWaiting){
					waiting_list->add((*iter)->getBase(),(*iter));
                                        if(this->trace){
                                            ((ReachabilityTraceableDart*)(*iter))->trace = new TraceDart((*iter), parent, youngest, start, upper, marking->getGeneratedBy());
                                            this->last = ((ReachabilityTraceableDart*)(*iter))->trace;
                                        }
				}

			delete marking;

			return false;
		}
	}
        TimeDartBase* dart;
        if(this->trace){
             dart = new ReachabilityTraceableDart(marking, youngest, INT_MAX);
             ((ReachabilityTraceableDart*)dart)->trace = new TraceDart(dart, parent, youngest, start,  upper, marking->getGeneratedBy());
            this->last = ((ReachabilityTraceableDart*)(dart))->trace;
        } else {
            dart = new TimeDartBase(marking, youngest, INT_MAX);
        }
        stored++;
	m.push_back(dart);

	waiting_list->add(dart->getBase(), dart);
	return true;
}

TimeDartBase* TimeDartPWHashMap::getNextUnexplored(){
	return waiting_list->pop();
}

bool TimeDartPWPData::add(NonStrictMarkingBase* marking, int youngest, WaitingDart* parent, int upper, int start){

	return true;
}

TimeDartBase* TimeDartPWPData::getNextUnexplored(){
  
    EncodingPointer<TimeDartBase>* p = waiting_list->pop();
    NonStrictMarkingBase* m = passed.enumerateDecode(*p);
    TimeDartBase* dart = p->encoding.getMetaData();
    dart->setBase(m);
    
    p->encoding.release();
    delete p;
    return dart;
}

std::ostream& operator<<(std::ostream& out, TimeDartPWHashMap& x){
	out << "Passed and waiting:" << std::endl;
	for(TimeDartPWHashMap::HashMap::iterator iter = x.markings_storage.begin(); iter != x.markings_storage.end(); iter++){
		for(TimeDartPWHashMap::TimeDartList::iterator m_iter = iter->second.begin(); m_iter != iter->second.end(); m_iter++){
			out << "- "<< *m_iter << std::endl;
		}
	}
	out << "Waiting:" << std::endl << x.waiting_list;
	return out;
}

} /* namespace DiscreteVerification */
} /* namespace VerifyTAPN */
