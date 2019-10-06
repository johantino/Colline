// StatisticsHandler.cpp: implementation of the StatisticsHandler class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Colline.h"
#include "StatisticsHandler.h"
#include "iostream.h"
#include <conio.h> //for press key
#include "Konst.h"
#include "Agent.h" //for updateGridView
#include "dCollector.h" //for saveDNA
#include "Inpoder.h" //for saveDNA
#include "Effector.h" //for saveDNA
#include "Fountain.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

StatisticsHandler::StatisticsHandler(bool isNewOrganism, int restoredSesReward, /*Environment* environ,*/ Grid* aGrid, byte fileIdVal, int restoredDNAsampleNum)
{
	//env = environ;
	agentGrid = aGrid;
	//max_fitn_rew_sess = 1.5*(INIT_FITN_ENV / (AGENT_LIFETIME/SESSION_LENGTH)); //theory: if more than this are paid in one session, the sys gets unstable
	//trainLastFitnessBalance = 0; //fitnesColl - fitn_env....delete?
	//agentView = aView;
	//trainingView = tView;
	//gridView = gView;
	dnaSampleNum = restoredDNAsampleNum; //0

	nextViewFrame = new CUIntArray;
	int totalCells = agentGrid->getHightY() * agentGrid->getWidthX();
	//int totalCells = gridView->getWidth() * gridView->getHight();
	nextViewFrame->SetSize(totalCells);
	for (int i=0; i<totalCells; i++) {
		nextViewFrame->SetAt(i, COLOR_GRID);
	}
	//int fileIdVal = (byte)time(NULL); //used to avoid overwriting of files
	fname_begin = "cdata" + numToString( fileIdVal );
	
	writeConstants();
	int useThisNumOfOks;
	//check whether new organism or restoring previously
	if (isNewOrganism) {
		file_fitnColline = fopen(fname_begin + FN_FITNCOL, "w" );
		file_fitnBuff = fopen(fname_begin + FN_FITNBUFF,"w");
		file_fitnEnv = fopen(fname_begin + FN_FITNENV,"w");
		file_maters = fopen(fname_begin + FN_MATERS,"w");
		file_agents = fopen(fname_begin + FN_AGENTS,"w");
		file_bits_eaten = fopen(fname_begin + FN_BITSEATEN,"w");
		file_correct_resp = fopen(fname_begin + FN_RESPCORRECT,"w");
		file_wrong_resp = fopen(fname_begin + FN_RESPWRONG,"w");
		file_category = fopen(fname_begin + FN_CATEGORY, "w");
		useThisNumOfOks = 0;
	} else { //append to previously stored data
		file_fitnColline = fopen(fname_begin + FN_FITNCOL, "a+" );
		file_fitnBuff = fopen(fname_begin + FN_FITNBUFF,"a+");
		file_fitnEnv = fopen(fname_begin + FN_FITNENV,"a+");
		file_maters = fopen(fname_begin + FN_MATERS,"a+");
		file_agents = fopen(fname_begin + FN_AGENTS,"a+");
		file_bits_eaten = fopen(fname_begin + FN_BITSEATEN,"a+");
		file_correct_resp = fopen(fname_begin + FN_RESPCORRECT,"a+");
		file_wrong_resp = fopen(fname_begin + FN_RESPWRONG,"a+");
		file_category = fopen(fname_begin + FN_CATEGORY, "a+");
		useThisNumOfOks = DC_NUM_OF_OKS_RESTORING;
	}

	//Runtime adjustable parameters
	MAX_STATE_BUYERS_INP = MAX_STATE_BUYERS_INP_INIT;
	BIDP_BASE_INP = BIDP_BASE_INP_INIT;
	BIDP_STEP_INP = BIDP_STEP_INP_INIT;
	BIDP_BASE_EFF = BIDP_BASE_EFF_INIT;
	BIDP_STEP_EFF = BIDP_STEP_EFF_INIT;
	SESSION_LENGTH = SESSION_LENGTH_INIT;
	AGENT_LIFETIME_MAX = AGENT_LIFETIME_MAX_INIT;
	COLL_MIN_PROCTIME = COLL_MIN_PROCTIME_INIT;
	COLL_MAX_PROCTIME = COLL_MIN_PROCTIME + (COLLECTION_TIME-1);
	GRID_ZOOM = GRID_ZOOM_INIT;
	C_MATSTART_PRC = C_MATSTART_PRC_INIT;
	DC_REW_FROM_BID_AGR_LEVEL = DC_REW_FROM_BID_AGR_LEVEL_INIT;
	MAX_INACTIVE_SESS = MAX_INACTIVE_SESS_INIT;
	MUTATION_RATE = MUTATION_RATE_INIT;
	HIGH_MUT_AGENTS_PRC = HIGH_MUT_AGENTS_PRC_INIT;
	MIN_NUM_OF_AGENTS_IN_GRID = MIN_NUM_OF_AGENTS_IN_GRID_INIT;
	IDEAL_NUM_OF_AGENTS_IN_GRID = IDEAL_NUM_OF_AGENTS_IN_GRID_INIT;
	GRAVITY_CENTER = GRAVITY_CENTER_INIT;
	DC_EARLY_RESP_REW_I0_PRC = DC_EARLY_RESP_REW_I0_PRC_INIT;
	DC_EARLY_RESP_REW_I1_PRC = DC_EARLY_RESP_REW_I1_PRC_INIT;
	DC_EARLY_RESP_REW_I2_PRC = DC_EARLY_RESP_REW_I2_PRC_INIT;
	MAX_NUM_OF_OBS_INP = MAX_NUM_OF_OBS_INP_INIT;
	MAX_NUM_OF_OBS_EFF = MAX_NUM_OF_OBS_EFF_INIT;
	//end adj.para.

	testNumOfProcEffectors = 0;
	accBorn = 0;
	accDead = 0;
	agentsAlive = 0; //delete?
	numOfAgents = 0;
	numOfAgents_coll = 0;
	numOfAgents_inp = 0;
	numOfAgents_eff = 0;
	numOfAgents_foun = 0;
	prevAgentsAlive = 0;
	numOfObservers = 0;
	numOfObs_foun=0;
	numOfObs_coll=0;
	numOfObs_inp=0;
	numOfObs_eff=0;
	numOfDrifters = 0;
	numOfDrif_foun=0;
	numOfDrif_coll=0;
	numOfDrif_inp=0;
	numOfDrif_eff=0;
	numOfProcessors = 0;
	numOfProc_foun=0;
	numOfProc_coll=0;
	numOfProc_inp=0;
	numOfProc_eff=0;
	numOfMaters = 0;
	numOfNonMaters = 0; //needed?
	numOfMaters_coll=0;
	numOfMaters_inp=0;
	numOfMaters_eff=0;

	fitnessEnvironment = 0;
	//fitnessBuffer0_cp = 0;
	//fitnessBuffer1_cp = 0;
	fitnessColline = 0;
	fitnessColline_coll = 0;
	fitnessColline_inp = 0;
	fitnessColline_eff = 0;

	lastSesNumOfProcEff = new UIntGrid( NUM_OF_BID_LEVELS, (COLL_MAX_PROCTIME - COLL_MIN_PROCTIME + 1));
	lastSesFitnessPaid = new UIntGrid(NUM_OF_BID_LEVELS, (COLL_MAX_PROCTIME - COLL_MIN_PROCTIME + 1));
	lastSesCorrectResp = new UIntGrid(NUM_OF_BID_LEVELS, (COLL_MAX_PROCTIME - COLL_MIN_PROCTIME + 1));
	lastSesWrongResp = new UIntGrid(NUM_OF_BID_LEVELS, (COLL_MAX_PROCTIME - COLL_MIN_PROCTIME + 1));
	lastSesNumOfCorr_EstForNext = new UIntGrid( NUM_OF_BID_LEVELS, 1);
	lastSesRewardOnLevels = new UIntGrid( NUM_OF_BID_LEVELS, 1);

	thisSesNumOfProcEff = new UIntGrid(NUM_OF_BID_LEVELS, (COLL_MAX_PROCTIME - COLL_MIN_PROCTIME + 1));
	thisSesFitnessPaid = new UIntGrid(NUM_OF_BID_LEVELS, (COLL_MAX_PROCTIME - COLL_MIN_PROCTIME + 1));
	thisSesCorrectResp = new UIntGrid(NUM_OF_BID_LEVELS, (COLL_MAX_PROCTIME - COLL_MIN_PROCTIME + 1));
	thisSesCorrectResp->setValue(useThisNumOfOks, NUM_OF_BID_LEVELS-1, COLL_MAX_PROCTIME - COLL_MIN_PROCTIME);
	thisSesWrongResp = new UIntGrid(NUM_OF_BID_LEVELS, (COLL_MAX_PROCTIME - COLL_MIN_PROCTIME + 1));
	thisSesNumOfCorr_EstForNext = new UIntGrid( NUM_OF_BID_LEVELS, 1); 
	thisSesRewardOnLevels = new UIntGrid( NUM_OF_BID_LEVELS, 1);

	lastCorrectRespCatZero = new UIntGrid( NUM_OF_BID_LEVELS, (COLL_MAX_PROCTIME - COLL_MIN_PROCTIME + 1));
	lastCorrectRespCatZero->setValue(useThisNumOfOks, NUM_OF_BID_LEVELS-1, COLL_MAX_PROCTIME - COLL_MIN_PROCTIME);
	lastCorrectRespCatOne  = new UIntGrid( NUM_OF_BID_LEVELS, (COLL_MAX_PROCTIME - COLL_MIN_PROCTIME + 1));
	lastCorrectRespCatOne->setValue(useThisNumOfOks, NUM_OF_BID_LEVELS-1, COLL_MAX_PROCTIME - COLL_MIN_PROCTIME);


	lastSesCorrectCategory = false;
	
	lastSesCollineAnswer = false;
	lastSesChosenPickiLevel = 0;
	lastSesBitsProduced = 0;
	lastSesBitsProcessed = 0;
	lastSesBitsDiscU = 0;
	lastSesBitsDiscP = 0;
	lastSesBitsFromEnv = 0;
	lastSesBitsEaten = 0;
	lastSesFitnBalance_end = 0;
	lastSesFromEnvToBuff = 0;
	lastSesFitnessBuffer0_cp = 0;
	lastSesFitnessBuffer1_cp = 0;
	lastSesMatingBonusBuffer_cp = 0;

	thisSesCorrectCategory = false;
	thisSesChosenPickiLevel = 0;
	thisSesBitsProduced = 0;
	thisSesBitsProcessed = 0; //bits processed by decCat and effectors
	thisSesBitsDiscU = 0;	//needed?
	thisSesBitsDiscP = 0; //inpoders processed messages not sold
	thisSesBitsFromEnv = 0;
	thisSesBitsEaten = 0;
	thisSesFitnessBuffer0_cp = 0;
	thisSesFitnessBuffer1_cp = 0;
	thisSesMatingBonusBuffer_cp = 0;

	thisBlockSessionReward = restoredSesReward; //is 0 if new organism

	currentSpeciesCount = new UIntGrid(NUM_OF_SPECIES, 3); //remove
	speciesStat_col = new UIntGrid( NUM_STAT_SPECIES , NUM_OF_SPECIES);
	speciesStat_inp = new UIntGrid( NUM_STAT_SPECIES , NUM_OF_SPECIES);
	speciesStat_eff = new UIntGrid( NUM_STAT_SPECIES , NUM_OF_SPECIES);
	numOfCatZero = 0;
	numOfCatOne = 0;
	performance_cat0= MAX_PERF_SCORE/2; //start with medium score
	performance_cat1= MAX_PERF_SCORE/2;

	avgAgentAge = AGENT_LIFETIME_MAX_INIT/2; //to start somewhere
	percThatReachMaxAge = 50;
	avgStateBuyers_inp = MAX_STATE_BUYERS_INP/2;
	percAuctionsThatHitMax_inp = 50;
}

StatisticsHandler::~StatisticsHandler()
{

}

void StatisticsHandler::storePopulation(int cycNum, int sesNum, int matingBuffer, int fitnBuff0, int fitnBuff1, int idCounterValue) {
	//close all files
	Observable* currObs;
	Agent* currAgent;
	Fountain* currFountain;
	char tab = '	';
	FILE* agentPopFile = fopen(fname_begin + FN_AGENTPOP + "_" + numToString(sesNum) + FN_EXT, "w");
	CList<Observable*, Observable*>* allAgents = agentGrid->getNeighbours( agentGrid->getCellAt( agentGrid->getWidthX()/2, agentGrid->getHightY()/2), agentGrid->getWidthX()/2 + 2, true);
	if (allAgents->GetCount() != numOfAgents) {
		cout << "ERROR: storePop, numOfagents error" << endl;
		cout << "vec: " << allAgents->GetCount() << endl;
		cout << "stat: " << numOfAgents << endl;
		pressSpaceToQuit();
	}
	fprintf(agentPopFile, "%d\n", numOfAgents);
	for (int i=0; i<numOfAgents; i++) {
		currObs = allAgents->GetAt( allAgents->FindIndex(i) );
		fprintf(agentPopFile, "%d%c", currObs->getType(), tab);
		if (currObs->getType() == TYPE_FOUNTAIN) {
			currFountain = (Fountain*)currObs;
			int fountainAppVal = currFountain->getAppearBusSmall()->getValue();
			if (fountainAppVal >= pow(2, APPEAR_SIZE_BUS_SMALL)) {
				cout << "error: store Pop. fountain app value" << endl;
				cout << "value: " << fountainAppVal << endl;
				pressSpaceToQuit();
			} //fix, delete this test later
			fprintf(agentPopFile, "%d%c", fountainAppVal, tab);
			fprintf(agentPopFile, "%d%c", currFountain->getPosition()->getPosX(), tab);
			fprintf(agentPopFile, "%d%c", currFountain->getPosition()->getPosY(), tab);
			fprintf(agentPopFile, "%d\n", currFountain->getLoadingSlot());
		} else {
			currAgent = (Agent*)currObs;
			// [type, appMat, appBusSmall, dna_ag, dna_type, init_fitn, fitness, posX, posY, age, matStat, lastPrSes]
			fprintf(agentPopFile, "%d%c", currAgent->getAppearMat()->getValue(), tab);
			fprintf(agentPopFile, "%d%c", currAgent->getAppearBusSmall()->getValue(), tab);
			fprintf(agentPopFile, "%d%c", currAgent->getAgentDNA()->getValue(), tab);
			fprintf(agentPopFile, "%d%c", currAgent->getTypeDNA()->getValue(), tab);
			fprintf(agentPopFile, "%d%c", currAgent->getInitialFitness(), tab);
			fprintf(agentPopFile, "%d%c", currAgent->getFitness(), tab);
			fprintf(agentPopFile, "%d%c", currAgent->getPosition()->getPosX(), tab);
			fprintf(agentPopFile, "%d%c", currAgent->getPosition()->getPosY(), tab);
			fprintf(agentPopFile, "%d%c", currAgent->getAge(), tab);
			int matStat = currAgent->isReadyToMate();
			if (matStat != 0 && matStat != 1) {
				cout << "errror: matstat storePopulation" << endl;
				pressSpaceToQuit();
			} //delete this test later
			fprintf(agentPopFile, "%d%c", matStat, tab);
			fprintf(agentPopFile, "%d\n", currAgent->getLastProcessedInSession());
		}
	}

	fclose(agentPopFile);
	delete allAgents;
	FILE* organismVarFile = fopen(fname_begin + FN_ORGANISMVAR + "_" + numToString(sesNum) + FN_EXT, "w");
	fprintf(organismVarFile, "%d\n", cycNum );
	fprintf(organismVarFile, "%d\n", sesNum );
	fprintf(organismVarFile, "%d\n", fitnessEnvironment);
	fprintf(organismVarFile, "%d\n", matingBuffer);
	fprintf(organismVarFile, "%d\n", fitnBuff0 );
	fprintf(organismVarFile, "%d\n", fitnBuff1 );
	fprintf(organismVarFile, "%d\n", thisBlockSessionReward );
	fprintf(organismVarFile, "%d\n", dnaSampleNum );
	fprintf(organismVarFile, "%d\n", idCounterValue );
	fclose(organismVarFile);
}

/*void StatisticsHandler::restorePopulation() {
	int numOfEntries, type, posX, posY, loadingSlot, dna_obs_val, dna_ag_val, dna_type_val;
	int initFitn, currFitn;
	FILE* agentPopFile;
	if ((agentPopFile = fopen(fname_begin + FN_AGENTPOP, "r")) == NULL) {
		cout << "ERROR: couldn't open restore population, file not found" << endl;
		pressSpaceToQuit();
	} else
		cout << "Restoring population..." << endl;
	fseek( agentPopFile, 0, SEEK_SET);
	fscanf(agentPopFile,"%d", &numOfEntries); //first value in file gives num of agents
	for (int i=0; i<numOfEntries; i++) {
		fscanf(agentPopFile,"%d", &type);
		cout << type  << " ";
		if (type == TYPE_FOUNTAIN) {
			fscanf(agentPopFile,"%d", &posX);
			fscanf(agentPopFile,"%d", &posY);
			fscanf(agentPopFile,"%d", &loadingSlot);
			
			cout << posX << " ";
			cout << posY << " ";
			cout << loadingSlot << " " << endl;
			//read fountain values
		} else if (type == TYPE_COLLECTOR || type == TYPE_INPODER || type == TYPE_EFFECTOR) {
			fscanf(agentPopFile,"%d", &dna_obs_val);
			fscanf(agentPopFile,"%d", &dna_ag_val);
			fscanf(agentPopFile,"%d", &dna_type_val);
			fscanf(agentPopFile,"%d", &initFitn);
			fscanf(agentPopFile,"%d", &currFitn);
			fscanf(agentPopFile,"%d", &posX);
			fscanf(agentPopFile,"%d", &posY);
			cout << dna_obs_val << " ";
			cout << dna_ag_val << " ";
			cout << dna_type_val << " ";
			cout << initFitn << " ";
			cout << currFitn << " ";
			cout << posX << " ";
			cout << posY << endl;
		} else {
			cout << "ERROR: reading file in rest.pop." << endl;
			pressSpaceToQuit();
		}
	}

}*/

void StatisticsHandler::writeConstants() {
	file_constants = fopen(fname_begin + FN_CONSTANTS + numToString(dnaSampleNum) + ".m", "w");
	fprintf(file_constants, "%s", "GRIDSIZE = ");
	fprintf(file_constants, "%d\n", GRIDSIZE);
	fprintf(file_constants, "%s", "IDEAL_NUM_OF_AGENTS_IN_GRID = ");
	fprintf(file_constants, "%d\n", IDEAL_NUM_OF_AGENTS_IN_GRID);
	fprintf(file_constants, "%s", "MIN_NUM_OF_AGENTS_IN_GRID = ");
	fprintf(file_constants, "%d\n", MIN_NUM_OF_AGENTS_IN_GRID);

	fprintf(file_constants, "%s", "INIT_NUM_OF_COLL = ");
	fprintf(file_constants, "%d\n", INIT_NUM_OF_COLL);
	fprintf(file_constants, "%s", "INIT_NUM_OF_INP = ");
	fprintf(file_constants, "%d\n", INIT_NUM_OF_INP);
	fprintf(file_constants, "%s", "INIT_NUM_OF_EFF = ");
	fprintf(file_constants, "%d\n", INIT_NUM_OF_EFF);
	fprintf(file_constants, "%s", "INIT_FITN_COL_BASE = ");
	fprintf(file_constants, "%d\n", INIT_FITN_COL_BASE);
	fprintf(file_constants, "%s", "INIT_FITN_COL_RAND = ");
	fprintf(file_constants, "%d\n", INIT_FITN_COL_RAND);
	fprintf(file_constants, "%s", "INIT_FITN_INP_BASE = ");
	fprintf(file_constants, "%d\n", INIT_FITN_INP_BASE);
	fprintf(file_constants, "%s", "INIT_FITN_INP_RAND = ");
	fprintf(file_constants, "%d\n", INIT_FITN_INP_RAND);
	fprintf(file_constants, "%s", "INIT_FITN_EFF_BASE = ");
	fprintf(file_constants, "%d\n", INIT_FITN_EFF_BASE);
	fprintf(file_constants, "%s", "INIT_FITN_EFF_RAND = ");
	fprintf(file_constants, "%d\n", INIT_FITN_EFF_RAND);
	fprintf(file_constants, "%s", "MUTATION_RATE = ");
	fprintf(file_constants, "%d\n", MUTATION_RATE);

	/*fprintf(file_constants, "%s", "MUTATION_RATE_SPEC_PM = ");
	fprintf(file_constants, "%d\n", MUTATION_RATE_SPEC_PM);
	fprintf(file_constants, "%s", "MUTATION_RATE_BUS_APP_PM = ");
	fprintf(file_constants, "%d\n", MUTATION_RATE_BUS_APP_PM);
	fprintf(file_constants, "%s", "MUTATION_RATE_DNA_AG_PM = ");
	fprintf(file_constants, "%d\n", MUTATION_RATE_DNA_AG_PM);
	fprintf(file_constants, "%s", "MUTATION_RATE_DNA_TYPE_PM = ");
	fprintf(file_constants, "%d\n", MUTATION_RATE_DNA_TYPE_PM);*/
	fprintf(file_constants, "%s", "MUTATION_RATE_HIGH_ADD_PM = ");
	fprintf(file_constants, "%d\n", MUTATION_RATE_HIGH_ADD_PM);
	fprintf(file_constants, "%s", "HIGH_MUT_AGENTS_PRC = ");
	fprintf(file_constants, "%d\n", HIGH_MUT_AGENTS_PRC);
	fprintf(file_constants, "%s", "APPEAR_SIZE_MAT = ");
	fprintf(file_constants, "%d\n", APPEAR_SIZE_MAT);
	fprintf(file_constants, "%s", "APPEAR_SIZE_BUS = ");
	fprintf(file_constants, "%d\n", APPEAR_SIZE_BUS);
	fprintf(file_constants, "%s", "N_DNA_AG = ");
	fprintf(file_constants, "%d\n", N_DNA_AG);
	fprintf(file_constants, "%s", "N_DNA_COLL = ");
	fprintf(file_constants, "%d\n", N_DNA_COLL);
	fprintf(file_constants, "%s", "N_DNA_INP = ");
	fprintf(file_constants, "%d\n", N_DNA_INP);
	fprintf(file_constants, "%s", "N_DNA_EFF = ");
	fprintf(file_constants, "%d\n", N_DNA_EFF);
	fprintf(file_constants, "%s", "SAMPLE_PERCENT_AGENTS = ");
	fprintf(file_constants, "%d\n", SAMPLE_PERCENT_AGENTS);
	fprintf(file_constants, "%s", "APPEAR_SIZE_BUS = ");
	fprintf(file_constants, "%d\n", APPEAR_SIZE_BUS);
//	fprintf(file_constants, "%s", "NUM_OF_PICKI_LEVELS = ");
//	fprintf(file_constants, "%d\n", NUM_OF_PICKI_LEVELS);
	fprintf(file_constants, "%s", "COLL_MIN_PROCTIME = ");
	fprintf(file_constants, "%d\n", COLL_MIN_PROCTIME);
	fprintf(file_constants, "%s", "COLL_MAX_PROCTIME = ");
	fprintf(file_constants, "%d\n", COLL_MAX_PROCTIME);
	fprintf(file_constants, "%s", "COLLECTION_TIME = ");
	fprintf(file_constants, "%d\n", COLLECTION_TIME);
	fprintf(file_constants, "%s", "MAX_NUM_OF_OBS_COL = ");
	fprintf(file_constants, "%d\n", MAX_NUM_OF_OBS_COL);
	fprintf(file_constants, "%s", "MAX_NUM_OF_OBS_INP = ");
	fprintf(file_constants, "%d\n", MAX_NUM_OF_OBS_INP);
//	fprintf(file_constants, "%s", "MAX_NUM_OF_OBS_BOTH_EFF = ");
//	fprintf(file_constants, "%d\n", MAX_NUM_OF_OBS_BOTH_EFF);
	fprintf(file_constants, "%s", "MAX_NUM_OF_OBS_EFF = ");
	fprintf(file_constants, "%d\n", MAX_NUM_OF_OBS_EFF);
	fprintf(file_constants, "%s", "VICINITY_MAT = ");
	fprintf(file_constants, "%d\n", VICINITY_MAT);
//	fprintf(file_constants, "%s", "VICINITY_BUS_COLL = ");
//	fprintf(file_constants, "%d\n", VICINITY_BUS_COLL);
//	fprintf(file_constants, "%s", "VICINITY_BUS_INP = ");
//	fprintf(file_constants, "%d\n", VICINITY_BUS_INP);
//	fprintf(file_constants, "%s", "VICINITY_BUS_EFF = ");
//	fprintf(file_constants, "%d\n", VICINITY_BUS_EFF);
	fprintf(file_constants, "%s", "VIC_ADD_INP = ");
	fprintf(file_constants, "%d\n", VIC_ADD_INP);
	fprintf(file_constants, "%s", "VIC_ADD_EFF = ");
	fprintf(file_constants, "%d\n", VIC_ADD_EFF);
	fprintf(file_constants, "%s", "MIN_STEPSIZE = ");
	fprintf(file_constants, "%d\n", MIN_STEPSIZE);
	fprintf(file_constants, "%s", "MAX_REWARD = ");
	fprintf(file_constants, "%d\n", MAX_REWARD);
	fprintf(file_constants, "%s", "MIN_REWARD = ");
	fprintf(file_constants, "%d\n", MIN_REWARD);
	/*fprintf(file_constants, "%s", "C_PFITNTOOFF_BASE = ");
	fprintf(file_constants, "%d\n", C_PFITNTOOFF_BASE);
	fprintf(file_constants, "%s", "C_MAX_TO_OFF_PRC = ");
	fprintf(file_constants, "%d\n", C_MAX_TO_OFF_PRC);
	fprintf(file_constants, "%s", "C_MIN_TO_OFF_PRC = ");
	fprintf(file_constants, "%d\n", C_MIN_TO_OFF_PRC);
	fprintf(file_constants, "%s", "C_MATSTART = ");
	fprintf(file_constants, "%f\n", C_MATSTART);//note: double
	fprintf(file_constants, "%s", "C_MATSTOP = ");
	fprintf(file_constants, "%f\n", C_MATSTOP);//note: double
*/
	fprintf(file_constants, "%s", "C_PRC_TO_OFF_BOOST = ");
	fprintf(file_constants, "%d\n", C_PRC_TO_OFF_BOOST);
	fprintf(file_constants, "%s", "C_PRC_TO_OFF_VERY_HIGH = ");
	fprintf(file_constants, "%d\n", C_PRC_TO_OFF_VERY_HIGH);
	fprintf(file_constants, "%s", "C_PRC_TO_OFF_HIGH = ");
	fprintf(file_constants, "%d\n", C_PRC_TO_OFF_HIGH);
	fprintf(file_constants, "%s", "C_PRC_TO_OFF_MED = ");
	fprintf(file_constants, "%d\n", C_PRC_TO_OFF_MED);
	fprintf(file_constants, "%s", "C_PRC_TO_OFF_LOW = ");
	fprintf(file_constants, "%d\n", C_PRC_TO_OFF_LOW);
	fprintf(file_constants, "%s", "C_PRC_TO_OFF_VERY_LOW = ");
	fprintf(file_constants, "%d\n", C_PRC_TO_OFF_VERY_LOW);
	fprintf(file_constants, "%s", "C_CHANCE_VERY_HIGH_PRC = ");
	fprintf(file_constants, "%d\n", C_CHANCE_VERY_HIGH_PRC);
	fprintf(file_constants, "%s", "C_CHANCE_HIGH_PRC = ");
	fprintf(file_constants, "%d\n", C_CHANCE_HIGH_PRC);
	fprintf(file_constants, "%s", "C_CHANCE_MED_PRC = ");
	fprintf(file_constants, "%d\n", C_CHANCE_MED_PRC);
	fprintf(file_constants, "%s", "C_CHANCE_LOW_PRC = ");
	fprintf(file_constants, "%d\n", C_CHANCE_LOW_PRC);
	fprintf(file_constants, "%s", "C_CHANCE_VERY_LOW_PRC = ");
	fprintf(file_constants, "%d\n", C_CHANCE_VERY_LOW_PRC);
	fprintf(file_constants, "%s", "C_PRC_TO_OFF_OVERCROWD_ADJ = ");
	fprintf(file_constants, "%d\n", C_PRC_TO_OFF_OVERCROWD_ADJ);
	fprintf(file_constants, "%s", "C_PRC_TO_OFF_SMALL_POP_SUB = ");
	fprintf(file_constants, "%d\n", C_PRC_TO_OFF_SMALL_POP_SUB);
	fprintf(file_constants, "%s", "C_MATSTART_PRC = ");
	fprintf(file_constants, "%d\n", C_MATSTART_PRC);
	fprintf(file_constants, "%s", "C_MATSTOP_PRC = ");
	fprintf(file_constants, "%d\n", C_MATSTOP_PRC);
	fprintf(file_constants, "%s", "C_AGENT_HEALTHY_PRC = ");
	fprintf(file_constants, "%d\n", C_AGENT_HEALTHY_PRC);
/*	fprintf(file_constants, "%s", "C_MAT_OVERCROWD_SCANRAD = ");
	fprintf(file_constants, "%d\n", C_MAT_OVERCROWD_SCANRAD);
	fprintf(file_constants, "%s", "C_MAT_OVERCROWD_PRC = ");
	fprintf(file_constants, "%d\n", C_MAT_OVERCROWD_PRC);*/
	/*fprintf(file_constants, "%s", "C_MAT_USE_HIGH = ");
	fprintf(file_constants, "%d\n", C_MAT_USE_HIGH);
	fprintf(file_constants, "%s", "C_MAT_USE_MED = ");
	fprintf(file_constants, "%d\n", C_MAT_USE_MED);*/
	fprintf(file_constants, "%s", "MIN_BUS_FITN_COL_PRC = ");
	fprintf(file_constants, "%d\n", MIN_BUS_FITN_COL_PRC);
	fprintf(file_constants, "%s", "MIN_BUS_FITN_INP_PRC = ");
	fprintf(file_constants, "%d\n", MIN_BUS_FITN_INP_PRC);
	fprintf(file_constants, "%s", "MIN_BUS_FITN_EFF_PRC = ");
	fprintf(file_constants, "%d\n", MIN_BUS_FITN_EFF_PRC);
	fprintf(file_constants, "%s", "MAX_INACTIVE_SESS = ");
	fprintf(file_constants, "%d\n", MAX_INACTIVE_SESS);
	fprintf(file_constants, "%s", "MAX_NUM_OFFSPRING = ");
	fprintf(file_constants, "%d\n", MAX_NUM_OFFSPRING);
	fprintf(file_constants, "%s", "AGENT_LIFETIME_MAX = ");
	fprintf(file_constants, "%d\n", AGENT_LIFETIME_MAX);
	fprintf(file_constants, "%s", "SESSION_LENGTH = ");
	fprintf(file_constants, "%d\n", SESSION_LENGTH);
	fprintf(file_constants, "%s", "SESS_BETW_REWARD_UPDATE = ");
	fprintf(file_constants, "%d\n", SESS_BETW_REWARD_UPDATE);
	fprintf(file_constants, "%s", "LOADING_TIME = ");
	fprintf(file_constants, "%d\n", LOADING_TIME);
	fprintf(file_constants, "%s", "MIN_BID = ");
	fprintf(file_constants, "%d\n", MIN_BID);
	fprintf(file_constants, "%s", "BIDP_BASE_COL = ");
	fprintf(file_constants, "%d\n", BIDP_BASE_COL);
	fprintf(file_constants, "%s", "BIDP_STEP_COL = ");
	fprintf(file_constants, "%d\n", BIDP_STEP_COL);
	fprintf(file_constants, "%s", "BIDP_BASE_INP = ");
	fprintf(file_constants, "%d\n", BIDP_BASE_INP);
	fprintf(file_constants, "%s", "BIDP_STEP_INP = ");
	fprintf(file_constants, "%d\n", BIDP_STEP_INP);
	fprintf(file_constants, "%s", "BIDP_BASE_EFF = ");
	fprintf(file_constants, "%d\n", BIDP_BASE_EFF);
	fprintf(file_constants, "%s", "BIDP_STEP_EFF = ");
	fprintf(file_constants, "%d\n", BIDP_STEP_EFF);
	fprintf(file_constants, "%s", "NUM_OF_PIPES = ");
	fprintf(file_constants, "%d\n", NUM_OF_PIPES);
	fprintf(file_constants, "%s", "DC_ONLY_READS_MESSAGE = ");
	fprintf(file_constants, "%d\n", (int)DC_ONLY_READS_MESSAGE);
	fprintf(file_constants, "%s", "DC_MIN_OK_RESP_EFF = ");
	fprintf(file_constants, "%d\n", DC_MIN_OK_RESP_EFF);
	fprintf(file_constants, "%s", "DC_MAX_OK_RESP_EFF = ");
	fprintf(file_constants, "%d\n", DC_MAX_OK_RESP_EFF);
	fprintf(file_constants, "%s", "DC_MAX_REWARDED = ");
	fprintf(file_constants, "%d\n", DC_MAX_REWARDED);
	fprintf(file_constants, "%s", "DC_MIN_ACC_OK_LEVEL = ");
	fprintf(file_constants, "%d\n", DC_MIN_ACC_OK_LEVEL);
	fprintf(file_constants, "%s", "DC_ALLOWED_OVERPAY_PRC = ");
	fprintf(file_constants, "%d\n", DC_ALLOWED_OVERPAY_PRC);
	fprintf(file_constants, "%s", "DC_ADDITIONAL_EST_PRC = ");
	fprintf(file_constants, "%d\n", DC_ADDITIONAL_EST_PRC);
	fprintf(file_constants, "%s", "DC_EARLY_RESP_REW_I0_PRC = ");
	fprintf(file_constants, "%d\n", DC_EARLY_RESP_REW_I0_PRC);
	fprintf(file_constants, "%s", "DC_EARLY_RESP_REW_I1_PRC = ");
	fprintf(file_constants, "%d\n", DC_EARLY_RESP_REW_I1_PRC);
	fprintf(file_constants, "%s", "DC_EARLY_RESP_REW_I2_PRC = ");
	fprintf(file_constants, "%d\n", DC_EARLY_RESP_REW_I2_PRC);
//	fprintf(file_constants, "%s", "DC_EARLY_RESP_REW_I3_PRC = ");
//	fprintf(file_constants, "%d\n", DC_EARLY_RESP_REW_I3_PRC);
	fprintf(file_constants, "%s", "DC_MAX_LATE_RESP_PRC = ");
	fprintf(file_constants, "%d\n", DC_MAX_LATE_RESP_PRC);
/*	fprintf(file_constants, "%s", "DC_NO_REWARD_AREA_MIN_X = ");
	fprintf(file_constants, "%d\n", DC_NO_REWARD_AREA_MIN_X);
	fprintf(file_constants, "%s", "DC_NO_REWARD_AREA_MAX_X = ");
	fprintf(file_constants, "%d\n", DC_NO_REWARD_AREA_MAX_X);
	fprintf(file_constants, "%s", "DC_NO_REWARD_AREA_MIN_Y = ");
	fprintf(file_constants, "%d\n", DC_NO_REWARD_AREA_MIN_Y);
	fprintf(file_constants, "%s", "DC_NO_REWARD_AREA_MAX_Y = ");
	fprintf(file_constants, "%d\n", DC_NO_REWARD_AREA_MAX_Y);
	fprintf(file_constants, "%s", "DC_REW_AREA_NORTH_MAX_Y = ");
	fprintf(file_constants, "%d\n", DC_REW_AREA_NORTH_MAX_Y);
	fprintf(file_constants, "%s", "DC_REW_AREA_SOUTH_MIN_Y = ");
	fprintf(file_constants, "%d\n", DC_REW_AREA_SOUTH_MIN_Y);
	fprintf(file_constants, "%s", "DC_REW_AREA_COLUMN_MIN_X = ");
	fprintf(file_constants, "%d\n", DC_REW_AREA_COLUMN_MIN_X);
	fprintf(file_constants, "%s", "DC_REW_AREA_COLUMN_MAX_X = ");
	fprintf(file_constants, "%d\n", DC_REW_AREA_COLUMN_MAX_X);*/

	/*fprintf(file_constants, "%s", "NO_MATING_AREA_MIN_X = ");
	fprintf(file_constants, "%d\n", NO_MATING_AREA_MIN_X);
	fprintf(file_constants, "%s", "NO_MATING_AREA_MAX_X = ");
	fprintf(file_constants, "%d\n", NO_MATING_AREA_MAX_X);
	fprintf(file_constants, "%s", "NO_MATING_AREA_MIN_Y = ");
	fprintf(file_constants, "%d\n", NO_MATING_AREA_MIN_Y);
	fprintf(file_constants, "%s", "NO_MATING_AREA_MIN_Y = ");
	fprintf(file_constants, "%d\n", NO_MATING_AREA_MAX_Y);*/
	fprintf(file_constants, "%s", "STAT_SAMPLE_RATE = ");
	fprintf(file_constants, "%d\n", STAT_SAMPLE_RATE);
	fprintf(file_constants, "%s", "SESS_BETW_AGENT_SAMPLE = ");
	fprintf(file_constants, "%d\n", SESS_BETW_AGENT_SAMPLE);
	fprintf(file_constants, "%s", "MAX_STATE_BUYERS_INP = ");
	fprintf(file_constants, "%d\n", MAX_STATE_BUYERS_INP);
	fprintf(file_constants, "%s", "MAX_STATE_BUYERS_COL = ");
	fprintf(file_constants, "%d\n", MAX_STATE_BUYERS_COL);

	//fprintf(file_constants, "%s", "max_fitn_rew_sess = "); //'pseudo konstant'
	//fprintf(file_constants, "%d\n", max_fitn_rew_sess);
	fclose(file_constants);
}

CList<Observable*, Observable*>* StatisticsHandler::pickSample(CList<Observable*, Observable*>* fullList, int sampleSize) {
	if (sampleSize > fullList->GetCount()) {
		cout << "ERROR: pickSample..." << endl;
		pressSpaceToQuit();
	}
	CList<Observable*, Observable*>* sample = new CList<Observable*, Observable*>(10);
	CUIntArray* randIndexList = new CUIntArray();
	randIndexList->SetSize(sampleSize);
	int bigListSize = fullList->GetCount();
	int loopCheck = 0;
	int i;
	bool isTaken;
	int numFilled = 0;
	unsigned int val;
	unsigned int indexInBigList;
	unsigned int emptyPos = 99999;
	for (i=0; i<sampleSize; i++) {
		randIndexList->SetAt(i,emptyPos);
	}
	//fill randIndexList with sampleSize random indices
	while (numFilled<sampleSize) {
		indexInBigList = (rand() * (bigListSize-1)) / RAND_MAX;
		//check if index is already taken
		isTaken = false;
		for (i=0; i<numFilled; i++) {
			val = randIndexList->GetAt(i);
			if (val == indexInBigList) {
				isTaken = true;
				i=sampleSize;
			}
		}
		if (!isTaken) {
			randIndexList->SetAt(numFilled, indexInBigList);
			numFilled++;
		}
		loopCheck++;
		if (loopCheck > 20000) {
			cout << "ERROR: picksamle loopcheck" << endl;
			pressSpaceToQuit();
		}
	}
	//randIndexList is now filled with random indices in big list
	int bigIndex;
	//cout << "big list size: " << bigListSize << endl;
	for (i=0; i<sampleSize; i++) {
		bigIndex = randIndexList->GetAt(i);
		//cout << "now adding index " << bigIndex << endl;
		//pressSpaceOrQuit();
		sample->AddTail( fullList->GetAt( fullList->FindIndex( bigIndex ) ) );
	}
	delete fullList;
	return sample;
}

void StatisticsHandler::saveDNAinfo() {
	//remember collectors!
	//save inpoder data:
	CList<Observable*, Observable*>* allCollectors = agentGrid->getCollectors( agentGrid->getCellAt( agentGrid->getWidthX()/2, agentGrid->getHightY()/2), agentGrid->getWidthX()/2 + 1, true);
	CList<Observable*, Observable*>* sampleCollectors = pickSample(allCollectors, (allCollectors->GetCount() * SAMPLE_PERCENT_AGENTS)/100);
	if (sampleCollectors->GetCount() > 0)
		saveDNAinfo_coll(sampleCollectors);
	else
		delete sampleCollectors;
	
	//save inpoder data:
	CList<Observable*, Observable*>* allInpoders = agentGrid->getInpoders( agentGrid->getCellAt( agentGrid->getWidthX()/2, agentGrid->getHightY()/2), agentGrid->getWidthX()/2 + 1, true);
	CList<Observable*, Observable*>* sampleInpoders = pickSample(allInpoders, (allInpoders->GetCount() * SAMPLE_PERCENT_AGENTS)/100);
	saveDNAinfo_inp(sampleInpoders);
	
	//save effector data:
	CList<Observable*, Observable*>* allEffectors = agentGrid->getEffectors( agentGrid->getCellAt( agentGrid->getWidthX()/2, agentGrid->getHightY()/2), agentGrid->getWidthX()/2 + 1, true);
	CList<Observable*, Observable*>* sampleEffectors = pickSample(allEffectors, (allEffectors->GetCount() * SAMPLE_PERCENT_AGENTS)/100);
	saveDNAinfo_eff(sampleEffectors);
	dnaSampleNum++;
}

void StatisticsHandler::saveDNAinfo_common(FILE* f, Observable* currAgent) {
	char tab = '	';
	Agent* agent = (Agent*)currAgent;
	//dna_obs | dna_ag | dna_inp | fitness | appMat | appIdealMat | bidPof | maxAge | age | readyToMate | numOfOffspring
	fprintf(f, "%d%c", agent->getAppearMat()->getValue(), tab );
	fprintf(f, "%d%c", agent->getAppearBusSmall()->getValue(), tab );
	fprintf(f, "%d%c", agent->getAgentDNA()->getValue(), tab );
	fprintf(f, "%d%c", agent->getTypeDNA()->getValue(), tab );
	fprintf(f, "%d%c", agent->getFitness(), tab );
	fprintf(f, "%d%c", agent->getAppIdealMat()->getValue(), tab );
	//fprintf(f, "%d%c", agent->getAppPickiMat()->getValue(), tab );
	fprintf(f, "%d%c", agent->getBidPoF(), tab );
	fprintf(f, "%d%c", agent->getMaxLifeTime(), tab);
	fprintf(f, "%d%c", agent->getAge(), tab );
	fprintf(f, "%d%c", (int)agent->isReadyToMate(), tab );
	fprintf(f, "%d%c", agent->getNumOfOffspring(), tab );
}
void StatisticsHandler::saveDNAinfo_coll(CList<Observable*, Observable*>* sample) {
	Observable* currAgent;
	Collector* colPointer;
	char header[] = "%apMat	apBusSm	dna_a	dna_i	fitn	apIdM	bidP	maxAge	age	rtm	#off	apIdB	apPiB";
	char tab = '	';
	FILE* file_col = fopen(fname_begin + FN_DNACOLL + "_"  + numToString(dnaSampleNum) + FN_EXT, "w" );
	fprintf(file_col, "%s\n", header);
	for (int i=0; i<sample->GetCount(); i++) {
		currAgent = sample->GetAt( sample->FindIndex(i) );
		colPointer = (Collector*)currAgent;
		saveDNAinfo_common(file_col, currAgent);
		fprintf(file_col, "%d%c", colPointer->getAppBusIdeal()->getValue(), tab );
		fprintf(file_col, "%d%c", colPointer->getAppBusPicki()->getValue(), tab );
		fprintf(file_col, "%d%c", (int)colPointer->getObservesForProcessed() , tab );
		fprintf(file_col, "%d%c", colPointer->getCollThr() , tab );
		fprintf(file_col, "%d\n", colPointer->getCollMax()  );
	}
	fclose(file_col);
	delete sample;
}

void StatisticsHandler::saveDNAinfo_inp(CList<Observable*, Observable*>* sample) {
	// (common) | appIdealBus | appPickiBus | smallFood | food
	Observable* currAgent;
	Inpoder* inpPointer;
	char header[] = "%apMat	apBusSm	dna_a	dna_i	fitn	apIdM	bidP	maxAge	age	rtm	#off	apIdB	apPiB	smFo	food";
	char tab = '	';
	FILE* file_inp = fopen(fname_begin + FN_DNAINP + "_"  + numToString(dnaSampleNum) + FN_EXT, "w" );
	fprintf(file_inp, "%s\n", header);
	for (int i=0; i<sample->GetCount(); i++) {
		currAgent = sample->GetAt( sample->FindIndex(i) );
		inpPointer = (Inpoder*)currAgent;
		saveDNAinfo_common(file_inp, currAgent);
		fprintf(file_inp, "%d%c", inpPointer->getAppBusIdeal()->getValue(), tab );
		fprintf(file_inp, "%d%c", inpPointer->getAppBusPicki()->getValue(), tab );
		fprintf(file_inp, "%d%c", (int)inpPointer->getSmallFood(), tab );
		fprintf(file_inp, "%d\n", inpPointer->getFood()->getValue() );
	}
	fclose(file_inp);
	delete sample;
}

void StatisticsHandler::saveDNAinfo_eff(CList<Observable*, Observable*>* sample) {
	// (common) | appIdealBus1 | appPickiBus1 | appIdealBus2 | appPickiBus2 | typeOfProc | numOfObs1 | numOfObs2
	Observable* currAgent;
	Effector* effPointer;
	char header[] = "%apMat	aoBusSm	dna_a	dna_i	fitn	apIdM	bidP	maxAge	age	rtm	#off	apIdB1	apPiB1	apIdB2	apPiB2	ToP	#Obs1	#Obs2";
	char tab = '	';
	FILE* file_eff = fopen(fname_begin + FN_DNAEFF + "_"  + numToString(dnaSampleNum) + FN_EXT, "w" );
	fprintf(file_eff, "%s\n", header);
	for (int i=0; i<sample->GetCount(); i++) {
		currAgent = sample->GetAt( sample->FindIndex(i) );
		effPointer = (Effector*)currAgent;
		saveDNAinfo_common(file_eff, currAgent);
		fprintf(file_eff, "%d%c", effPointer->getAppIdealBus1()->getValue(), tab);
		fprintf(file_eff, "%d%c", effPointer->getAppPickiBus1()->getValue(), tab);
		fprintf(file_eff, "%d%c", effPointer->getAppIdealBus2()->getValue(), tab);
		fprintf(file_eff, "%d%c", effPointer->getAppPickiBus2()->getValue(), tab);
		fprintf(file_eff, "%d%c", effPointer->getProcType(), tab);
		fprintf(file_eff, "%d%c", effPointer->getNumOfObsAgents1(), tab);
		fprintf(file_eff, "%d\n", effPointer->getNumOfObsAgents2());
	}
	fclose(file_eff);
	delete sample;
}

int StatisticsHandler::getLastCorrectRespOnLevel(int row) {
	int numOfEff = lastSesCorrectResp->getRowSum(row);
	return numOfEff;
}

int StatisticsHandler::getLastCorrectRespOnLevel(bool cat, int row) {
	int numOfEff=0;
	if (cat == 0) 
		numOfEff = lastCorrectRespCatZero->getRowSum(row);
	else 
		numOfEff = lastCorrectRespCatOne->getRowSum(row);
	return numOfEff;
}

int StatisticsHandler::getLastWrongRespOnLevel(int row) {
	int numOfEff = lastSesWrongResp->getRowSum(row);
	return numOfEff;
}

int StatisticsHandler::getLastRespOnLevel(int row) {
	int numOfEff = lastSesNumOfProcEff->getRowSum(row);
	return numOfEff;
}

int StatisticsHandler::getLastCorrectRespTotal() {
	int numOfEff = lastSesCorrectResp->getTotalSum();
	return numOfEff;
}

int StatisticsHandler::getLastCorrectRespTotal(bool cat) {
	int numOfEff=-1;
	if (cat==0) {
		//cout << "now reading cat 0 lastresp" << endl;
		numOfEff = lastCorrectRespCatZero->getTotalSum();
	} else {
		//cout << "now reading cat 1 lastresp" << endl;
		numOfEff = lastCorrectRespCatOne->getTotalSum();
	}
	//int numOfEff = lastSesCorrectResp->getTotalSum();
	return numOfEff;
}

/*int StatisticsHandler::getLastEarlyCorrectResp(bool cat) {
	int numOfEff = -1;
	if (cat==0) 
		numOfEff = lastCorrectRespCatZero->getColumnSum(0);
	else
		numOfEff = lastCorrectRespCatOne->getColumnSum(0);
	return numOfEff;
}*/

int StatisticsHandler::getLastCorrectRespAtCollIndex(bool cat, int collIndex) {
	int numOfEff = -1;
	if (cat==0) 
		numOfEff = lastCorrectRespCatZero->getColumnSum(collIndex);
	else
		numOfEff = lastCorrectRespCatOne->getColumnSum(collIndex);
	return numOfEff;
}

int StatisticsHandler::getThisCorrectRespTotal() {
	int numOfEff = thisSesCorrectResp->getTotalSum();
	return numOfEff;
}

/*int StatisticsHandler::getMaxFitnRewSess() {
	return max_fitn_rew_sess;
}*/

void StatisticsHandler::resetSessionData( bool newLoadedCat) {
	if (newLoadedCat==0)
		numOfCatZero++;
	else
		numOfCatOne++;
	lastSesCorrectCategory = thisSesCorrectCategory;
	thisSesCorrectCategory = newLoadedCat;

	if (lastSesCorrectCategory == 0) {
		delete lastCorrectRespCatZero;
		//cout << "DEBUG: lastCZero are set to thisCorrResp" << endl;
		//pressSpaceOrQuit();
		lastCorrectRespCatZero = thisSesCorrectResp->clone();
	} else {
		delete lastCorrectRespCatOne;
		//cout << "DEBUG: lastCOne are set to thisCorrResp" << endl;
		//pressSpaceOrQuit();
		lastCorrectRespCatOne = thisSesCorrectResp->clone();
	}
	delete lastSesNumOfProcEff;
	lastSesNumOfProcEff = thisSesNumOfProcEff->clone();
	thisSesNumOfProcEff->clear();
	delete lastSesFitnessPaid;
	lastSesFitnessPaid = thisSesFitnessPaid->clone();
	thisSesFitnessPaid->clear();
	delete lastSesCorrectResp;
	lastSesCorrectResp = thisSesCorrectResp->clone();
	thisSesCorrectResp->clear();
	delete lastSesWrongResp;
	lastSesWrongResp = thisSesWrongResp->clone();
	thisSesWrongResp->clear();
	delete lastSesNumOfCorr_EstForNext;
	lastSesNumOfCorr_EstForNext = thisSesNumOfCorr_EstForNext->clone(); //delete?
	thisSesNumOfCorr_EstForNext->clear();

	lastSesBitsProduced = thisSesBitsProduced;
	lastSesBitsProcessed = thisSesBitsProcessed;
	lastSesBitsFromEnv = thisSesBitsFromEnv;
	lastSesBitsEaten = thisSesBitsEaten;
	lastSesBitsDiscP = thisSesBitsDiscP;
	lastSesBitsDiscU = thisSesBitsDiscU;
	lastSesFromEnvToBuff = thisBlockSessionReward;
	
	thisSesBitsProduced = 0;
	thisSesBitsProcessed = 0;
	thisSesBitsEaten = 0;
	thisSesBitsDiscU = 0;
	thisSesBitsDiscP = 0;
	thisSesBitsFromEnv = 0;

	lastSesCollineAnswer = getCollineAnswer();
	if (lastSesCorrectCategory==0)//calc cat0 performance
		performance_cat0 = FORGET_FACTOR_PERF*performance_cat0 + (lastSesCollineAnswer == lastSesCorrectCategory);
	else //calc cat1 performance
		performance_cat1 = FORGET_FACTOR_PERF*performance_cat1 + (lastSesCollineAnswer == lastSesCorrectCategory);

	lastSesFitnBalance_end = fitnessColline - fitnessEnvironment /*-matingBuffer*/; //no need to incl buffers:they should be approx zero at this point
	
}

bool StatisticsHandler::getCollineAnswer() {
	int good=0;
	int bad=0;
	bool collineAnswer;
	good =  lastSesCorrectResp->getTotalSum();
	bad = lastSesWrongResp->getTotalSum();
	if (good>bad)
		collineAnswer = lastSesCorrectCategory;
	else
		collineAnswer = !lastSesCorrectCategory;
	return collineAnswer;
}


void StatisticsHandler::updateSessionReward() {
	int medianFitness = (fitnessColline - fitnessEnvironment)/2 + fitnessEnvironment; //if env_fitness is very low, more than env_fitness is transferred back to colline (vice versa)
	thisBlockSessionReward = medianFitness / SESS_BETW_REWARD_UPDATE;
}

/*void StatisticsHandler::setThisSesCorrectCategory(bool cat) {
	if (cat==0)
		numOfCatZero++;
	else
		numOfCatOne++;
	//cout << "debug: num of zero cat: " << numOfCatZero	<< endl;
	//cout << "debug: num of one cat : " << numOfCatOne	<< endl;
	lastSesCorrectCategory = thisSesCorrectCategory;
	thisSesCorrectCategory = cat;
}*/

void StatisticsHandler::setThisSesChosenPickiLevel(int chosenLevel) {
	lastSesChosenPickiLevel = thisSesChosenPickiLevel;
	thisSesChosenPickiLevel = chosenLevel;
}

void StatisticsHandler::setThisSesFitnessBuffer(bool bufferType, int buffer) {
	if (bufferType==0) {
		lastSesFitnessBuffer0_cp = thisSesFitnessBuffer0_cp;
		thisSesFitnessBuffer0_cp = buffer;
		//lastSesFitnessBuffer1_cp = thisSesFitnessBuffer1_cp; //1 stay the same
	} else {
		lastSesFitnessBuffer1_cp = thisSesFitnessBuffer1_cp;
		thisSesFitnessBuffer1_cp = buffer;
		//lastSesFitnessBuffer0_cp = thisSesFitnessBuffer0_cp; //0 stay the same
	}
	//lastSesFitnessBuffer = thisSesFitnessBuffer;
	//thisSesFitnessBuffer = buffer;
}

void StatisticsHandler::setThisSesMatingBonusBuffer(int value) {
	lastSesMatingBonusBuffer_cp = thisSesMatingBonusBuffer_cp;
	thisSesMatingBonusBuffer_cp = value;
}


void StatisticsHandler::setThisSesRewardOnLevels(UIntGrid* values) {
	delete lastSesRewardOnLevels;
	lastSesRewardOnLevels = thisSesRewardOnLevels;
	thisSesRewardOnLevels = values;
}

bool StatisticsHandler::getSessionCategory() {
	return thisSesCorrectCategory;
}

void StatisticsHandler::setSessionCategory(bool cat) {
	thisSesCorrectCategory =  cat;
}

int StatisticsHandler::getSessionReward() {
	return thisBlockSessionReward;
}

/*
void StatisticsHandler::setLastSesCorrectResp(UIntGrid* lastSesValues) {
	delete lastSesCorrectResp;
	lastSesCorrectResp = lastSesValues;
}

void StatisticsHandler::setLastSesFitnessPaid(UIntGrid* lastSesValues) {
	delete lastSesFitnessPaid;
	lastSesFitnessPaid = lastSesValues;
}

void StatisticsHandler::setLastSesNumOfProcEff(UIntGrid* lastSesValues) {
	delete lastSesNumOfProcEff;
	lastSesNumOfProcEff = lastSesValues;
}

void StatisticsHandler::setLastSesWrongResp(UIntGrid* lastSesValues) {
	delete lastSesWrongResp;
	lastSesWrongResp = lastSesValues;
}
*/

void StatisticsHandler::accumulateBorn() {
	accBorn++;
}

void StatisticsHandler::accumulateDead() {
	accDead++;
}

void StatisticsHandler::addToNumOfDiscUBits(int numOfBits) {
	thisSesBitsDiscU += numOfBits;
}

void StatisticsHandler::addToNumOfDiscPBits(int numOfBits) {
	thisSesBitsDiscP += numOfBits;
}

void StatisticsHandler::addToNumOfProducedBits(int numOfBits) {
	thisSesBitsProduced += numOfBits;
}

void StatisticsHandler::addToNumOfProcessedBits(int numOfBits) {
	thisSesBitsProcessed += numOfBits;
}

void StatisticsHandler::addToNumOfEatenBits(int numOfBits) {
	thisSesBitsEaten += numOfBits;
}

void StatisticsHandler::addToNumBitsFromEnv(int numOfBits) {
	thisSesBitsFromEnv += numOfBits;
}

/*void StatisticsHandler::addToNumOfProcEffectors_total() {
	testNumOfProcEffectors++;
}*/

void StatisticsHandler::adjustAgents(int value, int type) {
	if ((value !=-1) && (value != 1)) {
		cout << "Error: adjust agents" << endl;
		pressSpaceToQuit();
	}
	numOfAgents += value;
	if (type == TYPE_COLLECTOR)
		numOfAgents_coll += value;
	else if (type == TYPE_INPODER)
		numOfAgents_inp += value;
	else if (type == TYPE_EFFECTOR)
		numOfAgents_eff += value;
	else if (type == TYPE_FOUNTAIN)
		numOfAgents_foun += value;
	else{
		cout << "ERROR: adjustagents"<<endl;
		pressSpaceToQuit();
	}
}

void StatisticsHandler::adjustObservers(int value, int type) {
	if ((value !=-1) && (value != 1)) {
		cout << "Error: adjust observers" << endl;
		pressSpaceToQuit();
	}
	numOfObservers += value;
	if (type == TYPE_FOUNTAIN)
		numOfObs_foun += value;
	else if (type == TYPE_COLLECTOR)
		numOfObs_coll += value;
	else if (type == TYPE_INPODER)
		numOfObs_inp += value;
	else if (type == TYPE_EFFECTOR)
		numOfObs_eff += value;
	else {
		cout << "ERROR: adjustagents"<<endl;
		pressSpaceToQuit();
	}
}

void StatisticsHandler::adjustProcessors(int value, int type) {
	if ((value !=-1) && (value != 1)) {
		cout << "Error: adjust processors" << endl;
		pressSpaceToQuit();
	}
	numOfProcessors += value;
	if (type == TYPE_FOUNTAIN)
		numOfProc_foun += value;	
	else if (type == TYPE_COLLECTOR)
		numOfProc_coll += value;
	else if (type == TYPE_INPODER)
		numOfProc_inp += value;
	else if (type == TYPE_EFFECTOR)
		numOfProc_eff += value;
	else {
		cout << "ERROR: adjustagents"<<endl;
		pressSpaceToQuit();
	}
}

void StatisticsHandler::adjustDrifters(int value, int type) {
	if ((value !=-1) && (value != 1)) {
		cout << "Error: adjust drifters" << endl;
		pressSpaceToQuit();
	}
	numOfDrifters += value;
	if (type == TYPE_FOUNTAIN)
		numOfDrif_foun += value;
	else if (type == TYPE_COLLECTOR)
		numOfDrif_coll += value;
	else if (type == TYPE_INPODER)
		numOfDrif_inp += value;
	else if (type == TYPE_EFFECTOR)
		numOfDrif_eff += value;
	else {
		cout << "ERROR: adjustagents"<<endl;
		pressSpaceToQuit();
	}
}

void StatisticsHandler::adjustMaters(int value, int type) {
	if ((value !=-1) && (value != 1)) {
		cout << "Error: adjust maters" << endl;
		pressSpaceToQuit();
	}
	numOfMaters += value;
	if (type == TYPE_COLLECTOR)
		numOfMaters_coll += value;
	else if (type == TYPE_INPODER)
		numOfMaters_inp += value;
	else if (type == TYPE_EFFECTOR)
		numOfMaters_eff += value;
	else {
		cout << "ERROR: adjustagents"<<endl;
		pressSpaceToQuit();
	}
}

void StatisticsHandler::adjustFitnessEnvironment(int change) {
	fitnessEnvironment += change;
}

/*void StatisticsHandler::adjustFitnessBuffer(bool bufferType, int change) {
	if (bufferType==0)
		fitnessBuffer0_cp += change;
	else
		fitnessBuffer1_cp += change;
}*/
int StatisticsHandler::getFitnessColline() {
	return fitnessColline;
}

void StatisticsHandler::adjustFitnessColline(int change, int type) {
	fitnessColline += change;
	if (type == TYPE_COLLECTOR)
		fitnessColline_coll += change;
	else if (type == TYPE_INPODER)
		fitnessColline_inp += change;
	else if (type == TYPE_EFFECTOR)
		fitnessColline_eff += change;
	else if (type == TYPE_FOUNTAIN) {
		fitnessColline -= change; //fitness goes to environment
	} else {
		cout << "ERROR: adjustagents" << endl;
		pressSpaceToQuit();
	}
}

void StatisticsHandler::pressSpaceOrQuit() {
	int ch;
	cout << "press space or 'q' to quit..." << endl;
	while ((ch != ' ') && (ch != 'q')) {
		ch = _getch();
	}
	if (ch == 'q')
		exit(0);
}

void StatisticsHandler::pressSpaceToQuit() {
	int ch;
	cout << "press space..." << endl;
	while (ch != ' ') {
		ch = _getch();
	}
	exit(0);
}

CString StatisticsHandler::numToString(int number) {
	char str[5];
	_itoa(number, str, 10);
	CString info = str;
	return info;
}

void StatisticsHandler::setThisSesEstimatedOkResp(int val, int level) {
	//cout << "setting num of resp est for next to " << val << " on level " << level << endl;
	thisSesNumOfCorr_EstForNext->setValue(val, level, 0);
}

int StatisticsHandler::getThisFitnessPaidTotal() {
	int fitnPaid = thisSesFitnessPaid->getTotalSum();
	return fitnPaid;
}

int StatisticsHandler::getAverage(int fValue, int agents) {
	int avg;
	if (agents == 0)
		avg = 0;
	else
		avg =  fValue/agents;
	return avg;
}

int StatisticsHandler::getNumOfAgents() {
	return numOfAgents;
}

void StatisticsHandler::addToNumOfProcEffectors(int bidAgrLevel, int cIndex) {
	if (cIndex < 0 || bidAgrLevel<0) {
		cout << "ERROR 103: cIndex negative: " << cIndex << " , (bidAgrLev="<< bidAgrLevel << ")" << endl;
		pressSpaceToQuit();
	}
	thisSesNumOfProcEff->addToValue(1,bidAgrLevel,cIndex);
}

void StatisticsHandler::addToCorrectResp(int bidAgrLevel, int cIndex) {
	if (cIndex < 0 || bidAgrLevel<0) {
		cout << "ERROR 742: cIndex negative: " << cIndex << " , (bidAgrLev="<< bidAgrLevel << ")" << endl;
		pressSpaceToQuit();
	}
	thisSesCorrectResp->addToValue(1, bidAgrLevel,cIndex);
}

void StatisticsHandler::addToWrongResp(int bidAgrLevel, int cIndex) {
	if (cIndex < 0 || bidAgrLevel<0) {
		cout << "ERROR 174: cIndex negative: " << cIndex << " , (bidAgrLev="<< bidAgrLevel << ")" << endl;
		pressSpaceToQuit();
	}
	thisSesWrongResp->addToValue(1,bidAgrLevel,cIndex);
}

void StatisticsHandler::addToFitnessPaid(int value, int bidAgrLevel, int cIndex) {
	if (cIndex < 0 || bidAgrLevel<0) {
		cout << "ERROR 017: cIndex negative: " << cIndex << " , (bidAgrLev="<< bidAgrLevel << ", value="<<value<<")" << endl;
		pressSpaceToQuit();
	}
	thisSesFitnessPaid->addToValue(value,bidAgrLevel,cIndex);
}

int StatisticsHandler::getPerformance(bool category) {
	if (category==0)
		return ((int)performance_cat0);
	else
		return ((int)performance_cat1);
	return 0;
}

/*void StatisticsHandler::updateTrainingView(int sNum) {
	//DecisionCategory* decCat= (DecisionCategory*)decCategory;
	CStringArray* info = new CStringArray();
	int floor_perf0 = floor(performance_cat0);
	int floor_perf1 = floor(performance_cat1);
	info->Add( numToString(sNum) );
	info->Add( numToString( lastSesCorrectCategory ));
	info->Add( numToString( lastSesCollineAnswer ));
	info->Add( numToString( lastSesChosenPickiLevel ));
	info->Add( numToString( lastSesBitsFromEnv));
	info->Add( numToString( lastSesBitsEaten));
	info->Add( numToString( lastSesBitsDiscP));
	info->Add( numToString( lastSesFitnBalance_end ));
	info->Add( numToString( lastSesFromEnvToBuff ));
	info->Add( numToString( lastSesFitnessBuffer0_cp ));
	info->Add( numToString( lastSesFitnessBuffer1_cp ));
	info->Add( numToString( lastSesMatingBonusBuffer_cp ));
	info->Add( numToString( floor_perf0 ));
	info->Add( numToString( floor_perf1 ));
	trainingView->updateInfoTraining(info, lastSesNumOfProcEff, lastSesFitnessPaid, lastSesCorrectResp, lastSesWrongResp, lastSesNumOfCorr_EstForNext, lastSesRewardOnLevels);
	delete info;
}*/

/*void StatisticsHandler::updateAgentView(int cNum) {
	CStringArray* info = new CStringArray();
	info->Add( numToString(cNum) );
	//Table begin
	info->Add( numToString(numOfProc_foun) );
	info->Add( numToString(numOfObs_foun) );
	info->Add( numToString(numOfDrif_foun) );
	info->Add( numToString( 0 ) );
	info->Add( numToString(numOfAgents_foun) );

	info->Add( numToString(numOfProc_coll) );
	info->Add( numToString(numOfObs_coll) );
	info->Add( numToString(numOfDrif_coll - numOfMaters_coll) );
	info->Add( numToString(numOfMaters_coll) );
	info->Add( numToString(numOfAgents_coll) );

	info->Add( numToString(numOfProc_inp) );
	info->Add( numToString(numOfObs_inp) );
	info->Add( numToString(numOfDrif_inp - numOfMaters_inp) );
	info->Add( numToString( numOfMaters_inp ) );
	info->Add( numToString(numOfAgents_inp) );

	info->Add( numToString(numOfProc_eff) );
	info->Add( numToString(numOfObs_eff) );
	info->Add( numToString(numOfDrif_eff - numOfMaters_eff) );
	info->Add( numToString( numOfMaters_eff ) );
	info->Add( numToString(numOfAgents_eff) );

	info->Add( numToString( numOfProcessors ) );// numOfProc_foun + numOfProc_coll + numOfProc_inp + numOfProc_eff) );
	info->Add( numToString(numOfObservers) );
	info->Add( numToString( numOfDrifters - numOfMaters)); // numOfDrif_coll - numOfMaters_coll) );
	info->Add( numToString( numOfMaters ) );
	info->Add( numToString( numOfAgents) );
	//table end
	info->Add( numToString(fitnessColline_coll/1000) );
	info->Add( numToString(fitnessColline_inp/1000) );
	info->Add( numToString(fitnessColline_eff/1000) );
	info->Add( numToString(fitnessColline/1000) );
	
	info->Add( numToString( getAverage(fitnessColline_coll, numOfAgents_coll) ) );
	info->Add( numToString( getAverage(fitnessColline_inp, numOfAgents_inp) ) );
	info->Add( numToString( getAverage(fitnessColline_eff, numOfAgents_eff)) );
	info->Add( numToString( getAverage(fitnessColline, (numOfAgents-numOfAgents_foun)) ) );
	
	agentView->updateInfoAgents(info);
	delete info;
}*/

void StatisticsHandler::changeGridView(Observable* changingAgent, GridCell* oldPos) {
	char status = changingAgent->getStatus();
	int type = changingAgent->getType();
	GridCell* currentPos;
	bool isMater;
	if (type != TYPE_FOUNTAIN) {
		Agent* cAgent = (Agent*)changingAgent;
		currentPos = cAgent->getPosition();
		isMater = cAgent->isReadyToMate();
	} else {
		currentPos = oldPos;
		isMater = false;
	}
	//find index
	int newIndex = (currentPos->getPosY())*(agentGrid->getWidthX()) + currentPos->getPosX();
	int oldIndex = (oldPos->getPosY())*(agentGrid->getWidthX()) + oldPos->getPosX();
	//find color 
	int color;
	if (status == ST_DRIFTER) {
		if (isMater)
			color = COLOR_MATE; //purple
		else
			color = COLOR_DRIF; //grey
	}
	else if (status == ST_OBSERVER) {
		if (type == TYPE_FOUNTAIN)
			color = COLOR_FOUN;
		else if (type == TYPE_COLLECTOR)
			color = COLOR_COLL; //green
		else if (type == TYPE_INPODER)
			color = COLOR_INP; //red
		else if (type == TYPE_EFFECTOR)
			color = COLOR_EFF; //blue
		else {
			cout << "ERROR:changeGridview" << endl;
			pressSpaceToQuit();
		}
	} else if (status == ST_PROCESSOR)
		color = COLOR_PROC;
	else if (status == ST_DEAD)
		color = COLOR_GRID;
	else {
		cout << "ERROR: changeGridView ..2" << endl;
		pressSpaceToQuit();
	}
	nextViewFrame->SetAt(newIndex, color);
	if (newIndex != oldIndex) {
		//clear old position
		int backColor = COLOR_GRID;
		nextViewFrame->SetAt(oldIndex, backColor);
	}
}

/*void StatisticsHandler::updateGridView() {
	gridView->paintGrid( nextViewFrame );
}*/

void StatisticsHandler::saveCycleData(int cNum) {
	if (cNum%STAT_SAMPLE_RATE == 0) {
		char space = ' ';
		fprintf( file_fitnColline, "%d%c", fitnessColline, space );
		fprintf( file_fitnColline, "%d%c", fitnessColline_coll, space );
		fprintf( file_fitnColline, "%d%c", fitnessColline_inp, space );
		fprintf( file_fitnColline, "%d\n", fitnessColline_eff );
		fprintf( file_fitnBuff, "%d%c", thisSesFitnessBuffer0_cp, space );
		fprintf( file_fitnBuff, "%d\n", thisSesFitnessBuffer1_cp );
		fprintf( file_fitnEnv, "%d\n", fitnessEnvironment );
	}
}

void StatisticsHandler::saveSessionData() {
	char space = ' ';
	fprintf(file_maters, "%d%c", numOfMaters, space );
	fprintf(file_maters, "%d%c", numOfMaters_coll, space );
	fprintf(file_maters, "%d%c", numOfMaters_inp, space );
	fprintf(file_maters, "%d\n", numOfMaters_eff );
	fprintf(file_agents, "%d%c", numOfAgents, space );
	fprintf(file_agents, "%d%c", numOfAgents_coll, space );
	fprintf(file_agents, "%d%c", numOfAgents_inp, space );
	fprintf(file_agents, "%d\n", numOfAgents_eff);
	fprintf(file_bits_eaten, "%d\n", thisSesBitsEaten );
	fprintf(file_correct_resp , "%d\n", thisSesCorrectResp->getTotalSum() );
	fprintf(file_wrong_resp, "%d\n", thisSesWrongResp->getTotalSum() );
	fprintf(file_category, "%d\n", (int)thisSesCorrectCategory);
	//paidInSession
}

void StatisticsHandler::closeFiles() {
	cout << "info: Closing files...";
	fclose( file_fitnColline);
	fclose( file_fitnBuff);
	fclose( file_fitnEnv);

	fclose( file_maters );
	fclose( file_agents );
	fclose( file_bits_eaten );
	fclose( file_correct_resp);
	fclose( file_wrong_resp);
	fclose( file_category );
}

int StatisticsHandler::calcSpeciesNum(Message* appMating, Message* appIdeal) {
	if (appMating->getSize() != 2 || appIdeal->getSize() != 2) {
		cout << "ERROR: statHandler, calcSpeciesNum, only impl for a 2 bit app" << endl;
		pressSpaceToQuit();
	}
	int specNum = -1;
	int val1 = appMating->getValue();
	int val2 = appIdeal->getValue();
	if (val1==0 && val2==0)
		specNum=0;
	else if (val1==1 && val2==1)
		specNum=1;
	else if (val1==2 && val2==2)
		specNum=2;
	else if (val1==3 && val2==3)
		specNum=3;
	else if ((val1==0 && val2==1) || (val1==1 && val2==0))
		specNum=4;
	else if ((val1==0 && val2==2) || (val1==2 && val2==0))
		specNum=5;
	else if ((val1==0 && val2==3) || (val1==3 && val2==0))
		specNum=6;
	else if ((val1==1 && val2==2) || (val1==2 && val2==1))
		specNum=7;
	else if ((val1==1 && val2==3) || (val1==3 && val2==1))
		specNum=8;
	else if ((val1==2 && val2==3) || (val1==3 && val2==2))
		specNum=9;
	if (specNum==-1) {
		cout << "ERROR; statHandler calcspec, 991" << endl;
		pressSpaceToQuit();
	}
	return specNum;
}

void StatisticsHandler::updateCurrentSpeciesCount(int type, Message* appMating, Message* appIdeal, bool isNewBorn) {
	int change;
	int specNum = calcSpeciesNum(appMating, appIdeal);
	if (isNewBorn)
		change=1;
	else
		change=-1;
	if (type==TYPE_COLLECTOR)
		currentSpeciesCount->addToValue( change, specNum, 0);
	else if (type==TYPE_INPODER)
		currentSpeciesCount->addToValue( change, specNum, 1);
	else if (type==TYPE_EFFECTOR)
		currentSpeciesCount->addToValue( change, specNum, 2);
	else {
		cout << "ERROR: sstatHan, update, 400" << endl;
	}
}


int StatisticsHandler::getEnvFitnIndex() {
	int index = 0;
	int total = fitnessEnvironment + fitnessColline;
	if (total == 0) {
		cout << "ERROR: 843" << endl;
		pressSpaceToQuit();
	} else 
		index = (fitnessEnvironment*100)/(total/2);
	return index;
}

int StatisticsHandler::getSysFitnIndex() {
	int index = 0;
	int total = fitnessEnvironment + fitnessColline;
	if (total == 0) {
		cout << "ERROR: 843" << endl;
		pressSpaceToQuit();
	} else 
		index = (fitnessColline*100)/(total/2);
	return index;
}

//not tested!
void StatisticsHandler::updateAvgAgentAge(int dyingAge) {
	/* TEMPTEST
	double oldVal = avgAgentAge;
	avgAgentAge = (1-FORGET_FACTOR_AGE)*dyingAge + FORGET_FACTOR_AGE*oldVal;
	oldVal = percThatReachMaxAge;
	percThatReachMaxAge = (dyingAge >= AGENT_LIFETIME_MAX) + oldVal*0.99;
	*/
}

void StatisticsHandler::updateAvgStateBuyers_inp(int numOfBuyers) {
	/* TEMPTEST
	double oldVal = avgStateBuyers_inp;
	avgStateBuyers_inp = (1-FORGET_FACTOR_AGE)*numOfBuyers + FORGET_FACTOR_AGE*oldVal;
	oldVal = percThatReachMaxAge;
	percAuctionsThatHitMax_inp = (numOfBuyers == MAX_STATE_BUYERS_INP) + oldVal*0.99;
	*/
}

void StatisticsHandler::updateStatAgentDies(Observable* dAgent) {
	//cout << "entering agent sies in sh" << endl;
	
	Agent* dyingAgent = (Agent*)dAgent;
	int type = dyingAgent->getType();
	int status = dyingAgent->getStatus();
	if ( status == ST_OBSERVER)
		adjustObservers(-1, type);
	else if (status == ST_DRIFTER)
		adjustDrifters(-1, type);
	else if ( status == ST_PROCESSOR)
		adjustProcessors(-1, type);
	else {
		cout << "Error: set to dead 459" << endl;
		pressSpaceToQuit();
	}
	adjustAgents(-1 , type );
	//updateCurrentSpeciesCount(type, dyingAgent->getAppearMat(), dyingAgent->getAppIdealMat(), false); //NB! make one method in statHandler which adjust all for newBorn/dead
	
	updateAvgAgentAge( dyingAgent->getAge() );
	/* TEMPTEST
	if (type==TYPE_COLLECTOR)
		speciesStat_col->addToValue(-1, _SP_POS_COUNT, dyingAgent->speciesNum);	
	else if (type==TYPE_INPODER)
		speciesStat_inp->addToValue(-1, _SP_POS_COUNT, dyingAgent->speciesNum);	
	else if (type==TYPE_EFFECTOR)
		speciesStat_eff->addToValue(-1, _SP_POS_COUNT, dyingAgent->speciesNum);	
	if (dyingAgent->isReadyToMate() )
		adjustMaters(-1, type ); //syncMan->numOfMatersSub();
	accumulateDead();
	//cout << "ag dies in sh....ok" << endl;
	//pressSpaceOrQuit();
	*/
}

void StatisticsHandler::updateStatAgentBorn(Observable* nAgent) {
	//cout << "entering agent born in sh" << endl;
	
	Agent* newBorn = (Agent*)nAgent;
	int type = newBorn->getType();
	accumulateBorn();
	if (newBorn->isReadyToMate()) {
		adjustMaters(1,type);
		cout << "ERROR? how can this be? 843" << endl;
		pressSpaceToQuit();
	}
	//updateCurrentSpeciesCount(type, newBorn->getAppearMat(), newBorn->getAppIdealMat(), true);
	/* TEMPTEST
	if (type==TYPE_COLLECTOR)
		speciesStat_col->addToValue(1, _SP_POS_COUNT, newBorn->speciesNum);	
	else if (type==TYPE_INPODER)
		speciesStat_inp->addToValue(1, _SP_POS_COUNT, newBorn->speciesNum);	
	else if (type==TYPE_EFFECTOR)
		speciesStat_eff->addToValue(1, _SP_POS_COUNT, newBorn->speciesNum);	
	*/
	adjustObservers(1,type); //because 1 is subtracted in setToDrifter
    adjustAgents(1, type);
	//cout << "agent born sh...ok" << endl;
	//if (newBorn->getId() >515)
	//	pressSpaceOrQuit();
	
}

/*void StatisticsHandler::updateAllViews(int cNum, int sNum) {
	updateGridView();
	updateAgentView( cNum );
	updateTrainingView( sNum );
}*/
