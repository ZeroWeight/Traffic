/*--------------------- stratygy selete ----------------------------*/

/*comment the lines below to switch the strategy
-------------------- Example --------------------
#undef MANUAL		//means the non_control strategy
#undef ST1			//the first strategy
//#undef ST2		//the second strategy
#undef COMBO
means switch on the second strategy*/

#undef MANUAL		//means the non_control strategy
#undef ST1			//the first strategy
#undef ST2		//the second strategy
//#undef COMBO		//the combo strategy

/*--------------------------------------------------------------------*/

/*--------------------------- MAX LOAD -------------------------------*/
//comment the line below to switch on the max_follow stragegy

#undef MAX_LOAD
/*--------------------------------------------------------------------*/
/*----------------------  ratio setting  -----------------------------*/
//if you choose the combo stratery, set a sequence of ratio for the cars

#define R_0 10			//means the non_control strategy
#define R_1 10			//the first strategy
#define R_2 10			//the second strategy

/*--------------------------------------------------------------------*/