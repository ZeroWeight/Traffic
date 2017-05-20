//the code below just consider the vehicle v
if(S_end-v.pos>S_inter)//far from intersection
{
	//code block 1
	if(car-following_requirement_met())
		//the car-following requirement is  met
		car_following();
	else
		//the head car of the queue or
		//the car-following requirement isn't met
		free_driving();
}
else //close to the intersection
{	
	if(light==red)
		method1.3();//the method mentioned in 1.3
	else
		if(Q_block.empty()){
			if(time_remain>T_safe)
			//same as the code block 1
			else 
				if(!pass_check())
				//same as the red light case
			}
		else
			if(time_remain>T_safe)
			acc_to_speed(v_dis);
			else 
				if(!pass_check())
				//same as the red light case
			}
		
}