//---------------------------------------------------------------------Librar
#include <iostream>
#include <cstring>
#include <thread>
#include <mutex> 
#include <cstdlib>
#include "table.h"
#define MAX 5 // Define maximum number of initial processes
#define TIME 6 // Define time slice size in seconds
#define MEMORYSIZE 4000
#define KERNELSIZE 100

using namespace std;

//----------------------------------------------------------------------PCB
// Process Control Block: PCB
typedef struct {
    int id;
    int arrivedTime;
    int requiredTime;
    int usedTime;
    char state[6];

    int size;
    int startAddress;
}PCB;

// Process organization: Linked list (doubly linked list with a head node)
// Using a single PCB linked list due to the small scale
typedef struct PCBNode {
    PCB pcb;
    PCBNode *before;
    PCBNode *next;
}PCBNode, *pPCBNode;

// Process control: Primitives
void schedule(pPCBNode &p); // Process scheduling primitive
void kill(pPCBNode &p); // Process termination primitive
void creatPCB(); // Process creation primitive
void block(int id); // Process blocking primitive
void wakeup(int id); // Process wakeup primitive

// Global variables for PCB
pPCBNode ReadyList = new PCBNode; // Kernel data structure (PCB linked list)
mutex mtx; // Mutex lock for the kernel data structure (PCB linked list)
int pcbID; // Used to generate PCB IDs during creation
clock_t beginTime; // Used to calculate the arrival time

//----------------------------------------------------------------------Mem
// Free Memory Partition Table: FMPT
typedef struct FreeMemNode {
    int size;
    int startAddress;
}FreeMemNode, *pFreeMemNode;

// Memory management
int allocate(int memoryNeeded); // Memory allocation
void releaseAndMerge(int toReleasedSize, int startAddress); // Memory deallocation and merging
void mergeAdjacentPartitions(int index); // Merge adjacent partitions

// Global variables for Mem
vector<FreeMemNode> FMPT; // Free memory partition table

//----------------------------------------------------------------------OS
// Utility functions in the OS
void initPCB(); // Initialize PCB
void showPCB(); // Print PCB linked list to display the status of each PCB
void initMem(); // Initialize memory
void showMem(); // Print the free memory partition table
void initOS(); // Initialize the OS
void startOS(); // Start the OS
pPCBNode checkReadyList(); // Check if there are any pending PCBs in the ReadyList

void showOptions() {
	cout << "\n[G7_Shell]#" << endl;
    cout << "What is your option?" << endl;
    cout << "1. Create a PCB" << endl;
    cout << "2. Show PCB and Memory" << endl;
    cout << "3. Block a PCB" << endl;
    cout << "4. Wake up a PCB" << endl;
	cout << "5. Exit" << endl;
    cout << "Enter your choice (1-5): ";
}


int getUserChoice() {
    int choice;
    cin >> choice;
    cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Ignore remaining characters in the input buffer
    return choice;
}

void processChoice(int choice) {
    switch (choice) {
        case 1:
            creatPCB();
            break;
        case 2:
            showPCB();
            showMem();
            break;
        case 3:
            cout << "Enter the PCB ID to block: ";
            int blockId;
            cin >> blockId;
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            block(blockId);
            break;
        case 4:
            cout << "Enter the PCB ID to wake up: ";
            int wakeupId;
            cin >> wakeupId;
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            wakeup(wakeupId);
            break;
		case 5:
			cout << "Exiting";
			exit(0);
        default:
            cout << "Invalid choice. Please try again." << endl;
            break;
    }
}


// Init PCB
void initPCB() {
    pPCBNode p = ReadyList;
    
    for (int i = 0; i < MAX; i++) {
    	pPCBNode temp = new PCBNode;
        // Set pointer fields
        temp->before = p;
        temp->next = NULL;
        p->next = temp;
        p = temp;
        // Set data fields
        temp->pcb.id = pcbID++;
        temp->pcb.arrivedTime = 0;
        temp->pcb.requiredTime = rand() % 10 + 1;
        temp->pcb.usedTime = 0;
        strcpy(temp->pcb.state, "Wait");
        temp->pcb.size = rand() % 89 + 1;
        temp->pcb.startAddress = allocate(temp->pcb.size);
        // Update the tail node of the list (ReadyList->before)
        ReadyList->before = temp;
    }

    // if (ReadyList->next == NULL) cout << "ReadyList" << endl;
    // else 
    //     cout << "ID next: " << ReadyList->next->pcb.id << endl; 
}

// Print PCB linked list to display the status of each PCB
void showPCB() {
    //mtx.lock();
	pPCBNode p = ReadyList->next;

    cout << "Process Control Block Table" << endl;
    VariadicTable<int, int, int, int, char*, int, int> vt({"PID", "Arrived Time", "Required Time", "Used Time", "State", "Memory Start", "Memory Size"}, 10);
	
    // while(p) {
	// 	PCB pcb = p->pcb;
    //     vt.addRow(pcb.id, pcb.arrivedTime, pcb.requiredTime, pcb.usedTime, pcb.state, pcb.startAddress, pcb.size);
	// 	p = p->next;
	// }

    while(p) {
		PCB *pcb = &(p->pcb);
        vt.addRow(pcb->id, pcb->arrivedTime, pcb->requiredTime, pcb->usedTime, pcb->state, pcb->startAddress, pcb->size);
		p = p->next;
	}

    vt.print(std::cout);
    //mtx.unlock();
}



//Init Mem
void initMem(){

    FreeMemNode tmp = {.size = MEMORYSIZE - KERNELSIZE, .startAddress = KERNELSIZE };
    FMPT.push_back(tmp);
}

void showMem(){

    cout << "Free Memory Partition Table" << endl;
    VariadicTable<int, int, int> vt({"ID", "Start Address", "Free Size"}, 10);
	
    for(int i= 0 ;i< FMPT.size();i++ ){
        vt.addRow(i,FMPT[i].startAddress, FMPT[i].size);
    }
    
    vt.print(std::cout);
}

pPCBNode checkReadyList() {
    
	pPCBNode p = ReadyList->next;
	while(p) {
		if (strcmp(p->pcb.state, "Wait") == 0) {
			mtx.unlock();
            //cout << "p != NULL\n";
			return p;
		}
		p = p->next;
        //cout << "Next\n";
	}

    //cout << "p = NULL\n";
	return NULL;
    
}

void initOS(){
    // Initialize memory
    initMem();
    // Simulate a randomly distributed free memory table
    vector<FreeMemNode> test;
    for (int i = 0; i < 15; i++) {
        int memSize = rand() % 100 + 1;
        int startAddress = allocate(memSize);
        if (startAddress != -1) {
            FreeMemNode tmp = {.size = memSize, .startAddress = startAddress};
            test.push_back(tmp);
        }
    }
    for (int i = 1; i < test.size() - 1; i += 2) {
        releaseAndMerge(test[i].size, test[i].startAddress);
    }
    cout << "\nInitial Memory Allocation" << endl;
    showMem();

    // StartPCB
    initPCB();
    cout << "\nInitial Task List" << endl;
    showPCB();
}

void startOS() {
	pPCBNode p = NULL;
	while(1) {
        p = checkReadyList();
		if (p != NULL) {
			strcpy(p->pcb.state, "Running");
            int remainingTime = p->pcb.requiredTime - p->pcb.usedTime;
			if (remainingTime <= TIME) {
				this_thread::sleep_for(remainingTime*1000ms);
				p->pcb.usedTime = p->pcb.requiredTime;
                // Process termination primit2ive
                //showPCB();
                //showMem();
                // cout << "The Process with ID = " << p->pcb.id << " has beend killed." << endl;
                kill(p);
			} 
            else {			
                this_thread::sleep_for(remainingTime*1000ms);
				//showPCB();
                //showMem();
                strcpy(p->pcb.state, "Wait");
				p->pcb.usedTime += TIME;
				if (p->next) {
                    // Process switching primitive
					schedule(p);
				}
			}
		}
	}
}

void creatPCB(){
	
    mtx.lock(); // mtx.lock is used to lock the kernel data structure (PCB linked list)
	    pPCBNode temp = new PCBNode;
	    // Set pointer fields
	    temp->before = ReadyList->before;
	    temp->next = NULL;
	    ReadyList->before->next = temp;
	    ReadyList->before = temp;
	    // Set data fields
	    temp->pcb.id = pcbID;
        temp->pcb.arrivedTime = ((clock() - beginTime) / CLOCKS_PER_SEC );
	    temp->pcb.requiredTime = rand() % 10 + 1;
	    temp->pcb.usedTime = 0;
	    temp->pcb.size = rand() % 49 + 1;
		temp->pcb.startAddress = allocate(temp->pcb.size);
	    strcpy(temp->pcb.state, "Wait");
	    // Update the tail node of the list
	    pcbID++;
    mtx.unlock();
    
    VariadicTable<int, int, int, int, char*, int, int> vt({"PID", "Arrived Time", "Required Time", "Used Time", "State", "Memory Start", "Memory Size"}, 10);
    PCB pcb = temp->pcb;

    vt.addRow(pcb.id, pcb.arrivedTime, pcb.requiredTime, pcb.usedTime, pcb.state, pcb.startAddress, pcb.size);

    vt.print(std::cout);

	
}

void schedule(pPCBNode &p){
	mtx.lock();
	    p->before->next = p->next;
	    p->next->before = p->before;
	    p->before = ReadyList->before;
	    ReadyList->before->next = p;
	    ReadyList->before = p;
	    p->next = NULL;
	mtx.unlock();    
}

void kill(pPCBNode &p){
	mtx.lock();
    strcpy(p->pcb.state, "Killed");
		releaseAndMerge(p->pcb.size,p->pcb.startAddress);
		if (p->next) {
		    p->before->next = p->next;
		    p->next->before = p->before;
		    delete p;
		}else{
			p->before->next = p->next;
			ReadyList->before = ReadyList;
			delete p;
		}
	mtx.unlock();  

}

// Tip: Be cautious when controlling the mutex in a function with multiple exit points.
void block(int id){
	mtx.lock();
	    pPCBNode p = ReadyList->next;
	    while(p) {
			if(p->pcb.id == id){
	            strcpy(p->pcb.state, "Blocked");
                cout << "ID=" << id << " is blocked" << endl;
                mtx.unlock();
	            return;
	        }
			else{
	            p = p->next;
	        }
		}
	   // The process to be blocked does not exist
        cout << "The process to be blocked does not exist" << endl;
	mtx.unlock();   
}


void wakeup(int id){
	mtx.lock();   
	    pPCBNode p = ReadyList->next;
	    while(p) {
			if(p->pcb.id == id){
	            strcpy(p->pcb.state, "Wait");
                cout << "ID=" << id << " is woken up" << endl;
                mtx.unlock();
	            return;
	        }
			else{
	            p = p->next;
	        }
		}
	    // The process to be woken up does not exist
        cout << "The process to be woken up does not exist" << endl;
	mtx.unlock();   
}

int allocate(int memoryNeeded) {
    int tmp = -1;
    for (int i = 0; i < FMPT.size(); i++) {
        //cout << "memoryNeeded: " << memoryNeeded << ", FMPT[" << i << "].size: " << FMPT[i].size << ", FMPT[" << i << "].startAddress: " << FMPT[i].startAddress << endl;
        if (FMPT[i].size >= memoryNeeded) {
            tmp = FMPT[i].startAddress;
            FMPT[i].size -= memoryNeeded;
            FMPT[i].startAddress += memoryNeeded;
            break;
        }
    }

    if (tmp == -1)
        cout << "Allocation Failure" << endl;

    return tmp;
}


void releaseAndMerge(int toReleasedSize, int startAddress) {
    int endAddress = startAddress + toReleasedSize;

    for (int i = 0; i < FMPT.size(); i++) {
        if (endAddress == FMPT[i].startAddress) {
            FMPT[i].startAddress = startAddress;
            FMPT[i].size += toReleasedSize;
            mergeAdjacentPartitions(i);
            return;
        } else if (startAddress == FMPT[i].startAddress + FMPT[i].size) {
            FMPT[i].size += toReleasedSize;
            mergeAdjacentPartitions(i);
            return;
        } else if (endAddress < FMPT[i].startAddress) {
            FreeMemNode tmp = {.size = toReleasedSize, .startAddress = startAddress};
            FMPT.insert(FMPT.begin() + i, tmp);
            mergeAdjacentPartitions(i);
            return;
        }
    }

    FreeMemNode tmp = {.size = toReleasedSize, .startAddress = startAddress};
    FMPT.push_back(tmp);
    mergeAdjacentPartitions(FMPT.size() - 1);
}


void mergeAdjacentPartitions(int index) {
    if (index > 0 && FMPT[index - 1].startAddress + FMPT[index - 1].size == FMPT[index].startAddress) {
        FMPT[index - 1].size += FMPT[index].size;
        FMPT.erase(FMPT.begin() + index);
    }
    if (index < FMPT.size() - 1 && FMPT[index].startAddress + FMPT[index].size == FMPT[index + 1].startAddress) {
        FMPT[index].size += FMPT[index + 1].size;
        FMPT.erase(FMPT.begin() + index + 1);
    }
}


