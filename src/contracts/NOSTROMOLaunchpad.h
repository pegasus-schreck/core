using namespace QPI;

constexpr uint64 NOSTROMO_MAX_USERS = 131072;
constexpr uint64 NOSTROMO_MAX_PROJECTS = 1024;


// Tier enumerations
constexpr uint8 NONE = 0;
constexpr uint8 EGG = 1;
constexpr uint8 DOG = 2;
constexpr uint8 ALIEN = 3;
constexpr uint8 WARRIOR = 4;
constexpr uint8 QUEEN = 5;

// State enumerations
constexpr uint8 VOTE_STATE = 0;
constexpr uint8 REGISTER_STATE = 1;
constexpr uint8 INVESTMENT_PHASE_1 = 2;
constexpr uint8 INVESTMENT_PHASE_2 = 3;
constexpr uint8 INVESTMENT_PHASE_3 = 4;
constexpr uint8 CLOSED_FAILED = 5;
constexpr uint8 CLOSED_SUCCESS = 6;
constexpr uint8 BLOCKED = 7;

struct NOSTROMO2
{
};

struct NOSTROMO : public ContractBase
{

	REGISTER_USER_FUNCTIONS_AND_PROCEDURES
	_

	INITIALIZE
	_

	BEGIN_EPOCH
	_

	END_EPOCH
	_

	BEGIN_TICK
	_

	END_TICK
	_

	PRE_ACQUIRE_SHARES
	_

	POST_ACQUIRE_SHARES
	_

	PRE_RELEASE_SHARES
	_

	POST_RELEASE_SHARES
	_

	EXPAND
	_

};
