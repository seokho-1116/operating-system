#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>

#define QUEUE_SIZE 5
#define FILE_NAME "input.txt"
#define INSERT 0
#define TIMEOUT 1
#define COMPLETE -1
#define SUCCEED 3
#define QUEUE_EMPTY 4

typedef struct Pcb* PcbPointer;
struct Pcb {
    int type;
    int processId;
    int priority;
    int remainTime;
    int computingTime;
    int queueId;
    int turnAroundTime;
    int arrivalTime;
    PcbPointer leftLink;
    PcbPointer rightLink;
};

typedef struct InputData* InputDataPointer;
struct InputData {
    int type;
    int processId;
    int priority;
    int computingTime;
};

void scheduleWithData(PcbPointer*, int, int*);
FILE* openFile(void);
InputDataPointer extractDataFromInput(FILE*, char*);
InputDataPointer extractValue(char*);
void inputDataToQueue(PcbPointer*, InputDataPointer, int, int);
bool isQueueEmpty(PcbPointer*, int);
void inputPcbToQueueLast(PcbPointer*, PcbPointer*);
int scheduleProcess(PcbPointer*, int, int*);
void printAllDataInQueues(PcbPointer* queues, int);
bool isNotFirstQueue(int);
void removePcbLink(PcbPointer*, PcbPointer*);
bool isLastDataInQueue(PcbPointer);
bool isUnderTimeQuantum(int, int);
void printPcbInfo(PcbPointer);
void movePcbToNextQueue(PcbPointer*, PcbPointer*, int);
bool isLastQueue(int);
void scheduleWithRemainder(PcbPointer*, int, int*);

int main(int argc, char const *argv[]) {
	if (argc != 2) {
		printf("Usage : %s <TIME>\n", argv[0]);
		exit(1);
	}

    int time = atoi(argv[1]);
    PcbPointer queues[QUEUE_SIZE] = { NULL };
    int elapsedTime = 0;

    printf("prcessId\t queueId\t priority\t computingTime\t turnAroundTime\n");

    scheduleWithData(queues, time, &elapsedTime);
    scheduleWithRemainder(queues, time, &elapsedTime);
}

void scheduleWithData(PcbPointer* queues, int time, int* elapsedTime) {
    char* line = NULL;
    FILE* filePointer = NULL;
    int timeoutCount = 0;

    filePointer = openFile();

    while (true) {
        InputDataPointer data = extractDataFromInput(filePointer, line);

        if (data->type == INSERT) {
            inputDataToQueue(queues, data, time, timeoutCount);
            free(data);
        } else if (data->type == TIMEOUT) {
            timeoutCount++;
            scheduleProcess(queues, time, elapsedTime);
        } else if (data->type == COMPLETE) {
            break;
        } else {
            printf("Error: Invalid input data\n");
            exit(1);
        }
    }
    fclose(filePointer);
}

FILE* openFile(void) {
    FILE* filePointer = NULL;
    filePointer = fopen(FILE_NAME, "r");
    if (filePointer == NULL) {
        printf("open error!\n");
        exit(0);
    }
    return filePointer;
}

InputDataPointer extractDataFromInput(FILE* filePointer, char* line) {
    char buffer[256] = { 0 };
    line = fgets(buffer, 256, filePointer);

    char* newLine = strchr(line, '\n');

    if (newLine)
        *newLine = 0;

    return extractValue(line);
}

InputDataPointer extractValue(char* line) {
    char* dataType= strtok(line, " ");
    char* dataId = strtok(NULL, " ");
    char* dataPriority = strtok(NULL, " ");
    char* dataComputingTime = strtok(NULL, " ");

    InputDataPointer result = (InputDataPointer) malloc(sizeof(struct InputData));
    result->type = atoi(dataType);
    result->processId = dataId == NULL ? 0 : atoi(dataId);
    result->priority = dataPriority == NULL ? 0 : atoi(dataPriority);
    result->computingTime = dataComputingTime == NULL ? 0 : atoi(dataComputingTime);
    return result;
}

void inputDataToQueue(PcbPointer* queues, InputDataPointer data, int t, int timeoutCount) {
    PcbPointer newJob = (PcbPointer) malloc(sizeof(struct Pcb));

    if (newJob == NULL) {
        printf("Memory allocation error\n");
        exit(0);
    }

    newJob->processId = data->processId;
    newJob->priority = data->priority;
    newJob->remainTime = data->computingTime;
    newJob->computingTime = data->computingTime;
    newJob->rightLink = NULL;
    newJob->leftLink = NULL;
    newJob->queueId = 0;
    newJob->turnAroundTime = 0;
    newJob->arrivalTime = t * timeoutCount;

    if (isQueueEmpty(queues, 0)) {
        queues[0] = newJob;
        return;
    } else {
        inputPcbToQueueLast(&queues[0], &newJob);
    }
}

bool isQueueEmpty(PcbPointer* queues, int queueIndex) {
    return queues[queueIndex] == NULL;
}

void inputPcbToQueueLast(PcbPointer* queue, PcbPointer* pcb) {
    PcbPointer head = (*queue);

    while (head->rightLink != NULL) {
        head = head->rightLink;
    }

    head->rightLink = (*pcb);
    (*pcb)->leftLink = head;
}

int scheduleProcess(PcbPointer* queues, int time, int* elapsedTime) {
    PcbPointer selectedQueue = NULL;
    int queueIndex;

    for (queueIndex = 0; queueIndex < QUEUE_SIZE; queueIndex++) {
        selectedQueue = queues[queueIndex];
        if (selectedQueue != NULL) {
            break;
        }
    }
    
    if (selectedQueue == NULL) {
        //printf("\t\t\t\t\t\t\t\t\t\t\t------------------Empty------------------\n");
        return QUEUE_EMPTY;
    }

    int timeForEachQueue = time;
    if (isNotFirstQueue(queueIndex)) {
        timeForEachQueue = (int) pow(time, queueIndex);
    }

    //printAllDataInQueues(queues, (*elapsedTime));

    removePcbLink(&queues[queueIndex], &selectedQueue);

    if (isUnderTimeQuantum(selectedQueue->remainTime, timeForEachQueue)) {
        (*elapsedTime) += selectedQueue->remainTime;
        selectedQueue->turnAroundTime = (*elapsedTime) - selectedQueue->arrivalTime;

        selectedQueue->remainTime = 0;

        printPcbInfo(selectedQueue);
        free(selectedQueue);
    } else {
        (*elapsedTime)+= timeForEachQueue;

        selectedQueue->remainTime -= timeForEachQueue;

        movePcbToNextQueue(queues, &selectedQueue, queueIndex);
    }

    return SUCCEED;
}

void printAllDataInQueues(PcbPointer* queues, int elapsedTime) {
    printf("\n\t\t\t\t\t\t\t\t\t\tTime:%d\n", elapsedTime);

    for (int queueIndex = 0; queueIndex < QUEUE_SIZE; queueIndex++) {
        if (queues[queueIndex] != NULL) {
            printf("\t\t\t\t\t\t\t\t\t\tqueue %d: ", queueIndex);
            PcbPointer head = queues[queueIndex];
            while (head != NULL) {
                printf("[%d, %d, %d] ", head->processId, head->priority, head->remainTime);
                head = head->rightLink;
            }
            printf("\n\n");
        }
    }
}

bool isNotFirstQueue(int index) {
    return index != 0;
}

void removePcbLink(PcbPointer* queue, PcbPointer* pcb) {
    if (isLastDataInQueue(*pcb)) {
        (*queue) = NULL;
    } else {
        (*queue) = (*pcb)->rightLink;
        (*pcb)->rightLink->leftLink=NULL;
        (*pcb)->rightLink = NULL;
    }
}

bool isLastDataInQueue(PcbPointer pcb) {
    return pcb->rightLink == NULL;
}

bool isUnderTimeQuantum(int remainTime, int time) {
    return remainTime <= time;
}

void printPcbInfo(PcbPointer pcb) {
    printf(
        "%d\t\t %d\t\t %d\t\t %d\t\t %d\t\t \n", 
        pcb->processId, pcb->queueId, pcb->priority, pcb->computingTime,pcb->turnAroundTime);
}

void movePcbToNextQueue(PcbPointer* queues, PcbPointer* pcb, int queueIndex) { 
    if (isLastQueue(queueIndex)) {
        if(isQueueEmpty(queues, queueIndex)) {
            queues[queueIndex] = (*pcb);
        } else {
            inputPcbToQueueLast(&queues[queueIndex], pcb);
        }
    } else {
        (*pcb)->queueId = queueIndex + 1;

        if (isQueueEmpty(queues, queueIndex + 1)) {
            queues[queueIndex + 1] = (*pcb);
        } else {
            inputPcbToQueueLast(&queues[queueIndex + 1], pcb);
        }
    }
}

bool isLastQueue(int index) {
    return index == QUEUE_SIZE - 1;
}

void scheduleWithRemainder(PcbPointer* queues, int time, int* elapsedTime) {
    while (scheduleProcess(queues, time, elapsedTime) == SUCCEED);
}
