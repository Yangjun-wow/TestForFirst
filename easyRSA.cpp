#include <iostream>
#include<stdio.h>
#include<cstring>
#include <ctime>
#include<math.h>
#include <cstdlib>
#include <fstream>
#define K 20
using namespace std;
int PublicKey[2];          //公钥
int PrivateKey[2];         //私钥

//函数声明
int GeneratePrimeNumber(int N); //随机生成质数 限制范围2~N
int JudgePrime(int M);          //判断M是否为质数
int JudgeEachPrime(int a, int b);     //判断a，b是否互质
int EuclidExtension(int a, int b,int &x,int &y);    //用扩展欧几里得算法求解方程(即辗转相除法)
void GenerateKey();             //随机生成公钥与私钥
void Encryption(string s1);              //用公钥加密
void Decryption(string s1);              //用私钥解密
int PowMod(int &base, int &pow, int &n);     //模幂运算

int main()
{
    int choice;
    cout << "请输入数字以选择：" << endl;
    cout << "------1.生成公钥私钥" << endl;
    cout << "------2.加密" << endl;
    cout << "------3.解密" << endl;
    cin >> choice;
    if(choice==1)
    {
        srand(time(NULL));
        GenerateKey();
        cout << "公钥："
             << "n=" << PublicKey[0] << "  "
             << "e=" << PublicKey[1] << endl;
        cout << "私钥："
             << "n=" << PrivateKey[0] << "  "
             << "d=" << PrivateKey[1];
    }
    else if(choice==2)
    {
        string s1;
        cout << "请输入文件地址：";
        cin >> s1;
        cout << "请输入公钥：n e=";
        cin >> PublicKey[0] >> PublicKey[1];
        Encryption(s1);

    }
    else if(choice==3)
    {
        string s1;
        cout << "请输入文件地址：";
        cin >> s1;
        cout << "请输入私钥：n d=";
        cin >> PrivateKey[0] >> PrivateKey[1];
        Decryption(s1);
    }

}

//函数定义
int GeneratePrimeNumber(int N)
{
    int judge=0;
    int random_number = rand() % (N-1)+2;  //随机生成一个数（2~N）
    judge = JudgePrime(random_number);
    while(judge==0)
    {
        random_number--;
        judge = JudgePrime(random_number);
    }
    return random_number;
}

int JudgePrime(int M)
{
    int judge=1;
    for (int i = 2; i <= (int)sqrt(M);i++)
    {
        if(M%i==0)
        {
            judge = 0;
            break;
        }
    }
    return judge;
}

int JudgeEachPrime(int a, int b)
{
    int t = 0;
    while(b>0)
    {
        t = a % b;
        a = b;
        b = t;
    }
    if(a==1)
        return 1;
    else
        return 0;
}

void GenerateKey()
{
    int p, q, kn;
    int e,d;     //e,d分别为公钥私钥一部分
    p=GeneratePrimeNumber(1000);
    q = GeneratePrimeNumber(1000);
    while(p==q)       //两质数相同则q重新生成
        q = GeneratePrimeNumber(1000);
    //cout << p<<" "<<q<<endl;
    PublicKey[0] = PrivateKey[0] = p * q;

    kn = (p - 1) * (q - 1);         //计算p*q的欧拉函数

    srand(time(NULL));
    e = rand() % (kn-2)+2;  //随机生成一个数（2~N）
    while(!JudgeEachPrime(kn,e))
        e = rand() % (kn - 2) + 2;
    //用扩展欧几里得算法求d
    int y;     //工具变量
    int gcd = EuclidExtension(e, kn, d, y);
    //cout << "gcd=" << gcd;
    while(d<0)
        d += kn;

    PublicKey[1] = e;
    PrivateKey[1] = d;

}

int EuclidExtension(int a, int b,int &x,int &y)
{
    if (b == 0)
    {
        x = 1;
        y = 0;
        return a;
    }
    int ans = EuclidExtension(b, a % b, x, y);

    int tmp = x;
    x = y;
    y = tmp - a / b * y;
    return ans;
}

void Encryption(string s1)
{
    int Store[K];
    ifstream ifile;
    ifile.open(s1);
    char ch;
    int i = 0;
    while(ifile.get(ch))
    {
        Store[i] = (int)ch;
        Store[i] = PowMod(Store[i], PublicKey[1], PublicKey[0]);
        i++;
    }
    ifile.close();
    ofstream ofile(s1);
    int j;
    for (j = 0; j < i;j++)
    {
        ofile << Store[j] << " ";
    }
    ofile.close();
}

void Decryption(string s1)
{
    int Store[K]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    ifstream ifile(s1);
    int i;
    for (i = 0; i < K;i++)
    {
        ifile >> Store[i];
        if(Store[i]!=0)
        {
            Store[i] = PowMod(Store[i], PrivateKey[1], PrivateKey[0]);
        }
    }
    ifile.close();
    ofstream ofile(s1);
    for (i = 0; i < K;i++)
    {
        if(Store[i]!=0)
        {
            ofile << (char)Store[i];
        }
    }
    ofile.close();
}

int MulMod(int a, int b, int n)
{
    return a * b % n;
}
 
//模幂运算，返回值 x=base^pow mod n
int PowMod(int &base, int &pow, int &n)
{
    int a=base, b=pow, c=1;
    while(b)
    {
        while(!(b & 1))
        {
            b>>=1;            //a=a * a % n;    //函数看起来可以处理64位的整数，但由于这里a*a在a>=2^32时已经造成了溢出，因此实际处理范围没有64位
            a=MulMod(a, a, n);
        }        b--;        //c=a * c % n;        //这里也会溢出，若把64位整数拆为两个32位整数不知是否可以解决这个问题。
        c=MulMod(a, c, n);
    }    return c;
}
