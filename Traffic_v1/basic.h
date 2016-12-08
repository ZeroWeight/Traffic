#pragma once
enum DIR {
	A, B, C, D, DIR_NUM
};
enum TR {
	Left, Right, Center, TR_NUM
};
enum Color {
	Green, Yellow, Red
};
//don't change the code below
const double near_side[DIR_NUM] = { 5.0,7.25,7.25,5.0 };
const double far_side[DIR_NUM] = { 0,12.25,12.25,0 };
const double lane_in[DIR_NUM][TR_NUM][2] = {
	{{far_side[A],6.3125},{far_side[A],7.0625},{far_side[A],6.6875}},
	{{6.3125,far_side[B] },{7.0625,far_side[B] },{6.6875,far_side[B] }},
	{{ far_side[C],5.9375},{ far_side[C],5.1875},{ far_side[C],5.5625}},
	{{5.9375,far_side[D] },{5.1875,far_side[D] },{5.5625,far_side[D] }}
};
const double lane_out[DIR_NUM][TR_NUM][2] = {
	{ { near_side[D],6.3125 },{ near_side[B],5.1875 },{ near_side[C],6.6875 } },
	{ { 5.9375,near_side[A] },{ 7.0625,near_side[C] },{ 6.6875,near_side[D] } },
	{ { near_side[B],5.9375 },{ near_side[D],7.0625 },{ near_side[A],5.5625 } },
	{ { 6.3125,near_side[C] },{ 5.1875,near_side[A] },{ 5.5625,near_side[B] } }
};
const double main_lane[3] = { 5,6.125,7.25 };
const double aux_lane[4] = { 5.375,5.75,6.5,6.875 };