#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define FILENAME "attendance_db.txt"
#define MAX_ID 20
#define MAX_DATE 15

// --- Data Structure ---
typedef struct {
    char studentID[MAX_ID];
    char date[MAX_DATE];
    int status; // 1 = Present, 0 = Absent
} AttendanceRecord;

// --- Function Prototypes ---
void markAttendance();
AttendanceRecord* loadRecords(int *count);
void viewStudentStats(AttendanceRecord *records, int count);
void checkDefaulters(AttendanceRecord *records, int count);
void generateAnalytics(AttendanceRecord *records, int count);
void clearInputBuffer();

int main() {
    int choice, recordCount = 0;
    AttendanceRecord *data = NULL;

    while(1) {
        printf("\n--- Attendance Analytics System ---\n");
        printf("1. Mark Attendance (Save to File)\n");
        printf("2. View Student Percentage\n");
        printf("3. Identify Defaulters (<75%%)\n");
        printf("4. Class Analytics (Lecture-wise)\n");
        printf("5. Exit\n");
        printf("Enter choice: ");
        scanf("%d", &choice);
        clearInputBuffer();

        switch(choice) {
            case 1:
                markAttendance();
                break;
            case 2:
                // Load data dynamically for analysis
                if(data != NULL) free(data); // Free previous load
                data = loadRecords(&recordCount);
                if(data) viewStudentStats(data, recordCount);
                break;
            case 3:
                if(data != NULL) free(data);
                data = loadRecords(&recordCount);
                if(data) checkDefaulters(data, recordCount);
                break;
            case 4:
                if(data != NULL) free(data);
                data = loadRecords(&recordCount);
                if(data) generateAnalytics(data, recordCount);
                break;
            case 5:
                if(data != NULL) free(data);
                printf("Exiting system...\n");
                exit(0);
            default:
                printf("Invalid choice.\n");
        }
    }
    return 0;
}

// --- 1. File I/O & Data Entry ---
void markAttendance() {
    FILE *fp = fopen(FILENAME, "a"); // Append mode
    if (!fp) {
        perror("Error opening file");
        return;
    }

    AttendanceRecord rec;
    printf("Enter Date (YYYY-MM-DD): ");
    scanf("%s", rec.date);
    
    printf("Enter Student ID: ");
    scanf("%s", rec.studentID);
    
    printf("Is student present? (1 for Yes, 0 for No): ");
    scanf("%d", &rec.status);

    // Write to file: ID Date Status
    fprintf(fp, "%s %s %d\n", rec.studentID, rec.date, rec.status);
    fclose(fp);
    printf("Attendance saved successfully.\n");
}

// --- 2. Dynamic Memory & Loading ---
AttendanceRecord* loadRecords(int *count) {
    FILE *fp = fopen(FILENAME, "r");
    if (!fp) {
        printf("No database found. Mark attendance first.\n");
        *count = 0;
        return NULL;
    }

    // Step 1: Count lines to determine memory needed
    int lines = 0;
    char ch;
    while(!feof(fp)) {
        ch = fgetc(fp);
        if(ch == '\n') lines++;
    }
    rewind(fp); // Go back to start of file

    // Step 2: Dynamic Allocation
    /* Using malloc to allocate exactly the memory needed 
       based on file size (Syllabus application)
    */
    AttendanceRecord *records = (AttendanceRecord*) malloc(lines * sizeof(AttendanceRecord));
    if (!records) {
        printf("Memory allocation failed!\n");
        fclose(fp);
        return NULL;
    }

    // Step 3: Load data into array
    int i = 0;
    while(fscanf(fp, "%s %s %d", records[i].studentID, records[i].date, &records[i].status) != EOF) {
        i++;
    }
    
    *count = i; // Update the count variable in main via pointer
    fclose(fp);
    
    
    
    return records;
}
// --- 3. Student Logic ---
void viewStudentStats(AttendanceRecord *records, int count) {
    char targetID[MAX_ID];
    printf("Enter Student ID to search: ");
    scanf("%s", targetID);

    int total = 0, present = 0;
    
    // Pointer traversal
    for (int i = 0; i < count; i++) {
        if (strcmp((records + i)->studentID, targetID) == 0) {
            total++;
            if ((records + i)->status == 1) present++;
        }
    }

    if (total == 0) {
        printf("No records found for %s.\n", targetID);
    } else {
        float percentage = ((float)present / total) * 100;
        printf("Stats for %s: Attended %d/%d (%.2f%%)\n", targetID, present, total, percentage);
    }
}

// --- 4. Defaulter Logic ---
void checkDefaulters(AttendanceRecord *records, int count) {
    // Note: In a real scenario, we would need a list of ALL unique students.
    // For this demo, we ask which student to check against the threshold.
    
    float threshold;
    printf("Enter Attendance Threshold (e.g., 75.0): ");
    scanf("%f", &threshold);
    
    // Simple approach: We will scan the loaded records, find unique IDs 
    // (omitted for brevity, doing a single pass check approach here)
    printf("\n--- Checking Loaded Records ---\n");
    printf("Note: In this basic version, enter ID to verify eligibility.\n");
    
    char targetID[MAX_ID];
    printf("Enter ID to check eligibility: ");
    scanf("%s", targetID);
    
    int total = 0, present = 0;
    for(int i=0; i<count; i++) {
         if (strcmp(records[i].studentID, targetID) == 0) {
            total++;
            if (records[i].status == 1) present++;
        }
    }
    
    if (total > 0) {
        float pct = ((float)present / total) * 100;
        if (pct < threshold) {
            printf("[ALERT] %s is a DEFAULTER. Attendance: %.2f%%\n", targetID, pct);
        } else {
            printf("[OK] %s is safe. Attendance: %.2f%%\n", targetID, pct);
        }
    }
}

// --- 5. Analytics (Lecture-wise) ---
void generateAnalytics(AttendanceRecord *records, int count) {
    printf("\n--- Lecture-wise Trends ---\n");
    
    // In a full system, we would group by unique dates.
    // Here we iterate and print raw data formatted as a report.
    
    int total_presents_overall = 0;
    
    for(int i=0; i<count; i++) {
        if(records[i].status == 1) total_presents_overall++;
    }
    
    printf("Total Lectures Recorded: %d\n", count); // Assumes 1 record = 1 student presence in a class
    printf("Total Presences Recorded: %d\n", total_presents_overall);
    printf("Average Attendance Density: %.2f%%\n", ((float)total_presents_overall/count)*100);
}

void clearInputBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}