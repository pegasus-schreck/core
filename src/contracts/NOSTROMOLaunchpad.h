//#pragma once
//#include "qpi.h"
using namespace QPI;

//
// Various tier levels and their respective constants for checks below.
//
constexpr uint8 NOST_NONE = 0;
constexpr uint8 NOST_EGG = 1;
constexpr uint8 NOST_DOG = 2;
constexpr uint8 NOST_ALIEN = 3;
constexpr uint8 NOST_WARRIOR = 4;
constexpr uint8 NOST_QUEEN = 5;

//
// Various project states and their respective constancts.
//
constexpr uint8 NOST_VOTE_STATE = 0;
constexpr uint8 NOST_REGISTER_STATE = 1;
constexpr uint8 NOST_INVESTMENT_PHASE_1 = 2;
constexpr uint8 NOST_INVESTMENT_PHASE_2 = 3;
constexpr uint8 NOST_INVESTMENT_PHASE_3 = 4;
constexpr uint8 NOST_CLOSED_FAILED = 5;
constexpr uint8 NOST_CLOSED_SUCCESS = 6;
constexpr uint8 NOST_BLOCKED = 7;
constexpr uint8 NOST_DRAFT = 8;

//
// Constants for sizing
//
constexpr uint64 NOSTROMO_MAX_USERS = 131072;
constexpr uint64 NOSTROMO_MAX_PROJECTS = 1024;
constexpr uint64 NOSTROMO_MAX_LEVELS = 8;

//
// Return codes constants
//
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
    // Structures used for changeProjectState method.
    //
    struct changeProjectState_input {
        uint64 projectIdentity;
        uint8 newProjectState;
    };

    struct changeProjectState_output {
        uint8 status;
    };

    //
    // Structure for tier definitions.
    //
    struct NOSTROMOTier {
        uint64 stakeAmount;
        uint64 poolWeight;
    };

    //
    // Structures for addUserTier method.
    //
    struct addUserTier_input {
        uint8 tier;
    };

    struct addUserTier_output {
        uint8 status;
    };

    //
    // Structures for removeUserTier method.
    //
    struct removeUserTier_input {

    };

    struct removeUserTier_output {
        uint8 status;
    };

protected:

    typedef array<projectMeta,NOSTROMO_MAX_PROJECTS> projectMetadata;
    typedef array<projectFinance,NOSTROMO_MAX_PROJECTS> projectFinancials;
    typedef array<bit, NOSTROMO_MAX_PROJECTS> votes; 

    QPI::HashMap<id, votes, NOSTROMO_MAX_USERS> projectVoting;
    QPI::HashMap<uint8, NOSTROMOTier, NOSTROMO_MAX_LEVELS> tiers;
    QPI::HashMap<id, uint8, NOSTROMO_MAX_USERS> userTiers;

    projectMetadata metadataMaster; 
    projectFinancials financeMaster;

    uint64 projectNextId;    
    sint64 transactionFee;
    sint64 projectFee;

    //nostTiers tiers;

    uint64 stakedQubicsInContract;

    id wallet;
    id admin;

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

    struct changeProjectState_locals {
        projectMeta metadata;
    };

    PUBLIC_PROCEDURE_WITH_LOCALS(changeProjectState)

        //
        // Make sure the ID is at least within range of what has been stored thus far
        //        
        if (input.projectIdentity < state.projectNextId) {
            output.status = NOST_INVALID_PROJECT_ID;
            return;
        }

        if (input.newProjectState > NOST_DRAFT) {
            output.status = NOST_INVALID_STATE;
            return;
        }

        //
        // Update project state in metadata array.
        //
        locals.metadata = state.metadataMaster.get(input.projectIdentity);
        locals.metadata.projState = input.newProjectState;
        state.metadataMaster.set(input.projectIdentity, locals.metadata);
        output.status = NOST_SUCCESS;
    _

    struct addUserTier_locals {
        uint8 foundTier;
        NOSTROMOTier stakingTier;
    };

    PUBLIC_PROCEDURE_WITH_LOCALS(addUserTier)
 
        //
        // Make sure it is a valid tier, if not return error code.
        //
        if (input.tier > 5) {
            output.status = NOST_INVALID_TIER;
            qpi.transfer(qpi.invocator(), qpi.invocationReward());
            return;
        }

        //
        // Ensure proper balance transfer attempted or return
        // an error code.
        //
        if (qpi.invocationReward() < state.transactionFee) {
            output.status = NOST_INSUFFICIENT_BALANCE;
            qpi.transfer(qpi.invocator(), qpi.invocationReward());     
            return;
        }

        //
        // If user doesn't appear in map we continue to checks below,
        // else check the tier and return error if one is set.
        //
        if (state.userTiers.get(qpi.invocator(), locals.foundTier)) {
            if(locals.foundTier != NOST_NONE) {
                output.status = NOST_TIER_ALREADY_SET;
                qpi.transfer(qpi.invocator(), qpi.invocationReward());
                return;
            }
        }

        //
        // We must check to ensure user has the proper balance or return error code. 
        //
        if (state.tiers.get(input.tier, locals.stakingTier)) {
            if(locals.stakingTier.stakeAmount + state.transactionFee != qpi.invocationReward()) {
                output.status = NOST_INSUFFICIENT_BALANCE;
                qpi.transfer(qpi.invocator(), qpi.invocationReward());
                return;
            }
        }

        //
        // Set the user tier and update staking volume
        //
        state.userTiers.set(qpi.invocator(), input.tier);
        state.stakedQubicsInContract += locals.stakingTier.stakeAmount;

        //
        // Zero for status means life is good.
        //
        output.status = NOST_SUCCESS; 
    _
    
    struct removeUserTier_locals {
        uint8 foundTier;
        NOSTROMOTier stakingTier;
    };    

    PUBLIC_PROCEDURE_WITH_LOCALS(removeUserTier)
        
        // 
        // Check to ensure user has sufficient balance.
        //
        if (qpi.invocationReward() < state.transactionFee) {
            output.status = NOST_INSUFFICIENT_BALANCE;
            qpi.transfer(qpi.invocator(), qpi.invocationReward());
            return;
        }

        //
        // Validate if the user is already in a tier.
        //
        if (state.userTiers.get(qpi.invocator(), locals.foundTier))
        {
            if(locals.foundTier == NONE) {
                output.status = NOST_NO_TIER_FOUND;
                return;
            }   
        }
        else
        {
            output.status = NOST_USER_NOT_FOUND;
            return;
        }

        //
        // Set user tier to NONE
        //
        state.userTiers.set(qpi.invocator(), NOST_NONE);

        //
        // Return the staked qubics
        //
        state.tiers.get(locals.foundTier, locals.stakingTier);
        qpi.transfer(qpi.invocator(), locals.stakingTier.stakeAmount);

        // Update the staked qubics amount
        state.stakedQubicsInContract -= locals.stakingTier.stakeAmount;

        output.status = NOST_SUCCESS;
    _

	REGISTER_USER_FUNCTIONS_AND_PROCEDURES
        REGISTER_USER_PROCEDURE(createProject, 1);
        REGISTER_USER_PROCEDURE(getProject, 2);
        REGISTER_USER_PROCEDURE(changeProjectState, 3);
        REGISTER_USER_PROCEDURE(addUserTier, 4);
        REGISTER_USER_PROCEDURE(removeUserTier, 5);
    _

    INITIALIZE
        state.stakedQubicsInContract = 0;
        state.admin = qpi.invocator();
        state.wallet = qpi.invocator();
        state.transactionFee = 1000;
        state.projectFee = 10000;
        state.projectNextId = 1; 

        //
        // Initialize the tier information and set it.
        //
        state.tiers.reset();
        state.tiers.set(NOST_EGG, NOSTROMOTier{ 1, 55 });
        state.tiers.set(NOST_DOG, NOSTROMOTier{ 5, 300 });
        state.tiers.set(NOST_ALIEN, NOSTROMOTier{ 10, 75 });
        state.tiers.set(NOST_WARRIOR, NOSTROMOTier{ 30, 305 });
        state.tiers.set(NOST_QUEEN, NOSTROMOTier{ 100, 1375 });
    _
};
