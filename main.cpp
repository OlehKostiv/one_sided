#include <iostream>
#include <algorithm>
#include "OneSided.h"
using namespace std;
//using MaChain = Chain<int>;
using MaChain = Chain<double>;

void show(MaChain& mc) {
	for (MaChain::iterator iter = mc.begin(); iter != mc.end(); ++iter)
		cout << *iter << " ";
	cout << endl;
}


int main() {
	{
		cout << MaChain().length() << endl;

		//MaChain ch({ 0,1,2,3,4,5,6,7 });

		double arr[] = { 0,1,2,3,4,5,6,7 };
		MaChain ch(arr);

		for (int size = ch.length(), i = size - 1; i < size + 1; i++)// ch[last], ch[last+1]
			try {
			cout << ch[i] << endl;
		}
		catch (MaChain::OutOfRange) {
			cout << "outtarange" << endl;
		}

		MaChain ch1(ch); 
		for_each(ch1.begin(), ch1.end(), [](double& d) {d *=-1;});

		ch1.insert(100, 3);
		ch1.insert(100, 0); // ->[0
		ch1.insert(100, ch1.length()); // n]<-
		show(ch1);

		ch1.erase(0); // 0x[
		ch1.erase(ch1.length() - 1); //]xn
		ch1.erase(3);
		show(ch1);

		for (int i : {-1, ch1.length()}) { // x[..]x
			try {
				ch1.erase(i);
			}
			catch (MaChain::OutOfRange) {
				cout << "outtarange" << endl;
			}
		}

		ch1.insert(ch, 5);
		show(ch1);
		std::swap(ch, ch1);

        Chain<MaChain> cch = { ch1, ch, MaChain({1e-2, 2e-3, 3e-4})};
        for (Chain<MaChain>::iterator iter = cch.begin(); iter != cch.end(); ++iter)
            show(*iter);
	}
		
	system("pause");
	return 0;
}