#include "rc_rs.h"
#include "rc_dict.h"
#include <cassert>
#include <set>
namespace RC_DICTCODEC {

	Galois_domain::Galois_domain(const char g[], size_t m, bool b07oppo)
		:_m(1 << m)
		, _cm(m)
		, m_bgxop(b07oppo)
		, startOrder(1)
	{
		for (size_t i = 0; i<m + 1; ++i)
			_g.push_back(g[i]);
		fill_galois_elems();
	}
	bool Galois_domain::fill_galois_elems()
	{
		unsigned short v = 1;
		Galois_elems.clear();
		unsigned short gn = 0;
		for (size_t i = 0; i<_cm + 1; ++i)
		{
			gn <<= 1;
			if (!m_bgxop)
				gn += _g[_cm - i] ? 1 : 0;
			else
				gn += _g[i] ? 1 : 0;
		}
		//printf ("g = %02X\n0\n",gn);

		for (size_t i = 0; i<_m; ++i)
			Galois_index.push_back(0);
		Galois_index[0] = -1;
		std::set<unsigned short> s;

		for (unsigned short i = 0; i<_m - 1; ++i)
		{
			//printf ("a%d = %02X\n",i,v);
			assert(s.find(v) == s.end());
			s.insert(v);
			Galois_index[v] = i;
			//a^i
			Galois_elems.push_back(v);
			v <<= 1;
			if (v & (1 << _cm))
			{
				v ^= gn;
				assert(v < (1 << _cm));
			}
		}
		Galois_elems.push_back(0);
		return true;

	}
	std::string Galois_domain::printGF()
	{
		char buf[1024];
		std::string res;
		const size_t sz = Galois_elems.size();
		for (size_t i = 0; i + 1<sz; ++i)
		{
			snprintf(buf, 1024, "a^%d=", i);
			res += buf;
			size_t val = Galois_elems[i];
			bool ou = false;

			for (size_t tj = 0; tj<_cm; ++tj)
			{
				size_t j = _cm - 1 - tj;
				if (!((val >> j) % 2))
					continue;
				if (ou)
					res += "+";
				if (j == 0)
					res += "1";
				else if (j == 1)
					res += "x";
				else
				{
					snprintf(buf, 1024, "x^%d", j);
					res += buf;
				}


				ou = true;
			}
			res += "=";
			for (size_t tj = 0; tj<_cm; ++tj)
			{
				size_t j = _cm - 1 - tj;
				snprintf(buf, 1024, "%d", (val >> j) % 2);
				res += buf;
				ou = true;
			}
			res += "\n";
		}

		return res;
	}

	std::vector<unsigned short> Galois_domain::geneGx(unsigned short startA, size_t Nz)
	{
		std::vector<unsigned short> res;
		res.push_back(1);
		res.push_back(startA);
		unsigned short bA = startA;
		startOrder = IndexOf()[startA];
		for (size_t i = 1; i<Nz; ++i)
		{
			res.push_back(0);//x * G
			bA = mod_m_cross(bA, Galois_elems[1]);
			for (size_t j = i + 1; j>0; --j)
			{
				unsigned short v = mod_m_cross(res[j - 1], bA);
				//printf ("%d,%d:",res[j-1],bA);
				res[j] = mod_m_add(res[j], v);
			}
			//printf ("Gx(%d)=",i);
			//for (int i=0;i<res.size();++i)
			//	printf("%2X",res[i]);
			//printf ("\n");

		}
		return res;
	}


	rc_codec_rs::rc_codec_rs(size_t n, size_t k, unsigned short G[], size_t len, const Galois_domain & domain, bool byteOppo)
		:rc_codec_base<unsigned short>(n, k, 1 << domain.m())
		, _d(domain)
		, m_bGxop(byteOppo)
		, _cm(domain.m())
	{
		for (size_t i = 0; i<len; ++i)
			_G.push_back(G[i]);
		for (size_t i = 0; i<_n - _k; ++i)
			initial_status.push_back(0);
        int proc = 6;
		for (size_t i = 0; i<n*proc; ++i)
			_rawbuf.push_back(0);
		assert((_n - _k) % 2 == 0);
		_t = (_n - _k) / 2;
	}
	rc_codec_rs::~rc_codec_rs()
	{

	}
	void rc_codec_rs::set_initial_status(unsigned short status[/*n-k*/])
	{
		initial_status.clear();
		for (size_t i = 0; i<_n - _k; ++i)
			initial_status.push_back(status[i]);
	}


	//所有译码器必须实现下列函数
	/*!
	* \brief encode 编码器。输入为数据，k符号，输出为编码，n符号。
	* \param data  数据
	* \param code  编码。
	* \return 自定义。
	* \attention data和code的指针地址可能为同一个地址，所以实现者请酌情考虑。
	*/
	int rc_codec_rs::encode(const unsigned short data[/*k*/], unsigned short code[/*n*/])
	{
		const size_t polym = _n - _k;
        unsigned short * _buf = _rawbuf.data() + 0*_n;
		for (size_t i = 0; i<_k; ++i)
		{
			_buf[i] = data[i];
			code[i] = data[i];
		}
		for (size_t i = 0; i<polym; ++i)
			_buf[i + _k] = initial_status[i];

		const size_t pln = _G.size();
		const unsigned short * pli = _G.data();
		assert(pln == polym + 1);
		for (size_t i = 0; i<_k; ++i)
		{
			if (_buf[i])
			{
				unsigned short cc = _buf[i];
				if (m_bGxop)
				{
					for (size_t j = 0; j<pln; ++j)
					{
						_buf[i + j] = mod_m_add(
							_buf[i + j],
							mod_m_cross(cc, pli[j])
						);
					}
				}
				else {
					for (size_t j = 0; j<pln; ++j)
					{
						_buf[i + j] = mod_m_add(
							_buf[i + j],
							mod_m_cross(cc, pli[pln - 1 - j])
						);
					}
				}
			}
		}
		for (size_t i = 0; i<polym; ++i)
			code[i + _k] = _buf[i + _k];


		return 0;
	}
	/*!
	* \brief encode 计算校验。输入为编码，n符号，输出为校验后的编码，n符号。
	* \param data  编码
	* \param code  编码校验结果，可能包含信息部分。
	* \return 自定义。
	* \attention code和check的指针可能为同一个内存，请注意设计。
	*/
	int rc_codec_rs::chcode(const unsigned short code[/*n*/], unsigned short check[/*n*/])
	{
		const size_t polym = _n - _k;
        unsigned short * _buf = _rawbuf.data() + 0*_n;
		for (size_t i = 0; i<_n; ++i)
		{
			_buf[i] = code[i];
			check[i] = code[i];
		}
		const size_t pln = _G.size();
		const unsigned short * pli = _G.data();
		//printf ("\npln=%d, polym +1 = %d\n",pln,polym+1);
		assert(pln == polym + 1);
		for (size_t i = 0; i<_k; ++i)
		{
			if (_buf[i])
			{
				unsigned short cc = _buf[i];
				if (m_bGxop)
				{
					for (size_t j = 0; j<pln; ++j)
					{
						_buf[i + j] = mod_m_add(
							_buf[i + j],
							mod_m_cross(cc, pli[j])
						);
					}
				}
				else {
					for (size_t j = 0; j<pln; ++j)
					{
						_buf[i + j] = mod_m_add(
							_buf[i + j],
							mod_m_cross(cc, pli[pln - 1 - j])
						);
					}
				}
			}
		}
		for (size_t i = 0; i<polym; ++i)
			check[i + _k] = _buf[i + _k];

		return 0;
	}
	/*!
	* \brief hash   从校验结果计算错误图案的唯一字符串。
	* \param check  校验后的结果，可能包含信息部分。
	* \return 错误图案字符串
	*/
	std::string rc_codec_rs::hash(const unsigned short check[/*n*/]) const
	{
		unsigned char mask[] = "0123456789ABCDEFG";
		std::string res;
		for (size_t i = 0; i<_n - _k; ++i)
		{
			res.push_back(mask[check[_k + i] & 0x0F]);
			res.push_back(mask[(check[_k + i] >> 4) & 0x0F]);
			res.push_back(mask[(check[_k + i] >> 8) & 0x0F]);
			res.push_back(mask[(check[_k + i] >> 12) & 0x0F]);
		}
		return res;
	}


	int rc_codec_rs::decode_rs(const unsigned short code[/*n*/], unsigned short correct[/*n*/])
		/* 参考Simon Rock liff 在1991年6月26日的开源代码版本。需要注意的是，该算法中数组0下标表示的是多项式的最低位
		* 我们已经在函数开始和结束做了转换，以弥和此差异。
		* assume we have received bits grouped into mm-bit symbols in recd[i],
		i=0..(_n-1),  and recd[i] is index form (ie as powers of alpha).
		We first compute the 2*_t syndromes by substituting alpha**i into rec(X) and
		evaluating, storing the syndromes in s[i], i=1..2tt (leave s[0] zero) .
		Then we use the Berlekamp iteration to find the error location polynomial
		elp[i].   If the degree of the elp is >_t, we cannot correct all the errors
		and hence just put out the information symbols uncorrected. If the degree of
		elp is <=_t, we substitute alpha**i , i=1..n into the elp to get the roots,
		hence the inverse roots, the error location numbers. If the number of errors
		located does not equal the degree of the elp, we have more than _t errors
		and cannot correct them.  Otherwise, we then solve for the error value at
		the error location and correct the error.  The procedure is that found in
		Lin and Costello. For the cases where the number of errors is known to be too
		large to correct, the information symbols as received are output (the
		advantage of systematic encoding is that hopefully some of the information
		symbols will be okay and that if we are in luck, the errors are in the
		parity part of the transmitted codeword).  Of course, these insoluble cases
		can be returned as error flags to the calling routine if desired.   */
	{
		int i = 0, j = 0, u = 0, q = 0;

		if (buf_elp.size() != (_n - _k + 2) * (_n - _k))
			buf_elp.resize((_n - _k + 2) * (_n - _k), 0);
		if (buf_d.size() != _n - _k + 2)
			buf_d.resize(_n - _k + 2, 0);
		if (buf_l.size() != _n - _k + 2)
			buf_l.resize(_n - _k + 2, 0);
		if (buf_u_lu.size() != _n - _k + 2)
			buf_u_lu.resize(_n - _k + 2, 0);
		if (buf_s.size() != _n - _k + 1)
			buf_s.resize(_n - _k + 1, 0);
		if (buf_root.size() != _t)
			buf_root.resize(_t, 0);
		if (buf_loc.size() != _t)
			buf_loc.resize(_t, 0);
		if (buf_z.size() != _t + 1)
			buf_z.resize(_t + 1, 0);
		if (buf_err.size() != _n)
			buf_err.resize(_n, 0);
		if (buf_reg.size() != _t + 1)
			buf_reg.resize(_t + 1, 0);
		if (buf_recd.size() != _n)
			buf_recd.resize(_n, 0);


		int* elp = buf_elp.data(),
			*d = buf_d.data(),
			*l = buf_l.data(),
			*u_lu = buf_u_lu.data(),
			*s = buf_s.data();
		int * root = buf_root.data(),
			*loc = buf_loc.data(),
			*z = buf_z.data(),
			*err = buf_err.data(),
			*reg = buf_reg.data();
		int * recd = buf_recd.data();
		int count = 0, syn_error = 0;


		for (int i = 0; i<_n; ++i)
		{
			recd[i] = _d.IndexOf()[code[_n - 1 - i]];
			if (recd[i] == 0xffff)
				recd[i] = -1;
			//printf ("%02X,",recd[i]);
		}

		/* first form the syndromes */
		for (i = 1; i <= _n - _k; i++)
		{
			s[i] = 0;
			for (j = 0; j<_n; j++)
				if (recd[j] != -1)
					s[i] ^= _d.Alpha_to()[(recd[j] + (i + _d.get_startA() - 1)*j) % _n];      /* recd[j] in index form */
																							  /* convert syndrome from polynomial form to index form  */
			if (s[i] != 0)  syn_error = 1;        /* set flag if non-zero syndrome => error */
			s[i] = _d.IndexOf()[s[i]];
		};

		if (syn_error)       /* if errors, try and correct */
		{
			/* compute the error location polynomial via the Berlekamp iterative algorithm,
			following the terminology of Lin and Costello :   d[u] is the 'mu'th
			discrepancy, where u='mu'+1 and 'mu' (the Greek letter!) is the step number
			ranging from -1 to 2*_t (see L&C),  l[u] is the
			degree of the elp at that step, and u_l[u] is the difference between the
			step number and the degree of the elp.
			*/
			/* initialise table entries */
			d[0] = 0;           /* index form */
			d[1] = s[1];        /* index form */
			elp[0 * (_n - _k) + 0] = 0;      /* index form */
			elp[1 * (_n - _k) + 0] = 1;      /* polynomial form */
			for (i = 1; i<_n - _k; i++)
			{
				elp[0 * (_n - _k) + i] = -1;   /* index form */
				elp[1 * (_n - _k) + i] = 0;   /* polynomial form */
			}
			l[0] = 0;
			l[1] = 0;
			u_lu[0] = -1;
			u_lu[1] = 0;
			u = 0;

			do
			{
				u++;
				if (d[u] == -1)
				{
					l[u + 1] = l[u];
					for (i = 0; i <= l[u]; i++)
					{
						elp[(u + 1)* (_n - _k) + i] = elp[u* (_n - _k) + i];
						elp[u* (_n - _k) + i] = _d.IndexOf()[elp[u* (_n - _k) + i]];
					}
				}
				else
					/* search for words with greatest u_lu[q] for which d[q]!=0 */
				{
					q = u - 1;
					while ((d[q] == -1) && (q>0)) q--;
					/* have found first non-zero d[q]  */
					if (q>0)
					{
						j = q;
						do
						{
							j--;
							if ((d[j] != -1) && (u_lu[q]<u_lu[j]))
								q = j;
						} while (j>0);
					};

					/* have now found q such that d[u]!=0 and u_lu[q] is maximum */
					/* store degree of new elp polynomial */
					if (l[u]>l[q] + u - q)  l[u + 1] = l[u];
					else  l[u + 1] = l[q] + u - q;

					/* form new elp(x) */
					for (i = 0; i<_n - _k; i++)    elp[(u + 1)* (_n - _k) + i] = 0;
					for (i = 0; i <= l[q]; i++)
						if (elp[q* (_n - _k) + i] != -1)
							elp[(u + 1)* (_n - _k) + i + u - q] = _d.Alpha_to()[(d[u] + _n - d[q] + elp[q* (_n - _k) + i]) % _n];
					for (i = 0; i <= l[u]; i++)
					{
						elp[(u + 1)* (_n - _k) + i] ^= elp[u* (_n - _k) + i];
						elp[u* (_n - _k) + i] = _d.IndexOf()[elp[u* (_n - _k) + i]];  /*convert old elp value to index*/
					}
				}
				u_lu[u + 1] = u - l[u + 1];

				/* form (u+1)th discrepancy */
				if (u<_n - _k)    /* no discrepancy computed on last iteration */
				{
					if (s[u + 1] != -1)
						d[u + 1] = _d.Alpha_to()[s[u + 1]];
					else
						d[u + 1] = 0;
					for (i = 1; i <= l[u + 1]; i++)
						if ((s[u + 1 - i] != -1) && (elp[(u + 1)* (_n - _k) + i] != 0))
							d[u + 1] ^= _d.Alpha_to()[(s[u + 1 - i] + _d.IndexOf()[elp[(u + 1)* (_n - _k) + i]]) % _n];
					d[u + 1] = _d.IndexOf()[d[u + 1]];    /* put d[u+1] into index form */
				}
			} while ((u<_n - _k) && (l[u + 1] <= _t));

			u++;
			if (l[u] <= _t)         /* can correct error */
			{
				/* put elp into index form */
				for (i = 0; i <= l[u]; i++)   elp[u* (_n - _k) + i] = _d.IndexOf()[elp[u* (_n - _k) + i]];

				/* find roots of the error location polynomial */
				for (i = 1; i <= l[u]; i++)
					reg[i] = elp[u* (_n - _k) + i];
				count = 0;
				for (i = 1; i <= _n; i++)
				{
					q = 1;
					for (j = 1; j <= l[u]; j++)
						if (reg[j] != -1)
						{
							reg[j] = (reg[j] + j) % _n;
							q ^= _d.Alpha_to()[reg[j]];
						};
					if (!q)        /* store root and error location number indices */
					{
						root[count] = i;
						loc[count] = _n - i;
						count++;
					};
				};
				if (count == l[u])    /* no. roots = degree of elp hence <= _t errors */
				{
					/* form polynomial z(x) */
					for (i = 1; i <= l[u]; i++)        /* Z[0] = 1 always - do not need */
					{
						if ((s[i] != -1) && (elp[u* (_n - _k) + i] != -1))
							z[i] = _d.Alpha_to()[s[i]] ^ _d.Alpha_to()[elp[u* (_n - _k) + i]];
						else if ((s[i] != -1) && (elp[u* (_n - _k) + i] == -1))
							z[i] = _d.Alpha_to()[s[i]];
						else if ((s[i] == -1) && (elp[u* (_n - _k) + i] != -1))
							z[i] = _d.Alpha_to()[elp[u*(_n - _k) + i]];
						else
							z[i] = 0;
						for (j = 1; j<i; j++)
							if ((s[j] != -1) && (elp[u*(_n - _k) + i - j] != -1))
								z[i] ^= _d.Alpha_to()[(elp[u* (_n - _k) + i - j] + s[j]) % _n];
						z[i] = _d.IndexOf()[z[i]];         /* put into index form */
					};

					/* evaluate errors at locations given by error location numbers loc[i] */
					for (i = 0; i<_n; i++)
					{
						err[i] = 0;
						if (recd[i] != -1)        /* convert recd[] to polynomial form */
							recd[i] = _d.Alpha_to()[recd[i]];
						else  recd[i] = 0;
					}
					for (i = 0; i<l[u]; i++)    /* compute numerator of error term first */
					{
						err[loc[i]] = 1;       /* accounts for z[0] */
						for (j = 1; j <= l[u]; j++)
							if (z[j] != -1)
								err[loc[i]] ^= _d.Alpha_to()[(z[j] + j*root[i]) % _n];
						if (err[loc[i]] != 0)
						{
							err[loc[i]] = _d.IndexOf()[err[loc[i]]];
							q = 0;     /* form denominator of error term */
							for (j = 0; j<l[u]; j++)
								if (j != i)
									q += _d.IndexOf()[1 ^ _d.Alpha_to()[(loc[j] + root[i]) % _n]];
							q = q % _n;
							err[loc[i]] = _d.Alpha_to()[(err[loc[i]] - q + _n) % _n];
							recd[loc[i]] ^= err[loc[i]];  /*recd[i] must be in polynomial form */
						}
					}
				}
				else    /* no. roots != degree of elp => >_t errors and cannot solve */
					for (i = 0; i<_n; i++)        /* could return error flag if desired */
						if (recd[i] != -1)        /* convert recd[] to polynomial form */
							recd[i] = _d.Alpha_to()[recd[i]];
						else  recd[i] = 0;     /* just output received codeword as is */
			}
			else         /* elp has degree has degree >_t hence cannot solve */
				for (i = 0; i<_n; i++)       /* could return error flag if desired */
					if (recd[i] != -1)        /* convert recd[] to polynomial form */
						recd[i] = _d.Alpha_to()[recd[i]];
					else  recd[i] = 0;     /* just output received codeword as is */
		}
		else
			/* no non-zero syndromes => no errors: output received codeword */
			for (i = 0; i<_n; i++)
				if (recd[i] != -1)        /* convert recd[] to polynomial form */
					recd[i] = _d.Alpha_to()[recd[i]];
				else  recd[i] = 0;

				//由于上面算法根从A（1）开始，这里要做旋转。
				if (_d.get_startA() == 1)
				{
					for (int i = 0; i<_n; ++i)
						correct[i] = recd[_n - 1 - i];

				}
				else
				{
					for (int i = 0; i<_n; ++i)
					{
						correct[i] = recd[_n - 1 - i];
						if (correct[i] != code[i])
						{
							const int turn = (_d.get_startA() - 1) * (i + 1) % _n;
							unsigned short recov = _d.mod_m_cross(correct[i] ^ code[i], _d(turn));
							correct[i] = recov ^ code[i];
						}
					}
				}

				return 0;
	}

}
