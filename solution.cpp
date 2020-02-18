#include <bits/stdc++.h>
using namespace std;

int t, x, res;
int main(){
	cin >> t;
	while (t --){
		cin >> x;
		res = 0;
		while (x != 1){
			if (x % 5 == 0){
				x /= 5; x *= 4; res ++;
			}
			else if (x % 3 == 0){
				x /= 3; x *= 2; res ++;
			}
			else if (x % 2 == 0){
				x /= 2; x *= 1; res ++;
			}
			else{
				res = -1;
				break;
			}
		}
		cout << res << endl;
	}
	return 0;
}
