if (Q_block.empty()){
	if (Light == Green)
		Q_in.first().drive_freely();
	else
		Q_in.first().brake_to(S_end);
}
else{
	if(Q_block.last().pos-Q_in.first().pos< S_control)
		Q_in.first().brake_to
		(Q_block.last().pos-S_stop);	
	else
		Q_in.first().drive_freely();
}