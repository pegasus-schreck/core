//#pragma once
//#include "qpi.h"
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
constexpr uint8 NOST_INVALID_STATE = 10;

struct NOST2
{
};

struct NOST : public ContractBase
{

public:

    //
    // Structures for Project Management
    //
    struct projectMeta {
        id owner;
        uint8 projState;
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
        uint64 projectIdentity;
    };

    struct getProject_output {
        uint8 status;
        projectMeta metadata;
        projectFinance finance;
    };

    //
    // Structures for user management
    //
    struct NOSTROMOTier {
        uint64 stakeAmount;
        uint64 poolWeight;
    };


protected:

    typedef array<projectMeta,NOSTROMO_MAX_PROJECTS> projectMetadata;
    typedef array<projectFinance,NOSTROMO_MAX_PROJECTS> projectFinancials;
    typedef array<bit, NOSTROMO_MAX_PROJECTS> votes; 
    typedef QPI::HashMap<id, votes, NOSTROMO_MAX_USERS> voterList;
    typedef QPI::HashMap<uint8, NOSTROMOTier, 8> nostTiers;

    projectMetadata metadataMaster; 
    projectFinancials financeMaster;
    voterList projectVoting;

    uint64 projectNextId;
    
    sint64 transactionFee;
    sint64 projectFee;

    nostTiers tiers;

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
        locals.metadata.projState = NOST_DRAFT;
        locals.metadata.yesvotes = 0;
        locals.metadata.novotes = 0;

        locals.financials.totalAmount = input.totalAmount;
        locals.financials.threshold = input.threshold;
        locals.financials.tokenPrice = input.tokenPrice;
        locals.financials.raiseInQubics = input.raiseInQubics;
        locals.financials.tokensInSale = input.tokensInSale;
 
        //
        // Add project info to each respective mgmt array.
        //
        state.financeMaster.set(state.projectNextId, locals.financials);
        state.metadataMaster.set(state.projectNextId, locals.metadata);

        //
        // Incremenet ProjectId counter and return related output data 
        //
        state.projectNextId += 1;
        output.prodId = state.projectNextId;
        output.status = NOST_SUCCESS;   
    _ 

    PUBLIC_PROCEDURE(getProject)
 
        //
        // Make sure the ID is at least within range of what has been stored thus far
        //
        if (input.projectIdentity < state.projectNextId) {
            output.status = NOST_INVALID_PROJECT_ID;
            return;
        }

        output.metadata = state.metadataMaster.get(input.projectIdentity);
        output.finance = state.financeMaster.get(input.projectIdentity);
        output.status = NOST_SUCCESS;
    _

    struct changeProjectState_input {
        uint64 projectIdentity;
        uint8 newProjectState;
    };

    struct changeProjectState_output {
        uint8 status;
    };

    PUBLIC_PROCEDURE(changeProjectState)

        //
        // Make sure the ID is at least within range of what has been stored thus far
        //        
        if (input.projectIdentity < state.projectNextId) {
            output.status = NOST_INVALID_PROJECT_ID;
            return;
        }

        if(input.newProjectState < NOST_VOTE_STATE || input.newProjectState > NOST_DRAFT) {
            output.status = NOST_INVALID_STATE;
            return;
        }

        state.metadataMaster.set(input.projectIdentity).projState = input.newProjectState;
        output.status = NOST_SUCCESS;
    _



	REGISTER_USER_FUNCTIONS_AND_PROCEDURES
        REGISTER_USER_PROCEDURE(createProject, 1);
        REGISTER_USER_PROCEDURE(getProject, 2);
    _

    INITIALIZE
        state.stakedQubicsInContract = 0;
        state.admin = qpi.invocator();
        state.wallet = qpi.invocator();
        state.transactionFee = 1000;
        state.projectFee = 10000;
        state.projectNextId = 1;
        state.tiers.reset();
        state.tiers.set(Tiers.EGG, NOSTROMOTier{ 1, 55 });
        state.tiers.set(Tiers.DOG, NOSTROMOTier{ 5, 300 });
        state.tiers.set(Tiers.ALIEN, NOSTROMOTier{ 10, 75 });
        state.tiers.set(Tiers.WARRIOR, NOSTROMOTier{ 30, 305 });
        state.tiers.set(Tiers.QUEEN, NOSTROMOTier{ 100, 1375 });
    _
};
