#include "LdpcDecode.h"
/*************��Ϣ�ڵ��У��ڵ�Ĺ��캯��************/
NodeData::parentnode::parentnode(int maxc, int length) :
	size(maxc), code_length(length)
{
	cmessage.reserve(code_length);
	for (size_t i = 0; i != code_length; ++i)
		cmessage.push_back(0.0);
}

NodeData::childnode::childnode(int maxr, int length) :
	size(maxr), code_length(length)
{
	rmessage.reserve(code_length);
	for (size_t i = 0; i != code_length; ++i)
		rmessage.push_back(0.0);
}

/******************��������������*******************/
void ldpcdecoder::SetMaxIter(int iter)
{
	max_iter = iter;
}
/*****************���ر��ε����Ĵ���******************/
int ldpcdecoder::GetTheIter() const
{
	return iter;
}
/*********************��������ź�********************/
void ldpcdecoder::IitialLdpcDecoder(int clength)
{
	code_length = clength;
	max_iter = 50;
	LLR.reserve(code_length);
	result.reserve(code_length);
	for (size_t i = 0; i != code_length; ++i)
	{
		LLR.push_back(0.0);
	}
	for (size_t i = 0; i != code_length; ++i)
	{
		result.push_back(0);
	}
}

/*************��ȡ���Ա��벿�ֵ�H������Ϣ*************/
void ldpcdecoder::GetHMatrix(int N, int R)
{

	NN = N;
	RR = R;

    std::string filename1 = "./Matrixs/N"+std::to_string(N)+"_M"+ std::to_string(R)+"/column_weight_distribution.txt";
	//std::string filename1 = "Matrixs/N256_M128/column_weight_distribution.txt";
	ifstream inc(filename1);
	if (!inc)
	{
		cout << "Failed to open column_weight_distribution.txt!" << endl;
		return;
	}
    std::string filename2 = "./Matrixs/N" + std::to_string(N) + "_M" + std::to_string(R) + "/row_weight_distribution.txt";
	ifstream inr(filename2);
	if (!inr)
	{
		cout << "Failed to open row_weight_distribution.txt!" << endl;
		return;
	}
	string r;
	message_node.reserve(N);
	check_node.reserve(R);
	for (size_t i = 0; i != NN; ++i)
	{
		string number;
		getline(inc, r);
		istringstream stream(r);
		stream >> number;
		message_node.push_back(CreateParentNode(stoi(number)));
		for (int j = 0; j < message_node[i].size; ++j)
		{
			stream >> number;
			message_node[i].cindex.push_back(stoi(number));
		}
	}

	for (size_t i = 0; i != RR; ++i)
	{
		string number;
		getline(inr, r);
		istringstream stream(r);
		stream >> number;
		check_node.push_back(CreateChildNode(stoi(number)));
		for (int j = 0; j < check_node[i].size; ++j)
		{
			stream >> number;
			check_node[i].rindex.push_back(stoi(number));
		}
	}
	inc.close();
	inr.close();
}
/******************�����Ϣ��У��ڵ�*****************/
NodeData::parentnode ldpcdecoder::CreateParentNode(int cw)
{
	return parentnode(cw, code_length);
}

NodeData::childnode ldpcdecoder::CreateChildNode(int rw)
{
	return childnode(rw, code_length);
}
/*********************�����������********************/
vector<int> ldpcdecoder::StartDecode(vector<double>channel_out, const double n0)
{
	int aux, auy;
	double multipl, sum;
	double *q_result = new double[channel_out.size()];
	int onetime_error_bit;
	/**************************��ʼ��***************************/
	for (size_t i = 0; i != NN; ++i)
	{
		LLR[i] = 4.0*channel_out[i] / n0;
		for (size_t j = 0; j != message_node[i].size; ++j)
		{
			aux = message_node[i].cindex[j];
			message_node[i].cmessage[aux] = LLR[i];
		}
	}
	/**************************��ʼ����*************************/
	for (iter = 0; iter < max_iter; ++iter)
	{
		/************************У��ڵ����***********************/
		for (size_t i = 0; i != RR; ++i)
		{
			for (size_t j = 0; j != check_node[i].size; ++j)
			{
				multipl = 1.0;
				auy = check_node[i].rindex[j];
				for (size_t k = 0; k != check_node[i].size; ++k)
				{
					double t = 0.0;
					if (k != j)
					{
						aux = check_node[i].rindex[k];
						t = message_node[aux].cmessage[i];
						multipl *= tanh(t / 2);
					}
				}
				check_node[i].rmessage[auy] = 2 * atanh(multipl);
			}
		}


		/************************��Ϣ�ڵ����***********************/
		for (size_t i = 0; i != NN; ++i)
		{
			for (size_t j = 0; j != message_node[i].size; ++j)
			{
				sum = 0.0;
				auy = message_node[i].cindex[j];
				for (size_t k = 0; k != message_node[i].size; ++k)
				{
					if (k != j)
					{
						aux = message_node[i].cindex[k];
						sum += check_node[aux].rmessage[i];
					}
				}
				message_node[i].cmessage[auy] = LLR[i] + sum;
			}
		}

		/************************�����о�����***********************/
		for (size_t i = 0; i != NN; ++i)
		{
			q_result[i] = 0;
			for (size_t j = 0; j != message_node[i].size; ++j)
			{
				aux = message_node[i].cindex[j];
				q_result[i] += check_node[aux].rmessage[i];
			}
			q_result[i] += LLR[i];
			if (q_result[i] < 0.0)
				result[i] = 1;
			else
				result[i] = 0;
		}
		/************************z*H=0?*************************/
		//int error = 0;
		//for (size_t i = 0; i != RR; ++i)
		//{
		//	int num = 0;
		//	for (size_t j = 0; j != check_node[i].size; ++j)
		//	{
		//		if (result[check_node[i].rindex[j]] == 1)
		//			++num;
		//	}
		//	if (num % 2 == 1) ++error;
		//}
		//if (error == 0)	break;

	}
	/******************�����������ı�����*****************/
	//onetime_error_bit = 0;
	//for (size_t i = RR; i != NN; ++i)
	//{
	//	if (result[i] != origsignal[i - RR])
	//		++onetime_error_bit;
	//}
	delete[] q_result;
	//return onetime_error_bit;
	return result;
}
/***********************��ʾ���**********************/
void ldpcdecoder::PrintResult() const
{
	for (size_t i = code_length / 2; i != code_length; ++i)
		cout << result[i] << ends;
}
/***********************�������**********************/
void ldpcdecoder::Clear()
{
	result.clear();
	LLR.clear();
}


