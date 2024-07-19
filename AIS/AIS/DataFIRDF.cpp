#include "stdafx.h"
#include "DataFIRDF.h"

CDataFIRDF::CDataFIRDF(void)
{
	///变量初始化///
	m_nLPFLen	= 0;
	m_pDataI	= NULL;
	m_pDataQ	= NULL;
	m_pLPFBuf	= NULL;
}

CDataFIRDF::~CDataFIRDF(void)
{
	///释放资源////
	if(NULL != m_pDataI)
	{
		delete []m_pDataI;
	}
	if(NULL != m_pDataQ)
	{
		delete []m_pDataQ;
	}
	if (NULL != m_pLPFBuf)
	{
		delete []m_pLPFBuf;
	}
}

BOOL CDataFIRDF::IinitialLPFPara(short nLPFDesiType,double dPassWidth,double dStopWidth,
								 double dPassRipple,double dStopAttenuation)
{	
	double dDelB = dStopWidth-dPassWidth;
	///相对过度宽不小于0.001///
	if (dDelB<0.001 || dDelB>0.35)
	{
		dStopWidth = g_dllStopwidthFactor*dPassWidth;
	}
	//获取滤波器长度
	m_nLPFLen = LPFLength(nLPFDesiType,dPassWidth,dStopWidth,dPassRipple,dStopAttenuation);	

	if (NULL != m_pLPFBuf)
	{
		delete []m_pLPFBuf;
    }
    m_pLPFBuf=new double[m_nLPFLen];
    memset(m_pLPFBuf, 0, m_nLPFLen*sizeof(double));

	if ( nLPFDesiType == FIRLPFDT_Equiripple )
	{
		///等波纹滤波器设计///
		EquirippleFD(dPassWidth,dStopWidth,dPassRipple,dStopAttenuation);
	}
	else
	{
		////利用窗函数设计低通滤波器////
		double wc1=PI*(dPassWidth+dStopWidth)/2;
		WindowFuncFD(nLPFDesiType,wc1,dStopAttenuation);
	}

	//初始化数据缓存器//
	if(NULL != m_pDataI)
	{
		delete []m_pDataI;
	}
	if(NULL != m_pDataQ)
	{
		delete []m_pDataQ;
	}
	m_pDataI = new double[m_nLPFLen];
    memset(m_pDataI, 0, m_nLPFLen*sizeof(double));
	m_pDataQ = new double[m_nLPFLen];
    memset(m_pDataQ, 0, m_nLPFLen*sizeof(double));

    return (true);
}

SHORT CDataFIRDF::LPFLength(short nLPFDesiType,double dPassWidth,
						  double dStopWidth, double dPassRipple,
						  double dStopAttenuation)
{
	////计算滤波器长度////
	SHORT		nLen = 0;

	switch(nLPFDesiType)
	{
	case FIRLPFDT_Hanning:
		nLen = static_cast<SHORT>( 8.0/(dStopWidth-dPassWidth) );	
		break;
	case FIRLPFDT_Hamming:
		nLen = static_cast<SHORT>( 8.0/(dStopWidth-dPassWidth) );
		break;
	case FIRLPFDT_Blackman:
		nLen = static_cast<SHORT>( 12.0/(dStopWidth-dPassWidth) );
		break;
	case FIRLPFDT_Kaiser:		
		nLen = static_cast<SHORT>( (dStopAttenuation-7.95)/PI/(dStopWidth-dPassWidth)/2.285+1 ); 		
		break;
	case FIRLPFDT_Equiripple:
		{
			double delta1 = log10(dPassRipple);
			double delta2 = pow(10.0,-dStopAttenuation/20);
			delta2 = log10(delta2);

			double D1 = ( 0.005309*delta1*delta1 + 0.07114*delta1 - 0.4761)*delta2;
			double D2 =  0.00266*delta1*delta1 + 0.5941*delta1 + 0.4278 ;

			double F = 11.01217 + 0.51244*(delta1-delta2);

			double dF =  (dStopWidth- dPassWidth)/2;

			nLen = static_cast<SHORT>((D1-D2)/dF - F*dF + 1 + 0.5);
		}
		break;
	default:
		nLen = 71;
	}
	////滤波器长度取奇数///
	SHORT nLPFLen = nLen;
	if ( nLen%2==0 )
	{
		nLPFLen += 1;
	}
	else
	{
		nLPFLen += 2; 
	}
	///滤波器长度最小为5///
	return (nLPFLen>4 ? nLPFLen:5);
}
void CDataFIRDF::WindowFuncFD(short nWinType,double dBandwidth,
							  double dStopAttenuation)
{
	int i, j, k;
	//产生理想低通滤波器//
	for(i=0;i<m_nLPFLen/2;i++)
	{				
		m_pLPFBuf[i]=sin(dBandwidth*(i-m_nLPFLen/2))/(PI*(i-m_nLPFLen/2));
		m_pLPFBuf[m_nLPFLen-1-i]=m_pLPFBuf[i];
	}
	m_pLPFBuf[m_nLPFLen/2]=(dBandwidth/PI);

	//产生窗//
	double *pWindow=new double[m_nLPFLen];
    memset(pWindow, 0, m_nLPFLen*sizeof(double));

	int N = m_nLPFLen;
	double dOmiga = 2.0*PI/N;
	switch(nWinType)
	{	
	case FIRLPFDT_Hanning:		//Hanning
		for(i=0; i<N; i++)
		{
			*(pWindow+i) = (1.0-cos(i*dOmiga))*0.5;
		}
		break;
	case FIRLPFDT_Hamming:		//Hamming
		for(i=0; i<N; i++)
		{
			*(pWindow+i) =  0.54 - 0.46*cos(i*dOmiga);
		}
		break;
	case FIRLPFDT_Blackman:		//Blackman
		for(i=0; i<N; i++)
		{
			*(pWindow+i) =  0.42 - 0.5*cos(i*dOmiga) + 0.08*cos(2*i*dOmiga);
		}
		break;
	case FIRLPFDT_Kaiser:		//Kaiser
		{
			double beta = .0;			// dStopRipple<=21 dB
			if ( dStopAttenuation>50.0 )
			{
				beta = 0.1102*(dStopAttenuation-8.7); // dStopRipple>50 dB
			}
			else if ( dStopAttenuation>21.0 ) // 21<dStopRipple<50 dB
			{
				beta = 0.5842*pow(dStopAttenuation-21.0,0.4)+0.07886*(dStopAttenuation-21);				
			}	

			double bes = 1.0;;
			double dFact;
			for (k=0;k<20;k++)
			{
				dFact = 1.0;
				for (j=1;j<k+2;j++)
				{
					dFact *= j;
				}
				bes += (pow(beta/2,k+1)/dFact)*(pow(beta/2,k+1)/dFact);
			}

			double xind = pow(1.0*(N-1),2);

			double delta ;
			double I0;
			double dTemp;
			int m = (N-1)/2;

			for(int i=0; i<m+1; i++)
			{		

				dTemp = 4.0*i*i;
				delta = beta*sqrt(1.0-dTemp/xind);
				I0=1.0;
				for (k=0;k<20;k++)
				{
					dFact = 1.0;
					for (j=1;j<k+2;j++)
					{
						dFact *= j;
					}
					I0 += (pow(delta/2,k+1)/dFact)*(pow(delta/2,k+1)/dFact);					
				}

				*(pWindow+i+m)=I0/bes;
				if(i>0)	{*(pWindow+m-i)=I0/bes;}
			}

		}
		break;	
	default://case Boxcar:
		for(i=0; i<N; i++)
		{
			*(pWindow+i) = 1.0;
		}
	}	

	/// 滤波器加窗 ///
	for(i=0;i<m_nLPFLen;i++)
	{
		m_pLPFBuf[i]  *=  pWindow[i];
	}

	///释放资源///
	delete []pWindow;
}

//等波纹滤波器设计
void CDataFIRDF::EquirippleFD(double dPassWidth,double dStopWidth,
							  double dPassRipple,double dStopAttenuation)
{
	short i = 0;
	double delta1 = dPassRipple;
	double delta2 = pow(10.0,-dStopAttenuation/20);

	short nfcns = m_nLPFLen/2+1;

	// Grid density (should be at least 16)
	short nGrid = 16;  	
	double delf = 1.0/(nGrid*nfcns);
	// compute grid points	
	short nPassLen = static_cast<short>(dPassWidth/delf+1);
	short nStopLen =   static_cast<short>((1-dStopWidth)/delf + 1);
	short nGridLen = nPassLen+nStopLen;
	//need more grid points
	if (nGridLen<m_nLPFLen)
	{
		nGrid = 16*4;
		delf = 1.0/(nGrid*nfcns);
		nPassLen = static_cast<short>(dPassWidth/delf + 1);
		nStopLen = static_cast<short>((1-dStopWidth)/delf + 1);
		nGridLen = nPassLen+nStopLen;
	}

	// Generate frequency grid
	double *pGridBuf = new double[nGridLen];

	for(i=0;i<nPassLen;i++)
	{
		pGridBuf[i] = i*delf;
	}
	pGridBuf[nPassLen-1] = dPassWidth;
	for(i=0;i<nStopLen;i++)
	{
		pGridBuf[i+nPassLen] = dStopWidth+i*delf;
	}
	pGridBuf[nGridLen-1] = 1.0;
	for(i=0;i<nGridLen;i++)
	{
		pGridBuf[i] /= 2;
	}

	double dDelGrid = 1.0*(nGridLen-1)/nfcns;
	//获取nfcns+1个交错点组的初始位置//
	short *pIext = new short[nfcns+2];
	for(i=0;i<nfcns;i++)
	{
		pIext[i]= static_cast<short>(dDelGrid*i);
	}
	pIext[nfcns] = nGridLen-1;
	pIext[nfcns+1] = nGridLen;
	//理想滤波器冲击响应//
	double *pIdH = new double[nGridLen];
    memset(pIdH, 0, nGridLen*sizeof(double));
	for(i=0;i<nPassLen;i++)
	{
		pIdH[i] = 1.0;
	}
	//加权函数//
	double *pIdW = new double[nGridLen];
    memset(pIdW, 0, nGridLen*sizeof(double));
	for(i=0;i<nPassLen;i++)
	{
		pIdW[i] = 1.0;
	}
	for(i=nPassLen;i<nGridLen;i++)
	{
		pIdW[i] = delta1/delta2;
	}

	short nz = nfcns + 1;
	short nzz = nfcns + 2;

	// Remez exchange loop
	short itrmax = 250;	
	short niter = 0;
	short jchnge = 1;
	short jet = static_cast<short>(1.0*(nfcns - 1)/15) + 1;
//	double devl = -1.0;
	double *pAd = new double[nz];
    memset(pAd, 0, nz*sizeof(double));
	double *pY = new double[nz];
    memset(pY, 0, nz*sizeof(double));
	double *pC = new double[nz];
    memset(pC, 0, nz*sizeof(double));
	double *pXCos = new double[nzz];
    memset(pXCos, 0, nzz*sizeof(double));

	short l,nu;
	double q,y,xx,dnum,dden,dev,dErr;
    dErr = .0;

	while (jchnge > 0)
	{
		pIext[nzz-1] = nGridLen;
		niter = niter + 1;
		if( niter > itrmax )break;	

		for (i=0;i<nz;i++)
		{
			l = pIext[i];
			pXCos[i] = cos(2*PI*pGridBuf[l]);
		}

		for(i=0;i<nz;i++)
		{
			q = pXCos[i];
			y = 1.0;
			for (short j=0;j<jet;j++)
			{				
				for (short k=j;k<nz;k+=jet)
				{
					xx = 2*(q-pXCos[k]);
					if( fabs(xx)>1e-10 )
					{
						y *= xx;
					}
				}				
			}			
			pAd[i] = 1.0/y;
		}

		dnum = .0;
		dden = .0;
		for(i=0;i<nz;i++)
		{
			l = pIext[i];
			dnum += pAd[i]*pIdH[l];
			dden += pow(-1.0,i)*pAd[i]/pIdW[l];
		}
		dev = dnum/dden;		// delta2

		nu = 1;
		if( dev > 0 )nu = -1;
		dev = -nu*dev;

		for(i=0;i<nz;i++)
		{
			l = pIext[i];
			pY[i] = pIdH[l]+nu*pow(-1.0,i)*dev/pIdW[l];
		}

		//计算误差// 
	//	double devl = dev;		
		short k1 = pIext[0];
		short knz = pIext[nz-1];
		short klow = 0;		
		short flag34 = 1;
		short nut = -nu;
		short kup,flag;
		double dy1,dcomp,dtemp,dsum;
		dy1 = 0;
		dcomp = 0;
		jchnge = 0;
		dtemp = 0;
		dcomp = 0;
		short j = 0;
		while (j<nzz-1)
		{
			kup = pIext[j+1];
			l = pIext[j] + 1;
			nut = -nut;

			if(j == 1)dy1 = dcomp;			
			dcomp = dev;
			flag = 1;

			if (l<kup)
			{				
				// gee
				dsum = .0;
				dErr = .0;
				for (i=0;i<nz;i++)
				{
					pC[i] = pAd[i]/(cos(2*PI*pGridBuf[l])-pXCos[i]);
					dsum += pC[i];

					dErr += pC[i]*pY[i];
				}				
				dErr = (dErr/dsum - pIdH[l])*pIdW[l];

				dtemp = nut*dErr - dcomp;

				if( dtemp>0 )
				{
					dcomp = nut*dErr;
					l = l + 1;

					while (l<kup)
					{
						// gee
						dsum = .0;
						dErr = .0;
						for (i=0;i<nz;i++)
						{
							pC[i] = pAd[i]/(cos(2*PI*pGridBuf[l])-pXCos[i]);
							dsum += pC[i];

							dErr += pC[i]*pY[i];
						}				
						dErr = (dErr/dsum - pIdH[l])*pIdW[l];

						dtemp = nut*dErr - dcomp;

						if( dtemp>0 )
						{
							dcomp = nut*dErr;
							l = l + 1;
						}
						else break;
					}

					pIext[j] = l - 1;
					j = j + 1;
					klow = l - 1;
					jchnge = jchnge + 1;
					flag = 0;
				}
			}

			if (flag)
			{
				l = l - 2;
				//				 if (l>0)l=l-1;

				while (l>klow)
				{
					// gee
					dsum = .0;
					dErr = .0;
					for (i=0;i<nz;i++)
					{
						pC[i] = pAd[i]/(cos(2*PI*pGridBuf[l])-pXCos[i]);
						dsum += pC[i];

						dErr += pC[i]*pY[i];
					}				
					dErr = (dErr/dsum - pIdH[l])*pIdW[l];

					dtemp = nut*dErr - dcomp;

					if( dtemp > 0 || jchnge > 0 )
						break;

					l = l - 1;
				}

				if(l<=klow)
				{
					l = pIext[j] + 1;
					if(jchnge>0)
					{
						pIext[j] = l - 1;
						j = j + 1;
						klow = l - 1;
						jchnge = jchnge + 1;
					}
					else
					{
						l = l + 1;
						while (l<kup)
						{
							// gee
							dsum = .0;
							dErr = .0;
							for (i=0;i<nz;i++)
							{
								pC[i] = pAd[i]/(cos(2*PI*pGridBuf[l])-pXCos[i]);
								dsum += pC[i];

								dErr += pC[i]*pY[i];
							}				
							dErr = (dErr/dsum - pIdH[l])*pIdW[l];

							dtemp = nut*dErr - dcomp;
							if(dtemp > 0)
								break;
							l = l + 1;
						}

						if( l < kup && dtemp > 0 )
						{
							dcomp = nut*dErr;
							l = l + 1;

							while (l<klow)
							{
								// gee
								dsum = .0;
								dErr = .0;
								for (i=0;i<nz;i++)
								{
									pC[i] = pAd[i]/(cos(2*PI*pGridBuf[l])-pXCos[i]);
									dsum += pC[i];

									dErr += pC[i]*pY[i];
								}				
								dErr = (dErr/dsum - pIdH[l])*pIdW[l];

								dtemp = nut*dErr - dcomp;
								if(dtemp > 0)
								{
									dcomp = nut*dErr;
									l = l + 1;
								}
								else
									break;
							}

							pIext[j] = l - 1;
							j = j + 1;
							klow = l - 1;
							jchnge = jchnge + 1;
						}
						else
						{
							klow = pIext[j];
							j = j + 1;
						}
					}
				}
				else if(dtemp>0)
				{
					dcomp = nut*dErr;
					l = l - 1;
					while (l>=klow)
					{
						// gee
						dsum = .0;
						dErr = .0;
						for (i=0;i<nz;i++)
						{
							pC[i] = pAd[i]/(cos(2*PI*pGridBuf[l])-pXCos[i]);
							dsum += pC[i];

							dErr += pC[i]*pY[i];
						}				
						dErr = (dErr/dsum - pIdH[l])*pIdW[l];

						dtemp = nut*dErr - dcomp;
						if(dtemp > 0)
						{
							dcomp = nut*dErr;
							l = l - 1;
						}
						else
							break;
					}

					klow = pIext[j];
					pIext[j] = l + 1;
					j = j + 1;
					jchnge = jchnge + 1;
				}
				else
				{
					klow = pIext[j];
					j = j + 1;
				}
			}
		}

		double ynz;
		short nut1,luck;
		nut1 = 0;
		luck = 0;
		while ( j==nzz-1 )
		{
			ynz = dcomp;
			k1 = k1<pIext[0]? k1:pIext[0];
			knz = knz>pIext[nz-1] ? knz:pIext[nz-1];
			nut1 = nut;
			nut = -nu;
			l = 0;
			kup = k1;
			dcomp = ynz*1.00001;
			luck = 1;
			flag = 1;
			//l = l + 1;

			while (l<kup)
			{
				// gee
				dsum = .0;
				dErr = .0;
				for (i=0;i<nz;i++)
				{
					pC[i] = pAd[i]/(cos(2*PI*pGridBuf[l])-pXCos[i]);
					dsum += pC[i];

					dErr += pC[i]*pY[i];
				}				
				dErr = (dErr/dsum - pIdH[l])*pIdW[l];

				dtemp = nut*dErr - dcomp;
				if(dtemp > 0)
				{
					dcomp = nut*dErr;
					j = nzz-1;
					l = l + 1;

					while (l<kup)
					{
						// gee
						dsum = .0;
						dErr = .0;
						for (i=0;i<nz;i++)
						{
							pC[i] = pAd[i]/(cos(2*PI*pGridBuf[l])-pXCos[i]);
							dsum += pC[i];

							dErr += pC[i]*pY[i];
						}				
						dErr = (dErr/dsum - pIdH[l])*pIdW[l];

						dtemp = nut*dErr - dcomp;
						if(dtemp > 0)
						{
							dcomp = nut*dErr;
							l = l + 1;
						}
						else
							break;
					}

					pIext[j] = l - 1;
					j = j + 1;
					jchnge = jchnge + 1;
					flag = 0;
					break;
				}
				l = l + 1;
			}

			if (flag)
			{
				luck = 6;
				l = nGridLen;
				klow = knz;
				nut = -nut1;
				dcomp = dy1*1.00001;
				l = l - 1;

				while (l>klow)
				{
					// gee
					dsum = .0;
					dErr = .0;
					for (i=0;i<nz;i++)
					{
						pC[i] = pAd[i]/(cos(2*PI*pGridBuf[l])-pXCos[i]);
						dsum += pC[i];

						dErr += pC[i]*pY[i];
					}				
					dErr = (dErr/dsum - pIdH[l])*pIdW[l];

					dtemp = nut*dErr - dcomp;
					if (dtemp>0)
					{
						j = nzz-1;
						dcomp = nut*dErr;
						luck = luck + 10;
						l = l - 1;

						while(l>klow)
						{
							// gee
							dsum = .0;
							dErr = .0;
							for (i=0;i<nz;i++)
							{
								pC[i] = pAd[i]/(cos(2*PI*pGridBuf[l])-pXCos[i]);
								dsum += pC[i];

								dErr += pC[i]*pY[i];
							}				
							dErr = (dErr/dsum - pIdH[l])*pIdW[l];

							dtemp = nut*dErr - dcomp;
							if(dtemp > 0)
							{
								dcomp = nut*dErr;
								l = l - 1;
							}
							else
								break;
						}
						pIext[j] = l + 1;
						j = j + 1;
						jchnge = jchnge + 1;
						flag = 0;
						break;
					}
					l = l - 1;
				}

				if (flag)
				{
					flag34 = 0;

					if( luck != 6 )
					{
						for (i=nz-1;i>0;i--)
						{
							pIext[i]=pIext[i-1];
						}
						pIext[0] = k1;	
						jchnge = jchnge + 1;
					}
					break;
				}
			}
		}
		///
		if( flag34 && j > nzz-1 )
		{
			if(luck > 9)
			{
				for (i=0;i<nfcns;i++)
				{
					pIext[i]=pIext[i+1];
				}
				pIext[nfcns] = pIext[nzz-1];
				pIext[nz] = pIext[nzz-1];

				jchnge = jchnge + 1;
			}
			else
			{
				dy1 = dy1>dcomp ? dy1:dcomp;
				k1 = pIext[nzz-1];
				l = nGridLen;
				klow = knz;
				nut = -nut1;
				dcomp = dy1*1.00001;
				l = l - 1;

				while (l>klow)
				{
					// gee
					dsum = .0;
					dErr = .0;
					for (i=0;i<nz;i++)
					{
						pC[i] = pAd[i]/(cos(2*PI*pGridBuf[l])-pXCos[i]);
						dsum += pC[i];

						dErr += pC[i]*pY[i];
					}				
					dErr = (dErr/dsum - pIdH[l])*pIdW[l];

					dtemp = nut*dErr - dcomp;
					if(dtemp > 0)
					{
						j = nzz-1;
						dcomp = nut*dErr;
						luck = luck + 10;
						l = l - 1;

						while(l>klow)
						{
							// gee
							dsum = .0;
							dErr = .0;
							for (i=0;i<nz;i++)
							{
								pC[i] = pAd[i]/(cos(2*PI*pGridBuf[l])-pXCos[i]);
								dsum += pC[i];

								dErr += pC[i]*pY[i];
							}				
							dErr = (dErr/dsum - pIdH[l])*pIdW[l];

							dtemp = nut*dErr - dcomp;
							if(dtemp > 0)
							{
								dcomp = nut*dErr;
								l = l - 1;
							}
							else
								break;
						}
						pIext[j] = l + 1;
						jchnge = jchnge + 1;
						for (i=0;i<nfcns;i++)
						{
							pIext[i]=pIext[i+1];
						}
						pIext[nfcns] = pIext[nzz-1];
						pIext[nz] = pIext[nzz-1];
						break;
					}
					l = l - 1;
				}

				if(luck != 6) 
				{					 					
					for (i=nz-1;i>0;i--)
					{
						pIext[i]=pIext[i-1];
					}
					pIext[0] = k1;	
					jchnge = jchnge + 1;
				}
			}
		}		
	}

	// 释放资源
	delete []pIdH;
	delete []pIdW;
	delete []pIext;	

	//Inverse Fourier transformation	

	double fsh = 1.0e-6;	
	short cn = 2*nfcns - 1;
	delf = 1.0/cn;
	l = 0;
	pXCos[nz] = -2;

	double *pCoff = new double[nfcns];
    memset(pCoff, 0, nfcns*sizeof(double));

	double ft,xt,xe;
	for (i=0;i<nfcns;i++)
	{
		ft = i*delf;
		xt = cos(2*PI*ft);

		xe = pXCos[l];
		while( (xt <= xe) && (xe-xt >= fsh) ) 
		{
			l = l + 1;
			xe = pXCos[l];
		}
		if( fabs(xt-xe) < fsh )
		{
			pCoff[i] = pY[l];
		}
		else
		{
			pGridBuf[0] = ft;

			// gee
			double dsum = .0;
			for (short k=0;k<nz;k++)
			{
				pC[k]=pAd[k]/(cos(2*PI*ft)-pXCos[k]);
				dsum += pC[k];
				pCoff[i] += pC[k]*pY[k];
			}
			pCoff[i] /= dsum;
		}		

		l = l-1>0 ? (l-1):0;		
	}

	//释放资源
	delete []pXCos;
	delete []pAd;
	delete []pC;
	delete []pY;
	delete []pGridBuf;


	dden = 2*PI/cn; //

	double *alpha = new double[nfcns];
    memset(alpha, 0, nfcns*sizeof(double));
	//	short nm1 = nfcns-1;
	for(i=0;i<nfcns;i++)
	{	
		dnum = i*dden;
		if( nfcns-1 < 1 )
		{
			alpha[i] = pCoff[0];
		}
		else
		{
			alpha[i] = pCoff[0];
			for (short k=1;k<nfcns;k++)
			{
				alpha[i] += 2*cos(dnum*k)*pCoff[k];
			}			
		}
	}

	//释放资源	
	delete []pCoff;

	alpha[0] /= cn;
	for(i=1;i<nfcns;i++)
	{
		alpha[i] *= 2.0/cn;
	}

	for(i=0;i<nfcns-1;i++)
	{
		m_pLPFBuf[i] = 0.5*alpha[nfcns-1-i];
		m_pLPFBuf[cn-1-i] = 0.5*alpha[nfcns-1-i];
	}
	m_pLPFBuf[nfcns-1] = alpha[0];

	//释放资源
	delete []alpha;	
}

void CDataFIRDF::Filter(double *pDataI,double* pDataQ,int nDataLen)
{
	int i;
	double dTempI,dTempQ;
	for( int k=0;k < nDataLen;k++ )
	{
		for( i=m_nLPFLen-1;i>0;i-- )
		{
			*(m_pDataI+i) = *(m_pDataI+i-1);
			*(m_pDataQ+i) = *(m_pDataQ+i-1);
		}

		m_pDataI[0] = *(pDataI+k);
		m_pDataQ[0] = *(pDataQ+k);

		dTempI = 0.0;
		dTempQ = 0.0;
		for( i=0;i<m_nLPFLen;i++ )
		{
			dTempI += m_pLPFBuf[i]*m_pDataI[i];
			dTempQ += m_pLPFBuf[i]*m_pDataQ[i];
		}

		*(pDataI + k) = dTempI;
		*(pDataQ + k) = dTempQ;
	}
}

void CDataFIRDF::Filter( double *pData,int nDataLen )
{
	int i;
	double dTemp;
	for( int k=0;k < nDataLen;k++ )
	{
		for( i=m_nLPFLen-1;i>0;i-- )
		{
			*(m_pDataI+i) = *(m_pDataI+i-1);
		}

		m_pDataI[0] = *(pData+k);

		dTemp = 0.0;
		for( i=0;i<m_nLPFLen;i++ )
		{
			dTemp += m_pLPFBuf[i]*m_pDataI[i];
		}

		*(pData + k) = dTemp;
	}
}
