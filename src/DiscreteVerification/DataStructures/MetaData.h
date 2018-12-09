/* 
 * File:   MetaData.h
 * Author: Peter G. Jensen
 *
 * Created on 13 September 2015, 14:31
 */

#ifndef METADATA_H
#define	METADATA_H

#include <iostream>
#include "../../Core/TAPN/TAPN.hpp"
#include "ptrie.h"

using namespace ptrie;
using namespace std;

namespace VerifyTAPN {
namespace DiscreteVerification {

	class NonStrictMarking;
 
   struct MetaData {
    public:
        MetaData() : passed(false), inTrace(false), totalDelay(0) {};
        virtual ~MetaData(){};
        bool passed;
        bool inTrace;
        int totalDelay;
    };
    
    struct MetaDataWithTrace : public MetaData {
        const TAPN::TimedTransition* generatedBy;
    };
   
    struct WorkflowSoundnessMetaData : public MetaData {
	   public:
    	WorkflowSoundnessMetaData() : MetaData(), parents() { totalDelay = INT_MAX;};
        virtual ~WorkflowSoundnessMetaData(){parents.clear();}
		   vector<MetaData*> parents;
	   };
           
    
    struct MetaDataWithTraceAndEncoding : public MetaDataWithTrace {
        ptriepointer_t<MetaData*> ep;
            MetaDataWithTraceAndEncoding* parent;
    };
    
    struct WorkflowSoundnessMetaDataWithEncoding : public MetaDataWithTraceAndEncoding {
    public:
        WorkflowSoundnessMetaDataWithEncoding() : MetaDataWithTraceAndEncoding(), parents() 
            { totalDelay = INT_MAX;};
            virtual ~WorkflowSoundnessMetaDataWithEncoding()
            {
                parents.clear();
            }
            
            vector<MetaData*> parents;
    };
}
}
#endif	/* METADATA_H */

