using namespace QPI;

constexpr uint32 NOSTROMO_MAX_USERS = 8192;
constexpr uint32 NOSTROMO_MAX_PROJECTS = 1024;
constexpr uint32 NOSTROMO_MAX_TIERS = 8;

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

struct NOST2
{
};

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
    };

    struct removeTier_input {
    };

    struct removeTier_output {
        uint8 status;
        array<uint8, 32> message;
    };

    struct createProject_input {
        uint64 totalAmount;
        uint8 threeshold;
        uint64 tokenPrice;
        uint8 raiseInQubics;
        uint64 tokensInSale;
    };

    struct createProject_output {
        uint8 status;
        array<uint8, 32> message;
    };

    struct voteProject_input {
        uint64 projectId;
        uint8 vote;
    };

    struct voteProject_output {
        uint8 status;
        array<uint8, 32> message;
    };

    struct registerUserForProject_input {
        uint64 projectId;
    };

    struct registerUserForProject_output {
        uint8 status;
        array<uint8, 32> message;
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

    id admin;
    id wallet;
    QPI::HashMap<uint8, NOSTROMOTier, NOSTROMO_MAX_TIERS> tiers;                         
    QPI::HashMap<id, uint8, NOSTROMO_MAX_USERS> userTiers;              
    QPI::HashMap<uint64, NOSTROMOProject, NOSTROMO_MAX_PROJECTS> projects;
    uint64 stakedQubicsInContract;
    sint64 transactionFee;
    uint64 projectFee;
    uint64 projectNextId;

    struct addUserTier_locals {
        uint8 userTier;
    };

    // To stake Qubic tokens and get a tier for an user
    PUBLIC_PROCEDURE_WITH_LOCALS(addUserTier)

        if (input.tier <= 0 || input.tier > 5) {
            output.status = 1;
            if (qpi.invocationReward() > 0) {
                qpi.transfer(qpi.invocator(), qpi.invocationReward());
            }
            return;
        }

        if (qpi.invocationReward() < state.transactionFee) {
            output.status = 2;
            if (qpi.invocationReward() > 0) {
                qpi.transfer(qpi.invocator(), qpi.invocationReward());
            }
            return;
        }

        //locals.invocatorId = qpi.invocator();
        //state.userTiers.get(state.wallet, state.userTier);

        if(!state.userTiers.get(qpi.invocator(), locals.userTier)) {
            output.status = 4;
            return;
        }

        //state.userTiers.get(qpi.invocator(), state.userTier, id)
        //locals.userTier = state.userTiers.getElementIndex(qpi.invocator());

        if (locals.userTier != NONE) {
            output.status = 3;
            if (qpi.invocationReward() > 0) {
                qpi.transfer(qpi.invocator(), qpi.invocationReward());
            }
            return;
        }

        //state.tiers.get(input.tier, locals.rewardTier);
        if (locals.stakedQubics + state.transactionFee != qpi.invocationReward())
        {
            output.status = 3; 
            if (qpi.invocationReward() > 0) {
                qpi.transfer(qpi.invocator(), qpi.invocationReward());
            }
            return;
        }

        // Set the user tier
        //state.userTiers.set(qpi.invocator(), input.tier);

        // Update the staked qubics amount
        state.stakedQubicsInContract += locals.stakedQubics;

        output.status = 0; 
    _

private: 

	REGISTER_USER_FUNCTIONS_AND_PROCEDURES    
        REGISTER_USER_PROCEDURE(addUserTier, 1);
	_

};
