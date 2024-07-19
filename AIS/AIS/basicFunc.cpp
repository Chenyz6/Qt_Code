
#include "basicFunc.h"
#include <cmath>
#include <vector>
using namespace std;

double biterr(string b1, string b2) {
    int len1 = b1.length();
    int len2 = b2.length();
    if (len1 != len2)
        return 1;
    int cnt = 0;

    for (int i = 0; i < len1; i++) {
        if (b1.substr(i, 1) != b2.substr(i, 1))
            cnt++;
    }
    return cnt * 1.0 / len1;
}
string deScramble(string data, vector<int> regIni, vector<int> poly) {
    int n = regIni.size();
    int mLen = pow(2.0, n) - 1;
    if (mLen > 200000)
        mLen = 200000;
    vector<int> G;
    for (int i = 0; i < n; i++)
        G.at(i) = 0;
    for (int i = 0; i < poly.size(); i++)
        G.at(poly.at(i) - 1) = 1;
    string m = "";
    for (int i = 0; i < mLen; i++) {
        int tmp = 0;
        for (int j = 0; j < n; j++)
            tmp += G.at(j) * regIni.at(j);
        if (tmp % 2 == 1)
            m += "1";
        else
            m += "0";
        for (int j = n - 1; j > 0; j--)
            regIni.at(j) = regIni.at(j - 1);
        regIni.at(0) = tmp % 2;
    }

    int rep = ceil(data.length() * 1.0 / mLen);
    for (int i = 0; i < rep; i++)
        m += m;
    string ret = Xor(data, m.substr(0, data.length()));
    return ret;
}
string deScramble9600(string bin) {
    int regCnt = 18;
    int reg[18] = {0};

    string ret = "";
    for (int i = 0; i < bin.length(); i++) {
        reg[0] = _ttoi(bin.substr(i, 1).c_str());
        int tmp = reg[0] + reg[12] + reg[17];
        string bit;
        // bit.Format("%d",tmp%2);
        bit = to_string(tmp % 2);
        ret += bit;

        for (int j = 17; j > 0; j--)
            reg[j] = reg[j - 1];
    }
    return ret;
}
string deStuffing9600(string str) {
    vector<int> deleteIndex;
    for (int i = 5; i < str.length();) {
        if (str.substr(i - 5, 6) == "111110") {
            deleteIndex.push_back(i);
            i += 6;
        } else
            i++;
    }
    for (int i = deleteIndex.size() - 1; i >= 0; i--)
        str.erase(deleteIndex.at(i));
    return str;
}
/************************************************************************
二进制字符串转8进制数组
*************************************************************************/
string binstr2octstr(string str, string msbLoc) {
    string ret = "";
    int octCnt = ceil(str.length() * 1.0 / 3);
    int strLen = str.length();
    for (int i = 0; i < octCnt * 3 - strLen; i++)
        str = "0" + str;
    for (int i = 0; i < octCnt; i++) {
        string subStr = str.substr(i * 3, 3);
        ret += binstr2decstr(subStr, msbLoc);
    }
    return ret;
}
/************************************************************************
二进制字符串转二进制数组
*************************************************************************/
int *binstr2binvec(string input) {
    int len = input.length();
    int *output = new int[len];
    for (int i = 0; i < len; i++)
        output[i] = _ttoi(input.substr(i, 1).c_str());
    return output;
}
string binvec2binstr(int *input, int len) {
    string ret = "";
    for (int i = 0; i < len; i++) {
        if (input[i] == 1)
            ret += "1";
        else
            ret += "0";
    }
    return ret;
}
/************************************************************************/
/* 解相位模糊
type：IQ，I~Q，QI，Q~I*/
/************************************************************************/
string phaseDebluer(string bin, string type) {
    string ret;
    if (type == "IQ")
        ret = bin;
    if (type == "I~Q") {
        ret = bin;
        for (int i = 1; i < bin.length(); i += 2) {
            if (ret.substr(i, 1) == "1")
                // ret.SetAt(i,'0');
                ret.replace(i, 1, "0");
            else
                // ret.SetAt(i,'1');
                ret.replace(i, 1, "1");
        }
    }
    if (type == "QI") {
        ret = "";
        for (int i = 0; i < bin.length(); i += 2)
            ret += fliplr(bin.substr(i, 2));
    }
    if (type == "Q~I") {
        ret = "";
        for (int i = 0; i < bin.length(); i += 2) {
            string tmp = fliplr(bin.substr(i, 2));
            if (tmp.substr(0, 1) == "1")
                // tmp.SetAt(0,'0');
                tmp.replace(0, 1, "0");
            else
                // tmp.SetAt(0,'1');
                tmp.replace(0, 1, "1");

            ret += tmp;
        }
    }
    return ret;
}
/************************************************************************/
/* 比特取反 */
/************************************************************************/
string bitNot(string bin) {
    string ret = "";
    for (int i = 0; i < bin.length(); i++) {
        if (bin[i] == '1')
            ret += "0";
        else
            ret += "1";
    }
    return ret;
}
/************************************************************************
函数名：fliplr
功能：同matlab的fliplr
输入：str:string类型的01字符串
************************************************************************/
string fliplr(string str) {
    string result = "";
    for (int i = str.length() - 1; i >= 0; i--)
        result += str.substr(i, 1);
    return result;
}

/************************************************************************
函数名：byteReverse
功能：按字节取反
输入：str:string类型的01字符串
************************************************************************/
string byteReverse(string bin) {
    string ret = "";
    int loop = bin.length() / 8;
    for (int i = 0; i < loop; i++)
        ret += fliplr(bin.substr(i * 8, 8));
    return ret;
}
/************************************************************************/
/* 求和                                                                     */
/************************************************************************/
int sum(string bin) {
    int ret = 0;
    for (int i = 0; i < bin.length(); i++)
        ret += _ttoi(bin.substr(i, 1).c_str());
    return ret;
}
/************************************************************************
函数名：binstr2decnum
功能：01码组成的字符串转十进制数字
输入：str:string类型的01字符串
      msbLoc:string类型，“left-msb”或“right-msb”
输出：十进制值
************************************************************************/
int binstr2decnum(string str, string msbLoc) {
    string str1 = str;
    if (msbLoc == "right-msb")
        str1 = fliplr(str);
    int result = 0;
    for (int i = str1.length() - 1; i >= 0; i--)
        result +=
            _ttoi(str1.substr(i, 1).c_str()) * pow(2.0, str1.length() - 1 - i);
    return result;
}

string decnum2binstr(int d, string msbLoc, int size) {
    int remain = d;
    string tmp;
    string ret = "";
    while (remain > 0) {
        if (remain % 2 == 0)
            ret = "0" + ret;
        else
            ret = "1" + ret;
        remain = (remain - (remain % 2)) / 2;
    }
    int len = ret.length();
    for (int i = 0; i < size - len; i++)
        ret = "0" + ret;
    if (msbLoc == "right-msb")
        ret = fliplr(ret);
    return ret;
}
/************************************************************************
函数名：hex2bin
功能：十六进制转二进制
输入：str:string类型的十六进制字符串
输出：二进制值的字符串
************************************************************************/
string hex2bin(string str) // 十六进制数如果以0x开头，这个函数首先会把这个头去掉
{
    string bin = "";
    // str.MakeLower();
    // 大写字母转换为小写字母
    for (int i = 0; i < str.size(); i++) {
        str[i] = tolower(str[i]);
    }
    if (str.substr(0, 2) == "0b")
        return str.substr(2);
    if (str.find('x') >= 0) {
        str.erase(str.find('x') - 1, 2);
        // str.TrimLeft('0');
        // str.Remove('x');

        // str.erase(std::remove(str.begin(), str.end(), 'a'), str.end());
    }

    for (int i = 0; i < str.length(); i++) {
        TCHAR ch = str[i];
        switch (ch) {
        case '0':
            bin += "0000";
            break;
        case '1':
            bin += "0001";
            break;
        case '2':
            bin += "0010";
            break;
        case '3':
            bin += "0011";
            break;
        case '4':
            bin += "0100";
            break;
        case '5':
            bin += "0101";
            break;
        case '6':
            bin += "0110";
            break;
        case '7':
            bin += "0111";
            break;
        case '8':
            bin += "1000";
            break;
        case '9':
            bin += "1001";
            break;
        case 'a':
            bin += "1010";
            break;
        case 'b':
            bin += "1011";
            break;
        case 'c':
            bin += "1100";
            break;
        case 'd':
            bin += "1101";
            break;
        case 'e':
            bin += "1110";
            break;
        case 'f':
            bin += "1111";
            break;
        default:
            break;
        }
    }
    return bin;
}

/************************************************************************
函数名：binstr2decstr
功能：01码组成的字符串转十进制数字字符串
输入：str:string类型的01字符串
msbLoc:string类型，“left-msb”或“right-msb”
输出：十进制字符串
************************************************************************/
string binstr2decstr(string str, string msbLoc) {
    int v = binstr2decnum(str, msbLoc);
    string result;
    // result.Format("%d",v);
    result = to_string(v);
    return result;
}

/************************************************************************
函数名：binstr2hexstr
功能：二进制字符串转16进制字符串
输入：str：二进制字符串，string
      msbLoc:string类型，“left-msb”或“right-msb”
输出：16进制字符串
************************************************************************/
string binstr2hexstr(string str, string msbLoc) {
    string binstr = str;
    if (msbLoc == "right-msb")
        binstr = fliplr(str);

    // 对binstr左边补0，使其长度为4的倍数
    int zeroCnt = 4 * ceil(binstr.length() * 1.0 / 4) - binstr.length();
    string binstr1 = "";
    for (int i = 0; i < zeroCnt; i++)
        binstr1 += "0";
    binstr1 += binstr;

    string result = "";
    for (int i = 0; i < binstr1.length() / 4; i++) {
        string binTmp = binstr1.substr(i * 4, 4);
        if (binTmp == "0000")
            result += "0";
        if (binTmp == "0001")
            result += "1";
        if (binTmp == "0010")
            result += "2";
        if (binTmp == "0011")
            result += "3";
        if (binTmp == "0100")
            result += "4";
        if (binTmp == "0101")
            result += "5";
        if (binTmp == "0110")
            result += "6";
        if (binTmp == "0111")
            result += "7";
        if (binTmp == "1000")
            result += "8";
        if (binTmp == "1001")
            result += "9";
        if (binTmp == "1010")
            result += "A";
        if (binTmp == "1011")
            result += "B";
        if (binTmp == "1100")
            result += "C";
        if (binTmp == "1101")
            result += "D";
        if (binTmp == "1110")
            result += "E";
        if (binTmp == "1111")
            result += "F";
    }
    return result;
}
void str2intvec(string input, int *vec);
/************************************************************************/
/* 将input转换为int数组 */
/************************************************************************/
void str2intvec(string input, vector<int> &intvec) {
    for (int i = 0; i < input.length(); i++)
        intvec.push_back(_ttoi(input.substr(i, 1).c_str()));
}
/************************************************************************/
/* 二进制字符串转ascii码字符串
bin为输入二进制字符串，asciiSize=7，表示每7比特为一个ascii字符，asciiSize=8
表示每8比特表示一个ascii字符*/
/************************************************************************/
string binstr2ascii(string bin, int asciiSize, string msbLoc) {
    // use http://www.asciitable.com/ for determining additional character types
    char rchSpecialCharacters[] = {
                                   0x21, 0x22, 0x23, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2a, 0x2b,
                                   0x2c, 0x2d, 0x2e, 0x2f, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f, 0x40, 0x5b,
                                   0x5c, 0x5d, 0x5e, 0x5f, 0x60, 0x7b, 0x7c, 0x7d, 0x7e, 0x00};

    unsigned char rchExtendedAscii[] = {
                                        0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8a, 0x8b,
                                        0x8c, 0x8d, 0x8e, 0x8f, 0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97,
                                        0x99, 0x9a, 0x9c, 0x0d, 0x9f, 0xa0, 0xa1, 0xa2, 0xa3, 0xa4, 0xa5, 0x00};
    // 建立ascii表
    char asciiTable[256];
    for (unsigned short i = 0; i <= 255; ++i) {
        asciiTable[i] = ' '; // table is defaulted to whitespace

        if ((i >= 'a') && (i <= 'z')) {
            asciiTable[i] = (char)i; // no problems with lower case letters
        }
        if (true) { // case of !bCaseSensitive fixed up later on
            if ((i >= 'A') && (i <= 'Z')) {
                asciiTable[i] = (char)i; // map upper case to lower case
            }
        }
        if ((i >= '0') && (i <= '9')) {
            asciiTable[i] = (char)i; // use digits
        }
    }

    if (true) {
        for (char *c = rchSpecialCharacters; 0 != *c; ++c) {
            asciiTable[*c] = *c;
        }
    }
    if (true) {
        for (unsigned char *c = rchExtendedAscii; 0 != *c; ++c) {
            asciiTable[*c] = static_cast<char>(*c);
        }
    }

    // 调整bin长度
    int zeroCnt = asciiSize * ceil(bin.length() * 1.0 / asciiSize) - bin.length();
    string binstr = "";
    for (int i = 0; i < zeroCnt; i++)
        binstr += "0";
    binstr += bin;

    string result = "";
    for (int i = 0; i < binstr.length() / asciiSize; i++) {
        string bin_tmp = binstr.substr(asciiSize * i, asciiSize);
        // 将bin转换为十进制数字，即为ascii码表索引
        int index = binstr2decnum(bin_tmp, msbLoc);
        char asciiChar = asciiTable[index];
        string tmp;
        // tmp.Format("%c",asciiChar);
        tmp = asciiChar;
        result += tmp;
    }
    return result;
}

/************************************************************************/
/* 异或                                                                     */
/************************************************************************/
string Xor(string bin1, string bin2) {
    string ret = "";
    if (bin1.length() != bin2.length())
        return ret;
    for (int i = 0; i < bin1.length(); i++) {
        if (bin1.substr(i, 1) == bin2.substr(i, 1))
            ret += "0";
        else
            ret += "1";
    }
    return ret;
}

/************************************************************************/
/* 按位与                                                                     */
/************************************************************************/
string And(string bin1, string bin2) {
    string ret = "";
    if (bin1.length() != bin2.length())
        return ret;
    for (int i = 0; i < bin1.length(); i++) {
        if (bin1.substr(i, 1) == "0" || bin2.substr(i, 1) == "0")
            ret += "0";
        else
            ret += "1";
    }
    return ret;
}
/************************************************************************/
/* 解块交织,r,l为交织参数   列入行出 */
/************************************************************************/
string deInterleave1(string bin, int r, int l) {
    string ret = "";
    if (bin.length() != r * l)
        return ret;
    vector<string> mat;
    for (int i = 0; i < r; i++)
        mat.push_back("");
    for (int i = 0; i < l; i++) {
        for (int j = 0; j < r; j++)
            mat.at(j) += bin.substr(i * r + j, 1);
    }
    for (int i = 0; i < r; i++)
        ret += mat.at(i);
    return ret;
}
/************************************************************************/
/* 解块交织,r,l为交织参数   行入列出 */
/************************************************************************/
string deInterleave2(string bin, int r, int l) {
    string ret = "";
    if (bin.length() != r * l)
        return ret;
    vector<string> mat;
    for (int i = 0; i < l; i++)
        mat.push_back("");
    for (int i = 0; i < r; i++) {
        for (int j = 0; j < l; j++)
            mat.at(j) += bin.substr(i * r + j, 1);
    }
    for (int i = 0; i < l; i++)
        ret += mat.at(i);
    return ret;
}

/************************************************************************/
/* 奇偶校验
msg：信息
type=“odd”：奇校验
type=“even”：偶校验
返回校验比特*/
/************************************************************************/
string oddEvenCheck(string msg, string type) // 奇偶校验
{
    string ret;
    int oneCnt = 0;
    for (int i = 0; i < msg.length(); i++)
        oneCnt += _ttoi(msg.substr(i, 1).c_str());
    if (type == "odd")
        // ret.Format("%d",(oneCnt+1)%2);
        ret = to_string((oneCnt + 1) % 2);
    else
        // ret.Format("%d",oneCnt%2);
        ret = to_string(oneCnt % 2);
    return ret;
}

/************************************************************************/
/* 解2相差分 */
/************************************************************************/
string diffDecode2(string code) // 解2相差分
{
    int len1 = code.length();
    string ret = "0";
    string tmp;
    for (int i = 1; i < len1; i++) {
        // tmp.Format("%d",(_ttoi(code.substr(i,1))+_ttoi(code.substr(i-1,1)))%2);
        tmp = to_string((_ttoi(code.substr(i, 1).c_str()) +
                         _ttoi(code.substr(i - 1, 1).c_str())) %
                        2);
        ret += tmp;
    }
    return ret;
}

/************************************************************************/
/* 解4相差分 */
/************************************************************************/
string diffDecode4(string code) // 解4相差分
{
    int len1 = code.length();
    double len2 = floor((len1 / 2) * 1.0);
    string ret = "00";
    for (int i = 1; i < len2; i++) {
        if (Xor(code.substr(2 * i - 2, 1), code.substr(2 * i - 1, 1)) == "0") {
            ret += Xor(code.substr(2 * i - 2, 1), code.substr(2 * i, 1));
            ret += Xor(code.substr(2 * i - 1, 1), code.substr(2 * i + 1, 1));
        } else {
            ret += Xor(code.substr(2 * i - 1, 1), code.substr(2 * i + 1, 1));
            ret += Xor(code.substr(2 * i - 2, 1), code.substr(2 * i, 1));
        }
    }
    return ret;
}

/************************************************************************/
/* 解8相差分 */
/************************************************************************/
string diffDecode8(string code) {
    int len1 = code.length();
    double len2 = floor((len1 / 3) * 1.0);
    string ret = "000";

    for (int i = 1; i < len2; i++) {
        string xdk = code.substr(3 * i, 1);
        string ydk = code.substr(3 * i + 1, 1);
        string zdk = code.substr(3 * i + 2, 1);
        string xdk1 = code.substr(3 * i - 3, 1);
        string ydk1 = code.substr(3 * i - 2, 1);
        string zdk1 = code.substr(3 * i - 1, 1);

        if (Xor(Xor(xdk1, ydk1), zdk1) == "0") {
            if (Xor(xdk1, ydk1) == "0")
                ret += Xor(xdk, xdk1) + Xor(ydk, ydk1) + Xor(zdk, zdk1);
            else
                ret += Xor(ydk, ydk1) + Xor(xdk, xdk1) + Xor(zdk, zdk1);
        } else {
            if (Xor(xdk1, ydk1) == "0") {
                if (Xor(zdk, zdk1) == "0") {
                    string xak = Xor(xdk, xdk1);
                    ret += xak;
                    string yak = Xor(xak, Xor(zdk, zdk1));
                    ret += yak;
                    ret += Xor(xak, Xor(ydk, ydk1));
                } else {
                    string yak = Xor(ydk, ydk1);
                    string xak = Xor(yak, Xor(zdk, zdk1));
                    ret += xak;
                    ret += yak;
                    ret += Xor(yak, Xor(xdk, xdk1));
                }
            } else {
                if (Xor(zdk, zdk1) == "0") {
                    string xak = Xor(ydk, ydk1);
                    ret += xak;
                    ret += Xor(xak, Xor(zdk, zdk1));
                    ret += Xor(xak, Xor(xdk, xdk1));
                } else {
                    string yak = Xor(xdk, xdk1);
                    ret += Xor(yak, Xor(zdk, zdk1));
                    ret += yak;
                    ret += Xor(yak, Xor(ydk, ydk1));
                }
            }
        }
    }
    return ret;
}

/************************************************************************/
/* FSK扩展 */
/************************************************************************/
string fskExtend(string bitin) // fsk扩展，zyg版
{
    if (bitin.length() == 0)
        return "";
    string tmp = bitin.substr(0, 1);
    if (tmp == "1")
        bitin = "0" + bitin;
    string bitout = "";
    vector<int> bit0;
    for (int i = 0; i < bitin.length(); i++) {
        bitout += "0";
        if (bitin.substr(i, 1) == "0")
            bit0.push_back(i);
    }
    for (int i = 0; i < bit0.size(); i++) {
        if (i % 2 == 0)
            // bitout.SetAt(bit0.at(i),'0');
            bitout[bit0.at(i)] = '0';
        // bitout.replace(bit0.at(i),1,"0");
        else
            // bitout.SetAt(bit0.at(i),'1');
            bitout.replace(bit0.at(i), 1, "1");
    }
    if (bit0.size() == 0)
        bitout = "";
    for (int i = 1; i <= bit0.size() - 1; i++) {
        for (int j = bit0.at(i - 1) + 1; j <= bit0.at(i) - 1; j++)
            // bitout.SetAt(j,bitout.GetAt(bit0.at(i-1)));
            bitout[j] = bitout[bit0.at(i - 1)];
    }
    // if(bit0.size()>0 && bitin.Right(1)=="1")
    if (bit0.size() > 0 && *bitin.rbegin() == '1') {
        for (int i = bit0.at(bit0.size() - 1) + 1; i < bitout.length(); i++)
            // bitout.SetAt(i,bitout.GetAt(bit0.at(bit0.size()-1)));
            bitout[i] = bitout[bit0.at(bit0.size() - 1)];
    }
    if (tmp == "1")
        bitout = bitout.substr(1);
    return bitout;
}
string fskExtend_csc(string code) // fsk扩展，csc版
{
    if (code.length() == 0)
        return "";
    if (code.substr(0, 1) == "1")
        code = "010" + code;
    string inx1;
    inx1 = code;
    string outbits = "";
    for (int i = 0; i < code.length(); i++)
        outbits += "0";

    string a = "";
    for (int i = 0; i < inx1.length(); i++) {
        if (inx1.substr(i, 1) == "0")
            a += code.substr(i, 1);
    }
    for (int i = 1; i < a.length(); i = i + 2)
        a.replace(i, 1, "1");
    int cnt = 0;
    for (int i = 0; i < inx1.length(); i++) {
        if (inx1.substr(i, 1) == "0") {
            // outbits.SetAt(i,a.GetAt(cnt));
            outbits[i] = a[cnt];
            cnt++;
        }
    }
    int aa = 0;
    while (true) {
        string inxr = And(bitNot(inx1), inx1.substr(1) + "0");
        string ii = And("0" + inxr.substr(0, inxr.length() - 1), inx1);
        string outbitsTmp = "";
        for (int i = 0; i < inxr.length(); i++) {
            if (inxr.substr(i, 1) == "1")
                outbitsTmp += outbits.substr(i, 1);
        }
        cnt = 0;
        for (int i = 0; i < ii.length(); i++) {
            if (ii.substr(i, 1) == "1") {
                // outbits.SetAt(i,outbitsTmp.GetAt(cnt));
                outbits[i] = outbitsTmp[cnt];
                cnt++;
            }
        }
        inx1 = And(inx1, bitNot(ii));
        if (aa == 378)
            int sss = 1;
        if (sum(inx1) == 0)
            break;
        aa++;
    }
    return outbits;
}

/************************************************************************/
/* QPSK扩展 */
/************************************************************************/
string qpskExtend(string code) // qpsk扩展
{
    code = code.substr(0, code.length() - (code.length() % 2));
    string I = "";
    string Q = "";
    for (int i = 0; i < code.length(); i = i + 2) {
        I += code.substr(i, 1);
        Q += code.substr(i + 1, 1);
    }
    return Xor(I.substr(1), Q.substr(0, Q.length() - 1));
}

/************************************************************************/
/* OQPSK扩展 */
/************************************************************************/
string oqpskExtend(string code) {
    code = code.substr(0, code.length() - (code.length() % 2));
    string syn_OQPSK =
        "1111110000000011001100111100110011111100110000001100001100001111";
    int len1 = code.length();
    if (len1 > 117500)
        len1 = 117500;

    string a = "";
    string b = "";
    string y = "";
    for (int i = 0; i < len1; i = i + 2) {
        a += code.substr(i, 1);
        b += code.substr(i + 1, 1);
        y += "00";
    }
    vector<int> ns_5250;
    ns_5250.push_back(0);
    ns_5250.push_back(0);
    ns_5250.push_back(0);
    ns_5250.push_back(0);
    ns_5250.push_back(0);

    string _a = bitNot(a);
    string _b = bitNot(b);
    for (int k = 1; k <= 5; k++) {
        if (k == 1) {
            int cnt = 0;
            for (int i = 0; i < len1; i = i + 2) {
                // y.SetAt(i,_b.GetAt(cnt));
                y[i] = _b[cnt];
                cnt++;
            }
            cnt = 0;
            for (int i = 1; i < len1; i = i + 2) {
                // y.SetAt(i,_a.GetAt(cnt));
                y[i] = _a[cnt];
                cnt++;
            }
        }
        if (k == 2) {
            int cnt = 0;
            for (int i = 0; i < len1; i = i + 2) {
                // y.SetAt(i,a.GetAt(cnt));
                y[i] = a[cnt];
                cnt++;
            }
            cnt = 0;
            for (int i = 1; i < len1; i = i + 2) {
                // y.SetAt(i,_b.GetAt(cnt));
                y[i] = _b[cnt];
                cnt++;
            }
        }
        if (k == 3) {
            int cnt = 0;
            for (int i = 0; i < len1; i = i + 2) {
                // y.SetAt(i,b.GetAt(cnt));
                y[i] = b[cnt];
                cnt++;
            }
            cnt = 0;
            for (int i = 1; i < len1; i = i + 2) {
                // y.SetAt(i,a.GetAt(cnt));
                y[i] = a[cnt];
                cnt++;
            }
        }
        if (k == 4) {
            int cnt = 0;
            for (int i = 0; i < len1; i = i + 2) {
                // y.SetAt(i,_a.GetAt(cnt));
                y[i] = _a[cnt];
                cnt++;
            }
            cnt = 0;
            for (int i = 1; i < len1; i = i + 2) {
                // y.SetAt(i,b.GetAt(cnt));
                y[i] = b[cnt];
                cnt++;
            }
        }
        if (k == 5) {
            int cnt = 0;
            for (int i = 0; i < len1; i = i + 2) {
                // y.SetAt(i,b.GetAt(cnt));
                y[i] = b[cnt];
                cnt++;
            }
            cnt = 0;
            for (int i = 1; i < len1; i = i + 2) {
                // y.SetAt(i,_a.GetAt(cnt));
                y[i] = _a[cnt];
                cnt++;
            }
        }

        for (int i = 0; i < len1 - 100; i++) {
            string vague = y.substr(i, 64);
            if (vague == syn_OQPSK) {
                ns_5250.at(k - 1) = ns_5250.at(k - 1) + 1;
            }
        }
    }

    int maxIndex = -1;
    int maxVal = 0;
    for (int i = ns_5250.size() - 1; i >= 0; i--) {
        if (ns_5250.at(i) > maxVal) {
            maxVal = ns_5250.at(i);
            maxIndex = i + 1;
        }
    }

    len1 = code.length();
    a = "";
    for (int i = 0; i < len1; i = i + 2)
        a += code.substr(i, 1);
    b = "";
    for (int i = 1; i < len1; i = i + 2)
        b += code.substr(i, 1);
    y = "";
    for (int i = 0; i < len1; i++)
        y += "0";

    _a = bitNot(a);
    _b = bitNot(b);

    if (maxVal > 5) {
        if (maxIndex == 1) {
            int cnt = 0;
            for (int i = 0; i < len1; i = i + 2) {
                // y.SetAt(i,_b.GetAt(cnt));
                y[i] = _b[cnt];
                cnt++;
            }
            cnt = 0;
            for (int i = 1; i < len1; i = i + 2) {
                // y.SetAt(i,_a.GetAt(cnt));
                y[i] = _a[cnt];
                cnt++;
            }
        }
        if (maxIndex == 2) {
            int cnt = 0;
            for (int i = 0; i < len1; i = i + 2) {
                // y.SetAt(i,a.GetAt(cnt));
                y[i] = a[cnt];
                cnt++;
            }
            cnt = 0;
            for (int i = 1; i < len1; i = i + 2) {
                // y.SetAt(i,_b.GetAt(cnt));
                y[i] = _b[cnt];
                cnt++;
            }
        }
        if (maxIndex == 3) {
            int cnt = 0;
            for (int i = 0; i < len1; i = i + 2) {
                // y.SetAt(i,b.GetAt(cnt));
                y[i] = b[cnt];
                cnt++;
            }
            cnt = 0;
            for (int i = 1; i < len1; i = i + 2) {
                // y.SetAt(i,a.GetAt(cnt));
                y[i] = a[cnt];
                cnt++;
            }
        }
        if (maxIndex == 4) {
            int cnt = 0;
            for (int i = 0; i < len1; i = i + 2) {
                // y.SetAt(i,_a.GetAt(cnt));
                y[i] = _a[cnt];
                cnt++;
            }
            cnt = 0;
            for (int i = 1; i < len1; i = i + 2) {
                // y.SetAt(i,b.GetAt(cnt));
                y[i] = b[cnt];
                cnt++;
            }
        }
        if (maxIndex == 5) {
            int cnt = 0;
            for (int i = 0; i < len1; i = i + 2) {
                // y.SetAt(i,b.GetAt(cnt));
                y[i] = b[cnt];
                cnt++;
            }
            cnt = 0;
            for (int i = 1; i < len1; i = i + 2) {
                // y.SetAt(i,_a.GetAt(cnt));
                y[i] = _a[cnt];
                cnt++;
            }
        }
    } else
        y = code;
    return y;
}

/************************************************************************/
/* 预处理                                                                     */
/************************************************************************/
string preProcessing(string bin, vector<int> flag) {
    /*int flag_bit_not=flag.at(6);//flag.at(0);
  int flag_fliplr=flag.at(5);//flag.at(1);
  int flag_diff2=flag.at(2);//flag.at(2);
  int flag_diff4=flag.at(3);//flag.at(3);
  int flag_diff8=flag.at(4);//flag.at(4);
  int flag_fskExtend=flag.at(0);//flag.at(5);
  int flag_OQPSKExtend=flag.at(7);//flag.at(6);
  int flag_QPSK=flag.at(8);//flag.at(7);
  int beginBit=flag.at(1)-1;*/

    int flag_bit_not = flag.at(0);
    int flag_fliplr = flag.at(1);
    int flag_diff2 = flag.at(2);
    int flag_diff4 = flag.at(3);
    int flag_diff8 = flag.at(4);
    int flag_fskExtend = flag.at(5);
    int flag_OQPSKExtend = flag.at(6);
    int flag_QPSK = flag.at(7);

    string ret = bin; //.substr(beginBit);
    if (flag.size() > 8)
        ret = bin.substr(flag.at(8));

    if (flag_fskExtend)
        ret = fskExtend(ret);
    if (flag_OQPSKExtend)
        ret = oqpskExtend(ret);
    if (flag_QPSK)
        ret = qpskExtend(ret);
    if (flag_diff2)
        ret = diffDecode2(ret);
    if (flag_diff4)
        ret = diffDecode4(ret);
    if (flag_diff8)
        ret = diffDecode8(ret);
    if (flag_fliplr)
        ret = fliplr(ret);
    if (flag_bit_not)
        ret = bitNot(ret);
    return ret;
}

void splitString(string str, string mark, vector<string> &strVec) {
    string tmpStr = "";
    for (int i = 0; i < str.length(); i++) {
        string tmp = str.substr(i, mark.length());
        if (tmp == mark) {
            if (tmpStr != "") {
                strVec.push_back(tmpStr);
                tmpStr = "";
            }
        } else
            tmpStr += str.substr(i, 1);
    }
    if (tmpStr != "")
        strVec.push_back(tmpStr);
}

