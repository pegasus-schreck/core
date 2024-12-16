#pragma once
#include "qpi.h"

using namespace QPI;

/*
    Various tier levels and their respective constants for checks below.
*/
constexpr uint8 NONE = 0;
constexpr uint8 EGG = 1;
constexpr uint8 DOG = 2;
constexpr uint8 ALIEN = 3;
constexpr uint8 WARRIOR = 4;
constexpr uint8 QUEEN = 5;

/*
    Various project states and their respective constancts.
*/
constexpr uint8 VOTE_STATE = 0;
constexpr uint8 REGISTER_STATE = 1;
constexpr uint8 INVESTMENT_PHASE_1 = 2;
constexpr uint8 INVESTMENT_PHASE_2 = 3;
constexpr uint8 INVESTMENT_PHASE_3 = 4;
constexpr uint8 CLOSED_FAILED = 5;
constexpr uint8 CLOSED_SUCCESS = 6;
constexpr uint8 BLOCKED = 7;

constexpr uint64 NOSTROMO_MAX_USERS = 8192;
constexpr uint64 NOSTROMO_MAX_PROJECTS = 1024;
constexpr uint64 NOSTROMO_MAX_LEVELS = 8;

struct NOST : public ContractBase
{

public:

    struct NOSTROMOTier {
        uint64 stakeAmount;
        uint64 poolWeight;
    };

    struct NOSTROMOInvestment {
        id user;
        uint64 amount;
    };

    struct NOSTROMOProject {
        id owner;
        uint8 state;
        uint64 totalAmount;
        uint8 threeshold;
        uint64 tokenPrice;
        uint64 raisedAmount;
        uint8 raiseInQubics;
        uint64 tokensInSale;
        QPI::HashMap<id, uint8, NOSTROMO_MAX_USERS> votes;
        QPI::HashMap<id, uint8, NOSTROMO_MAX_USERS> registeredUsers;
        QPI::HashMap<id, NOSTROMOInvestment, NOSTROMO_MAX_USERS> investments;
    };

    struct addUserTier_input {
        uint8 tier;
    };

    struct addUserTier_output {
        uint8 status;
        array<uint8, 32> message;
    };

private:

    id admin;
    id wallet;
    QPI::HashMap<id, uint8, NOSTROMO_MAX_USERS> userTiers;              
    QPI::HashMap<uint8, NOSTROMOTier, NOSTROMO_MAX_LEVELS> tiers;                         
    QPI::HashMap<uint64, NOSTROMOProject, NOSTROMO_MAX_PROJECTS> projects;

    uint64 stakedQubicsInContract;
    uint64 transactionFee;
    uint64 projectFee;
    uint64 projectNextId;

    typedef id isAdmin_input; 
    typedef bit isAdmin_output;

public:

    struct addUserTier_locals {
        uint64 stakedQubics;
        uint8 foundTier;
    };

    PUBLIC_FUNCTION_WITH_LOCALS(addUserTier)

        if (input.tier <= 0 || input.tier > 5) {
            output.status = 1;
            copyMemory(output.message, "Bad tier");
            qpi.transfer(qpi.invocator(), qpi.invocationReward());
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

	REGISTER_USER_FUNCTIONS_AND_PROCEDURES    
        REGISTER_USER_FUNCTION(addUserTier, 1);
	_

    //INITIALIZE
    //    state.userTiers.reset()
};
