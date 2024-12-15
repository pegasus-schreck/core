#pragma once
#include "qpi.h"

using namespace QPI;

constexpr uint64 NOSTROMO_MAX_USERS = 8192;
constexpr uint64 NOSTROMO_MAX_PROJECTS = 1024;
constexpr uint64 NOSTROMO_MAX_TIERS = 8;

constexpr uint8 NONE = 0;
constexpr uint8 EGG = 1;
constexpr uint8 DOG = 2;
constexpr uint8 ALIEN = 3;
constexpr uint8 WARRIOR = 4;
constexpr uint8 QUEEN = 5;

constexpr uint8 VOTE_STATE = 0;
constexpr uint8 REGISTER_STATE = 1;
constexpr uint8 INVESTMENT_PHASE_1 = 2;
constexpr uint8 INVESTMENT_PHASE_2 = 3;
constexpr uint8 INVESTMENT_PHASE_3 = 4;
constexpr uint8 CLOSED_FAILED = 5;
constexpr uint8 CLOSED_SUCCESS = 6;
constexpr uint8 BLOCKED = 7;

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
    uint64 transactionFee;

public:

    struct addUserTier_locals {
        uint64 stakedQubics;
        uint8 foundTier;
    };

    // To stake Qubic tokens and get a tier for an user
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

/*
    INITIALIZE
        state.userTiers.reset();
        state.transactionFee = 0;
    -
*/
};
