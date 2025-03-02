using namespace QPI;

//
// Various tier levels and their respective constants for checks below.
//
enum tierLevel {
    NOST_NONE = 0,
    NOST_EGG = 1,
    NOST_DOG = 2,
    NOST_ALIEN = 3,
    NOST_WARRIOR = 4,
    NOST_QUEEN = 5
};

//
// Various project states and their respective constancts.
//
enum projectState {
    NOST_VOTE_STATE = 0,
    NOST_REGISTER_STATE = 1,
    NOST_INVESTMENT_PHASE_1 = 2,
    NOST_INVESTMENT_PHASE_2 = 3,
    NOST_INVESTMENT_PHASE_3 = 4,
    NOST_CLOSED_FAILED = 5,
    NOST_CLOSED_SUCCESS = 6,
    NOST_BLOCKED = 7,
    NOST_ASK_MORE_INFORMATION = 8,
    NOST_PREINVEST_STATE = 9,
    NOST_PREPARE_VOTE = 10,
    NOST_FUNDED = 11,
    NOST_DRAFT = 12,
};

//
// Constants for sizing
//
constexpr uint64 NOSTROMO_MAX_USERS = 131072;
constexpr uint64 NOSTROMO_MAX_PROJECTS = 1024;
constexpr uint64 NOSTROMO_MAX_LEVELS = 8;

//
// Return codes enum
//
enum returnCodeNost {
    NOST_SUCCESS = 0,
    NOST_INVALID_TIER = 1,
    NOST_INSUFFICIENT_BALANCE = 2,
    NOST_TIER_ALREADY_SET = 3,
    NOST_USER_NOT_FOUND = 4,
    NOST_NO_TIER_FOUND = 5,
    NOST_UNABLE_TO_UNSTAKE = 6,
    NOST_PROJECT_NOT_FOUND = 7,
    NOST_PROJECT_CREATE_FAILED = 8,
    NOST_INVALID_PROJECT_ID = 9,
    NOST_INVALID_STATE = 10,
    NOST_INVALID_TRANSITION = 11,
    NOST_ALREADY_REGISTERED = 12,
    NOST_NOT_REGISTERED = 13,
    NOST_ALREADY_VOTED = 14,
    NOST_REQUIRES_ADMIN = 15
};

//
// Vote enums for consistency in code
//
enum voteValue {
    NOST_NO_VOTE = 0,
    NOST_YES_VOTE = 1
};

struct NOST2
{
};

struct NOST : public ContractBase
{

public:

    //
    // Structure for tier definitions.
    //
    struct nostromoTier {
        uint8 stakeAmount;
        float poolWeight;
    };

    //
    // Structure to hold a projects metadata as defined below:
    //
    // owner : wallet address of project creator/owner
    // projectSt : current state of the project based on phase
    // yesvotes : number of yes votes received during voting cycle
    // novotes : number of no votes received during voting cycle
    // investOne : number of epochs spent during Investment phase 1 
    // investTwo : number of epochs spent during Investment phase 2
    // investThree : number of epochs spent during Investment phase 3
    //
    struct projectMeta {
        id owner;
        projectState projectSt;
        uint64 yesvotes;
        uint64 novotes;
        uint8 investOne;
        uint8 investTwo;
        uint8 investThree;
    };

    //
    // Structure used to hold financial data related to a project:
    //
    // totalAmount : total tokens allocated for project in createProject method
    // threshold : decimal representing % of threshold between min and max caps 
    // tokenPrice : initial token price set in createProject method
    // raiseInQubics : number of Qubics allocated during project raise
    // tokensInSale : number of tokens allocated during initial sale
    //
    struct projectFinance {
        uint64 totalAmount;
        float threshold;
        uint64 tokenPrice;
        uint8 raiseInQubics;
        uint64 tokensInSale;
    };    

private:

//
    // State hash map to manage tier information
    //
    QPI::HashMap<tierLevel, NOSTROMOTier, NOSTROMO_MAX_LEVELS> tiers;

    //
    // State hash map to manage user to tier mapping
    //
    QPI::HashMap<id, tierLevel, NOSTROMO_MAX_USERS> userTiers;

    //
    // State variable to track total QUBIC staked
    //
    uint64 stakedQubicsInContract;

    //
    // Cost of a transaction with Nostromo
    //
    uint64 transactionFee;

    //
    // Fee associated with creating a project
    //
    uint64 projectFee;    

    //
    // Counter/ID used to track projects
    //
    uint64 projectNextId;
    
    //
    // The wallet of the admin
    //
    id admin;

    //
    // Array used to hold the metadata of all projects array index corresponds to projectId
    //
    array<projectMeta,NOSTROMO_MAX_PROJECTS> projectMetadataList;

    //
    // Array used to hold the financial data of all projects array index corresponds to projectId
    //
    array<projectFinance,NOSTROMO_MAX_PROJECTS> projectFinanceList;

    //
    // Typedefs & method used to identify if current wallet is admin.
    //
    struct isAdmin_input {
        id passedId;
    };

    struct isAdmin_output {
        bit status;
    };

    PRIVATE_FUNCTION(isAdmin)
        output = (input.passedId == state.admin);
    _

protected:

    //
    // Structs and method for adding a tier to a user.
    //
    struct addUserTier_input {
        tierLevel tier;
    };

    struct addUserTier_output {
        returnCodeNost status;
    };

    struct addUserTier_locals {
        tierLevel foundTier;
        nostromoTier stakingTier;
    };

    PUBLIC_PROCEDURE_WITH_LOCALS(addUserTier)
 
        //
        // Ensure proper balance transfer attempted or return
        // an error code.
        //
        if (qpi.invocationReward() < state.transactionFee) {
            output.status = returnCodeNost.NOST_INSUFFICIENT_BALANCE;
            qpi.transfer(qpi.invocator(), qpi.invocationReward());     
            return;
        }

        //
        // If user doesn't appear in map we continue to checks below,
        // else check the tier and return error if one is set.
        //
        if (state.userTiers.get(qpi.invocator(), locals.foundTier)) {
            if(locals.foundTier != tierLevel.NOST_NONE) {
                output.status = returnCodeNost.NOST_TIER_ALREADY_SET;
                qpi.transfer(qpi.invocator(), qpi.invocationReward());
                return;
            }
        }

        //
        // We must check to ensure user has the proper balance or return error code. 
        //
        if (state.tiers.get(input.tier, locals.stakingTier)) {
            if(locals.stakingTier.stakeAmount + state.transactionFee != qpi.invocationReward()) {
                output.status = returnCodeNost.NOST_INSUFFICIENT_BALANCE;
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
        output.status = returnCodeNost.NOST_SUCCESS; 
    _

    //
    // Structures for and method for removing a tier for a user.
    //
    struct removeUserTier_input {

    };

    struct removeUserTier_output {
        returnCodeNost status;
    };

    struct removeUserTier_locals {
        tierLevel foundTier;
        nostromoTier stakingTier;
    };    

    PUBLIC_PROCEDURE_WITH_LOCALS(removeUserTier)
        
        // 
        // Check to ensure user has sufficient balance.
        //
        if (qpi.invocationReward() < state.transactionFee) {
            output.status = returnCodeNost.NOST_INSUFFICIENT_BALANCE;
            qpi.transfer(qpi.invocator(), qpi.invocationReward());
            return;
        }

        //
        // Validate if the user is already in a tier.
        //
        if (state.userTiers.get(qpi.invocator(), locals.foundTier)){
            if(locals.foundTier == tierLevel.NOST_NONE) {
                output.status = returnCodeNost.NOST_NO_TIER_FOUND;
                qpi.transfer(qpi.invocator(), qpi.invocationReward());
                return;
            }   
        }
        else {
            output.status = returnCodeNost.NOST_USER_NOT_FOUND;
            qpi.transfer(qpi.invocator(), qpi.invocationReward());
            return;
        }

        //
        // Set user tier to NONE
        //
        state.userTiers.set(qpi.invocator(), tierLevel.NOST_NONE);

        //
        // Return the staked qubics
        //
        state.tiers.get(locals.foundTier, locals.stakingTier);
        qpi.transfer(qpi.invocator(), locals.stakingTier.stakeAmount);

        // Update the staked qubics amount
        state.stakedQubicsInContract -= locals.stakingTier.stakeAmount;

        output.status = returnCodeNost.NOST_SUCCESS;
    _

    //
    // Structures for and method for creating a project.
    //
    struct createProject_input {
        projectFinance financeInput;
    };

    struct createProject_output {
        returnCodeNost status;
        uint64 prodId;
    };    

    struct createProject_locals {
        projectMeta metadata;
        projectFinance financials;
    };

    PUBLIC_PROCEDURE_WITH_LOCALS(createProject)

        //
        // Ensure user has proper funds for project creation
        //
        if (qpi.invocationReward() < (state.transactionFee + state.projectFee)) {
            output.status = returnCodeNost.NOST_INSUFFICIENT_BALANCE;
            qpi.transfer(qpi.invocator(), qpi.invocationReward());
            return;
        }

        //
        // Setup local structures and store them in the maintenance arrays
        //
        locals.metadata.owner = qpi.invocator();
        locals.metadata.projState = projectState.NOST_DRAFT;
        locals.metadata.yesvotes = 0;
        locals.metadata.novotes = 0;
        locals.metadata.investPhaseOne = 0;
        locals.metadata.investPhaseTwo = 0;
        locals.metadata.investPhaseThree = 0;

        locals.financials.totalAmount = input.financeInput.totalAmount;
        locals.financials.threshold = input.financeInput.threshold;
        locals.financials.tokenPrice = input.financeInput.tokenPrice;
        locals.financials.raiseInQubics = input.financeInput.raiseInQubics;
        locals.financials.tokensInSale = input.financeInput.tokensInSale;
 
        //
        // Add project info to each respective mgmt array.
        //
        state.projectFinanceList.set(state.projectNextId, locals.financials);
        state.projectMetadataList.set(state.projectNextId, locals.metadata);

        //
        // Incremenet ProjectId counter and return related output data 
        //
        output.prodId = state.projectNextId;
        state.projectNextId += 1;
        output.status = returnCodeNost.NOST_SUCCESS;   
    _ 

    //
    // Structures for and method for getting a project.
    //
    struct getProject_input {
        uint64 projectIdentity;
    };

    struct getProject_output {
        returnCodeNost status;
        projectMeta metadata;
        projectFinance finance;
    };

    PUBLIC_PROCEDURE(getProject)

        //
        // Make sure the ID is at least within range of what has been stored thus far
        //
        if (input.projectIdentity < state.projectNextId) {
            output.status = returnCodeNost.NOST_INVALID_PROJECT_ID;
            return;
        }

        output.metadata = state.projectMetadataList.get(input.projectIdentity);
        output.finance = state.projectFinanceList.get(input.projectIdentity);
        output.status = returnCodeNost.NOST_SUCCESS;
    _

    //
    // Structures for and method for changing a project state.
    //
    struct changeProjectState_input {
        uint64 projectIdentity;
        projectState newProjectState;
    };

    struct changeProjectState_output {
        returnCodeNost status;
    };

    struct changeProjectState_locals {
        projectMeta metadata;
    };

    PUBLIC_PROCEDURE_WITH_LOCALS(changeProjectState)

        //
        // Only admin can advance a project state or it is done implicitly by contract
        //
        if (!isAdmin(qpi.invocator())) {
            output.status = returnCodeNost.NOST_REQUIRES_ADMIN;
            return;
        }

        //
        // Make sure the ID is at least within range of what has been stored thus far
        //        
        if (state.projectNextId <= input.projectIdentity) {
            output.status = returnCodeNost.NOST_INVALID_PROJECT_ID;
            return;
        }

        //
        // We should allow the state change if are in Draft or Ask for more information as the project is being promoted to next step,
        // we update the project metadata and exit method, else this is an invalid state transition.
        if (input.newProjectState == projectState.NOST_PREPARE_VOTE) {
            locals.projectMeta = projectMetadataList.get(input.projectIdentity)
            if (locals.projectMeta.projectSt == projectState.NOST_DRAFT || locals.projectMeta.projectSt == projectState.NOST_ASK_MORE_INFORMATION) {
                locals.projectMeta.projectSt = projectState.NOST_PREPARE_VOTE;
                projectMetadataList.set(input.projectIdentity, locals.projectMeta);
                output.status = returnCodeNost.NOST_SUCCESS;
                return;
            }
            else {
                output.status = returnCodeNost.NOST_INVALID_TRANSITION;
                return;
            }
        }

        //
        // A transition to asking for more info can only come from a draft or blocked state.  We cannot
        // request this while in the vote or investment stages.
        //
        if (input.newProjectState == projectState.NOST_ASK_MORE_INFORMATION) {
            locals.projectMeta = projectMetadataList.get(input.projectIdentity)
            if (locals.projectMeta.projectSt == projectState.NOST_DRAFT || locals.projectMeta.projectSt == projectState.NOST_BLOCKED) {
                locals.projectMeta.projectSt = projectState.NOST_NOST_DRAFT;
                projectMetadataList.set(input.projectIdentity, locals.projectMeta);
                output.status = returnCodeNost.NOST_SUCCESS;
                return;                
            } 
            else {
                output.status = returnCodeNost.NOST_INVALID_TRANSITION;
                return;
            }
        }

        //
        // If we get this far an attempt to transition to a state isn't allowed
        // based on contract rules.  Other states should require automated SC
        // actions to ensure movement forward.
        //
        output.status = returnCodeNost.NOST_INVALID_TRANSITION;
    _


	REGISTER_USER_FUNCTIONS_AND_PROCEDURES
        REGISTER_USER_PROCEDURE(addUserTier, 1);
        REGISTER_USER_PROCEDURE(removeUserTier, 2);
        REGISTER_USER_PROCEDURE(createProject, 3);
        REGISTER_USER_PROCEDURE(getProject, 4);
    _

    INITIALIZE

        //
        // Initialize the tier information.
        //
        state.tiers.reset();
        state.tiers.set(tierLevel.NOST_EGG, nostromoTier{ 1, 5.5 });
        state.tiers.set(tierLevel.NOST_DOG, nostromoTier{ 5, 30.0 });
        state.tiers.set(tierLevel.NOST_ALIEN, nostromoTier{ 10, 75.0 });
        state.tiers.set(tierLevel.NOST_WARRIOR, nostromoTier{ 30, 305.0 });
        state.tiers.set(tierLevel.NOST_QUEEN, nostromoTier{ 100, 1375.0 });

        //
        // Initialize key state variables.
        //
        state.transactionFee = 1000;
        state.stakedQubicsInContract = 0;
        state.projectNextId = 0;
        state.projectFee = 10000;

        //
        // Wallet of admin set at initialization
        //
        state.admin = qpi.invocator();
    _

};