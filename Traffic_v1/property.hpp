/*--------------------- stratygy selete ----------------------------*/

/*comment the lines below to switch the strategy
-------------------- Example --------------------
#undef MANUAL		//means the non_control strategy
#undef ST1			//the first strategy
//#undef ST2		//the second strategy
#undef COMBO
means switch on the second strategy*/
#ifndef _A_0_
#undef MANUAL		//means the non_control strategy
#endif
#ifndef _A_1_
#undef ST1			//the first strategy
#endif
#ifndef _A_2_
#undef ST2		//the second strategy
#endif
#ifndef _A_C_
#undef COMBO
#endif
//the combo strategy

/*--------------------------------------------------------------------*/

/*--------------------------- MAX LOAD -------------------------------*/
//comment the line below to switch on the max_follow stragegy

#undef MAX_LOAD
/*--------------------------------------------------------------------*/

/*------------------------- Advanced Usage --------------------------*/
//#undef FIN