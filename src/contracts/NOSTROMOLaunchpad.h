using namespace QPI;

constexpr uint64 NOSTROMO_MAX_USERS = 131072;
constexpr uint64 NOSTROMO_MAX_PROJECTS = 1024;


// Tier enumerations
#define NONE 0
#define EGG 1
#define DOG 2
#define ALIEN 3
#define WARRIOR 4
#define QUEEN 5

// State enumerations
#define VOTE_STATE 0
#define REGISTER_STATE 1
#define INVESTMENT_PHASE_1 2
#define INVESTMENT_PHASE_2 3
#define INVESTMENT_PHASE_3 4
#define CLOSED_FAILED 5
#define CLOSED_SUCCESS 6
#define BLOCKED 7

struct NOSTROMO2
{
};

struct NOSTROMO : public ContractBase
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

    // Input and Output Structs
    struct addUserTier_input {
        uint8 tier;
    };

    struct addUserTier_output {
        uint8 status;
        array<uint8, 32> message;
    };

    struct removeUserTier_input {
    };

    struct removeUserTier_output {
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

    struct voteProject_output{
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

protected:

    uint64 stakedQubicsInContract;

    // To stake Qubic tokens and get a tier for an user
    PUBLIC_PROCEDURE(addUserTier)
        // Validate the input
        if (input.tier <= 0 || input.tier > 5) {
            output.status = 1; // Error
            copyMemory(output.message, "Bad tier");

            // WFS: add reward check
            if(qpi.invocationReward() > 0)
            {
                qpi.transfer(qpi.invocator(), qpi.invocationReward());
            }
            return;
        }

        // Validate the fee
        if (qpi.invocationReward() < state.transactionFee) {
            output.status = 2; // Error
            copyMemory(output.message, "Insufficient transaction fee"); 
            
            // WFS: add reward check
            if(qpi.invocationReward() > 0)
            {
                qpi.transfer(qpi.invocator(), qpi.invocationReward());
            }

            return;
        }

        // Validate if the user has a tier
        uint8 userTier = userTiers.get(qpi.invocator());
        if (userTier != NONE) {
            output.status = 2; // Error
            copyMemory(output.message, "Tier found");
            
            // WFS: add reward check
            if(qpi.invocationReward() > 0)
            {
                qpi.transfer(qpi.invocator(), qpi.invocationReward());
            }

            return;
        }

        // Stake the QUBIC tokens
        uint64 stakedQubics = tiers.get(input.tier);

        if (stakedQubics + state.transactionFee != qpi.invocationReward())
        {
            output.status = 3; // Error
            copyMemory(output.message, "No enought tokens to stake");
            
            // WFS: add reward check
            if(qpi.invocationReward() > 0)
            {
                qpi.transfer(qpi.invocator(), qpi.invocationReward());
            }

            return;
        }

        // Set the user tier
        userTiers.set(qpi.invocator(), input.tier);

        // Update the staked qubics amount
        stakedQubicsInContract += stakedQubics;

        output.status = 0; // Success
        copyMemory(output.message, "");
    _

    struct removeUserTier_locals {
        uint8 userTier;
    };

    // To unstake Qubic tokens and clear the user tier
    PUBLIC_PROCEDURE_WITH_LOCALS(removeUserTier)
        // Validate the fee
        if (qpi.invocationReward() < state.transactionFee) {
            output.status = 1; // Error
            copyMemory(output.message, "Insufficient transaction fee");

            // WFS: add reward check
            if(qpi.invocationReward() > 0)
            {
                qpi.transfer(qpi.invocator(), qpi.invocationReward());
            }

            return;
        }

        // Validate if the user has a tier
        locals.userTier = userTiers.get(qpi.invocator());
        if (locals.userTier == NONE) {
            output.status = 2; // Error
            copyMemory(output.message, "No tier found");

            // WFS: add reward check
            if(qpi.invocationReward() > 0)
            {
                qpi.transfer(qpi.invocator(), qpi.invocationReward());
            }

            return;
        }

        // Remove the tier
        userTiers.set(qpi.invocator(), NONE);

        // Return the staked qubics
        uint64 stakedQubics = tiers.get(userTier);
        qpi.transfer(qpi.invocator(), stakedQubics);

        // Update the staked qubics amount
        stakedQubicsInContract -= stakedQubics;

        output.status = 0; // Success
        copyMemory(output.message, "");
    _

    struct createProject_locals {
        NOSTROMOProject project;
    };

    // To create a new project
    PUBLIC_PROCEDURE_WITH_LOCALS(createProject)
        // Validate the fee
        if (qpi.invocationReward() < state.transactionFee + state.projectFee) {
            output.status = 1; // Error
            copyMemory(output.message, "Insufficient transaction fee");
            
            // WFS: add reward check
            if(qpi.invocationReward() > 0)
            {
                qpi.transfer(qpi.invocator(), qpi.invocationReward());
            }
            return;
        }

        locals.project = NOSTROMOProject{
            owner: qpi.invocator(),
            state : VOTE_PHASE,
            totalAmount : input.totalAmount,
            threeshold : input.threeshold,
            raiseInQubics : input.raisedInQubics,
            tokensInSale : tokensInSale
        };

        locals.project.votes.reset();
        locals.project.registeredUsers.reset();
        locals.project.investments.reset();

        // Add the project
        projects.set(projectNextId++, );

        output.status = 0; // Success
        copyMemory(output.message, "");
    _

    struct getProject_locals {
        Project project;
        ProjectResponse projectResponse;
    };

    // To get the project information
    PUBLIC_FUNCTION_WITH_LOCALS(getProject)

        // If project doesn't exist, return an error
        if (!state.projects.get(input.projectId, locals.project)) {
            output.status = 1;
            copyMemory(output.message, "Project not found");
            return;
        }

        // Build the response object
        locals.projectResponse.owner = locals.project.owner;
        locals.projectResponse.state = locals.project.state;
        locals.projectResponse.totalAmount = locals.project.totalAmount;
        locals.projectResponse.threeshold = locals.project.threeshold;
        locals.projectResponse.tokenPrice = locals.project.tokenPrice;
        locals.projectResponse.raisedAmount = locals.project.raisedAmount;
        locals.projectResponse.raiseInQubics = locals.project.raiseInQubics;
        locals.projectResponse.tokensInSale = locals.project.tokensInSale;

        output.status = 0; // Success
        output.project = locals.projectResponse;
        copyMemory(output.message, "Order retrieved successfully");
    _

    // To vote for a project
    PUBLIC_PROCEDURE_WITH_LOCALS(voteProject)
        if (qpi.invocationReward() < state.transactionFee) {
            output.status = 1;
            copyMemory(output.message, "Insufficient transaction fee");
            
            // WFS: add reward check
            if(qpi.invocationReward() > 0)
            {
                qpi.transfer(qpi.invocator(), qpi.invocationReward());
            }
            return;
        }

        // Check the user tier.
        uint8 tier;
        if (!state.users.get(qpi.invocator(), tier)) {
            output.status = 2;
            copyMemory(output.message, "No tier"); 
            
            // WFS: add reward check
            if(qpi.invocationReward() > 0)
            {
                qpi.transfer(qpi.invocator(), qpi.invocationReward());
            }
            return;
        }

        // Only users with a tier can vote
        if (tier == NONE) {
            output.status = 3;
            copyMemory(output.message, "No tier");
            
            // WFS: add reward check
            if(qpi.invocationReward() > 0)
            {
                qpi.transfer(qpi.invocator(), qpi.invocationReward());
            }
            return;
        }

        // Check the project exists
        Project project;
        if (!state.projects.get(input.projectId, project)) {
            output.status = 4;
            copyMemory(output.message, "Project not found");
            
            // WFS: add reward check
            if(qpi.invocationReward() > 0)
            {
                qpi.transfer(qpi.invocator(), qpi.invocationReward());
            }            
            return;
        }

        // Check the project is in VOTE phase
        if (project.state != VOTE_STATE) {
            output.status = 5;
            copyMemory(output.message, "NO_VOTE_STATE");
            // WFS: add reward check
            if(qpi.invocationReward() > 0)
            {
                qpi.transfer(qpi.invocator(), qpi.invocationReward());
            }
            return;
        }

        // Check user hasn't voted yet
        uint8 vote;
        if (project.votes.get(qpi.invocator(), vote)) {
            output.status = 6;
            copyMemory(output.message, "Voted");
            // WFS: add reward check
            if(qpi.invocationReward() > 0)
            {
                qpi.transfer(qpi.invocator(), qpi.invocationReward());
            }
            return;
        }

        project.votes.set(qpi.invocator(), input.vote);
    _

private:
    id admin;
    id wallet;
    QPI::HashMap<uint8, NOSTROMOTier, 5> tiers;                         
    QPI::HashMap<id, uint8, NOSTROMO_MAX_USERS> userTiers;              
    QPI::HashMap<uint64, NOSTROMOProject, NOSTROMO_MAX_PROJECTS> projects;


    uint64 transactionFee;
    uint64 projectFee;
    uint64 projectNextId;

    typedef id isAdmin_input; 
    typedef bit isAdmin_output;

    PRIVATE_FUNCTION(isAdmin)
        output = (qpi.invocator() == state.admin);
    _

    struct addUserTier_locals {
        // TODO...
    };

    // To update the project state
    PUBLIC_PROCEDURE_WITH_LOCALS(updateProjectState)
        // We don't know if its possible to update automatically the project state when the time of each phase ends or
        // if we need a function (or several) to call it and change manully the state. OnlyOwner function
    _



    // To register in a project
    PUBLIC_PROCEDURE_WITH_LOCALS(registerUserForProject)
        // TODO
    _

    // To invest in a project
    PUBLIC_PROCEDURE_WITH_LOCALS(investInProject)
        // TODO
    _

    // To send the raised funds to the project owner. 
    // Not all in a single call, we can pass in the input how many funds we are going to send
    PUBLIC_PROCEDURE_WITH_LOCALS(sendRaisedQubics)
 
    _

    // Called by investors to claim their tokens according the vesting schedule
    PUBLIC_PROCEDURE_WITH_LOCALS(claimProjectTokens)

    _

    // Register Functions and Procedures
    REGISTER_USER_FUNCTIONS_AND_PROCEDURES
        REGISTER_USER_PROCEDURE(addUserTier, 1);
        REGISTER_USER_PROCEDURE(removeUserTier, 2);
        REGISTER_USER_PROCEDURE(createProject, 3);
        REGISTER_USER_PROCEDURE(updateProjectState, 4);
        REGISTER_USER_PROCEDURE(voteProject, 5);
        REGISTER_USER_PROCEDURE(registerUserForProject, 6);
        REGISTER_USER_PROCEDURE(investInProject, 7);
        REGISTER_USER_PROCEDURE(sendRaisedQubics, 8);
        REGISTER_USER_PROCEDURE(claimProjectTokens, 9);
        REGISTER_USER_FUNCTION(getProject, 10);
    _

    INITIALIZE
        state.stakedQubicsInContract = 0;
        state.admin = qpi.invocator();
        state.wallet = qpi.invocator();
        state.transactionFee = 1000;
        state.projectFee = 10000;
        state.projectNextId = 1;
        state.tiers.reset();
        state.tiers.set(EGG, NOSTROMOTier{ 1, 55 });
        state.tiers.set(DOG, NOSTROMOTier{ 5, 300 });
        state.tiers.set(ALIEN, NOSTROMOTier{ 10, 75 });
        state.tiers.set(WARRIOR, NOSTROMOTier{ 30, 305 });
        state.tiers.set(QUEEN, NOSTROMOTier{ 100, 1375 });
        state.userTiers.reset();
        state.projects.reset();
};
