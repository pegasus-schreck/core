#pragma once
#include "qpi.h"

using namespace QPI;

/*
 * Various tier levels and their respective constants for checks below.
 */
constexpr uint8 NONE = 0;
constexpr uint8 EGG = 1;
constexpr uint8 DOG = 2;
constexpr uint8 ALIEN = 3;
constexpr uint8 WARRIOR = 4;
constexpr uint8 QUEEN = 5;

/*
 * Various project states and their respective constancts.
 */
constexpr uint8 VOTE_STATE = 0;
constexpr uint8 REGISTER_STATE = 1;
constexpr uint8 INVESTMENT_PHASE_1 = 2;
constexpr uint8 INVESTMENT_PHASE_2 = 3;
constexpr uint8 INVESTMENT_PHASE_3 = 4;
constexpr uint8 CLOSED_FAILED = 5;
constexpr uint8 CLOSED_SUCCESS = 6;
constexpr uint8 BLOCKED = 7;

/*
 * Constants for sizing
 */
constexpr uint64 NOSTROMO_MAX_USERS = 8192;
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

struct NOST : public ContractBase
{

public:

    struct createProject_input {
        uint64 totalAmount;
        uint8 threeshold;
        uint64 tokenPrice;
        uint8 raiseInQubics;
        uint64 tokensInSale;
    };

    struct createProject_output {
        uint8 status;
    };


    /*
     * Type definitions for use in contract
     */
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
        QPI::HashMap<id, uint8, 131072> votes;
        QPI::HashMap<id, uint8, NOSTROMO_MAX_USERS> registeredUsers;
        QPI::HashMap<id, NOSTROMOInvestment, 131072> investments;
    };

    struct ProjectResponse {
        id owner;
        uint8 state;
        uint64 totalAmount;
        uint8 threeshold;
        uint64 tokenPrice;
        uint64 raisedAmount;
        uint8 raiseInQubics;
        uint64 tokensInSale;
    };

    struct getProject_input {
        uint64 projectId;
    };

    struct getProject_output {
        uint8 status;
        array<uint8, 32> message;
        ProjectResponse project;
    };

    /*
     * Procedure & Function related structs
     */
    struct addUserTier_input {
        uint8 tier;
    };

    struct addUserTier_output {
        uint8 status;
    };

    struct removeUserTier_input {
    };

    struct removeUserTier_output {
        uint8 status;
    };



private:

    id admin;
    id wallet;
    QPI::HashMap<id, uint8, NOSTROMO_MAX_USERS> userTiers;              
    QPI::HashMap<uint8, NOSTROMOTier, NOSTROMO_MAX_LEVELS> tiers;                         
    QPI::HashMap<uint64, NOSTROMOProject, NOSTROMO_MAX_PROJECTS> projects;

    uint64 stakedQubicsInContract;
    sint64 transactionFee;
    uint64 projectFee;
    uint64 projectNextId;

    typedef id isAdmin_input; 
    typedef bit isAdmin_output;
/*
    typedef array<,131072> projectMetadata;
    typedef array<,131072> projectFinancials;
    typedef array<,131072> projectVoting;
*/    
public:
/*
    struct createProject_input {
        uint64 totalAmount;
        uint8 threeshold;
        uint64 tokenPrice;
        uint8 raiseInQubics;
        uint64 tokensInSale;
    };

    struct createProject_output {
        uint8 status;
    };
*/

    PUBLIC_PROCEDURE_WITH_LOCALS(createProject)

        //
        // Ensure user has proper funds for project creation
        //
        if (qpi.invocationReward() < (state.transactionFee + state.projectFee)) {
            output.status = NOST_INSUFFICIENT_BALANCE;
            qpi.transfer(qpi.invocator(), qpi.invocationReward());
            return;
        }


        /*
        NOSTROMOProject project = NOSTROMOProject{
            owner: qpi.invocator(),
            state : ProjectStates.VOTE_PHASE,
            totalAmount : input.totalAmount,
            threeshold : input.threeshold,
            raiseInQubics : input.raisedInQubics,
            tokensInSale : tokensInSale
        };
        project.votes.reset();
        project.registeredUsers.reset();
        project.investments.reset();

        //projects.set(projectNextId++, );

        output.status = 0;
    _   */

    struct addUserTier_locals {
        NOSTROMOTier stakingTier;
        uint8 foundTier;
    };

   /*
    * Method used to add a tier to a specific user and to update the 
    * staking balance.  Specific error codes are returned in event of 
    * an error.        
    */
    PUBLIC_PROCEDURE_WITH_LOCALS(addUserTier)
 
        //
        // Make sure it is a valid tier, if not return error code.
        //
        if (input.tier <= 0 || input.tier > 5) {
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
        // If user doesn't exist we continue to checks below,
        // else check the tier and return error if one is set.
        //
        if (state.userTiers.get(qpi.invocator(), locals.foundTier)) {
            if(locals.foundTier == NONE) {
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
        output.status = 0; 
    _

    struct removeUserTier_locals {
        uint8 foundTier;
        NOSTROMOTier stakingTier;
    };

   /*
    * Method used to remove a tier from a user during an unstaking event.
    */
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
        state.userTiers.set(qpi.invocator(), NONE);

        //
        // Return the staked qubics
        //
        state.tiers.get(locals.foundTier, locals.stakingTier);
        qpi.transfer(qpi.invocator(), locals.stakingTier.stakeAmount);

        // Update the staked qubics amount
        state.stakedQubicsInContract -= locals.stakingTier.stakeAmount;

        output.status = NOST_SUCCESS;
    _    

/*
    struct getProject_locals {
        ProjectResponse project;
        NOSTROMOProject foundProject;
    };
*/
    /*
     * Return the project details to the caller.
     */
    /*
    PUBLIC_PROCEDURE_WITH_LOCALS(getProject)

        //
        // If project doesn't exist, return an error
        //
        if (!state.projects.get(input.projectId, locals.foundProject)) {
            output.status = NOST_PROJECT_NOT_FOUND;
            return;
        }

        locals.project.owner = locals.foundProject.owner;
        locals.project.state = locals.foundProject.state;
        locals.project.totalAmount = locals.foundProject.totalAmount;
        locals.project.threeshold = locals.foundProject.threeshold;
        locals.project.tokenPrice = locals.foundProject.tokenPrice;
        locals.project.raisedAmount = locals.foundProject.raisedAmount;
        locals.project.raiseInQubics = locals.foundProject.raiseInQubics;
        locals.project.tokensInSale = locals.foundProject.tokensInSale;
        
        output.status = NOST_SUCCESS;
        output.project = locals.project;
    _
    */

	REGISTER_USER_FUNCTIONS_AND_PROCEDURES    
        REGISTER_USER_PROCEDURE(addUserTier, 1);
        REGISTER_USER_PROCEDURE(removeUserTier, 2);
        //REGISTER_USER_PROCEDURE(getProject, 3);
	_

    //INITIALIZE
    //    state.userTiers.reset()
};
