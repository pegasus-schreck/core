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
constexpr uint8 NOST_ASK_MORE_INFORMATION = 8;
constexpr uint8 NOST_PREINVEST_STATE = 9;
constexpr uint8 NOST_PREPARE_VOTE = 10;
constexpr uint8 NOST_FUNDED = 11;
constexpr uint8 NOST_DRAFT = 12;


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
constexpr uint8 NOST_INVALID_TRANSITION = 11;
constexpr uint8 NOST_ALREADY_REGISTERED = 12;
constexpr uint8 NOST_NOT_REGISTERED = 13;
constexpr uint8 NOST_ALREADY_VOTED = 14;
constexpr uint8 NOST_REQUIRES_ADMIN = 15;

//
// Vote enums for consistency in code
//
constexpr uint8 NOST_NO_VOTE = 0;
constexpr uint8 NOST_YES_VOTE = 1;

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
        uint8 investPhaseOne;
        uint8 investPhaseTwo;
        uint8 investPhaseThree;
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

    //
    // Structures for regForProject method.
    //
    struct regForProject_input {
        uint64 projectIdentity;
    };

    struct regForProject_output {
        uint8 status;
    };

    //
    // Structures for unregForProject method.
    //
    struct unregForProject_input {
        uint64 projectIdentity;
    };

    struct unregForProject_output {
        uint8 status;
    };

    //
    // Structures for voteProject method.
    //
    struct voteProject_input {
        uint64 projectId;
        uint8 vote;
    };

    struct voteProject_output {
        uint8 status;
    };

    // 
    // Structures foer checkProjectVote method
    //
    struct checkProjectVote_input {
        uint64 projectId;
    };

    struct checkProjectVote_output {
        uint64 yesvotes;
        uint64 novotes;
        uint8 status;
    };

    //
    // Structures for setting epoch counts
    //
    struct setPhaseOneEpochs_input {
        uint8 epochs;
    };

    struct setPhaseOneEpochs_output {
        uint8 status;
    };

    struct setPhaseTwoEpochs_input {
        uint8 epochs;
    };

    struct setPhaseTwoEpochs_output {
        uint8 status;
    };

    struct setPhaseThreeEpochs_input {
        uint8 epochs;
    };

    struct setPhaseThreeEpochs_output {
        uint8 status;
    };



private:

    typedef array<projectMeta,NOSTROMO_MAX_PROJECTS> projectMetadata;
    typedef array<projectFinance,NOSTROMO_MAX_PROJECTS> projectFinancials;
    
    typedef array<bit, NOSTROMO_MAX_PROJECTS> flags; 

    QPI::HashMap<id, flags, NOSTROMO_MAX_USERS> voteTracking;
    QPI::HashMap<id, flags, NOSTROMO_MAX_USERS> regTracking;
    QPI::HashMap<uint8, NOSTROMOTier, NOSTROMO_MAX_LEVELS> tiers;
    QPI::HashMap<id, uint8, NOSTROMO_MAX_USERS> userTiers;

    projectMetadata metadataMaster; 
    projectFinancials financeMaster;

    uint64 projectNextId;    
    sint64 transactionFee;
    sint64 projectFee;

    uint64 stakedQubicsInContract;

    id wallet;
    id admin;

    flags investProjects;
    flags preVoteProjects;
    flags inRegistration;

    typedef id isAdmin_input; 
    typedef bit isAdmin_output;

    uint8 investPhaseOneEpochs;
    uint8 investPhaseTwoEpochs;
    uint8 investPhaseThreeEpochs;

    PRIVATE_FUNCTION(isAdmin)
        output = (qpi.invocator() == state.admin);
    _

    struct manageInvestTier_input {
        projectMeta metadata;
        uint64 projectId;
    };

    struct manageInvestTier_output {
        uint8 status;
    };

    struct manageInvestTier_locals {
        uint8 placeHolder;
    };

    PRIVATE_FUNCTION_WITH_LOCALS(manageInvestTier) 

        if (input.metadata.projState == NOST_INVESTMENT_PHASE_1) {
            if (input.metadata.investPhaseOne < state.investPhaseOneEpochs) {
                
                input.metadata.investPhaseOne += 1;
            }
            else {
                input.metadata.projState = NOST_INVESTMENT_PHASE_2;
                state.metadataMaster.set(input.projectId, input.metadata);
            }
        }
        else if (input.metadata.projState == NOST_INVESTMENT_PHASE_2) {
            if (input.metadata.investPhaseTwo < state.investPhaseTwoEpochs) {

                input.metadata.investPhaseTwo += 1;
            }
            else {
                state.metadataMaster.set(input.projectId, input.metadata);
                input.metadata.projState = NOST_INVESTMENT_PHASE_3;
            }
        }
        else if (input.metadata.projState == NOST_INVESTMENT_PHASE_3){
            if (input.metadata.investPhaseThree < state.investPhaseThreeEpochs) {
                
                input.metadata.investPhaseThree += 1;
            }
            else {
                
            }
        }
        else {
            output.status = NOST_INVALID_TIER;
            return;
        }
    _

protected:

    PUBLIC_PROCEDURE_WITH_LOCALS(setPhaseOneEpochs)

        if (!isAdmin()) {
            output.status = NOST_REQUIRES_ADMIN;
            return;
        }

        state.investPhaseOneEpochs = input.epochs;
        output.status = NOST_SUCCESS;
    _

    PUBLIC_PROCEDURE_WITH_LOCALS(setPhaseTwoEpochs)

        if (!isAdmin()) {
            output.status = NOST_REQUIRES_ADMIN;
            return;
        }

        state.investPhaseTwoEpochs = input.epochs;
        output.status = NOST_SUCCESS;
    _

    PUBLIC_PROCEDURE_WITH_LOCALS(setPhaseThreeEpochs)

        if (!isAdmin()) {
            output.status = NOST_REQUIRES_ADMIN;
            return;
        }

        state.investPhaseThreeEpochs = input.epochs;
        output.status = NOST_SUCCESS;
    _        

    struct unregForProject_locals {
        flags userFlags; 
        bit regFlag;
        projectMeta metadata;
    };

    PUBLIC_PROCEDURE_WITH_LOCALS(unregForProject)
        
        if (input.projectIdentity < state.projectNextId) {
            output.status = NOST_INVALID_PROJECT_ID;
            return;
        }

        //
        // Ensure project is in proper state.
        //
        locals.metadata = state.metadataMaster.get(input.projectIdentity);
        if(locals.metadata.projState == NOST_REGISTER_STATE) {

            //
            // Check is user is already registered, if not toggle the bit to 
            // indicate success.  If user isn't in the listing add him and 
            // set the registration bit.
            //
            if (state.regTracking.get(qpi.invocator(), locals.userFlags)) {
                locals.regFlag = locals.userFlags.get(input.projectIdentity);

                //
                // Not registered treat as successful. otherwise unregister
                // and declare success.
                //
                if (locals.regFlag == 0) {
                    output.status = NOST_SUCCESS;
                    return;
                }
                else {
                    locals.userFlags.set(input.projectIdentity, 0);
                    state.regTracking.set(qpi.invocator(), locals.userFlags);
                    output.status = NOST_SUCCESS;
                    return;
                }
            }
            //
            // User never attempted to register, so success!
            //
            else {
                output.status = NOST_SUCCESS;
                return;
            }

        }
        else {
            output.status = NOST_INVALID_STATE;
            return;
        }
    _

    struct regForProject_locals {
        flags userFlags; 
        bit regFlag;
        projectMeta metadata;
        uint8 tier;
    };

    PUBLIC_PROCEDURE_WITH_LOCALS(regForProject)

        if (input.projectIdentity < state.projectNextId) {
            output.status = NOST_INVALID_PROJECT_ID;
            return;
        }

        //
        // Check user tier, if one hasn't been added or is NONE we 
        // return a bad status.
        //
        if (state.userTiers.get(qpi.invocator(), locals.tier)) {
            if (locals.tier == NOST_NONE) {
                output.status = NOST_INVALID_TIER;
                return;
            }
        }
        else {
            output.status = NOST_INVALID_TIER;
            return;
        }

        //
        // Ensure project is in proper state.
        //
        locals.metadata = state.metadataMaster.get(input.projectIdentity);
        if(locals.metadata.projState == NOST_REGISTER_STATE) {

            //
            // Check is user is already registered, if not toggle the bit to 
            // indicate success.  If user isn't in the listing add him and 
            // set the registration bit.
            //
            if (state.regTracking.get(qpi.invocator(), locals.userFlags)) {
                locals.regFlag = locals.userFlags.get(input.projectIdentity);

                if (locals.regFlag == 1) {
                    output.status = NOST_ALREADY_REGISTERED;
                    return;
                }
                else {
                    locals.userFlags.set(input.projectIdentity, 1);
                    state.regTracking.set(qpi.invocator(), locals.userFlags);
                    output.status = NOST_SUCCESS;
                    return;
                }
            }
            else {
                locals.userFlags.set(input.projectIdentity, 1);
                state.regTracking.set(qpi.invocator(), locals.userFlags);        
                output.status = NOST_SUCCESS;
                return;
            }

        }
        else {
            output.status = NOST_INVALID_STATE;
            return;
        }
    _

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
        locals.metadata.investPhaseOne = 0;
        locals.metadata.investPhaseTwo = 0;
        locals.metadata.investPhaseThree = 0;

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
        output.prodId = state.projectNextId;
        state.projectNextId += 1;
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
        if (state.projectNextId <= input.projectIdentity) {
            output.status = NOST_INVALID_PROJECT_ID;
            return;
        }

        if (input.newProjectState > NOST_DRAFT) {
            output.status = NOST_INVALID_STATE;
            return;
        }

        if (input.newProjectState == NOST_PREPARE_VOTE) {
            state.preVoteProjects.set(input.projectIdentity, 1);
        }

        //
        // Check current state to ensure we aren't making an invalid state transition.
        //
        locals.metadata = state.metadataMaster.get(input.projectIdentity);

        if (input.newProjectState >= NOST_INVESTMENT_PHASE_1 && input.newProjectState <= NOST_BLOCKED) {
            output.status = NOST_INVALID_TRANSITION;
            return;            
        }

        //
        // Must be done by smart contract
        //
        if (input.newProjectState == NOST_VOTE_STATE || input.newProjectState == NOST_REGISTER_STATE) {
            output.status = NOST_INVALID_TRANSITION;
            return;                        
        }

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
        if (state.userTiers.get(qpi.invocator(), locals.foundTier)){
            if(locals.foundTier == NOST_NONE) {
                output.status = NOST_NO_TIER_FOUND;
                return;
            }   
        }
        else{
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

    struct voteProject_locals {
        uint8 localTier;
        projectMeta metadata;
        flags votingList;
    };

    // To vote for a project
    PUBLIC_PROCEDURE_WITH_LOCALS(voteProject)

        if (qpi.invocationReward() < state.transactionFee) {
            output.status = NOST_INSUFFICIENT_BALANCE;
            qpi.transfer(qpi.invocator(), qpi.invocationReward());
            return;
        }

        if (input.projectId >= state.projectNextId) {
            output.status = NOST_INVALID_PROJECT_ID;
            qpi.transfer(qpi.invocator(), qpi.invocationReward());
            return;            
        }

        // Check the user tier, if they aren't yet in the array set
        // it to invalid tier and do the same thing if their tier
        // is NONE.
        if (state.userTiers.get(qpi.invocator(), locals.localTier)) {
            if (locals.localTier == NOST_NONE) {
                output.status = NOST_INVALID_TIER;
                qpi.transfer(qpi.invocator(), qpi.invocationReward());
                return;            
            }            
        }
        else {
            output.status = NOST_INVALID_TIER;
            qpi.transfer(qpi.invocator(), qpi.invocationReward());
            return;            
        }

        //
        // Check current state to ensure we can vote.
        //
        locals.metadata = state.metadataMaster.get(input.projectId);

        // Check the project is in VOTE phase
        if (locals.metadata.projState != NOST_VOTE_STATE) {
            output.status = NOST_INVALID_STATE;
            qpi.transfer(qpi.invocator(), qpi.invocationReward());
            return;
        }

        if (state.voteTracking.get(qpi.invocator(),locals.votingList)) {
            if (locals.votingList.get(input.projectId)) {
                output.status = NOST_ALREADY_VOTED;
                qpi.transfer(qpi.invocator(), qpi.invocationReward());
                return;
            }
            else {
                if (input.vote) {
                    locals.projectMeta.yesvotes += 1;
                }
                else {
                    locals.projectMeta.novotes += 1;
                }
                locals.votingList.set(input.projectId, 1);
                state.metadataMaster.set(input.projectId, locals.projectMeta);
                state.voteTracking.set(qpi.invocator(), locals.votingList);
            }

        }

        output.status = NOST_SUCCESS;
    _    

    struct checkProjectVote_locals {
        projectMeta metadata;
    };

    PUBLIC_PROCEDURE_WITH_LOCALS(checkProjectVote)
        if (input.projectId < state.projectNextId) {
            locals.metadata = state.metadataMaster.get(input.projectId);
            output.yesvotes = locals.metadata.yesvotes;
            output.novotes = locals.metadata.novotes;
            output.status = NOST_SUCCESS;
        }
        else {
            output.status = NOST_INVALID_PROJECT_ID;
        }
    _

	REGISTER_USER_FUNCTIONS_AND_PROCEDURES
        REGISTER_USER_PROCEDURE(createProject, 1);
        REGISTER_USER_PROCEDURE(getProject, 2);
        REGISTER_USER_PROCEDURE(changeProjectState, 3);
        REGISTER_USER_PROCEDURE(addUserTier, 4);
        REGISTER_USER_PROCEDURE(removeUserTier, 5);
        REGISTER_USER_PROCEDURE(voteProject, 6);
        REGISTER_USER_PROCEDURE(checkProjectVote, 7);
    _

    INITIALIZE
        state.stakedQubicsInContract = 0;
        state.admin = qpi.invocator();
        state.wallet = qpi.invocator();
        state.transactionFee = 1000;
        state.projectFee = 10000;
        state.projectNextId = 0; 

        //
        // Initialize the tier information and set it.
        //
        state.tiers.reset();
        state.tiers.set(NOST_EGG, NOSTROMOTier{ 1, 55 });
        state.tiers.set(NOST_DOG, NOSTROMOTier{ 5, 300 });
        state.tiers.set(NOST_ALIEN, NOSTROMOTier{ 10, 75 });
        state.tiers.set(NOST_WARRIOR, NOSTROMOTier{ 30, 305 });
        state.tiers.set(NOST_QUEEN, NOSTROMOTier{ 100, 1375 });

        state.investPhaseOneEpochs = 2;
        state.investPhaseTwoEpochs = 1;
        state.investPhaseThreeEpochs = 1;
    _

    
    struct BEGIN_EPOCH_locals {
        uint64 index;
        projectMeta metadata;
    };

    BEGIN_EPOCH_WITH_LOCALS

        for (locals.index = 0; locals.index < NOSTROMO_MAX_PROJECTS; locals.index++) {
            
            if (state.preVoteProjects.get(locals.index) == 1) {
                locals.metadata = state.metadataMaster.get(locals.index);

                //
                // Before transitioning state lets make sure we are in Pre.
                //
                if (locals.metadata.projState == NOST_PREPARE_VOTE) {
                    locals.metadata.projState = NOST_VOTE_STATE;
                    state.metadataMaster.set(locals.index, locals.metadata);
                }
                else {
                    //
                    // Set it to 0 so it doesn't trigger on the next Epoch regardless.
                    //
                    state.preVoteProjects.set(locals.index, 0);
                }
            }

            if (state.investProjects.get(locals.index) == 1) {
                locals.metadata = state.metadataMaster.get(locals.index);
                
            }
        }        
    _

	struct END_EPOCH_locals {
        uint64 index;
        projectMeta metadata;
    };
    
    END_EPOCH_WITH_LOCALS
	    
        //
        // Walk through lists, if in PRE_VOTE state move it to vote and wait till 
        // the end of the epoch.  If in VOTE_STATE we check the vote tally and if 
        // the vote wins it moves to registration, if it loses the vote it is set
        // to CLOSED_FAILED.
        //
        for (locals.index = 0; locals.index < NOSTROMO_MAX_PROJECTS; locals.index++) {
            
            //
            // If project has been moved to 
            // 
            if (state.preVoteProjects.get(locals.index) == 1) {
                locals.metadata = state.metadataMaster.get(locals.index);

                //
                // Before transitioning state lets make sure we are in Pre.
                //
                if (locals.metadata.projState == NOST_PREPARE_VOTE) {
                    locals.metadata.projState = NOST_VOTE_STATE;
                    state.metadataMaster.set(locals.index, locals.metadata);
                }
                else if (locals.metadata.projState == NOST_VOTE_STATE) {
                    
                    //
                    // Has been in voting for 1 epoch, check tally and set appropriately. 
                    //
                    if (locals.metadata.yesvotes > locals.metadata.novotes) {
                        locals.metadata.projState = NOST_REGISTER_STATE;
                        state.metadataMaster.set(locals.index, locals.metadata);

                        state.preVoteProjects.set(locals.index, 0);
                        state.inRegistration.set(locals.index, 1);
                    }
                    else {
                        locals.metadata.projState = NOST_CLOSED_FAILED;
                        state.metadataMaster.set(locals.index, locals.metadata);
                        state.preVoteProjects.set(locals.index, 0);
                    }
                }
                else {
                    //
                    // Set it to 0 so it doesn't trigger on the next Epoch regardless.
                    //
                    state.preVoteProjects.set(locals.index, 0);
                }
            }

            //
            // If it's been in registration for a week we move it to the investor phase.
            //
            if (state.inRegistration.get(locals.index) == 1) {
                locals.metadata = state.metadataMaster.get(locals.index);

                locals.metadata.projState = NOST_INVESTMENT_PHASE_1;
                state.metadataMaster.set(locals.index, locals.metadata);

                state.inRegistration.set(locals.index, 0);
                state.investProjects.set(locals.index, 1);
            }

            if (state.investProjects.get(locals.index) == 1) {
                locals.metadata = state.metadataMaster.get(locals.index);

            }
        }
    _

};
