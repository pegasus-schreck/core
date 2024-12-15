using namespace QPI;

constexpr uint32 NOSTROMO_MAX_USERS = 65536;
constexpr uint32 NOSTROMO_MAX_PROJECTS = 1024;

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

////////////////////////////////////////////////////////////////////////////////////////        
    struct getUserLockedInfo_input {
        id user;
        uint32 epoch;
    };

    struct getUserLockedInfo_output {
        uint64 lockedAmount;                   
    };

    struct getStateOfRound_input {
        uint32 epoch;
    };

    struct getStateOfRound_output {
        uint32 state;
    };
    
    struct getUserLockStatus_input {
        id user;
    };

    struct getUserLockStatus_output {
        uint64 status;
    };

    struct getEndedStatus_input {
        id user;
    };

    struct getEndedStatus_output {
        uint64 fullyUnlockedAmount;
        uint64 fullyRewardedAmount;
        uint64 earlyUnlockedAmount;
        uint64 earlyRewardedAmount;
    };

	struct lock_input {	
    };

    struct lock_output {	
        sint32 returnCode;
    };

    struct unlock_input {
        uint64 amount;                           	
        uint32 lockedEpoch;                      
    };

    struct unlock_output {
        sint32 returnCode;
    };

    QPI::HashMap<uint8, NOSTROMOTier, 5> tiers;                         
    //QPI::HashMap<id, uint8, NOSTROMO_MAX_USERS> userTiers;              
    //QPI::HashMap<uint64, NOSTROMOProject, NOSTROMO_MAX_PROJECTS> projects;

protected:

    id admin;
    id wallet;
    //QPI::HashMap<uint8, NOSTROMOTier, 5> tiers;                         
    //QPI::HashMap<id, uint8, NOSTROMO_MAX_USERS> userTiers;              
    //QPI::HashMap<uint64, NOSTROMOProject, NOSTROMO_MAX_PROJECTS> projects;

    uint64 stakedQubicsInContract;
    uint64 transactionFee;
    uint64 projectFee;
    uint64 projectNextId;

    struct RoundInfo {

        uint64 _totalLockedAmount;            
        uint64 _epochBonusAmount;             

    };

    array<RoundInfo, QEARN_MAX_EPOCHS> _initialRoundInfo;
    array<RoundInfo, QEARN_MAX_EPOCHS> _currentRoundInfo;

    struct EpochIndexInfo {

        uint32 startIndex;
        uint32 endIndex;
    };

    array<EpochIndexInfo, QEARN_MAX_EPOCHS> _epochIndex;

    struct LockInfo {

        uint64 _lockedAmount;
        id ID;
        uint32 _lockedEpoch;
        
    };

    array<LockInfo, QEARN_MAX_LOCKS> locker;

    struct HistoryInfo {

        uint64 _unlockedAmount;
        uint64 _rewardedAmount;
        id _unlockedID;

    };

    array<HistoryInfo, QEARN_MAX_USERS> earlyUnlocker;
    array<HistoryInfo, QEARN_MAX_USERS> fullyUnlocker;
    
    uint32 _earlyUnlockedCnt;
    uint32 _fullyUnlockedCnt;

    struct getStateOfRound_locals {
        uint32 firstEpoch;
    };

    struct addUserTier_locals {
        uint64 stakedQubics;
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

        //Tiers userTier = userTiers.get(qpi.invocator());
        //if (userTier != Tiers.NONE) {
        //    output.status = 3;
        //    if (qpi.invocationReward() > 0) {
        //        qpi.transfer(qpi.invocator(), qpi.invocationReward());
        //    }
        //    return;
        //}

        //locals.stakedQubics = tiers.get(input.tier);
        if (locals.stakedQubics + state.transactionFee != qpi.invocationReward())
        {
            output.status = 3; 
            if (qpi.invocationReward() > 0) {
                qpi.transfer(qpi.invocator(), qpi.invocationReward());
            }
            return;
        }

        // Set the user tier
        //userTiers.set(qpi.invocator(), input.tier);

        // Update the staked qubics amount
        //stakedQubicsInContract += stakedQubics;

        output.status = 0; 
    _

    PUBLIC_FUNCTION_WITH_LOCALS(getStateOfRound)
        if(input.epoch < QEARN_INITIAL_EPOCH) 
        {                                                           
            output.state = 2; 
            return ;
        }
        if(input.epoch > qpi.epoch()) 
        {
            output.state = 0;                                     
        }
        locals.firstEpoch = qpi.epoch() - 52;
        if(input.epoch <= qpi.epoch() && input.epoch >= locals.firstEpoch) 
        {
            output.state = 1;       
        }
        if(input.epoch < locals.firstEpoch) 
        {
            output.state = 2;      
        }
    _

   

    struct getUserLockedInfo_locals {
        uint32 _t;
        uint32 startIndex;
        uint32 endIndex;
    };

    PUBLIC_FUNCTION_WITH_LOCALS(getUserLockedInfo)

        locals.startIndex = state._epochIndex.get(input.epoch).startIndex;
        locals.endIndex = state._epochIndex.get(input.epoch).endIndex;
        
        for(locals._t = locals.startIndex; locals._t < locals.endIndex; locals._t++) 
        {
            if(state.locker.get(locals._t).ID == input.user) 
            {
                output.lockedAmount = state.locker.get(locals._t)._lockedAmount; 
                return;
            }
        }
    _

    struct getUserLockStatus_locals {
        uint64 bn;
        uint32 _t;
        uint32 _r;
        uint32 endIndex;
        uint8 lockedWeeks;
    };

    PUBLIC_FUNCTION_WITH_LOCALS(getUserLockStatus)

        output.status = 0ULL;
        locals.endIndex = state._epochIndex.get(qpi.epoch()).endIndex;
        
        for(locals._t = 0; locals._t < locals.endIndex; locals._t++) 
        {
            if(state.locker.get(locals._t)._lockedAmount > 0 && state.locker.get(locals._t).ID == input.user) 
            {
            
                locals.lockedWeeks = qpi.epoch() - state.locker.get(locals._t)._lockedEpoch;
                locals.bn = 1ULL<<locals.lockedWeeks;

                output.status += locals.bn;
            }
        }

    _
    
    struct getEndedStatus_locals {
        uint32 _t;
    };

    PUBLIC_FUNCTION_WITH_LOCALS(getEndedStatus)

        output.earlyRewardedAmount = 0;
        output.earlyUnlockedAmount = 0;
        output.fullyRewardedAmount = 0;
        output.fullyUnlockedAmount = 0;

        for(locals._t = 0; locals._t < state._earlyUnlockedCnt; locals._t++) 
        {
            if(state.earlyUnlocker.get(locals._t)._unlockedID == input.user) 
            {
                output.earlyRewardedAmount = state.earlyUnlocker.get(locals._t)._rewardedAmount;
                output.earlyUnlockedAmount = state.earlyUnlocker.get(locals._t)._unlockedAmount;

                break ;
            }
        }

        for(locals._t = 0; locals._t < state._fullyUnlockedCnt; locals._t++) 
        {
            if(state.fullyUnlocker.get(locals._t)._unlockedID == input.user) 
            {
                output.fullyRewardedAmount = state.fullyUnlocker.get(locals._t)._rewardedAmount;
                output.fullyUnlockedAmount = state.fullyUnlocker.get(locals._t)._unlockedAmount;
            
                return ;
            }
        }
    _

    struct lock_locals {

        LockInfo newLocker;
        RoundInfo updatedRoundInfo;
        EpochIndexInfo tmpIndex;
        uint32 t;
        uint32 endIndex;
        
    };

    PUBLIC_PROCEDURE_WITH_LOCALS(lock)
    
        if (qpi.invocationReward() < QEARN_MINIMUM_LOCKING_AMOUNT || qpi.epoch() < QEARN_INITIAL_EPOCH)
        {
            output.returnCode = QEARN_INVALID_INPUT_AMOUNT;         // if the amount of locking is less than 10M, it should be failed to lock.
            
            if(qpi.invocationReward() > 0) 
            {
                qpi.transfer(qpi.invocator(), qpi.invocationReward());
            }
            return;
        }

        locals.endIndex = state._epochIndex.get(qpi.epoch()).endIndex;

        for(locals.t = state._epochIndex.get(qpi.epoch()).startIndex ; locals.t < locals.endIndex; locals.t++) 
        {

            if(state.locker.get(locals.t).ID == qpi.invocator()) 
            {      // the case to be locked several times at one epoch, at that time, this address already located in state.Locker array, the amount will be increased as current locking amount.
                if(state.locker.get(locals.t)._lockedAmount + qpi.invocationReward() > QEARN_MAX_LOCK_AMOUNT)
                {
                    output.returnCode = QEARN_LIMIT_LOCK;
                    if(qpi.invocationReward() > 0) 
                    {
                        qpi.transfer(qpi.invocator(), qpi.invocationReward());
                    }
                    return;
                }

                locals.newLocker._lockedAmount = state.locker.get(locals.t)._lockedAmount + qpi.invocationReward();
                locals.newLocker._lockedEpoch = qpi.epoch();
                locals.newLocker.ID = qpi.invocator();

                state.locker.set(locals.t, locals.newLocker);

                locals.updatedRoundInfo._totalLockedAmount = state._initialRoundInfo.get(qpi.epoch())._totalLockedAmount + qpi.invocationReward();
                locals.updatedRoundInfo._epochBonusAmount = state._initialRoundInfo.get(qpi.epoch())._epochBonusAmount;
                state._initialRoundInfo.set(qpi.epoch(), locals.updatedRoundInfo);

                locals.updatedRoundInfo._totalLockedAmount = state._currentRoundInfo.get(qpi.epoch())._totalLockedAmount + qpi.invocationReward();
                locals.updatedRoundInfo._epochBonusAmount = state._currentRoundInfo.get(qpi.epoch())._epochBonusAmount;
                state._currentRoundInfo.set(qpi.epoch(), locals.updatedRoundInfo);
                
                output.returnCode = QEARN_LOCK_SUCCESS;          //  additional locking of this epoch is succeed
                return ;
            }

        }

        if(locals.endIndex == QEARN_MAX_LOCKS - 1) 
        {
            output.returnCode = QEARN_OVERFLOW_USER;
            if(qpi.invocationReward() > 0) 
            {
                qpi.transfer(qpi.invocator(), qpi.invocationReward());
            }
            return ;                        // overflow users in Qearn
        }
        
        if(qpi.invocationReward() > QEARN_MAX_LOCK_AMOUNT)
        {
            output.returnCode = QEARN_LIMIT_LOCK;
            if(qpi.invocationReward() > 0) 
            {
                qpi.transfer(qpi.invocator(), qpi.invocationReward());
            }
            return;
        }

        locals.newLocker.ID = qpi.invocator();
        locals.newLocker._lockedAmount = qpi.invocationReward();
        locals.newLocker._lockedEpoch = qpi.epoch();

        state.locker.set(locals.endIndex, locals.newLocker);

        locals.tmpIndex.startIndex = state._epochIndex.get(qpi.epoch()).startIndex;
        locals.tmpIndex.endIndex = locals.endIndex + 1;
        state._epochIndex.set(qpi.epoch(), locals.tmpIndex);

        locals.updatedRoundInfo._totalLockedAmount = state._initialRoundInfo.get(qpi.epoch())._totalLockedAmount + qpi.invocationReward();
        locals.updatedRoundInfo._epochBonusAmount = state._initialRoundInfo.get(qpi.epoch())._epochBonusAmount;
        state._initialRoundInfo.set(qpi.epoch(), locals.updatedRoundInfo);

        locals.updatedRoundInfo._totalLockedAmount = state._currentRoundInfo.get(qpi.epoch())._totalLockedAmount + qpi.invocationReward();
        locals.updatedRoundInfo._epochBonusAmount = state._currentRoundInfo.get(qpi.epoch())._epochBonusAmount;
        state._currentRoundInfo.set(qpi.epoch(), locals.updatedRoundInfo);

        output.returnCode = QEARN_LOCK_SUCCESS;            //  new locking of this epoch is succeed
    _

    struct unlock_locals {

        RoundInfo updatedRoundInfo;
        LockInfo updatedUserInfo;
        HistoryInfo unlockerInfo;
        
        uint64 amountOfUnlocking;
        uint64 amountOfReward;
        uint64 amountOfburn;
        uint64 rewardPercent;
        sint64 divCalcu;
        uint32 earlyUnlockingPercent;
        uint32 burnPercent;
        uint32 indexOfinvocator;
        uint32 _t;
        uint32 countOfLastVacancy;
        uint32 countOfLockedEpochs;
        uint32 startIndex;
        uint32 endIndex;
        
    };

    PUBLIC_PROCEDURE_WITH_LOCALS(unlock)

        if (input.lockedEpoch > QEARN_MAX_EPOCHS || input.lockedEpoch < QEARN_INITIAL_EPOCH)
        {

            output.returnCode = QEARN_INVALID_INPUT_LOCKED_EPOCH;               //   if user try to unlock with wrong locked epoch, it should be failed to unlock.
            return ;

        }

        if(input.amount < QEARN_MINIMUM_LOCKING_AMOUNT)
        {
            
            output.returnCode = QEARN_INVALID_INPUT_AMOUNT;
            return ;

        }

        locals.indexOfinvocator = QEARN_MAX_LOCKS;
        locals.startIndex = state._epochIndex.get(input.lockedEpoch).startIndex;
        locals.endIndex = state._epochIndex.get(input.lockedEpoch).endIndex;

        for(locals._t = locals.startIndex ; locals._t < locals.endIndex; locals._t++) 
        {

            if(state.locker.get(locals._t).ID == qpi.invocator()) 
            { 
                if(state.locker.get(locals._t)._lockedAmount < input.amount) 
                {

                    output.returnCode = QEARN_INVALID_INPUT_UNLOCK_AMOUNT;  //  if the amount to be wanted to unlock is more than locked amount, it should be failed to unlock
                    return ;  

                }
                else 
                {
                    locals.indexOfinvocator = locals._t;
                    break;
                }
            }

        }

        if(locals.indexOfinvocator == QEARN_MAX_LOCKS) 
        {
            
            output.returnCode = QEARN_EMPTY_LOCKED;     //   if there is no any locked info in state.Locker array, it shows this address didn't lock at the epoch (input.Locked_Epoch)
            return ;  
        }


        if(state.locker.get(locals.indexOfinvocator)._lockedAmount - input.amount < QEARN_MINIMUM_LOCKING_AMOUNT) 
        {
            locals.amountOfUnlocking = state.locker.get(locals.indexOfinvocator)._lockedAmount;
        }
        else 
        {
            locals.amountOfUnlocking = input.amount;
        }

        locals.countOfLockedEpochs = qpi.epoch() - input.lockedEpoch - 1;

        locals.earlyUnlockingPercent = QEARN_EARLY_UNLOCKING_PERCENT_0_3;
        locals.burnPercent = QEARN_BURN_PERCENT_0_3;

        if(locals.countOfLockedEpochs >= 4 && locals.countOfLockedEpochs <= 7) 
        {
            locals.earlyUnlockingPercent = QEARN_EARLY_UNLOCKING_PERCENT_4_7;
            locals.burnPercent = QEARN_BURN_PERCENT_4_7;
        }

        else if(locals.countOfLockedEpochs >= 8 && locals.countOfLockedEpochs <= 11) 
        {
            locals.earlyUnlockingPercent = QEARN_EARLY_UNLOCKING_PERCENT_8_11;
            locals.burnPercent = QEARN_BURN_PERCENT_8_11;
        }

        else if(locals.countOfLockedEpochs >= 12 && locals.countOfLockedEpochs <= 15) 
        {
            locals.earlyUnlockingPercent = QEARN_EARLY_UNLOCKING_PERCENT_12_15;
            locals.burnPercent = QEARN_BURN_PERCENT_12_15;
        }

        else if(locals.countOfLockedEpochs >= 16 && locals.countOfLockedEpochs <= 19) 
        {
            locals.earlyUnlockingPercent = QEARN_EARLY_UNLOCKING_PERCENT_16_19;
            locals.burnPercent = QEARN_BURN_PERCENT_16_19;
        }

        else if(locals.countOfLockedEpochs >= 20 && locals.countOfLockedEpochs <= 23) 
        {
            locals.earlyUnlockingPercent = QEARN_EARLY_UNLOCKING_PERCENT_20_23;
            locals.burnPercent = QEARN_BURN_PERCENT_20_23;
        }

        else if(locals.countOfLockedEpochs >= 24 && locals.countOfLockedEpochs <= 27) 
        {
            locals.earlyUnlockingPercent = QEARN_EARLY_UNLOCKING_PERCENT_24_27;
            locals.burnPercent = QEARN_BURN_PERCENT_24_27;
        }

        else if(locals.countOfLockedEpochs >= 28 && locals.countOfLockedEpochs <= 31) 
        {
            locals.earlyUnlockingPercent = QEARN_EARLY_UNLOCKING_PERCENT_28_31;
            locals.burnPercent = QEARN_BURN_PERCENT_28_31;
        }

        else if(locals.countOfLockedEpochs >= 32 && locals.countOfLockedEpochs <= 35) 
        {
            locals.earlyUnlockingPercent = QEARN_EARLY_UNLOCKING_PERCENT_32_35;
            locals.burnPercent = QEARN_BURN_PERCENT_32_35;
        }

        else if(locals.countOfLockedEpochs >= 36 && locals.countOfLockedEpochs <= 39) 
        {
            locals.earlyUnlockingPercent = QEARN_EARLY_UNLOCKING_PERCENT_36_39;
            locals.burnPercent = QEARN_BURN_PERCENT_36_39;
        }

        else if(locals.countOfLockedEpochs >= 40 && locals.countOfLockedEpochs <= 43) 
        {
            locals.earlyUnlockingPercent = QEARN_EARLY_UNLOCKING_PERCENT_40_43;
            locals.burnPercent = QEARN_BURN_PERCENT_40_43;
        }

        else if(locals.countOfLockedEpochs >= 44 && locals.countOfLockedEpochs <= 47) 
        {
            locals.earlyUnlockingPercent = QEARN_EARLY_UNLOCKING_PERCENT_44_47;
            locals.burnPercent = QEARN_BURN_PERCENT_44_47;
        }

        else if(locals.countOfLockedEpochs >= 48 && locals.countOfLockedEpochs <= 51) 
        {
            locals.earlyUnlockingPercent = QEARN_EARLY_UNLOCKING_PERCENT_48_51;
            locals.burnPercent = QEARN_BURN_PERCENT_48_51;
        }

        locals.rewardPercent = QPI::div(state._currentRoundInfo.get(input.lockedEpoch)._epochBonusAmount * 10000000ULL, state._currentRoundInfo.get(input.lockedEpoch)._totalLockedAmount);
        locals.divCalcu = QPI::div(locals.rewardPercent * locals.amountOfUnlocking , 100ULL);
        locals.amountOfReward = QPI::div(locals.divCalcu * locals.earlyUnlockingPercent * 1ULL , 10000000ULL);
        locals.amountOfburn = QPI::div(locals.divCalcu * locals.burnPercent * 1ULL, 10000000ULL);

        qpi.transfer(qpi.invocator(), locals.amountOfUnlocking + locals.amountOfReward);
        qpi.burn(locals.amountOfburn);

        locals.updatedRoundInfo._totalLockedAmount = state._currentRoundInfo.get(input.lockedEpoch)._totalLockedAmount - locals.amountOfUnlocking;
        locals.updatedRoundInfo._epochBonusAmount = state._currentRoundInfo.get(input.lockedEpoch)._epochBonusAmount - locals.amountOfReward - locals.amountOfburn;

        state._currentRoundInfo.set(input.lockedEpoch, locals.updatedRoundInfo);

        if(state.locker.get(locals.indexOfinvocator)._lockedAmount == locals.amountOfUnlocking)
        {
            locals.updatedUserInfo.ID = NULL_ID;
            locals.updatedUserInfo._lockedAmount = 0;
            locals.updatedUserInfo._lockedEpoch = 0;
        }
        else 
        {
            locals.updatedUserInfo.ID = qpi.invocator();
            locals.updatedUserInfo._lockedAmount = state.locker.get(locals.indexOfinvocator)._lockedAmount - locals.amountOfUnlocking;
            locals.updatedUserInfo._lockedEpoch = state.locker.get(locals.indexOfinvocator)._lockedEpoch;
        }

        state.locker.set(locals.indexOfinvocator, locals.updatedUserInfo);

        if(state._currentRoundInfo.get(input.lockedEpoch)._totalLockedAmount == 0 && input.lockedEpoch != qpi.epoch()) 
        {
            
            // If all users have unlocked early, burn bonus
            qpi.burn(state._currentRoundInfo.get(input.lockedEpoch)._epochBonusAmount);

            locals.updatedRoundInfo._totalLockedAmount = 0;
            locals.updatedRoundInfo._epochBonusAmount = 0;

            state._currentRoundInfo.set(input.lockedEpoch, locals.updatedRoundInfo);

        }

        if(input.lockedEpoch != qpi.epoch()) 
        {

            locals.unlockerInfo._unlockedID = qpi.invocator();
            
            for(locals._t = 0; locals._t < state._earlyUnlockedCnt; locals._t++) 
            {
                if(state.earlyUnlocker.get(locals._t)._unlockedID == qpi.invocator()) 
                {

                    locals.unlockerInfo._rewardedAmount = state.earlyUnlocker.get(locals._t)._rewardedAmount + locals.amountOfReward;
                    locals.unlockerInfo._unlockedAmount = state.earlyUnlocker.get(locals._t)._unlockedAmount + locals.amountOfUnlocking;

                    state.earlyUnlocker.set(locals._t, locals.unlockerInfo);

                    break;
                }
            }

            if(locals._t == state._earlyUnlockedCnt && state._earlyUnlockedCnt < QEARN_MAX_USERS) 
            {
                locals.unlockerInfo._rewardedAmount = locals.amountOfReward;
                locals.unlockerInfo._unlockedAmount = locals.amountOfUnlocking;

                state.earlyUnlocker.set(locals._t, locals.unlockerInfo);
                state._earlyUnlockedCnt++;
            }

        }

        output.returnCode = QEARN_UNLOCK_SUCCESS; //  unlock is succeed
    _

	REGISTER_USER_FUNCTIONS_AND_PROCEDURES
    
        //REGISTER_USER_FUNCTION(getLockInfoPerEpoch, 1);
        REGISTER_USER_FUNCTION(getUserLockedInfo, 1);
        REGISTER_USER_FUNCTION(getStateOfRound, 2);
        REGISTER_USER_FUNCTION(getUserLockStatus, 3);
        REGISTER_USER_FUNCTION(getEndedStatus, 4);

        REGISTER_USER_PROCEDURE(lock, 1);
		REGISTER_USER_PROCEDURE(unlock, 2);
        REGISTER_USER_PROCEDURE(addUserTier, 3);
	_

    struct BEGIN_EPOCH_locals
    {
        HistoryInfo INITIALIZE_HISTORY;
        LockInfo INITIALIZE_USER;
        RoundInfo INITIALIZE_ROUNDINFO;

        uint32 t;
        bit status;
        uint64 pre_epoch_balance;
        uint64 current_balance;
        ::Entity entity;
        uint32 locked_epoch;
    };

    BEGIN_EPOCH_WITH_LOCALS

        qpi.getEntity(SELF, locals.entity);
        locals.current_balance = locals.entity.incomingAmount - locals.entity.outgoingAmount;

        locals.pre_epoch_balance = 0ULL;
        locals.locked_epoch = qpi.epoch() - 52;
        for(locals.t = qpi.epoch() - 1; locals.t >= locals.locked_epoch; locals.t--) 
        {
            locals.pre_epoch_balance += state._currentRoundInfo.get(locals.t)._epochBonusAmount + state._currentRoundInfo.get(locals.t)._totalLockedAmount;
        }

        if(locals.current_balance - locals.pre_epoch_balance > QEARN_MAX_BONUS_AMOUNT)
        {
            qpi.burn(locals.current_balance - locals.pre_epoch_balance - QEARN_MAX_BONUS_AMOUNT);
            locals.INITIALIZE_ROUNDINFO._epochBonusAmount = QEARN_MAX_BONUS_AMOUNT;
        }
        else 
        {
            locals.INITIALIZE_ROUNDINFO._epochBonusAmount = locals.current_balance - locals.pre_epoch_balance;
        }
        locals.INITIALIZE_ROUNDINFO._totalLockedAmount = 0;

        state._initialRoundInfo.set(qpi.epoch(), locals.INITIALIZE_ROUNDINFO);
        state._currentRoundInfo.set(qpi.epoch(), locals.INITIALIZE_ROUNDINFO);

	_

    struct END_EPOCH_locals 
    {
        HistoryInfo INITIALIZE_HISTORY;
        LockInfo INITIALIZE_USER;
        RoundInfo INITIALIZE_ROUNDINFO;
        EpochIndexInfo tmpEpochIndex;

        uint64 _rewardPercent;
        uint64 _rewardAmount;
        uint64 _burnAmount;
        uint32 lockedEpoch;
        uint32 startEpoch;
        uint32 _t;
        sint32 st;
        sint32 en;
        uint32 endIndex;

    };

	END_EPOCH_WITH_LOCALS

        state._earlyUnlockedCnt = 0;
        state._fullyUnlockedCnt = 0;
        locals.lockedEpoch = qpi.epoch() - 52;
        locals.endIndex = state._epochIndex.get(locals.lockedEpoch).endIndex;
        
        locals._burnAmount = state._currentRoundInfo.get(locals.lockedEpoch)._epochBonusAmount;
        
        locals._rewardPercent = QPI::div(state._currentRoundInfo.get(locals.lockedEpoch)._epochBonusAmount * 10000000ULL, state._currentRoundInfo.get(locals.lockedEpoch)._totalLockedAmount);

        for(locals._t = state._epochIndex.get(locals.lockedEpoch).startIndex; locals._t < locals.endIndex; locals._t++) 
        {
            if(state.locker.get(locals._t)._lockedAmount == 0) 
            {
                continue;
            }

            ASSERT(state.locker.get(locals._t)._lockedEpoch == locals.lockedEpoch);

            locals._rewardAmount = QPI::div(state.locker.get(locals._t)._lockedAmount * locals._rewardPercent, 10000000ULL);
            qpi.transfer(state.locker.get(locals._t).ID, locals._rewardAmount + state.locker.get(locals._t)._lockedAmount);

            if(state._fullyUnlockedCnt < QEARN_MAX_USERS) 
            {

                locals.INITIALIZE_HISTORY._unlockedID = state.locker.get(locals._t).ID;
                locals.INITIALIZE_HISTORY._unlockedAmount = state.locker.get(locals._t)._lockedAmount;
                locals.INITIALIZE_HISTORY._rewardedAmount = locals._rewardAmount;

                state.fullyUnlocker.set(state._fullyUnlockedCnt, locals.INITIALIZE_HISTORY);

                state._fullyUnlockedCnt++;
            }

            locals.INITIALIZE_USER.ID = NULL_ID;
            locals.INITIALIZE_USER._lockedAmount = 0;
            locals.INITIALIZE_USER._lockedEpoch = 0;

            state.locker.set(locals._t, locals.INITIALIZE_USER);

            locals._burnAmount -= locals._rewardAmount;
        }

        locals.tmpEpochIndex.startIndex = 0;
        locals.tmpEpochIndex.endIndex = 0;
        state._epochIndex.set(locals.lockedEpoch, locals.tmpEpochIndex);

        locals.startEpoch = locals.lockedEpoch + 1;
        if (locals.startEpoch < QEARN_INITIAL_EPOCH)
            locals.startEpoch = QEARN_INITIAL_EPOCH;

        // remove all gaps in Locker array (from beginning) and update epochIndex
        locals.tmpEpochIndex.startIndex = 0;
        for(locals._t = locals.startEpoch; locals._t <= qpi.epoch(); locals._t++)
        {
            // This for loop iteration moves all elements of one epoch the to start of its range of the Locker array.
            // The startIndex is given by the end of the range of the previous epoch, the new endIndex is found in the
            // gap removal process.
            locals.st = locals.tmpEpochIndex.startIndex;
            locals.en = state._epochIndex.get(locals._t).endIndex;
            ASSERT(locals.st <= locals.en);

            while(locals.st < locals.en)
            {
                // try to set locals.st to first empty slot
                while (state.locker.get(locals.st)._lockedAmount && locals.st < locals.en)
                {
                    locals.st++;
                }

                // try set locals.en to last non-empty slot in epoch
                --locals.en;
                while (!state.locker.get(locals.en)._lockedAmount && locals.st < locals.en)
                {
                    locals.en--;
                }

                // if st and en meet, there are no gaps to be closed by moving in this epoch range
                if (locals.st >= locals.en)
                {
                    // make locals.en point behind last element again
                    ++locals.en;
                    break;
                }

                // move entry from locals.en to locals.st
                state.locker.set(locals.st, state.locker.get(locals.en));

                // make locals.en slot empty -> locals.en points behind last element again
                locals.INITIALIZE_USER.ID = NULL_ID;
                locals.INITIALIZE_USER._lockedAmount = 0;
                locals.INITIALIZE_USER._lockedEpoch = 0;
                state.locker.set(locals.en, locals.INITIALIZE_USER);
            }

            // update epoch index
            locals.tmpEpochIndex.endIndex = locals.en;
            state._epochIndex.set(locals._t, locals.tmpEpochIndex);

            // set start index of next epoch to end index of current epoch
            locals.tmpEpochIndex.startIndex = locals.tmpEpochIndex.endIndex;
        }

        locals.tmpEpochIndex.endIndex = locals.tmpEpochIndex.startIndex;
        state._epochIndex.set(qpi.epoch() + 1, locals.tmpEpochIndex);

        qpi.burn(locals._burnAmount);
	_
};
