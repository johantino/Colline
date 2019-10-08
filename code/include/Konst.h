#include "math.h"

const int GRIDSIZE = 80; //num of cells is GRIDSIZE*GRIDSIZE

const int INIT_NUM_OF_COLL =300;
const int INIT_NUM_OF_INP = 400;
const int INIT_NUM_OF_EFF = 400;
const int INIT_FITN_COL_BASE = 200;
const int INIT_FITN_COL_RAND = 3300; //random added to base
const int INIT_FITN_INP_BASE = 200;
const int INIT_FITN_INP_RAND = 3300; //random added to base
const int INIT_FITN_EFF_BASE = 200;
const int INIT_FITN_EFF_RAND = 5300; //random added to base
//const int INIT_FITN_COLLINE = INIT_COL_FITNESS*INIT_NUM_OF_COLL  +  INIT_INP_FITNESS*INIT_NUM_OF_INP + INIT_EFF_FITNESS*INIT_NUM_OF_EFF;
//const int INIT_FITN_ENV = 0; //INIT_FITN_COLLINE; //this balance should be kept 

//**********************************************
//***These parameters are adjustable runtime:
const int MAX_STATE_BUYERS_INP_INIT = 8; //only the MAX_STATE_BUYERS highest bidding effectors gets to buy sellers state
const int BIDP_BASE_INP_INIT = 20;
const int BIDP_STEP_INP_INIT = 10;
const int BIDP_BASE_EFF_INIT = 15;
const int BIDP_STEP_EFF_INIT = 5;
const int SESSION_LENGTH_INIT = 25;
const int AGENT_LIFETIME_MAX_INIT = SESSION_LENGTH_INIT*7; 
const int COLL_MIN_PROCTIME_INIT = 4; //first fountain(s) loaded in cycle 'loadingBeginCycle'; collection of answers begin in cycle loadingBeginCycle + COLL_MIN_PROCTIME
const int GRID_ZOOM_INIT = 2;
const int C_MATSTART_PRC_INIT = 200; //how good the agent must be in order to reproduce
const int DC_REW_FROM_BID_AGR_LEVEL_INIT = 0;
const int MAX_INACTIVE_SESS_INIT = 4; //if agent has not processed in this num of sessions it dies (unless it is mater)
const int HIGH_MUT_AGENTS_PRC_INIT = 5; //note: percent not promille 
const int MUTATION_RATE_INIT = 5; //promille chance per DNA bit
const int MIN_NUM_OF_AGENTS_IN_GRID_INIT = 1000;
const int IDEAL_NUM_OF_AGENTS_IN_GRID_INIT = 2200;
const int GRAVITY_CENTER_INIT = 50;
const int DC_EARLY_RESP_REW_I0_PRC_INIT = 30; //effector that responds in first collectionIndex (COLL_MIN_PROC_TIME) are rewarded less, later responses gets bonus (late respons -> more processing ->better answer....also late resp carries risk of rew-buffer empty)
const int DC_EARLY_RESP_REW_I1_PRC_INIT = 50;
const int DC_EARLY_RESP_REW_I2_PRC_INIT = 70;
const int MAX_NUM_OF_OBS_INP_INIT = 9;
const int MAX_NUM_OF_OBS_EFF_INIT = 6; //max num of observed agent for _each_ input

//to do
const int MAX_STATE_BUYERS_COL = 8; 
const int MAX_NUM_OF_OBS_COL = 10;
const int BIDP_BASE_COL = 20; //minimum bidsize (% of fitness)
const int BIDP_STEP_COL = 7; //a multiplum of 'STEP' is added to base, depending on dna
const int SHAKE_PERCENT = 100; //betw 0 and 100; 0=no shaking, 100=all frozen are shaken
const bool NEW_OBSERVERS_NOTIFY = true;
//***end runtime adjustable parameters
//***********************************************

/*const int MUTATION_RATE_SPEC_PM = 2; //promille chance per DNA bit 
const int MUTATION_RATE_BUS_APP_PM = 6;  
const int MUTATION_RATE_DNA_AG_PM = 4; 
const int MUTATION_RATE_DNA_TYPE_PM = 8; */


const int MUTATION_RATE_HIGH_ADD_PM = 200; //add this promillerate for HIGH_MUT_AGENTS_PRC % of newborn agents

const bool INP_CAN_BUY_FROM_EFF = false;
const int N_DNA_AG = 5;
const int N_DNA_COLL = 15;
const int N_DNA_INP = 15;
const int N_DNA_EFF = 27; 

const int SAMPLE_PERCENT_AGENTS = 17; //the percent of all agents picked when sampling gene-pool

const int APPEAR_SIZE_MAT = 2;
const int NUM_OF_SPECIES = 10; //Given from APPEAR_SIZE_MAT, 1 bit: 3 species, 2 bits: 10 species, 3 bits
//begin:Species stat
const int NUM_STAT_SPECIES = 1; //the number of statistics calculated for each species (e.g. avg. age, how many, etc)
const int _SP_POS_COUNT = 0;
//end: Species stat
const int APPEAR_SIZE_BUS_SMALL = 4; //without (fixed) type bits
const int APPEAR_SIZE_BUS = 2 + APPEAR_SIZE_BUS_SMALL; //two first bits are agent-type bits
//const int NUM_OF_PICKI_LEVELS = (APPEAR_SIZE_BUS - 2) + 1; //(since only eff responds type bits are ignored in appearance; level 0 ~ [0**], level 1 ~ [10*], level 2 ~ [110], level 3 ~ [111] (for a 3-bit appearance)
//const int COLL_MAX_PROCTIME = COLL_MIN_PROCTIME_INIT + (COLLECTION_TIME-1); //the last answers are collected in cycle loadingBeginCycle + COLL_MAX_PROCTIME ..DELETE!
const int COLLECTION_TIME = 13 ; //during this time (cycles) answers are collected from processing effectors


const int VICINITY_MAT = 5;
//const int VICINITY_BUS_COLL = 3;
//const int VICINITY_BUS_INP = 4;
//const int VICINITY_BUS_EFF = 7;
const int VIC_BIT_LENGTH = 1; //NB! remember adjust in agent constr.when changed
const int VIC_ADD_INP = 3; //value to add to dna value (from VIC_BIT_LENGTH bits)
const int VIC_ADD_EFF = 4; //do
const int MIN_STEPSIZE = 3;
//const int STEP_SIZE_BUS = 4;
//const int STEP_SIZE_MAT = 4;

const int MAX_REWARD = 80000; //the maximum reward an effector can receive...DELETE?
const int MIN_REWARD = 50; //if sessionMoneyBag goes negative this is needed


const int C_PRC_TO_OFF_VERY_HIGH = 100;
const int C_PRC_TO_OFF_HIGH = 55;
const int C_PRC_TO_OFF_MED  = 50;
const int C_PRC_TO_OFF_LOW  = 35;
const int C_PRC_TO_OFF_VERY_LOW = 10;
const int C_CHANCE_VERY_HIGH_PRC = 4;
const int C_CHANCE_HIGH_PRC = 29;
const int C_CHANCE_MED_PRC = 29;
const int C_CHANCE_LOW_PRC = 27;
const int C_CHANCE_VERY_LOW_PRC = 11;
const int C_PRC_TO_OFF_OVERCROWD_ADJ = 20; //this % value is added (in fractions) when close to IDEAL_NUM_OF_AGENTS_IN_GRID agents are reached
const int C_PRC_TO_OFF_SMALL_POP_SUB = 10; //this value is subtracted from prc_to_off when pop is below MIN_NUM_OF_AGENTS_IN_GRID

const int C_PRC_OF_SURPLUS_TO_OFF = 20;// this prc of fitness that is above the matstart-threshold is given to make them abit stronger, effect: highly rewarded agents do not only get more offspring also stronger
const int C_FITN_ABOVE_GIVES_BONUS = 250; //C_PRC_OF_SURPLUS_TO_OFF % of fitness above this threshold is given in fitn bonus
const int C_AGENT_HEALTHY_PRC = 400; // if agent fitness is above this prc * initFitness, status is 'really healthy': offspring is boosted
const int C_PRC_TO_OFF_BOOST = 10; //if agent is really healthy (fitn>(C_AGENT_HEALTHY_PRC*initFitn) offspring also gets extra healthy
const int C_MATSTOP_PRC  = C_PRC_TO_OFF_VERY_LOW+1;
//const int C_MAT_OVERCROWD_SCANRAD = 2; //the scanning radíus when checking neighbourhood for overcrowding
//const int C_MAT_OVERCROWD_PRC = 81; //if more than this % of neighbourhood is occupied, no mating occurs
//const int C_MATING_BONUS_PRC = 0; //prc of initial fitness given as bonus to maters (if matingBonusBuffer>0)
//const int C_MAX_BONUS_UP_TO_FITN_PRC = 750; //agent are max given bonus up to fitness equal this PRC*initFitn

//offspring are produced according to this principle:
// (?)

const int MIN_BUS_FITN_COL_PRC = 10;//if fitness gets below value*initialFitness , agent dies
const int MIN_BUS_FITN_INP_PRC = 10;
const int MIN_BUS_FITN_EFF_PRC = 20;

const int NUM_OF_TAKE_STEP_TRIES = 10;

const int MAX_NUM_OFFSPRING = 15; //the maximum number of offspring for an agent
const int BIDLEVELS_LENGTH = 2;
const int NUM_OF_BID_LEVELS = pow(2,BIDLEVELS_LENGTH);

const int SESS_BETW_REWARD_UPDATE = 12; //during these sessions all fitness in Colline are expected to have flowed back to env (agents dead) - one generation
const int LOADING_TIME = 5; //the messages from environment are loaded in LOADING_TIME cycles (maybe not all slots signed up to by fountains), note: time per 'image' - more images maybe loaded in one session
const int BEGIN_2ND_LOAD_TIME = 6;
const int BEGIN_3RD_LOAD_TIME = 12;
const bool SECOND_LOAD_ON = false;
const bool THIRD_LOAD_ON = false;

const int MIN_BID = 10;

const int NUM_OF_PIPES = 125; 
const int NUM_OF_FOUNTAINS = NUM_OF_PIPES;//each fountain has its own pipe

const bool DC_ONLY_READS_MESSAGE = false; //true: effector message can be sold to another effector: false: decCat buys message
const int DC_MIN_OK_RESP_EFF = 0;//50; //if less responds (ok), decrease the chosen effector level with one (unless already at level 1; level 0 is reserved for 'internal processing')
const int DC_MAX_OK_RESP_EFF = 0;//130; //if more responds (ok), increase the chosen effector level with one (unless already at highest level)
const int DC_MAX_REWARDED = 300; //if this # of OK's are reached in a session, decCat stops bidding
const int DC_MIN_ACC_OK_LEVEL = 3; //if the prev num of ok's is less, this number is used as estimate
const int DC_MIN_ACC_OK_TOTAL = 12; //used when picki levels are ignored
const int DC_NUM_OF_OKS_RESTORING = 20; //when restoring organism this is used as ok-estimate
const int DC_ALLOWED_OVERPAY_PRC = 30; //max fitness to agents/session is this percentage times fitnessBuffer
const int DC_ADDITIONAL_EST_PRC = 30; //when fitnessPaidMax is reached in last session the estimate is increased by this factor
//const int DC_EARLY_RESP_REW_I3_PRC = 100;
const int DC_MAX_LATE_RESP_PRC = 300;
/*const int DC_NO_REWARD_AREA_MIN_X = 44; //(OFF)
const int DC_NO_REWARD_AREA_MAX_X = 56;
const int DC_NO_REWARD_AREA_MIN_Y = 44;
const int DC_NO_REWARD_AREA_MAX_Y = 56;
const int DC_REW_AREA_NORTH_MAX_Y = 52; //(OFF)
const int DC_REW_AREA_SOUTH_MIN_Y = 48; //(OFF)
const int DC_REW_AREA_COLUMN_MIN_X = 25;
const int DC_REW_AREA_COLUMN_MAX_X = 75; */
const int DC_MAX_BUFFERSIZE_MUL = 3;

const int STAT_SAMPLE_RATE = SESSION_LENGTH_INIT; //cycle data is only stored every STAT_SAMPLE_RATE cycle
const int SESS_BETW_AGENT_SAMPLE = 15;

const double FORGET_FACTOR_PERF = 0.95; //used to calc performance for cat0 and cat1 respectively, value is number of correct last (1-FORGET_FACTOR)^-1 sessions 
const int MAX_PERF_SCORE = 20; // calculate as : 1/(1-FORGET_FACTOR_PERF) (roundoff error occurs by using formula)
const int MAX_MSIZE = 31; //maximum bits in message

const double FORGET_FACTOR_AGE = 0.99; //agent age of dying agents are multiplied with (1-FF) and added to accum. multiplied FF, effect: average value of past 1/(1-FF) agents


//---------------
const int TYPE_CATEGORY  = -1; //decision category, invisible to other agents
const int TYPE_FOUNTAIN  = 0; // don't change values: used in appearBus
const int TYPE_COLLECTOR = 1; // do.
const int TYPE_INPODER   = 2; // do.
const int TYPE_EFFECTOR  = 3; // do.

const int ST_DRIFTER   = -2;   //involved in evolution
const int ST_PROCESSOR = -3; //-          -  creation 
const int ST_OBSERVER  = -4;  //-         -  conditioning
const int ST_DEAD      = -5;
const int ST_FINISHED_THIS_CYCLE = -6;

const int PROC_AND  = 0;
const int PROC_OR   = 1;
const int PROC_XOR  = 2;
const int PROC_NAND = 3;
const int PROC_NOR  = 4;
const int PROC_XNOR = 5;
const int PROC_INA = 6; //input from INP1 or BOTH...value 'ripple on' when second input is present (needed?)
const int PROC_INB = 7; //input from INP2 or BOTH...value 'ripple on' when second input is present (needed?)

const int CAT_INP1 = -30; //used to label observed agent as an input1 supplier ONLY
const int CAT_INP2 = -31; //input 2
const int CAT_BOTH = -32; //both
const int CAT_NONE = -33;

//Filenames
constexpr auto FN_EXT = ".out";
constexpr auto FN_CONSTANTS = "_constants";
constexpr auto FN_FITNCOL = "_inf_fitn_colline.out";
constexpr auto FN_FITNBUFF = "_inf_fitn_buff.out";
constexpr auto FN_FITNENV = "_inf_fitn_env.out";
constexpr auto FN_MATERS = "_inf_maters.out";
constexpr auto FN_AGENTS = "_inf_agents.out";
constexpr auto FN_BITSEATEN = "_inf_bits_eaten.out";
constexpr auto FN_RESPCORRECT = "_inf_resp_correct.out";
constexpr auto FN_RESPWRONG = "_inf_resp_wrong.out";
constexpr auto FN_CATEGORY = "_inf_category.out";
constexpr auto FN_DNACOLL = "_dna_coll";
constexpr auto FN_DNAINP = "_dna_inp";
constexpr auto FN_DNAEFF = "_dna_eff";
constexpr auto FN_AGENTPOP = "_rest_agent_pop";
constexpr auto FN_ORGANISMVAR = "_rest_organism_var";

//colors
const UINT COLOR_GRID = RGB(245,245,245); //light grey
const UINT COLOR_FOUN = RGB(0,0,0);
const UINT COLOR_COLL = RGB(0,240,0); //green
const UINT COLOR_INP  = RGB(240,240,0); //yellow
const UINT COLOR_EFF  = RGB(0,0,255); //blue
const UINT COLOR_DRIF = RGB(200,200,200); //grey
const UINT COLOR_MATE = RGB(255,0,255); //purple
const UINT COLOR_PROC = RGB(255,0,0); //red

const int C_ACC_CHANCE_HIGH = C_CHANCE_VERY_HIGH_PRC + C_CHANCE_HIGH_PRC;
const int C_ACC_CHANCE_MED = C_ACC_CHANCE_HIGH + C_CHANCE_MED_PRC;
const int C_ACC_CHANCE_LOW = C_ACC_CHANCE_MED + C_CHANCE_LOW_PRC;
const int C_ACC_CHANCE_VERY_LOW = C_ACC_CHANCE_LOW + C_CHANCE_VERY_LOW_PRC;



//OLD
//const int C_PFITNTOOFF_BASE = 27; //    old: base percent of fitness transferred to offspring when mating: percent = base + (#off)^2
//const int C_MAX_TO_OFF_PRC = 55; //% times parents' initial fitness
//const int C_MIN_TO_OFF_PRC = 22; //% times parents' initial fitness (should be lower than C_MATSTOP)
//const double C_MATSTART = 2.5; //2.5;
//const double C_MATSTOP  = 0.25; //0.2;
//const int C_PRC_TO_OFF_VHIGH = 60;

//const int NO_MATING_AREA_MIN_X = 20;
//const int NO_MATING_AREA_MAX_X = 80;
//const int NO_MATING_AREA_MIN_Y = 20;
//const int NO_MATING_AREA_MAX_Y = 80;

