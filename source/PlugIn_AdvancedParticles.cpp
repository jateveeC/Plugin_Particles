// ************  Top/Header section ************
#include "Stdafx.h"
#include <cstring>
#include <cstdio>

#include "definitions\includes.h"


#pragma warning( error : 4706 )
#pragma warning( disable : 4996)

// ************  Early function declarations ************


// ************  Global Variables Section *************

HINSTANCE MyDllInstance=NULL;  // the instance handle of your dll

extern char BufferLog[4096]; // temporary global buffer to host error and warning messages

DWORD MyTomb4PatcherAddress = 0x6708E0;
								
char TexMyPluginName[80];  

StrMyData MyData;


// ************  Utilities section  ****************


// ************  Patcher Functions section  ***************

void Patch_00()
{
	UpdateLightning();
	Particles::UpdateParts();
}


void Patch_01()
{
	S_DrawSparks();
	Particles::DrawParts();
}


int Patch_UpdateParts(void)
{
	static DWORD VetOffset[] = { 0x4491D1 };

	return ApplyCallPatch(VetOffset, 1, (DWORD)Patch_00);
}


int Patch_DrawParts(void)
{
	static DWORD VetOffset[] = { 0x44E863 };

	return ApplyCallPatch(VetOffset, 1, (DWORD)Patch_01);
}



bool CreateMyCodePatches(void)
{
	if (SetReservedDataZone(0x6708E0, 128) != APPC_OK) return false;

	SET_PATCH(Patch_UpdateParts);
	SET_PATCH(Patch_DrawParts);

	return true;
}

// ************  Assembly Procedures section  ******************


void *SubPatchArray[] = {
// TYPE_HERE your asm procedure names to call from tomb4 code
	NULL
};


BEGIN_ASM_PROC(MainPatcher)
	and eax, 0ffffh
	mov eax, dword ptr [SubPatchArray+eax*4];
	jmp eax
END_ASM_PROC



// ************  CallBack functions section  *****************

// TYPE_HERE: your callback function

void cbInitProgram(int NumberLoadedPlugins, char *VetPluginNames[]) 
{

	// save number of plugins (enclosed yours and the tomb_nextgeneration.dll ) and their names
	// these data will be used to locate IDs of any loaded plugins using FindPluginID() function
	Trng.TotPlugins = NumberLoadedPlugins;
	Trng.pVetPluginNames = VetPluginNames;

	// clear all my global variables
	ClearMemory(&MyData, sizeof(StrMyData));
}


void cbInitGame(void)
{
	// here you can initialize your global data for whole adventure
	// this procedure will be called only once, before loading title level
	int groupIndex = 0;

	Particles::partGroups[groupIndex].attach.tether = Particles::TetherType::TETHER_NONE;
	Particles::partGroups[groupIndex].attach.cutoff = 1000;
	Particles::partGroups[groupIndex].UpdateS = FireflyUpdate;
	Particles::partGroups[groupIndex].blendingMode = 2;
	Particles::partGroups[groupIndex].spriteSlot = SLOT_DEFAULT_SPRITES;
	Particles::partGroups[groupIndex].groupIndex = groupIndex++;
	

	Particles::partGroups[groupIndex].attach.tether = Particles::TetherType::TETHER_NONE;
	Particles::partGroups[groupIndex].UpdateS = PixieUpdate;
	Particles::partGroups[groupIndex].blendingMode = 2;
	Particles::partGroups[groupIndex].spriteSlot = SLOT_DEFAULT_SPRITES;
	Particles::partGroups[groupIndex].groupIndex = groupIndex++;

	Particles::partGroups[groupIndex].attach.tether = Particles::TetherType::TETHER_NONE;
	Particles::partGroups[groupIndex].UpdateS = EmberUpdate;
	Particles::partGroups[groupIndex].blendingMode = 2;
	Particles::partGroups[groupIndex].spriteSlot = SLOT_DEFAULT_SPRITES;
	Particles::partGroups[groupIndex].groupIndex = groupIndex++;

	Particles::partGroups[groupIndex].attach.tether = Particles::TetherType::TETHER_NONE;
	Particles::partGroups[groupIndex].UpdateS = EmberUpdate;
	Particles::partGroups[groupIndex].drawMode = Particles::DrawMode::DRAW_LINE;
	Particles::partGroups[groupIndex].groupIndex = groupIndex++;

	Particles::partGroups[groupIndex].attach.tether = Particles::TetherType::TETHER_NONE;
	Particles::partGroups[groupIndex].blendingMode = 2;
	Particles::partGroups[groupIndex].spriteSlot = SLOT_DEFAULT_SPRITES;
	Particles::partGroups[groupIndex].groupIndex = groupIndex++;

	Particles::partGroups[groupIndex].attach.tether = Particles::TetherType::TETHER_NONE;
	Particles::partGroups[groupIndex].blendingMode = 5;
	Particles::partGroups[groupIndex].spriteSlot = SLOT_DEFAULT_SPRITES;
	Particles::partGroups[groupIndex].groupIndex = groupIndex++;

	Particles::partGroups[groupIndex].attach.tether = Particles::TetherType::TETHER_NONE;
	Particles::partGroups[groupIndex].drawMode = Particles::DrawMode::DRAW_LINE;
	Particles::partGroups[groupIndex].groupIndex = groupIndex++;

	Particles::partGroups[groupIndex].attach.tether = Particles::TetherType::TETHER_NONE;
	Particles::partGroups[groupIndex].UpdateM = PyroclastUpdate;
	Particles::partGroups[groupIndex].spriteSlot = SLOT_DEFAULT_SPRITES;
	Particles::partGroups[groupIndex].groupIndex = groupIndex++;

	Particles::partGroups[groupIndex].attach.tether = Particles::TetherType::TETHER_ROTATING;
	Particles::partGroups[groupIndex].UpdateS = GlitterUpdate;
	Particles::partGroups[groupIndex].blendingMode = 2;
	Particles::partGroups[groupIndex].spriteSlot = SLOT_DEFAULT_SPRITES;
	Particles::partGroups[groupIndex].groupIndex = groupIndex++;

	Particles::partGroups[groupIndex].attach.tether = Particles::TetherType::TETHER_ROTATING;
	Particles::partGroups[groupIndex].UpdateS = PollenUpdate;
	//Particles::partGroups[groupIndex].drawMode = Particles::DrawMode::DRAW_SPRITE;
	Particles::partGroups[groupIndex].blendingMode = 2;
	Particles::partGroups[groupIndex].spriteSlot = SLOT_DEFAULT_SPRITES;
	Particles::partGroups[groupIndex].attach.cutoff = 1000;
	Particles::partGroups[groupIndex].groupIndex = groupIndex++;

	Particles::nextPartGroup = groupIndex;
}


void cbInitLevel(int LevelNow, int LevelOld, DWORD FIL_Flags)
{
	// here you can initialize specific items of currnet level.
	// it will be called only once for level, when all items has been already initialized
	// and just a moment before entering in main game cycle.

	Particles::ClearParts();

}

// called everytime player save the game (but also when lara move from a level to another HUB saving). 
// in this procedure your plugin will save its own data in savegame
// SavingType inform about the kind of saving it is happening (SAVT_... values)
// pAdrZone is a byte pointer where this procedure will save the address from where begin data to save in savegame
// this function will return the size of data to save
// note: It's better you don't change anything of this code. It will save byself of data you stored in MyData.Save
// structure. If you wish adding other variables to save, just you add new fields in Structures of MyData.Save 
DWORD cbSaveMyData(BYTE **pAdrZone, int SavingType)
{
	DWORD SizeData;
	int i;
	static WORD *pVetExtras;
	int TotNWords;
	int TotNewActions;


	if (SavingType & SAVT_COMPLETED) {
		// this call is not to save data but only it is a confirm that the previous saving has been completed
		// now we can free the temporary memory used to save the data in previous call
		if (pVetExtras != NULL) {
			FreeMemory(pVetExtras);
			pVetExtras=NULL;
		}

		return 0;
	}


	TotNWords=0;
	pVetExtras = (WORD *) GetMemory(16);
	// save id of my plugin in first word

	pVetExtras[TotNWords++] = Trng.IdMyPlugin;

	if (SavingType & SAVT_LOCAL_DATA) {
		// save local data

		// save Local structure
		AddNGToken(NGTAG_LOCAL_DATA, NO_ARRAY, sizeof(StrSavegameLocalData), &MyData.Save.Local, 
						&pVetExtras, &TotNWords);


		// save all (currently enabled) progressive actions
		// before saving, compact progressive action array to remove intermediate free records
		TotNewActions=0;

		for (i=0;i<MyData.TotProgrActions;i++) {
			if (MyData.VetProgrActions[i].ActionType != AXN_FREE) {

				MyData.VetProgrActions[TotNewActions] = MyData.VetProgrActions[i];

				TotNewActions++;
			}
		}
		// update new valuese after recompatting
		MyData.LastProgrActionIndex =0;
		MyData.TotProgrActions= TotNewActions;

		// store all progressive action records
		AddNGToken(NGTAG_PROGRESSIVE_ACTIONS, MyData.TotProgrActions, sizeof(StrProgressiveAction), 
				&MyData.VetProgrActions[0], &pVetExtras, &TotNWords);



	}

	if (SavingType & SAVT_GLOBAL_DATA) {
		// save global data
		AddNGToken(NGTAG_GLOBAL_DATA, NO_ARRAY, sizeof(StrSavegameGlobalData), &MyData.Save.Global , 
						&pVetExtras, &TotNWords);
	}
	// write final sequence
	AddTokenFinalSequence(&pVetExtras, &TotNWords);

	// return to trng the infos about start of memory where there are our data and their size:
	*pAdrZone = (BYTE *) pVetExtras;
	SizeData = TotNWords * 2;

	return SizeData;
	

}
// called when a savegame will be loaded (but also when lara move from a level to another)
// pAdrZone will point to memory zone with data just loaded from savegame
// SizeData is the size of data pointed by pAdrZone
// note: it's better you don't change anything of this code. It's alread thought to reload all data you saved in MyData.Save 
// structure. There is no need of changes by you
void cbLoadMyData(BYTE *pAdrZone, DWORD SizeData)
{

	WORD *pVetExtras;
	StrParseNGField  ParseField;
	int Indice;
	int i;
	WORD TotActions;

	
	pVetExtras = (WORD*) pAdrZone;

	Indice=0;

	while (ParseNgField(pVetExtras ,Indice, &ParseField)==true) {
		
		// recover different ng token
		switch (ParseField.Type) {
		case NGTAG_LOCAL_DATA:
			// local data
			memcpy(&MyData.Save.Local, ParseField.pData, sizeof(StrSavegameLocalData));
			break;

		case NGTAG_GLOBAL_DATA:
			// global data
			memcpy(&MyData.Save.Global, ParseField.pData, sizeof(StrSavegameGlobalData));
			break;
			
		case NGTAG_PROGRESSIVE_ACTIONS:
			// progressive actions
			i= ParseField.StartDataIndex;
			// read tot actions value
			TotActions = pVetExtras[i++];
			// copy all tot records
			memcpy(&MyData.VetProgrActions[0], &pVetExtras[i], sizeof(StrProgressiveAction) * TotActions);
			MyData.TotProgrActions = TotActions;
			break;
		}
		Indice= ParseField.NextIndex; 
	}

}

// free memory used to store all data about your customize commands loaded in previous level
void FreeMemoryCustomize(void)
{
	int i;

	for (i=0;i<MyData.BaseCustomizeMine.TotCustomize;i++) {
		FreeMemory(MyData.BaseCustomizeMine.pVetCustomize[i].pVetArg);
	}

	if (MyData.BaseCustomizeMine.TotCustomize > 0) {
		FreeMemory(MyData.BaseCustomizeMine.pVetCustomize);
		MyData.BaseCustomizeMine.TotCustomize=0;
	}


	MyData.BaseCustomizeMine.pVetCustomize=NULL;
}

// free memory used to store all data about your parameters commands loaded in previous level
void FreeMemoryParameters(void)
{
	int i;

	for (i=0;i<MyData.BaseParametersMine.TotParameters;i++) {
		FreeMemory(MyData.BaseParametersMine.pVetParameters[i].pVetArg);
	}

	if (MyData.BaseParametersMine.TotParameters > 0) {
		FreeMemory(MyData.BaseParametersMine.pVetParameters);
		MyData.BaseParametersMine.TotParameters=0;
	}

	MyData.BaseParametersMine.pVetParameters=NULL;
}

// this procedure will be called at end of any level
// you can type here code to free resources allocated for level (that quits now)
void FreeLevelResources(void)
{

	// free memory used to store all data about your customize commands loaded in previous level
	FreeMemoryCustomize();
	// free memory used to store all data about your parameters commands loaded in previous level
	FreeMemoryParameters();
	MyData.BaseAssignSlotMine.TotAssign=0;

}
// it will be called before beginning the loading for a new level.
// you can type here code to initialise all variables used for level (to clear old values changed by previous level)
// and to free resources allocated in old level since now we'are going to another new level.

void cbInitLoadNewLevel(void)
{
	int i;

	StrProgressiveAction *pAction;

	// clear all LOCAL variables
	ClearMemory(&MyData.Save.Local,sizeof(StrSavegameLocalData));

	// clear progressive actions
	pAction= &MyData.VetProgrActions[0];

	for (i=0;i<MyData.TotProgrActions;i++) {
		if (pAction->ActionType != AXN_FREE) {
			// here you could analise to free resoruce allocated from this specific action

			pAction->ActionType = AXN_FREE;
		}
	}

	MyData.TotProgrActions=0;
	MyData.LastProgrActionIndex=0;

	// here you can initialise other variables of MyData different than Local and progressive actions
	// free resources allocate in previous level
	FreeLevelResources();
}


// this procedure will be called everytime a flipeffect of yours will be engaged
// you have to elaborate it and then return a TRET_.. value (most common is TRET_PERFORM_ONCE_AND_GO)
int cbFlipEffectMine(WORD FlipIndex, WORD Timer, WORD Extra, WORD ActivationMode)
{
	int RetValue;
	WORD TimerFull;

	RetValue = enumTRET.PERFORM_ONCE_AND_GO;
	// if the flip has no Extra paremeter you can handle a Timer value with values upto 32767
	// in this case you'll use the following TimerFull variable, where (with following code) we set a unique big number 
	// pasting togheter the timer+extra arguments:
	TimerFull = Timer | (Extra << 8);

	switch (FlipIndex) {
		// here type the "case Number:" for each flipeffect number. At end of the code you'll use the "break;" instruction to signal the code ending
		// Note: when you'll add your first "case Number:" then you can remove the following "case -1: and break;" instructions
	case -1: 
		break;
	default:
		SendToLog("WARNING: Flipeffect trigger number %d has not been handled in cbFlipEffectMine() function", FlipIndex);
		break;
	}

	// if there was the one-shot button enabled, return TRET_PERFORM_NEVER_MORE
	if (ActivationMode & enumSCANF.BUTTON_ONE_SHOT) RetValue= enumTRET.PERFORM_NEVER_MORE; 
	return RetValue;
}

// this procedure will be called everytime an action trigger of yours will be engaged 
// you have to elaborate it and then return a TRET_.. value (most common is TRET_PERFORM_ONCE_AND_GO)
int cbActionMine(WORD ActionIndex, int ItemIndex, WORD Extra, WORD ActivationMode)
{
	int RetValue;
	
	RetValue=TRET_PERFORM_ONCE_AND_GO;

	switch (ActionIndex) {
		// type here the code per your action trigger.
		// add "case Number:" and complete the code with "break;" instruction
	case -1:
		// note: remove this "case -1:" and its "break;" it has been added only to avoid warning messages about empty switch
		break;
	default:
		SendToLog("WARNING: action trigger number %d has not been handled in cbActionMine() function", ActionIndex);
		break;
	}
	
	// if there was the one-shot button enabled, return TRET_PERFORM_NEVER_MORE
	if (ActivationMode & enumSCANF.BUTTON_ONE_SHOT) RetValue= enumTRET.PERFORM_NEVER_MORE;
	return RetValue;


}

// this procedure will be called everytime a conditional trigger of yours will be engaged
// you have to elaborate it and then return a CTRET_.. value (most common is CTRET_ONLY_ONCE_ON_TRUE)
int cbConditionMine(WORD ConditionIndex, int ItemIndex, WORD Extra, WORD ActivationMode)
{
	int RetValue;
	
	RetValue=CTRET_ONLY_ONCE_ON_TRUE;

	switch (ConditionIndex){
		// type here the code for your condition trigger, inserting the code in the section
		// beginning with "case NumberOfAction:" and ending with row "break;"
	case -1:
		// note: remove this "case -1:" and its "break;" it has been added only to avoid warning messages about empty switch
		break;
	default:
		SendToLog("WARNING: condition trigger number %d has not been handled in cbConditionMine() function", ConditionIndex);
		break;


	}
	return RetValue;
	  
}

// this procedure vill be called for each Customize=CUST_... command read from script
// having one of yours CUST_ constant
// CustomizeValue will be the value of your CUST_ constant
// NumberOfItems will be the number of following Item (signed 16 bit values) following
// the CUST_ constant in the customize= script command
// pItemArray is the array with all NumberOfItems arguments of customize command
void cbCustomizeMine(WORD CustomizeValue, int NumberOfItems, short *pItemArray)
{
	// here you can replace this default management of anonymous customize commands
	// with your procedure where you can recognize each different CUST_ value and 
	// save its arguments in meaningful names fields, or elaboriting them immediatly
	// when it is possible (warning: in this moment nothing of level it has been yet loaded, excepting the script section)

	// ----- default management (optional)----
	// all customize values will be saved in MyData structure
	DWORD SizeMem;
	StrGenericCustomize *pMyCust;
	int TotCust;

	// ask memory to have another (new) record of StrGenericCustomize structure
	TotCust= MyData.BaseCustomizeMine.TotCustomize;
	TotCust++;
	SizeMem = TotCust * sizeof(StrGenericCustomize);
	MyData.BaseCustomizeMine.pVetCustomize = 
				(StrGenericCustomize *) ResizeMemory(MyData.BaseCustomizeMine.pVetCustomize, SizeMem);

	pMyCust = & MyData.BaseCustomizeMine.pVetCustomize[TotCust-1];
	
	// now require memory for all arguments (NumberOfItems) store in pItemArray

	pMyCust->pVetArg = (short *) GetMemory(2 * NumberOfItems);
	// copy data
	pMyCust->NArguments = NumberOfItems;
	memcpy(pMyCust->pVetArg, pItemArray, 2*NumberOfItems);
	pMyCust->CustValue = CustomizeValue;

	MyData.BaseCustomizeMine.TotCustomize= TotCust;
	// ---- end of default managemnt for generic customize -------------	
}

// callback called everytime in current level section of the script it has been found an AssignSlot command
// with one of your OBJ_ constants
void cbAssignSlotMine(WORD Slot, WORD ObjType)
{
	int i;

	i = MyData.BaseAssignSlotMine.TotAssign;

	if (i >= MAX_ASSIGN_SLOT_MINE) {
		SendToLog("ERROR: too many AssignSlot= commands for current plugin");
		return;
	}

	MyData.BaseAssignSlotMine.VetAssignSlot[i].MioSlot = Slot;
	MyData.BaseAssignSlotMine.VetAssignSlot[i].TipoSlot = ObjType;
	MyData.BaseAssignSlotMine.TotAssign++;

}
// this procedure vill be called for each Parameters=PARAM_... command read from script
// having one of yours PARAM_ constants
// ParameterValue will be the value of your PARAM_ constant
// NumberOfItems will be the number of following Item (signed 16 bit values) following
// the PARAM_ constant in the customize= script command
// pItemArray is the array with all NumberOfItems arguments of Parameter command
void cbParametersMine(WORD ParameterValue, int NumberOfItems, short *pItemArray)
{
	// here you can replace this default management of anonymous parameters commands
	// with your procedure where you can recognize each different Param_ value and 
	// save its arguments in meaningful names fields, or elaboriting them immediatly
	// when it is possible (warning: in this moment nothing of level it has been yet loaded, excepting the script section)

	// ----- default management (optional)----
	// all parameters values will be saved in MyData structure
	DWORD SizeMem;
	StrGenericParameters *pMyParam;
	int TotParam;

	// ask memory to have another (new) record of StrGenericparameters structure
	TotParam= MyData.BaseParametersMine.TotParameters;
	TotParam++;
	SizeMem = TotParam * sizeof(StrGenericParameters);
	MyData.BaseParametersMine.pVetParameters = 
		(StrGenericParameters *) ResizeMemory(MyData.BaseParametersMine.pVetParameters, SizeMem);

	pMyParam = & MyData.BaseParametersMine.pVetParameters[TotParam-1];

	pMyParam->ParamValue = ParameterValue;
	// now require memory for all arguments (NumberOfItems) store in pItemArray
	pMyParam->pVetArg = (short *) GetMemory(2 * NumberOfItems);
	// copy data
	pMyParam->NArguments = NumberOfItems;
	memcpy(pMyParam->pVetArg, pItemArray, 2*NumberOfItems);

	MyData.BaseParametersMine.TotParameters= TotParam;
	// ---- end of default managemnt for generic parameters -------------


}


// this procedure will be called every game cycle (at begin of cycle)
void cbCycleBegin(void)
{
	if (*Trng.pGlobTomb4->pAdr->pLevelNow == 0x4)
		GlitterInit(8, 24);
}

// Not yet linked! To link it add to RequireMyCallBacks() function the row:
//  	GET_CALLBACK(CB_CYCLE_END, 0, 0, cbCycleEnd);
// this procedure will be called everygame cycle, at end.
// you have to return a RET_CYCLE_ value
int cbCycleEnd(void)
{

	return RET_CYCLE_CONTINUE;	
}

// this function will be called for each your (common) progressive action to be peformed
void PerformMyProgrAction(StrProgressiveAction *pAction)
{


	switch (pAction->ActionType) {
// replace the "case -1:" with your first "case AXN_...:" progressive action to manage)		
	case -1:
		break;

	}

}

// callback called from trng for each frame in game cycle to perform your (common) progressive action
void cbProgrActionMine(void)
{
	int i;
	StrProgressiveAction *pAction;

	pAction = &MyData.VetProgrActions[0];
	for (i=0;i<MyData.TotProgrActions;i++) {
		if (pAction->ActionType != AXN_FREE) {
			PerformMyProgrAction(pAction);
		}
		pAction++;
	}

}


void InitEmitter(short item_number)
{
	auto item = &items[item_number];
	item->item_flags[0] = 0;
}


void EmitterControl(short item_number)
{
	auto item = &items[item_number];

	switch (item->trigger_flags)
	{
	case 0: // fireflies

		if (TriggerActive((StrItemTr4*)item))
		{
			if (!(Particles::RandomInt() & 15))
				FireflyInit(0, item_number);
			item->item_flags[0] = 1;
		}
		else if (item->item_flags[0])
		{
			//ClearEffect(0, item_number);
			item->item_flags[0] = 0;
		}

		break;
	
	case 6: // pixies/spores

		if (TriggerActive((StrItemTr4*)item))
		{
			if (!(Particles::RandomInt() & 15))
				PixieInit(1, item_number);
			item->item_flags[0] = 1;
		}
		else if (item->item_flags[0])
		{
			//ClearEffect(1, item_number);
			item->item_flags[0] = 0;
		}
		
		break;
	
	case 2: // fire embers

		if (TriggerActive((StrItemTr4*)item))
		{
			EmberInit(2, item_number);
			item->item_flags[0] = 1;
		}
		else if (item->item_flags[0])
			item->item_flags[0] = 0;

		break;

	case 3: // lava particles

		if (TriggerActive((StrItemTr4*)item))
		{
			LavaInit(4, Vector3f(item->pos.xPos, item->pos.yPos, item->pos.zPos));
			item->item_flags[0] = 1;
		}
		else if (item->item_flags[0])
			item->item_flags[0] = 0;
		break;

	case 4: // pyroclastic bombs

		if (TriggerActive((StrItemTr4*)item))
		{
			if (!item->item_flags[0])
			{
				SoundEffect(182, &item->pos.xPos, 0);
				PyroclastInit(7, item->pos.xPos, item->pos.yPos, item->pos.zPos, item->room_number);
				item->item_flags[0] = 60 + (Particles::RandomInt() & 31);
			}
			item->item_flags[0]--;
		}

		break;

	case 5: // Valheim pollen and wind-streaks

		if (TriggerActive((StrItemTr4*)item))
		{
			if (!(Particles::RandomInt() & 3))
				PollenInit(9, item_number);
			item->item_flags[0] = 1;
		}
		else if (item->item_flags[0])
			item->item_flags[0] = 0;
		break;

	case 7: // Valheim pollen and wind-streaks, wider area

		if (TriggerActive((StrItemTr4*)item))
		{
			if (!(Particles::RandomInt() & 3))
				PollenInit2(9, item_number);
			item->item_flags[0] = 1;
		}
		else if (item->item_flags[0])
			item->item_flags[0] = 0;
		break;

	case 8:
		if (TriggerActive((StrItemTr4*)item))
		{
			if (!(Particles::RandomInt() & 7))
				WindstreaksWide(item_number);
			item->item_flags[0] = 1;
		}
		else if (item->item_flags[0])
			item->item_flags[0] = 0;
		break;
	}
}

// inside this function you'll type call to functions to intialise your new objects or customize that olds.
// this callback will be called at start of loading new level and a bit after having started to load level data
void cbInitObjects(void) 
{
	auto obj = &objects[SLOT_UW_PROPULSOR];

	obj->initialise = InitEmitter;
	obj->control = EmitterControl;

	obj->draw_routine = nullptr;
	obj->collision = nullptr;
	obj->floor = nullptr;
	obj->ceiling = nullptr;
}

// FOR_YOU:
// in this function RequireMyCallBacks() you'll type
// a list of:
//		GET_CALLBACK(CB_..., ,)
// one for each callback you need
bool RequireMyCallBacks(void)
{
// ************  RequireMyCallBacks() function  *****************
	// protype of GET_CALLBACK:
	// GET_CALLBACK(CallBackCB, CBT_Flags, Index, MyProcToCall)
	
	GET_CALLBACK(CB_INIT_PROGRAM, 0, 0, cbInitProgram)
	GET_CALLBACK(CB_INIT_LEVEL, 0,0, cbInitLevel)
	GET_CALLBACK(CB_INIT_LOAD_NEW_LEVEL, 0,0, cbInitLoadNewLevel)
	GET_CALLBACK(CB_SAVING_GAME, 0, 0, cbSaveMyData)
	GET_CALLBACK(CB_LOADING_GAME, 0, 0, cbLoadMyData)
	GET_CALLBACK(CB_ASSIGN_SLOT_MINE, 0,0, cbAssignSlotMine)
	GET_CALLBACK(CB_INIT_OBJECTS, 0, 0, cbInitObjects)
	GET_CALLBACK(CB_CUSTOMIZE_MINE, 0,0, cbCustomizeMine)
	GET_CALLBACK(CB_INIT_GAME, 0, 0, cbInitGame)
	GET_CALLBACK(CB_FLIPEFFECT_MINE, 0, 0, cbFlipEffectMine)
	GET_CALLBACK(CB_ACTION_MINE, 0,0, cbActionMine)
	GET_CALLBACK(CB_CONDITION_MINE,0,0,cbConditionMine)
	GET_CALLBACK(CB_PARAMETER_MINE, 0, 0, cbParametersMine)
	GET_CALLBACK(CB_CYCLE_BEGIN, 0, 0, cbCycleBegin)
	GET_CALLBACK(CB_PROGR_ACTION_MINE, 0, 0, cbProgrActionMine)

	return true;
}
// FOR_YOU:
// This function will be the first code to be executed of your plugin
// It happens when trng will load your plugin.
// In this moment no directX graphic is yet active, so you can show
// message boxes (TryMessageBox) to advise your players or level designer 
// about something
// Note: if you wish abort all (because there is an error or something
// is missing) you have to exit from this function returning: false
bool InitializeAll(void)
{
// ************  InitializeAll() function  ****************
	//  perform all your patches
	CALL_CHECK(CreateMyCodePatches)

	// call the function that requires all callback you need
	CALL_CHECK(RequireMyCallBacks)

	// TYPE_HERE: code to allocate global resource to use in the whole game

	return true;
}

// FOR_YOU: Tyis function will be called when tomb4 game is to be closed.
// you should type in this function further codes to free the global
// resource you had allocated in the InitializeAll() function 
void ReleaseAll(void)
{
// ************  ReleaseAll() function  ******************
	FreeLevelResources();
}


BOOL APIENTRY DllMain( HINSTANCE hInstanceDll, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved)
{

    switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
			MyDllInstance = hInstanceDll;
			GetTrngInfo();
			// control per check control value about size and alignment with globtomb4 structure
			if (CheckControlGlobTomb4() == false) return FALSE;

			if  (InitializeAll()==false) {
				return FALSE;
			}
			return TRUE;
			

		case DLL_PROCESS_DETACH:
			ReleaseAll();
			break;
    }
    return TRUE;
}


