#pragma once
#include "qpi.h"

using namespace QPI;

struct NOST : public ContractBase
{

public:

    struct addUserTier_input {
        uint8 tier;
    };

    struct addUserTier_output {
        uint8 status;
    };

private:
    QPI::HashMap<id, uint8, 8192> userTiers;              
    sint64 transactionFee;

public:

    struct addUserTier_locals {
        uint64 stakedQubics;
        uint8 foundTier;
    };

    PUBLIC_FUNCTION_WITH_LOCALS(addUserTier)

        if (input.tier <= 0 || input.tier > 5) {
            output.status = 1;
            return;
        }

        if (qpi.invocationReward() < state.transactionFee) {
            output.status = 2;
            return;
        }

        if(!state.userTiers.get(qpi.invocator(), locals.foundTier)) {
            output.status = 4;
            return;
        }
        
        output.status = 0; 
    _

public: 

	REGISTER_USER_FUNCTIONS_AND_PROCEDURES    
        REGISTER_USER_FUNCTION(addUserTier, 1);
	_

};
