#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define QUEUE_SIZE 5
#define FILE_NAME "input.txt"
#define INSERT 0
#define TIMEOUT 1
#define COMPLETE -1

int currentTime;

typedef struct Pcb* PcbPointer;
struct Pcb {
    int type;
    int processId;
    int priority;
    int computingTime;
    int queueId;
    int turnAroundTime;
    int waitTime;
    PcbPointer leftLink;
    PcbPointer rightLink;
};

typedef struct InputData* InputDataPointer;
struct InputData {
    int type;
    int priority;
    int computingTime;
};

void scheduleWithData(PcbPointer*, int);
void initializeQueue(PcbPointer*);
InputDataPointer extractDataFromInput(FILE*, char*);
void inputDataToQueue(PcbPointer*, InputDataPointer);
void timeoutSchedule(PcbPointer*, InputDataPointer, int);
void inputPcbToQueueLast(PcbPointer*, PcbPointer*);
bool isLastQueue(int);
bool isUnderTimeQuantum(int, int);
bool isNextQueueNotEmpty(PcbPointer*, int);
FILE* openFile(void);
void scheduleWithRemainder();
InputDataPointer extractValue(char* line);

int main(int argc, char const *argv[]) {
    int q = 1;
    PcbPointer queues[5] = { NULL };

    //TODO: 시간 할당량 옵션 지정.
    initializeQueue(queues);
    scheduleWithData(queues);
    scheduleWithRemainder();
}

void initializeQueue(PcbPointer* queues) {
}

void scheduleWithData(PcbPointer* queues, int q) {
    char* line = NULL;
    FILE* filePointer = NULL;

    filePointer = openFile();

    while (true) {
        InputDataPointer data = extractDataFromInput(filePointer, line);

        if (data->type == INSERT) {
            inputDataToQueue(queues, data);
            free(data);
        } else if (data->type == TIMEOUT) {
            timeoutSchedule(queues, data, q);
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
    char buffer[256] = { NULL };
    line = fgets(buffer, 256, filePointer);

    char* newLine = strchr(line, '\n');

    if (newLine)
        *newLine = 0;

    return extractValue(line);
}

InputDataPointer extractValue(char* line) {
    char* dataType= strtok(line, " ");
    char* dataPriority = strtok(NULL, " ");
    char* dataComputingTime = strtok(NULL, " ");

    InputDataPointer result = (InputDataPointer) malloc(sizeof(struct InputData));
    result->type = atoi(dataType);
    result->priority = dataPriority == NULL ? 0 : atoi(dataPriority); 
    result->computingTime = dataComputingTime == NULL ? 0 : atoi(dataComputingTime);
    return result;
}

void inputDataToQueue(PcbPointer* queues, InputDataPointer data) {
    //TODO: queue에 데이터 삽입 구현
    PcbPointer newJob = (PcbPointer) malloc(sizeof(struct Pcb));
    PcbPointer currentJob = NULL;

    if (newJob == NULL) {
        printf("Memory allocation error\n");
        exit(0);
    }

    newJob->priority = data->priority;
    newJob->computingTime = data->computingTime;
    newJob->rightLink = NULL;
    newJob->leftLink = NULL;
    newJob->queueId = 0;

    if (queues[0] == NULL) {
        queues[0] = newJob;
        return;
    }

    currentJob = queues[0];
    if (currentJob == NULL) {
        queues[0]->rightLink = newJob;
        newJob->leftLink = queues[0];
    } else {
        inputPcbToQueueLast(currentJob, newJob);
    }
}

void inputPcbToQueueLast(PcbPointer* currentJob, PcbPointer* newJob) {
    while (currentJob->rightLink != NULL) {
        currentJob = currentJob->rightLink;
    }
    currentJob->rightLink = newJob;
    newJob->leftLink = currentJob;
}

int timeoutSchedule(PcbPointer* queues, InputDataPointer data, int q) {
    //TODO: 타임아웃 스케줄링
    //1. 스케줄링할 큐 선정
    //2. 큐에서 데이터 스케줄링
    //3. 선점점에 도달하는 데이터인지 아닌지 확인하고
    //4. 각각 별도로 스케줄링.
    PcbPointer selectedPcb = NULL;
    int queueIndex;

    for (queueIndex = 0; queues[queueIndex] != NULL; queueIndex++) {
        selectedPcb = queues[queueIndex];
    }

    if (queues[queueIndex] == NULL) {
        return -1;
    }

    //TODO: 시간 할당량 선정
    if (isUnderTimeQuantum(selectedPcb->computingTime, q)) {
        selectedPcb->computingTime = 0;
        selectedPcb->turnAroundTime; //TODO: 도착 시간을 어떻게 구할 것 인

        queues[queueIndex] = selectedPcb->rightLink;
        selectedPcb->rightLink->leftLink=NULL;
        selectedPcb->rightLink = NULL;

        free(selectedPcb);
    } else {
        selectedPcb->computingTime -= q;
        selectedPcb->turnAroundTime; //TODO: 도착 시간을 어떻게 구할 것 인가?

        queues[queueIndex] = selectedPcb->rightLink;
        selectedPcb->rightLink->leftLink=NULL;
        selectedPcb->rightLink = NULL;

        if (isLastQueue(queueIndex)) {
            inputPcbToQueueLast(queues[queueIndex], selectedPcb);
        }
        
        if (isNextQueueNotEmpty(queues, queueIndex)) {
            inputPcbToQueueLast(queues[queueIndex + 1], selectedPcb);
        } else {
            queues[queueIndex + 1] = selectedPcb;
        }
    }

    return 1;
}

bool isLastQueue(int index) {
    return index == QUEUE_SIZE - 1;
}

bool isUnderTimeQuantum(int computingTime, int q) {
    return computingTime <= q;
}

bool isNextQueueNotEmpty(PcbPointer* queues, int queueIndex) {
    return queues[queueIndex + 1] != NULL;
}

void scheduleWithRemainder() {
    //TODO: 나머지 스케줄링
}
