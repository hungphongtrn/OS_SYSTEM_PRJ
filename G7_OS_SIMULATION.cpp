#include "OS_SIM.h"

int main() {
    cout << "                              \n\
           ..        .:J!!????!J~J77Y~:^    \n\
         ^P###P^ ... 7PPYJ555J?57P55G7  \n\
 ~~     :#@@@&&BGGGGGG@@@@@@@@@@@@@@@5  \n\
:B^     !@@@&: ......:#@@#####B#@@@@@Y  \n\
?@7     :#@@@~::     .#@@! .  7B@@@G~   \n\
!@#^     ^YBBG5~     .#@@^  :P@@@@?     \n\
 P@&?.               .#@5  ~#@@@@7      \n\
 .P@@#57^:....:~J5^^^~&5. ^&@@@@J       \n\
   !G@@@@&&##&&@@@@@&P~   P@@@@#.       \n\
     ^?PB&@@@@@@&B57:    ~@@@@@5        \n"  << endl;

    initOS();
	beginTime = clock();
	thread myThread ( startOS );
	myThread.detach();

    while (true) {
        showOptions();
        int choice = getUserChoice();
        processChoice(choice);
    }

    return 0;
}