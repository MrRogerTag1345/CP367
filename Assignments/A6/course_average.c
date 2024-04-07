#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

void getrowandcol(int *arr);
void readfromtxtintoarray(int **arr, int row, int cols);
void Teacherprocess(int **arr);
void GTAprocess(int chapter, int **arr);

int main() {
    int arr[2]; // arr[0] = num of rows, arr[1] = num of cols
    getrowandcol(arr); // get num of rows and cols

    // allocate memory
    int ** mainarr = (int **)malloc(arr[0] * sizeof(int *));
    for (int i = 0; i < arr[0]; i++) {
        mainarr[i] = (int *)malloc(arr[1] * sizeof(int));
    }

    // Put values into a 2d array
    readfromtxtintoarray(mainarr, arr[0], arr[1]);

    // -----------------------------------------------------
    // create the processes and do the work.
    // -----------------------------------------------------
    Teacherprocess(mainarr);

    // Deallocation
    for (int i = 0; i < arr[0]; i++) {
        free(mainarr[i]);
    }
    free(mainarr);

    return 0;
}

void getrowandcol(int *arr) {
    FILE *countrandc = fopen("sample_in_grades.txt", "r"); // Open the file.
    int numStudents = 1, numAssignments = 0; // Rows = students, columns = assignments.
    int prevChar = ' ';
    int isNewLine = 1;
    int c;

    // Checks to see if the file is not NULL
    if (countrandc == NULL) {
        printf("Failed to open the file.\n");
        return;
    }

    // Count the number of rows and columns
    while ((c = fgetc(countrandc)) != EOF) {
        if (c == '\n') {
            numStudents += 1;
            isNewLine = 1;
        } else if (c != ' ') {
            if (isNewLine || prevChar == ' ') {
                numAssignments += 1;
            }
            isNewLine = 0;
        }
        prevChar = c;
    }

    // Get # cols for 1 row
    numAssignments = numAssignments / numStudents;
    fclose(countrandc); // close file handle

    // Set arr to values
    arr[0] = numStudents;
    arr[1] = numAssignments;
}

void readfromtxtintoarray(int **arr, int row, int cols) {
    FILE *gradesIn = fopen("sample_in_grades.txt", "r"); // Open the file.

    if (gradesIn == NULL) {
        printf("Failed to open the file.\n");
        return;
    }

    // Read values from the file and store them in the array
    for (int i = 0; i < row; i++) {
        for (int j = 0; j < cols; j++) {
            if (fscanf(gradesIn, "%d", &arr[i][j]) != 1) {
                printf("Failed to read a value from the file.\n");
                fclose(gradesIn);
                return;
            }
        }
    }

    // close file.
    fclose(gradesIn);
}

void Teacherprocess(int **arr) {
    for (int chapter = 0; chapter < 3; chapter++) {
        pid_t pidGTA = fork();

        /* GradTA process */
        if (pidGTA == 0) {
            GTAprocess(chapter, arr);
            exit(0);
        } else if (pidGTA < 0) {
            fprintf(stderr, "Failed to create GradTA process for Chapter %d\n", chapter + 1);
        } else {
            wait(NULL);
        }
    }
}

void GTAprocess(int chapter, int **arr) {
    for (int assign = 0; assign < 2; assign++) {
        pid_t pidTA = fork();

        if (pidTA == 0) {
            int sumgrade = 0;
            for (int std = 0; std < 10; std++) {
                sumgrade += arr[std][chapter * 2 + assign];
            }

            printf("Assignment %d - Average: %.6lf\n", (assign + 1 + (chapter * 2)), (sumgrade / 10.0));
            exit(0);
        } else if (pidTA < 0) {
            fprintf(stderr, "Failed to create TA process for Chapter %d Assignment %d\n", chapter + 1, assign + 1);
        } else {
            wait(NULL);
        }
    }
}
