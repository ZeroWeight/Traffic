if (light == green){
	Q_block.all_move_forward(v_dis)
}
else{
	if(!Q_block.empty()){
		if(Q_block.last().pos-Q_in.first().pos
			< S_stop){
			Vehicle v;
			v=Q_in.getfirst();
			Q_block.push(v);
		}
	}
	else{
		if(S_end-Q_in.first().pos< S_stop){
			Vehicle v;
			v=Q_in.getfirst();
			Q_block.push(v);
		}
	}
	
}