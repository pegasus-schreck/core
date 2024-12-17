#pragma once
#include "qpi.h"

using namespace QPI;

/*
 * Various tier levels and their respective constants for checks below.
 */
constexpr uint8 NOST_NONE = 0;
constexpr uint8 NOST_EGG = 1;
constexpr uint8 NOST_DOG = 2;
constexpr uint8 NOST_ALIEN = 3;
constexpr uint8 NOST_WARRIOR = 4;
constexpr uint8 NOST_QUEEN = 5;

/*
 * Various project states and their respective constancts.
 */
constexpr uint8 NOST_VOTE_STATE = 0;
constexpr uint8 NOST_REGISTER_STATE = 1;
constexpr uint8 NOST_INVESTMENT_PHASE_1 = 2;
constexpr uint8 NOST_INVESTMENT_PHASE_2 = 3;
constexpr uint8 NOST_INVESTMENT_PHASE_3 = 4;
constexpr uint8 NOST_CLOSED_FAILED = 5;
constexpr uint8 NOST_CLOSED_SUCCESS = 6;
constexpr uint8 NOST_BLOCKED = 7;
constexpr uint8 NOST_DRAFT = 8;
/*
 * Constants for sizing
 */
constexpr uint64 NOSTROMO_MAX_USERS = 131072;
constexpr uint64 NOSTROMO_MAX_PROJECTS = 1024;
constexpr uint64 NOSTROMO_MAX_LEVELS = 8;

/*
 * Return codes constants
 */
constexpr uint8 NOST_SUCCESS = 0;
constexpr uint8 NOST_INVALID_TIER = 1;
constexpr uint8 NOST_INSUFFICIENT_BALANCE = 2;
constexpr uint8 NOST_TIER_ALREADY_SET = 3;
constexpr uint8 NOST_USER_NOT_FOUND = 4;
constexpr uint8 NOST_NO_TIER_FOUND = 5;
constexpr uint8 NOST_UNABLE_TO_UNSTAKE = 6;
constexpr uint8 NOST_PROJECT_NOT_FOUND = 7;
constexpr uint8 NOST_PROJECT_CREATE_FAILED = 8;
constexpr uint8 NOST_INVALID_PROJECT_ID = 9;

struct NOST : public ContractBase
{

public:

    //
    // Structures used for the createProject method.
    //
    struct createProject_input {
        uint64 totalAmount;
        uint8 threshold;
        uint64 tokenPrice;
        uint8 raiseInQubics;
        uint64 tokensInSale;
    };

    struct createProject_output {
        uint8 status;
        uint64 prodId;
    };

    //
    // Structures used for the getProject method.
    //
    struct getProject_input {
        //uint64 projectIdentity;
    };

    struct getProject_output {
        //id owner;
        //uint8 state;
        //uint64 totalAmount;
        //uint8 threshold;
        //uint64 tokenPrice;
        //uint64 raiseAmount;
        //uint8 raiseInQubics;
        //uint64 tokensInSale;
        uint8 status;
    };


    //
    // Structures for Project Management
    //
    struct projectMeta {
        id owner;
        uint8 state;
        uint64 yesvotes;
        uint64 novotes;
    };

    struct projectFinance {
        uint64 totalAmount;
        uint8 threshold;
        uint64 tokenPrice;
        uint8 raiseInQubics;
        uint64 tokensInSale;
    };

    typedef array<bit, NOSTROMO_MAX_PROJECTS> votes; 
    typedef QPI::HashMap<id, votes, NOSTROMO_MAX_USERS> voterList;

protected:

    typedef array<projectMeta,NOSTROMO_MAX_PROJECTS> projectMetadata;
    typedef array<projectFinance,NOSTROMO_MAX_PROJECTS> projectFinancials;

    projectMetadata metadataMaster; 
    projectFinancials financeMaster;
    voterList projectVoting;

    uint64 projectNextId;
    
    sint64 transactionFee;
    sint64 projectFee;

    struct createProject_locals {
        projectMeta metadata;
        projectFinance financials;
    };

    PUBLIC_PROCEDURE_WITH_LOCALS(createProject)

        //
        // Ensure user has proper funds for project creation
        //
        if (qpi.invocationReward() < (state.transactionFee + state.projectFee)) {
            output.status = NOST_INSUFFICIENT_BALANCE;
            qpi.transfer(qpi.invocator(), qpi.invocationReward());
            return;
        }

        //
        // Setup local structures and store them in the maintenance arrays
        //
        locals.metadata.owner = qpi.invocator();
        locals.metadata.state = NOST_DRAFT;
        locals.metadata.yesvotes = 0;
        locals.metadata.novotes = 0;

        locals.financials.totalAmount = input.totalAmount;
        locals.financials.threshold = input.threshold;
        locals.financials.tokenPrice = input.tokenPrice;
        locals.financials.raiseInQubics = input.raiseInQubics;
        locals.financials.tokensInSale = input.tokensInSale;

        state.financeMaster.set(state.projectNextId, locals.financials);
        state.metadataMaster.set(state.projectNextId, locals.metadata);

        //
        // Incremenet ProjectId counter and return related output data 
        //
        state.projectNextId += 1;
        output.prodId = state.projectNextId;
        output.status = 0;   

    _ 

    struct getProject_locals {
        //projectMeta metadata;
        //projectFinance financial;
    };

    PUBLIC_PROCEDURE_WITH_LOCALS(getProject)

        output.status = NOST_NONE;

    -

	REGISTER_USER_FUNCTIONS_AND_PROCEDURES

        REGISTER_USER_PROCEDURE(createProject, 1);
        REGISTER_USER_PROCEDURE(getProject, 2);
	
    _

};
