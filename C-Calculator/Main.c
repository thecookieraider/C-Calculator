#include "C_Calculator_Core.h"
#include "Window.h"

int main(void) {
	WinMain(GetModuleHandle(NULL), NULL, GetCommandLine(), SW_SHOW);
	/* Console GUI
	char storage[MAXLINE];
	_CCALC_DT * ans = 0;
	do {
		//If the getline failed (line too large)
		if(getline(storage)) {
			//Skip all input and then let them retry
			while (getchar() != '\n');
			continue;
		}
		//Execute the line 
		_CCALC_DT result = exec(storage, ans);
		//If we got null, then something went wrong. Error message is printed by function which is executing
		if (result == -1 && __errFlag) {
			__errFlag = 0;
			continue;
		};
		//Print the number and give them another go
		*ans = result;
		printf("\n\n%lf\n\n", result);
	} while (storage[0] != 'q');

	
	return 0;
	*/
}