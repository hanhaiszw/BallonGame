// BallonGame.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include <vector>
#include <set>
#include <algorithm>
using namespace std;

//气球标号范围
const int MAX_NUM = 100;

//100以内的质数
static vector<int> primeArray;

//打印操作
template<typename T>
void print(T t) {
	for (auto i : t) {
		cout << i << "   ";
	}
	cout << endl;
}

//求出 (0,n]内所有的质数 包含n
vector<int> getPrimeArray(int n) {
	int m = n + 1;  //为了包含n
	vector<int> primeArray;
	if (m < 2) {
		return primeArray;
	}

	vector<bool> v(m, true);
	v[0] = false;
	v[1] = false;

	//处理偶数
	for (int i = 4; i < m; i += 2) v[i] = false;

	//处理奇数
	for (int i = 3; i*i < m; i += 2) {
		for (int j = i * i; j < m; j += i * 2) {
			v[j] = false;
		}
	}

	//存入质数
	for (int i = 0; i < m; i++) {
		if (v[i]) {
			primeArray.push_back(i);
		}
	}

	return primeArray;
}


//判断合数
bool isComposite(int a)
{
	if (a < 4) {
		return false;
	}

	for (int i = 2; i*i <= a; i++) {
		if (a % i == 0)
			return true;
	}
	return false;
}

//分解质因数
vector<int> decFactor(int n) {
	vector<int> facArray;
	if (n == 1) {
		facArray.push_back(1);
		return facArray;
	}

	int size = primeArray.size();
	int index = 0;
	while (n != 1) {
		int prime = primeArray[index];
		if (n != 1 && prime > n) {
			//分解失败
			//cout << "分解失败,质因子 >" <<MAX_NUM<< endl;
			facArray.clear();
			break;
		}
		//能被整除
		if (n % primeArray[index] == 0) {
			facArray.push_back(primeArray[index]);
			n /= primeArray[index];
		}
		else {
			index++;
			if (index == size) {
				//分解失败
				//cout << "分解失败,质因子 >" << MAX_NUM << endl;
				facArray.clear();
				break;
			}
		}

	}
	return facArray;
}


//求取组合的方法
template <typename T>
void combine_inner(T &data, int start, int n, int m, int depth, T temp, vector<T> &result)
{
	if (depth == m - 1)
	{
		//最内层循环 将temp加入result
		for (int i = start; i < n - (m - depth - 1); ++i)
		{
			temp[depth] = data[i];
			result.push_back(temp);
		}
	}
	else
		for (int i = start; i < n - (m - depth - 1); ++i)
		{
			temp[depth] = data[i];//每层输出一个元素
			combine_inner(data, i + 1, n, m, depth + 1, temp, result);
		}
}

//T可以调入vector<int>, string等，需要支持下标[]操作及size()函数
template <typename T>
vector<T> combine(T &data, int m)
{
	if (m <= 0)
		return{};
	int depth = 0;
	vector<T> result;  //T为vector<>类型
	T temp(m, 0);
	combine_inner(data, 0, data.size(), m, depth, temp, result);
	return result;
}



//求取乘数组合的方法
void multiComb_inner(int n, vector<int> temp, vector<vector<int>> &result) {
	//分解质因数
	vector<int> facArray = decFactor(n);

	int size = facArray.size();  //最多几个因子组成

	//求出所有的因子
	set<int> factors;
	factors.insert(n);  //先存入自身
	for (int i = 1; i <= size; i++) {
		auto v = combine(facArray, i);
		for (auto j : v) {
			int res = 1;
			for (int k : j) {
				res *= k;
			}
			//剪枝操作
			//1
			//找一下 是否存在与n配对的m值已经在factors中了
			//int m = n / res;
			////flag==1 说明找到了，flag==0说明没找到
			//int flag = count(factors.begin(), factors.end(), m);
			////如果不存在，则加入  m应在1-100范围内
			//if (!flag && res<=100) {
			//	factors.insert(res);
			//}
			//2
			if (res <= 100 && res*res <= n) {
				factors.insert(res);
			}
		}
	}
	//print(factors);

	//求组合
	for (auto factor : factors) {
		int flag = 0;  //记录push_back了几次
		temp.push_back(factor);
		++flag;
		//求余下的部分
		int m = n / factor;


		//一次组合的结果   不用加入1
		if (m != 1) {
			temp.push_back(m);
			++flag;
		}
		//查看下result中是否已经存在这个结果
		//不能改变temp中元素顺序
		auto sortTemp = temp;
		sort(sortTemp.begin(), sortTemp.end());
		auto pos = find_if(result.begin(), result.end(), [&sortTemp](vector<int> v) {return v == sortTemp; });
		if (pos == result.end()) {
			result.push_back(sortTemp);  //加入一次组合结果
		}

		//result.push_back(temp);  //加入一次组合结果
		//如果是合数 则送入递归
		if (isComposite(m)) {
			--flag;
			temp.pop_back();
			multiComb_inner(m, temp, result);
		}

		for (int i = 0; i < flag; i++) {
			temp.pop_back();
		}

	}

}

vector<vector<int>> multiComb(int n) {

	vector<vector<int>> result;
	vector<int> temp;  //存储每一次的结果
	multiComb_inner(n, temp, result);

	//除去有相同元素存在的组合
	for (auto it = result.begin(); it != result.end(); ) {
		vector<int>& v = *it;
		//1、首先判断是否存在元素大于MAX_NUM (100)
		//因为对每个temp加入时，都已排序  因此只需要判断最后一个值是不是大于100
		int size = v.size();
		if (v[size - 1] > MAX_NUM) {
			//执行删除
			it = result.erase(it);
			continue;
		}

		//
		//2、再判断是否有重复元素存在
		auto pos = unique(v.begin(), v.end());
		//说明有重复
		if (pos != v.end()) {
			it = result.erase(it);
		}
		else {
			it++;
		}

	}

	return result;
}


//挑战
bool challenge(int challenger, int winner, vector<vector<int>>& winMultiRet) {
	//分解挑战者的分数
	vector<vector<int>> chalRet = multiComb(challenger);
	int sizeC = chalRet.size();

	if (sizeC == 0) {
		//挑战者计算分数错误，挑战失败
		return false;
	}

	//挑战
	//双循环
	for (auto chal : chalRet) {
		for (auto win : winMultiRet) {
			//求两个vector的交集
			vector<int> vRet;
			//求交集 
			set_intersection(chal.begin(), chal.end(), win.begin(), win.end(), back_inserter(vRet));
			if (vRet.size() == 0) {
				//挑战失败
				return false;
			}
		}
	}

	//挑战成功
	winMultiRet = chalRet;
	return true;
}

//
int score_of_winner(vector<int> scores) {
	int size = scores.size();
	if (size < 1) {
		return 1;
	}
	sort(scores.begin(), scores.end(), greater<int>());

	int winner = scores[0];
	vector<vector<int>> winMultiRet = multiComb(winner);  //胜利者的拆分结果

														  //从次高到最低依次向胜利者挑战
	for (int i = 1; i < size; i++) {
		//
		int challenger = scores[i];
		bool ret = challenge(challenger, winner, winMultiRet);
		//挑战胜利
		if (ret) {
			winner = challenger;
		}
	}

	return winner;
}


int main()
{
	//1、求出100以内所有的质数
	primeArray = getPrimeArray(MAX_NUM);


	vector<int> v1{ 343,49,2 };  // 49
	vector<int> v2{ 3599,610,3 };   // 610
	vector<int> v3{ 62,36,7 };      // 62
	int winner = score_of_winner(v2);

	cout << "胜利者得分为 " << winner << endl;
	system("pause");
	return 0;
}



