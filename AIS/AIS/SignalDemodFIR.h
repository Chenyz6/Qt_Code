/*
 * Filter Coefficients (C Source) generated by the Filter Design and Analysis Tool
 *
 * Generated by MATLAB(R) 7.0.1 and the Signal Processing Toolbox 6.2.1.
 *
 * Generated on: 07-May-2009 09:54:57
 *
 */

/*
 *  FIR Filter (real)
 * -------------------------------
 * Filter Structure  : interp FIR
 * Filter Order      : 64
 * Stable            : Yes
 * Linear Phase      : Yes (Type 1)
 */

/* General type conversion for MATLAB generated C-code  */
//#include "tmwtypes.h"
/* 
 * Expected path to tmwtypes.h 
 * C:\MATLAB701\extern\include\tmwtypes.h 
 */
#pragma once

const int IBL = 64*8;
const double IB[64*8] = {
	+5.2007482e-004, +1.8799899e-002, -6.8607670e-002, +1.2280535e-001, +8.5744589e-001, +1.1023955e-001, -6.5553980e-002, +1.8406482e-002,
		+2.0174963e-004, +1.9174234e-002, -7.1644991e-002, +1.3565749e-001, +8.5686659e-001, +9.7969349e-002, -6.2490959e-002, +1.7996106e-002,
		+2.4019689e-004, +1.9527596e-002, -7.4658648e-002, +1.4878603e-001, +8.5570882e-001, +8.6003489e-002, -5.9425440e-002, +1.7570553e-002,
		+2.8304279e-004, +1.9857799e-002, -7.7641128e-002, +1.6218050e-001, +8.5397421e-001, +7.4350025e-002, -5.6363967e-002, +1.7131810e-002,
		+3.3081536e-004, +2.0162802e-002, -8.0584688e-002, +1.7582979e-001, +8.5166521e-001, +6.3016452e-002, -5.3312853e-002, +1.6681598e-002,
		+3.8378528e-004, +2.0440383e-002, -8.3481386e-002, +1.8972229e-001, +8.4878507e-001, +5.2009589e-002, -5.0278150e-002, +1.6221692e-002,
		+4.4231935e-004, +2.0688417e-002, -8.6323079e-002, +2.0384577e-001, +8.4533786e-001, +4.1335692e-002, -4.7265637e-002, +1.5753732e-002,
		+5.0649762e-004, +2.0904589e-002, -8.9101429e-002, +2.1818751e-001, +8.4132843e-001, +3.1000324e-002, -4.4280828e-002, +1.5279407e-002,
		+5.7666809e-004, +2.1086701e-002, -9.1807933e-002, +2.3273421e-001, +8.3676241e-001, +2.1008492e-002, -4.1328974e-002, +1.4800149e-002,
		+6.5315971e-004, +2.1232430e-002, -9.4433910e-002, +2.4747211e-001, +8.3164623e-001, +1.1364519e-002, -3.8415039e-002, +1.4317522e-002,
		+7.3662989e-004, +2.1339514e-002, -9.6970542e-002, +2.6238690e-001, +8.2598706e-001, +2.0721586e-003, -3.5543737e-002, +1.3832866e-002,
		+8.2736484e-004, +2.1405568e-002, -9.9408865e-002, +2.7746385e-001, +8.1979282e-001, -6.8655031e-003, -3.2719477e-002, +1.3347573e-002,
		+9.2555160e-004, +2.1428350e-002, -1.0173979e-001, +2.9268773e-001, +8.1307219e-001, -1.5445933e-002, -2.9946413e-002, +1.2862821e-002,
		+1.0312870e-003, +2.1405463e-002, -1.0395413e-001, +3.0804291e-001, +8.0583453e-001, -2.3667265e-002, -2.7228416e-002, +1.2379889e-002,
		+1.1454789e-003, +2.1334638e-002, -1.0604260e-001, +3.2351331e-001, +7.9808993e-001, -3.1528171e-002, -2.4569088e-002, +1.1899857e-002,
		+1.2685474e-003, +2.1213542e-002, -1.0799583e-001, +3.3908250e-001, +7.8984914e-001, -3.9027972e-002, -2.1971746e-002, +1.1423785e-002,
		+1.3997665e-003, +2.1039901e-002, -1.0980441e-001, +3.5473366e-001, +7.8112361e-001, -4.6166507e-002, -1.9439449e-002, +1.0952684e-002,
		+1.5408607e-003, +2.0811455e-002, -1.1145886e-001, +3.7044963e-001, +7.7192537e-001, -5.2944245e-002, -1.6974991e-002, +1.0487420e-002,
		+1.6911643e-003, +2.0525985e-002, -1.1294972e-001, +3.8621294e-001, +7.6226711e-001, -5.9362156e-002, -1.4580887e-002, +1.0028905e-002,
		+1.8513640e-003, +2.0181278e-002, -1.1426746e-001, +4.0200585e-001, +7.5216211e-001, -6.5421814e-002, -1.2259394e-002, +9.5779259e-003,
		+2.0217606e-003, +1.9775246e-002, -1.1540263e-001, +4.1781032e-001, +7.4162421e-001, -7.1125265e-002, -1.0012544e-002, +9.1350930e-003,
		+2.2026420e-003, +1.9305769e-002, -1.1634576e-001, +4.3360811e-001, +7.3066778e-001, -7.6475131e-002, -7.8420650e-003, +8.7012185e-003,
		+2.3943913e-003, +1.8770867e-002, -1.1708747e-001, +4.4938077e-001, +7.1930773e-001, -8.1474491e-002, -5.7495019e-003, +8.2767589e-003,
		+2.5971124e-003, +1.8168563e-002, -1.1761841e-001, +4.6510967e-001, +7.0755945e-001, -8.6126950e-002, -3.7360964e-003, +7.8622894e-003,
		+2.8111179e-003, +1.7497047e-002, -1.1792937e-001, +4.8077603e-001, +6.9543879e-001, -9.0436544e-002, -1.8029127e-003, +7.4582322e-003,
		+3.0365827e-003, +1.6754489e-002, -1.1801121e-001, +4.9636101e-001, +6.8296202e-001, -9.4407813e-002, +4.9255571e-005, +7.0651024e-003,
		+3.2738240e-003, +1.5939259e-002, -1.1785496e-001, +5.1184560e-001, +6.7014583e-001, -9.8045681e-002, +1.8198015e-003, +6.6831346e-003,
		+3.5228421e-003, +1.5049765e-002, -1.1745175e-001, +5.2721083e-001, +6.5700727e-001, -1.0135554e-001, +3.5083749e-003, +6.3127182e-003,
		+3.7838949e-003, +1.4084579e-002, -1.1679296e-001, +5.4243764e-001, +6.4356375e-001, -1.0434314e-001, +5.1147644e-003, +5.9540266e-003,
		+4.0571375e-003, +1.3042344e-002, -1.1587008e-001, +5.5750706e-001, +6.2983294e-001, -1.0701465e-001, +6.6390399e-003, +5.6073672e-003,
		+4.3426514e-003, +1.1921903e-002, -1.1467489e-001, +5.7240011e-001, +6.1583285e-001, -1.0937656e-001, +8.0813875e-003, +5.2727284e-003,
		+4.6402711e-003, +1.0722151e-002, -1.1319936e-001, +5.8709791e-001, +6.0158169e-001, -1.1143574e-001, +9.4422512e-003, +4.9504517e-003,
		+4.9504517e-003, +9.4422512e-003, -1.1143574e-001, +6.0158169e-001, +5.8709791e-001, -1.1319936e-001, +1.0722151e-002, +4.6402711e-003,
		+5.2727284e-003, +8.0813875e-003, -1.0937656e-001, +6.1583285e-001, +5.7240011e-001, -1.1467489e-001, +1.1921903e-002, +4.3426514e-003,
		+5.6073672e-003, +6.6390399e-003, -1.0701465e-001, +6.2983294e-001, +5.5750706e-001, -1.1587008e-001, +1.3042344e-002, +4.0571375e-003,
		+5.9540266e-003, +5.1147644e-003, -1.0434314e-001, +6.4356375e-001, +5.4243764e-001, -1.1679296e-001, +1.4084579e-002, +3.7838949e-003,
		+6.3127182e-003, +3.5083749e-003, -1.0135554e-001, +6.5700727e-001, +5.2721083e-001, -1.1745175e-001, +1.5049765e-002, +3.5228421e-003,
		+6.6831346e-003, +1.8198015e-003, -9.8045681e-002, +6.7014583e-001, +5.1184560e-001, -1.1785496e-001, +1.5939259e-002, +3.2738240e-003,
		+7.0651024e-003, +4.9255571e-005, -9.4407813e-002, +6.8296202e-001, +4.9636101e-001, -1.1801121e-001, +1.6754489e-002, +3.0365827e-003,
		+7.4582322e-003, -1.8029127e-003, -9.0436544e-002, +6.9543879e-001, +4.8077603e-001, -1.1792937e-001, +1.7497047e-002, +2.8111179e-003,
		+7.8622894e-003, -3.7360964e-003, -8.6126950e-002, +7.0755945e-001, +4.6510967e-001, -1.1761841e-001, +1.8168563e-002, +2.5971124e-003,
		+8.2767589e-003, -5.7495019e-003, -8.1474491e-002, +7.1930773e-001, +4.4938077e-001, -1.1708747e-001, +1.8770867e-002, +2.3943913e-003,
		+8.7012185e-003, -7.8420650e-003, -7.6475131e-002, +7.3066778e-001, +4.3360811e-001, -1.1634576e-001, +1.9305769e-002, +2.2026420e-003,
		+9.1350930e-003, -1.0012544e-002, -7.1125265e-002, +7.4162421e-001, +4.1781032e-001, -1.1540263e-001, +1.9775246e-002, +2.0217606e-003,
		+9.5779259e-003, -1.2259394e-002, -6.5421814e-002, +7.5216211e-001, +4.0200585e-001, -1.1426746e-001, +2.0181278e-002, +1.8513640e-003,
		+1.0028905e-002, -1.4580887e-002, -5.9362156e-002, +7.6226711e-001, +3.8621294e-001, -1.1294972e-001, +2.0525985e-002, +1.6911643e-003,
		+1.0487420e-002, -1.6974991e-002, -5.2944245e-002, +7.7192537e-001, +3.7044963e-001, -1.1145886e-001, +2.0811455e-002, +1.5408607e-003,
		+1.0952684e-002, -1.9439449e-002, -4.6166507e-002, +7.8112361e-001, +3.5473366e-001, -1.0980441e-001, +2.1039901e-002, +1.3997665e-003,
		+1.1423785e-002, -2.1971746e-002, -3.9027972e-002, +7.8984914e-001, +3.3908250e-001, -1.0799583e-001, +2.1213542e-002, +1.2685474e-003,
		+1.1899857e-002, -2.4569088e-002, -3.1528171e-002, +7.9808993e-001, +3.2351331e-001, -1.0604260e-001, +2.1334638e-002, +1.1454789e-003,
		+1.2379889e-002, -2.7228416e-002, -2.3667265e-002, +8.0583453e-001, +3.0804291e-001, -1.0395413e-001, +2.1405463e-002, +1.0312870e-003,
		+1.2862821e-002, -2.9946413e-002, -1.5445933e-002, +8.1307219e-001, +2.9268773e-001, -1.0173979e-001, +2.1428350e-002, +9.2555160e-004,
		+1.3347573e-002, -3.2719477e-002, -6.8655031e-003, +8.1979282e-001, +2.7746385e-001, -9.9408865e-002, +2.1405568e-002, +8.2736484e-004,
		+1.3832866e-002, -3.5543737e-002, +2.0721586e-003, +8.2598706e-001, +2.6238690e-001, -9.6970542e-002, +2.1339514e-002, +7.3662989e-004,
		+1.4317522e-002, -3.8415039e-002, +1.1364519e-002, +8.3164623e-001, +2.4747211e-001, -9.4433910e-002, +2.1232430e-002, +6.5315971e-004,
		+1.4800149e-002, -4.1328974e-002, +2.1008492e-002, +8.3676241e-001, +2.3273421e-001, -9.1807933e-002, +2.1086701e-002, +5.7666809e-004,
		+1.5279407e-002, -4.4280828e-002, +3.1000324e-002, +8.4132843e-001, +2.1818751e-001, -8.9101429e-002, +2.0904589e-002, +5.0649762e-004,
		+1.5753732e-002, -4.7265637e-002, +4.1335692e-002, +8.4533786e-001, +2.0384577e-001, -8.6323079e-002, +2.0688417e-002, +4.4231935e-004,
		+1.6221692e-002, -5.0278150e-002, +5.2009589e-002, +8.4878507e-001, +1.8972229e-001, -8.3481386e-002, +2.0440383e-002, +3.8378528e-004,
		+1.6681598e-002, -5.3312853e-002, +6.3016452e-002, +8.5166521e-001, +1.7582979e-001, -8.0584688e-002, +2.0162802e-002, +3.3081536e-004,
		+1.7131810e-002, -5.6363967e-002, +7.4350025e-002, +8.5397421e-001, +1.6218050e-001, -7.7641128e-002, +1.9857799e-002, +2.8304279e-004,
		+1.7570553e-002, -5.9425440e-002, +8.6003489e-002, +8.5570882e-001, +1.4878603e-001, -7.4658648e-002, +1.9527596e-002, +2.4019689e-004,
		+1.7996106e-002, -6.2490959e-002, +9.7969349e-002, +8.5686659e-001, +1.3565749e-001, -7.1644991e-002, +1.9174234e-002, +2.0174963e-004,
		+1.8406482e-002, -6.5553980e-002, +1.1023955e-001, +8.5744589e-001, +1.2280535e-001, -6.8607670e-002, +1.8799899e-002, +5.2007482e-004
};


//b = rcosfir(0.35, 8, 2, 1, 'sqrt');
const int MBL=33;
const double MB[33]={+2.1815107e-003, -2.7698753e-003, +1.8451065e-004, +2.6539942e-003, -4.1416890e-003, +1.9446541e-003, +5.3051648e-003, 
				   -8.2212599e-003, +1.4446256e-003, +6.7686925e-003, -1.7998466e-002, +1.8112536e-002, +4.0389449e-002, -9.5575463e-002, 
				   -5.9885061e-002, +4.2976085e-001, +7.7473012e-001, +4.2976085e-001, -5.9885061e-002, -9.5575463e-002, +4.0389449e-002, 
				   +1.8112536e-002, -1.7998466e-002, +6.7686925e-003, +1.4446256e-003, -8.2212599e-003, +5.3051648e-003, +1.9446541e-003, 
				   -4.1416890e-003, +2.6539942e-003, +1.8451065e-004, -2.7698753e-003, +2.1815107e-003};
