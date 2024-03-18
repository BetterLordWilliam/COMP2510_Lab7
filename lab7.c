#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// A01372608

typedef struct student{
    char *fname;
    char *lname;
    char *aNum;
    int midterm;
    int final;
    int average;
} Student;

typedef struct listNode{
    struct student *tStudent;
    struct listNode *next;
} ListNode;

FILE *in;
FILE *out;

// Function headers
void printErrorExit();
void innitList(ListNode *head, int *option);
void basicPrint(ListNode *head);
void insertStudent(ListNode *head, Student *newStudent);
void freeList(ListNode *head);
void exportList(int *option, ListNode *head);

int checkName(char *name);
int checkA_Num(char *aNum);
int checkGrade(int grade);

/**
 * main:            Drives the program.
 *                  Assumes two operands are required.
 * param argc:      Argument count 
 * param *argv[]:   Arguments themselves 
*/
int main(int argc, char* argv[]){

    // Lab setup
    if (argc != 3)
        return 1;

    in = fopen(argv[1], "r");
    out = fopen(argv[2], "w");

    // Important variables
    int option = 0, *optionP;

    ListNode *head = malloc(sizeof(struct listNode));   // Actual list head
    head->tStudent = NULL;                              // make NULL
    head->next = NULL;                                  // make NULL

    optionP = &option;

    // Bad file open
    if (in == NULL || out == NULL)
        return 1;


    innitList(head, optionP);
    // basicPrint(head);               // basic test print function
    exportList(optionP, head);
    
    freeList(head);

    return 0;
}

/**
 * printErrorExit:      Prints 'exit' to out file and closes files. Terminates program.
*/
void printErrorExit() {
    if (out != NULL) {
        if (in != NULL) fclose(in);
        fprintf(out, "error\n");
        fclose(out);
    } else {
        printf("printErrorExit called incorrectly.\n");
    }
    exit(1);
}

/**
 * checkGrade:          Checks if a grade is in the valid range.
 * param grade:         the grade that is going to be checked
*/
int checkGrade(int grade) {
    if (!(grade >= 0 && grade <= 100))
        return 0;

    return 1;
}

/**
 * checkA_Num:          Checks is an a_num is numeric and starts with A
 * param *A_Num:        a_num what we are checking
*/
int checkA_Num(char *A_Num) {
    if (A_Num == NULL)
        return 0;

    int pos = 0;
    int size = 0;

    while (*(A_Num + pos) != '\0') {
        // Starts with A
        if (pos == 0 && *(A_Num + pos) != 'A')
            return 0;
        // Digits are numbers
        else if (pos != 0 
            && !(*(A_Num + pos) >= '0' && *(A_Num + pos) <= '9')) {
            return 0;   
        }

        size++;
        pos++;
    }

    if (size == 8)
        return 1;
    else 
        return 0;
}

/**
 * checkName:           Checks if a name contains only alphabetical characters and dashes.
 * param *name:         name we are checking
*/
int checkName(char *name) {
    if(name == NULL)
        return 0;
    
    int pos = 0;

    while (*(name + pos) != '\0') {
        // character is alphabetical OR a dash
        if (!((*(name + pos) >= 'a' && *(name + pos) <= 'z') 
            || (*(name + pos) >= 'A' && *(name + pos) <= 'Z')
            || *(name + pos) == '-'))
            return 0;    

        pos++;
    }

    return 1;
}

/**
 * compareStudents:         compares two students to see what order they should fall in.
 * param *a:                first student
 * param *b:                second student
*/
int compareStudents(Student *a, Student *b) {
    int lastNameComparison = strcmp(a->lname, b->lname);
    if (lastNameComparison != 0) return lastNameComparison;

    int firstNameComparison = strcmp(a->fname, b->fname);
    if (firstNameComparison != 0) return firstNameComparison;

    int studentNumberComparison = strcmp(a->aNum, b->aNum);
    if (studentNumberComparison != 0) return studentNumberComparison;

    if (a->midterm != b->midterm) return a->midterm - b->midterm;

    return a->final - b->final;
}

/**
 * insertStudent:       inserts a student into his position in the list.
 * param *head:         the head of the list
 * param *student:      student that we are going to put into the list
*/
void insertStudent(ListNode *head, Student *newStudent) {
    ListNode* newNode = malloc(sizeof(struct listNode));
    
    if (newNode == NULL)
        printErrorExit();

    newNode->tStudent = newStudent;
    newNode->next = NULL;

    if (head->next == NULL || compareStudents(newStudent, head->next->tStudent) < 0) {
        newNode->next = head->next;
        head->next = newNode;
    } else {
        ListNode* current = head;
        while (current->next != NULL && compareStudents(newStudent, current->next->tStudent) > 0) {
            current = current->next;
        }
        newNode->next = current->next;
        current->next = newNode;
    }
}

/**
 * innitList:               reads the file input, creates student structures and inserts them into the list.
 * param *studentCount:     the number of numbers
 * param *option:           the option that the program will do
*/
void innitList(ListNode *head, int *option) {
    char check = 0; // blank slate
    int ncount = 0; // student count

    // Determine the number of students
    while ((check = fgetc(in)) != EOF) {
        if (check == '\n') {
            ncount++;
        }
    }
    ncount-=2;                                       // ensure correct number of students (subtract option and E).

    char buff[100];       
    fseek(in, 0, SEEK_SET);                          // set buffer back to the beginning.
    fgets(buff, 100, in);
    sscanf(buff, "%d", option);

    if (!(*option >= 1 && *option <= 5))             // Valid option range.
        printErrorExit();
    
    fseek(in, 2, SEEK_SET);                          // set buffer to the beginning of student list.
    int track = 0;

    while (fgets(buff, 100, in) != NULL && track < ncount) {
        char *lNameT = malloc(15 * sizeof(char));
        char *fNameT = malloc(15 * sizeof(char));
        char *aNumT = malloc(15 * sizeof(char));
        int mGradeT = 0;
        int fGradeT = 0;

        if (fNameT == NULL || lNameT == NULL || aNumT == NULL)                  // Variable malloc success check
            printErrorExit();

        int stat = sscanf(buff, "%s %s %s %d %d\n", 
            (char*)lNameT, (char*)fNameT, (char*)aNumT, &mGradeT, &fGradeT);    // Keep track of success

        if (stat != 5)
            printErrorExit();                                                   // There must be 5 things in each line

        if (checkGrade(mGradeT) && checkGrade(fGradeT) 
            && checkA_Num(aNumT) && checkName(fNameT) 
            && checkName(lNameT)) {

            Student *newStudent = malloc(sizeof(struct student));

            if (newStudent == NULL)
                printErrorExit();

            newStudent->lname = lNameT;
            newStudent->fname = fNameT;
            newStudent->aNum = aNumT;
            newStudent->midterm = mGradeT;
            newStudent->final = fGradeT;
            newStudent->average = (mGradeT + fGradeT) / 2;
            insertStudent(head, newStudent);

        } else {
            printErrorExit();
        }

        track++;
    }
}

/**
 * freeList:                Frees memory used by the linkedlist.
 * param *head:             the start of the list   
*/
void freeList(ListNode *head) {
    ListNode *itr = head->next;

    while (itr != NULL) {
        ListNode *temp = itr->next;
        free(itr->tStudent);
        free(itr);
        itr = temp;
    }
}

/**
 * basicPrint:              basic print function, prints all student information. For testing purposes.
 * param *head:             the start of the list
*/
void basicPrint(ListNode *head) {
    ListNode *itr = head->next;

    while(itr != NULL) {
        Student *cStudent = itr->tStudent;
        printf("%s %s %s Midterm: %d Final: %d Average: %d\n", 
            cStudent->lname, cStudent->fname, cStudent->aNum, 
            cStudent->midterm, cStudent->final, cStudent->average);

        itr = itr->next;
    }
}

/**
 * filterAndPut:            writes list node contents if they satisfy filter.
 * param siftLower:         the lower bound filter we will apply to the list contents (inclusive)
 * param siftUpper:         the uper bound filter we will apply to the list contents (exclusive)
 * param *head:             the start of the list
*/
void filterAndPut(int siftLower, int siftUpper, ListNode *head) {
    if (out != NULL) {
        ListNode *itr = head->next;
        if (itr == NULL)
            return;

        while (itr != NULL) {
            Student *cS = itr->tStudent;
            if (cS->average >= siftLower && cS->average < siftUpper)
                fprintf(out, "%s %s %s %d %d\n", 
                    cS->lname, cS->fname, cS->aNum, cS->midterm, cS->final);
            itr = itr->next;
        }

    } else {
        printErrorExit();
    }
}

/**
 * exportList:              Exports list contents to output file based of option.
 * param option:            denotes the type of export
 * param *head:             the start of the list
*/
void exportList(int *option, ListNode *head) {
    switch(*option) {
        case 1:
            filterAndPut(90, 100, head);
            break;
        case 2:
            filterAndPut(80, 90, head);
            break;
        case 3:
            filterAndPut(70, 80, head);
            break;
        case 4:
            filterAndPut(60, 70, head);
            break;
        case 5:
            filterAndPut(0, 60, head);
            break;
    }
}