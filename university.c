#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

#define MAX_STUDENTS 100
#define MAX_COURSES 10
#define MAX_SECTIONS 5
#define MAX_WAITLIST 10
#define MAX_NAME_LENGTH 50

struct course {
    int id;
    char name[100];
    int slots;
    int prerequisites[5];
    int waitlist_count;
    char waitlist[MAX_WAITLIST][30];
};

struct student {
    char name[MAX_NAME_LENGTH];
    char userid[30];
    char password[30];
    int semester;
    int courses[18];
    int course_count;
    float cgpa;
    int projects;
    int clubs;   
    time_t course_timestamps[18];  
};              


struct course sem2[6];
struct course sem3[6];
struct course sem4[7];

void read_courses_from_file();
void displaysemester(int semester);
void update_course_file();
void update_waitlist_file();
void read_waitlist_file();
int isStrongPassword(char *password);
void register_student();
int login_student(struct student *logged_in_student);
int check_availability(struct course *c);
int check_prerequisites(struct course *c, struct student *s);
void allocate_course(struct course *c, struct student *s, int course_num);
void rank_students_by_cgpa_and_projects_and_clubs(struct student *students[], int count);
void add_to_waitlist(struct course *c, char *userid);
void select_courses(struct student *s);
void update_student_courses(struct student *s);
void display_selected_courses(struct student *s);
void rank_students_by_cgpa_and_projects(int course_id, struct student *new_student);
void show_student_ranking(struct student *s, int course_id);
void drop_course(struct student *s, int course_id);
void remove_from_ranking(struct student *s, int course_id);

void read_courses_from_file() {
    const char *filenames[] = {
        "Semester2.txt", "Semester3.txt", "Semester4.txt",
    };
    struct course *semesters[] = {sem2, sem3, sem4};
    int semester_sizes[] = {6, 6, 7};
    int semester_count = sizeof(semesters) / sizeof(semesters[0]);

    int i, j, k;
    for (i = 0; i < semester_count; i++) {
        FILE *fp = fopen(filenames[i], "r");
        if (fp == NULL) {
            printf("Error: Unable to open file %s.\n", filenames[i]);
            continue;
        }
        for (j = 0; j < semester_sizes[i]; j++) {
            struct course *c = &semesters[i][j];
            fscanf(fp, "%d,%99[^,],%d", &c->id, c->name, &c->slots);
            for (k = 0; k < 5; k++) {
                fscanf(fp, ",%d", &c->prerequisites[k]);
                if (c->prerequisites[k] == 0) break;
            }
            c->waitlist_count = 0;
        }
        fclose(fp);
    }
}

void update_course_file() {
    const char *filenames[] = {
        "Semester2.txt", "Semester3.txt", "Semester4.txt",
    };
    struct course *semesters[] = {sem2, sem3, sem4};
    int semester_sizes[] = {6, 6, 7};
    int semester_count = sizeof(semesters) / sizeof(semesters[0]);

    int i, j, k;
    for (i = 0; i < semester_count; i++) {
        FILE *fp = fopen(filenames[i], "w");
        if (fp == NULL) {
            printf("Error: Unable to open file %s.\n", filenames[i]);
            continue;
        }
        for (j = 0; j < semester_sizes[i]; j++) {
            struct course *c = &semesters[i][j];
            fprintf(fp, "%d,%s,%d", c->id, c->name, c->slots);
            for (k = 0; c->prerequisites[k] != 0; k++) {
                fprintf(fp, ",%d", c->prerequisites[k]);
            }
            fprintf(fp, "\n");
        }
        fclose(fp);
    }
}

void read_waitlist_file() {
    const char *filenames[] = {
        "Waitlist_Semester2.txt", "Waitlist_Semester3.txt", "Waitlist_Semester4.txt",
    };
    struct course *semesters[] = {sem2, sem3, sem4};
    int semester_sizes[] = {6, 6, 7};
    int semester_count = sizeof(semesters) / sizeof(semesters[0]);

    int i, j;
    for (i = 0; i < semester_count; i++) {
        FILE *fp = fopen(filenames[i], "r");
        if (fp == NULL) {
            printf("Error: Unable to open file %s.\n", filenames[i]);
            continue;
        }
        for (j = 0; j < semester_sizes[i]; j++) {
            struct course *c = &semesters[i][j];
            fscanf(fp, "%d", &c->waitlist_count);
            for (int k = 0; k < c->waitlist_count; k++) {
                fscanf(fp, ",%29s", c->waitlist[k]);
            }
        }
        fclose(fp);
    }
}

void update_waitlist_file() {
    const char *filenames[] = {
        "Waitlist_Semester2.txt", "Waitlist_Semester3.txt", "Waitlist_Semester4.txt",
    };
    struct course *semesters[] = {sem2, sem3, sem4};
    int semester_sizes[] = {6, 6, 7};
    int semester_count = sizeof(semesters) / sizeof(semesters[0]);

    int i, j;
    for (i = 0; i < semester_count; i++) {
        FILE *fp = fopen(filenames[i], "w");
        if (fp == NULL) {
            printf("Error: Unable to open file %s.\n", filenames[i]);
            continue;
        }
        for (j = 0; j < semester_sizes[i]; j++) {
            struct course *c = &semesters[i][j];
            fprintf(fp, "%d", c->waitlist_count);
            for (int k = 0; k < c->waitlist_count; k++) {
                fprintf(fp, ",%s", c->waitlist[k]);
            }
            fprintf(fp, "\n");
        }
        fclose(fp);
    }
}

void register_student() {
    struct student s;
    FILE *fp;

    printf("Enter name: ");
    scanf("%s", s.name);

    printf("Enter userid: ");
    scanf("%s", s.userid);

    char password[30];
    do {
        printf("Enter password (at least 8 characters containing uppercase, lowercase, digits, and special characters): ");
        scanf("%s", password);
        if (!isStrongPassword(password)) {
            printf("Password is not strong enough. Please try again.\n");
        }
    } while (!isStrongPassword(password));
    strcpy(s.password, password);

    printf("Enter CGPA: ");
    scanf("%f", &s.cgpa);

    printf("Enter number of projects: ");
    scanf("%d", &s.projects);

    printf("Enter the number of clubs you are part of: ");
    scanf("%d",&s.clubs);

    s.semester = 0;
    s.course_count = 0;
    int i;
    for (i = 0; i < 6; i++) {
        s.courses[i] = 0; 
    }

    fp = fopen("StudentData.txt", "a");
    if (fp == NULL) {
        printf("Error: Unable to open file.\n");
        return;
    }

    fprintf(fp, "%s,%s,%s,%d,%d,%f,%d,%d", s.name, s.userid, s.password, s.semester, s.course_count, s.cgpa, s.projects,s.clubs);
    for (i = 0; i < 6; i++) {
        fprintf(fp, ",%d", s.courses[i]);
    }
    fprintf(fp, "\n");

    fclose(fp);

    printf("Student registered successfully!\n");
}

int login_student(struct student *logged_in_student) {
    char name[MAX_NAME_LENGTH], userid[30], password[30];
    FILE *fp;
    
    printf("Enter name: ");
    scanf("%s", name);
    printf("Enter userid: ");
    scanf("%s", userid);
    printf("Enter password: ");
    scanf("%s", password);

    fp = fopen("StudentData.txt", "r");
    if(fp == NULL) {
        printf("Error: Unable to open file.\n");
        return 1;
    }

    struct student s; // Temporary struct to read each student record
    int found = 0;
    while(fscanf(fp, "%49[^,],%29[^,],%29[^,],%d,%d,%f,%d,%d,%d,%d,%d,%d,%d,%d\n", s.name, s.userid, s.password, &s.semester, &s.course_count, &s.cgpa, &s.projects, &s.clubs, &s.courses[0], &s.courses[1], &s.courses[2], &s.courses[3], &s.courses[4], &s.courses[5]) == 14) {
        if(strcmp(name, s.name) == 0 && strcmp(userid, s.userid) == 0 && strcmp(password, s.password) == 0) {
            *logged_in_student = s; // Assign found student to logged_in_student pointer
            found = 1;
            break;
        }
    }

    fclose(fp);
    
    if (found) {
        printf("Login successful!\n");
        return 0;
    } else {
        printf("Login failed. Invalid credentials.\n");
        return 1;
    }
}

void displaysemester(int semester) {
    struct course *current_semester;
    int semester_size;

    switch (semester) {
        case 2: current_semester = sem2; semester_size = 6; break;
        case 3: current_semester = sem3; semester_size = 6; break;
        case 4: current_semester = sem4; semester_size = 7; break;
        default: printf("Invalid semester. Returning to main menu.\n"); return;
    }

    printf("Courses available for semester %d:\n", semester);
    for (int i = 0; i < semester_size; i++) {
        struct course *c = &current_semester[i];
        printf("Course ID: %d, Name: %s, Slots: %d\n", c->id, c->name, c->slots);
    }
}

int isStrongPassword(char *password) {
    int length = strlen(password);
    int has_upper = 0, has_lower = 0, has_digit = 0, has_special = 0;
    int i;
    for (i = 0; i < length; ++i) {
        if (isupper(password[i])) has_upper = 1;
        else if (islower(password[i])) has_lower = 1;
        else if (isdigit(password[i])) has_digit = 1;
        else has_special = 1;
    }
    return (has_upper && has_lower && has_digit && has_special && length >= 8);
}

int check_availability(struct course *c) {
    return c->slots > 0;
}

int check_prerequisites(struct course *c, struct student *s) {
    int i, j;
    for (i = 0; c->prerequisites[i] != 0; i++) {
        int prereq = c->prerequisites[i];
        int met = 0;
        for (j = 0; j < s->course_count; j++) {
            if (s->courses[j] == prereq) {
                met = 1;
                break;
            }
        }
        if (!met) return 0; 
    }
    return 1; 
}

void rank_students_by_cgpa_and_projects_and_clubs(struct student *students[], int count) {
    float weight_cgpa = 0.5;
    float weight_projects = 0.3;
    float weight_clubs = 0.2;

    // Sort students by their weighted scores
    for (int i = 0; i < count - 1; i++) {
        for (int j = 0; j < count - i - 1; j++) {
            float score_j = students[j]->cgpa * weight_cgpa + students[j]->projects * weight_projects + students[j]->clubs * weight_clubs;
            float score_j1 = students[j + 1]->cgpa * weight_cgpa + students[j + 1]->projects * weight_projects + students[j + 1]->clubs * weight_clubs;

            if (score_j < score_j1) {
                struct student *temp = students[j];
                students[j] = students[j + 1];
                students[j + 1] = temp;
            }
        }
    }
}

void add_to_waitlist(struct course *c, char *userid) {
    if (c->waitlist_count < MAX_WAITLIST) {
        strcpy(c->waitlist[c->waitlist_count++], userid);

        // Update the waitlist file for the course
        char waitlist_filename[50];
        sprintf(waitlist_filename, "Waitlist_%d.txt", c->id);
        FILE *fp = fopen(waitlist_filename, "a");
        if (fp != NULL) {
            fprintf(fp, "%s\n", userid);
            fclose(fp);
        } else {
            printf("Error: Unable to open waitlist file.\n");
        }
    } else {
        printf("Waitlist for course %s is full.\n", c->name);
    }
}

void allocate_course(struct course *c, struct student *s, int course_num) {
    if (check_availability(c)) {
        s->courses[s->course_count] = course_num;
        s->course_timestamps[s->course_count] = time(NULL);  // Add this line
        s->course_count++;
        c->slots--;
        printf("Course %s allocated successfully.\n", c->name);
        rank_students_by_cgpa_and_projects(course_num, s);
    } else {
        add_to_waitlist(c, s->userid);
        printf("Course %s is full. You have been waitlisted.\n", c->name);
    }
}


void select_courses(struct student *s) {
    int course_count;
    int course_num;
    struct course *current_semester;
    int semester_size;

    printf("Enter your semester (2, 3, or 4): ");
    scanf("%d", &s->semester);

    switch (s->semester) {
        case 2: current_semester = sem2; semester_size = 6; break;
        case 3: current_semester = sem3; semester_size = 6; break;
        case 4: current_semester = sem4; semester_size = 7; break;
        default: printf("Invalid semester. Returning to main menu.\n"); return;
    }

    displaysemester(s->semester);

    printf("Enter number of courses to select: ");
    scanf("%d", &course_count);

    if (course_count > 6 || course_count < 4) {
        printf("Invalid number of courses. Please select between 4 and 6 courses.\n");
        return;
    }

    for (int i = 0; i < course_count; i++) {
        printf("Enter course ID to select: ");
        scanf("%d", &course_num);

        int course_found = 0;
        struct course *selected_course = NULL;
        for (int j = 0; j < semester_size; j++) {
            if (current_semester[j].id == course_num) {
                selected_course = &current_semester[j];
                course_found = 1;
                break;
            }
        }

        if (!course_found) {
            printf("Invalid course ID. Please try again.\n");
            i--;
            continue;
        }

        if (!check_prerequisites(selected_course, s)) {
            printf("You do not meet the prerequisites for this course.\n");
            i--;
            continue;
        }

        allocate_course(selected_course, s, course_num);
    }

    update_student_courses(s);
}

void display_selected_courses(struct student *s) {
    printf("Selected courses:\n");
    for (int i = 0; i < s->course_count; i++) {
        printf("Course ID: %d\n", s->courses[i]);
    }
}

void update_student_courses(struct student *s) {
    FILE *fp, *temp_fp;
    char temp_filename[] = "temp_StudentData.txt";
    int found = 0;

    fp = fopen("StudentData.txt", "r");
    if (fp == NULL) {
        printf("Error: Unable to open file.\n");
        return;
    }

    temp_fp = fopen(temp_filename, "w");
    if (temp_fp == NULL) {
        printf("Error: Unable to create temporary file.\n");
        fclose(fp);
        return;
    }

    struct student current_student;

    while (fscanf(fp, "%49[^,],%29[^,],%29[^,],%d,%d,%f,%d,%d,%d,%d,%d,%d,%d,%d\n", current_student.name, current_student.userid, current_student.password,
                  &current_student.semester, &current_student.course_count, &current_student.cgpa, &current_student.projects, &current_student.clubs,
                  &current_student.courses[0], &current_student.courses[1], &current_student.courses[2], &current_student.courses[3],
                  &current_student.courses[4], &current_student.courses[5]) == 14) {
        if (strcmp(s->userid, current_student.userid) == 0) {
            found = 1;
            fprintf(temp_fp, "%s,%s,%s,%d,%d,%f,%d,%d", s->name, s->userid, s->password, s->semester, s->course_count, s->cgpa, s->projects, s->clubs);
            for (int i = 0; i < 6; i++) {
                fprintf(temp_fp, ",%d", s->courses[i]);
            }
            fprintf(temp_fp, "\n");
        } else {
            fprintf(temp_fp, "%s,%s,%s,%d,%d,%f,%d,%d", current_student.name, current_student.userid, current_student.password,
                    current_student.semester, current_student.course_count, current_student.cgpa, current_student.projects, current_student.clubs);
            for (int i = 0; i < 6; i++) {
                fprintf(temp_fp, ",%d", current_student.courses[i]);
            }
            fprintf(temp_fp, "\n");
        }
    }

    fclose(fp);
    fclose(temp_fp);

    remove("StudentData.txt");
    rename(temp_filename, "StudentData.txt");

    if (!found) {
        printf("Error: Student record not found.\n");
    }
}

int get_student_by_userid(char *userid, struct student *s) {
    FILE *fp = fopen("StudentData.txt", "r");
    if (fp == NULL) {
        printf("Error: Unable to open student data file.\n");
        return 0;
    }

    int i;
    while (fscanf(fp, "%[^,],%[^,],%[^,],%d,%d,%f,%d,%d", s->name, s->userid, s->password, &s->semester, &s->course_count, &s->cgpa, &s->projects, &s->clubs) == 8) {
        for (i = 0; i < MAX_COURSES; i++) {
            fscanf(fp, ",%d", &s->courses[i]);
        }
        fscanf(fp, "\n");

        if (strcmp(userid, s->userid) == 0) {
            fclose(fp);
            return 1;
        }
    }

    fclose(fp);
    return 0;
}

void drop_course(struct student *s, int course_id) {
    int i, j;
    int found = 0;

    // Check if the course exists in the student's course list
    for (i = 0; i < s->course_count; i++) {
        if (s->courses[i] == course_id) {
            found = 1;
            break;
        }
    }

    if (!found) {
        printf("Course not found in your list.\n");
        return;
    }

    // Check if the course was selected within 14 days
    time_t current_time = time(NULL);
    double difference_in_days = difftime(current_time, s->course_timestamps[i]) / (60 * 60 * 24);
    if (difference_in_days > 14) {
        printf("You cannot drop the course as 14 days have passed since selection.\n");
        return;
    }

    // Remove the dropped course from the student's course list
    for (j = i; j < s->course_count - 1; j++) {
        s->courses[j] = s->courses[j + 1];
        s->course_timestamps[j] = s->course_timestamps[j + 1];  // Shift timestamps
    }
    s->course_count--;

    // Clear the last course and timestamp entry
    s->courses[s->course_count] = 0;
    s->course_timestamps[s->course_count] = 0;

    // Find the dropped course in the current semester's course list
    struct course *current_semester;
    int semester_size;

    switch (s->semester) {
        case 2: current_semester = sem2; semester_size = 6; break;
        case 3: current_semester = sem3; semester_size = 6; break;
        case 4: current_semester = sem4; semester_size = 7; break;
        default: printf("Invalid semester. Returning to main menu.\n"); return;
    }

    struct course *dropped_course = NULL;
    for (i = 0; i < semester_size; i++) {
        if (current_semester[i].id == course_id) {
            dropped_course = &current_semester[i];
            break;
        }
    }

    if (dropped_course != NULL) {
        // Increase available slots for the dropped course
        dropped_course->slots++;

        // Check if there are students in the waitlist for the dropped course
        if (dropped_course->waitlist_count > 0) {
            // Allocate the course to the next student in waitlist
            char userid[30];
            strncpy(userid, dropped_course->waitlist[0], sizeof(userid) - 1);
            userid[sizeof(userid) - 1] = '\0';  // Ensure null termination

            // Remove the first student from the waitlist
            for (int k = 1; k < dropped_course->waitlist_count; k++) {
                strncpy(dropped_course->waitlist[k - 1], dropped_course->waitlist[k], sizeof(dropped_course->waitlist[k - 1]) - 1);
                dropped_course->waitlist[k - 1][sizeof(dropped_course->waitlist[k - 1]) - 1] = '\0';  // Ensure null termination
            }
            dropped_course->waitlist_count--;

            struct student next_student;
            if (get_student_by_userid(userid, &next_student)) {
                next_student.courses[next_student.course_count] = course_id;
                next_student.course_timestamps[next_student.course_count] = time(NULL);  // Add this line
                next_student.course_count++;
                dropped_course->slots--;
                printf("Course %s allocated to waitlisted student %s.\n", dropped_course->name, next_student.name);
                rank_students_by_cgpa_and_projects(course_id, &next_student);
                update_student_courses(&next_student);

                // Update the waitlist file for the course
                update_waitlist_file();
            }
        }

        // Remove student from the ranking
        remove_from_ranking(s, course_id);

        // Update course and student files
        update_course_file();
        update_student_courses(s);

        printf("Course %d dropped successfully.\n", course_id);
    } else {
        printf("Error: Course not found in the current semester.\n");
    }
}



void rank_students_by_cgpa_and_projects(int course_id, struct student *new_student) {
    struct student students[MAX_STUDENTS];
    int count = 0;
    FILE *fp = fopen("StudentData.txt", "r");
    if (fp == NULL) {
        printf("Error: Unable to open student data file.\n");
        return;
    }

    struct student s;
    int i;
    while (fscanf(fp, "%[^,],%[^,],%[^,],%d,%d,%f,%d,%d", s.name, s.userid, s.password, &s.semester, &s.course_count, &s.cgpa, &s.projects, &s.clubs) == 8) {
        for (i = 0; i < 6; i++) {
            fscanf(fp, ",%d", &s.courses[i]);
        }
        fscanf(fp, "\n");

        for (i = 0; i < s.course_count; i++) {
            if (s.courses[i] == course_id) {
                students[count++] = s;
                break;
            }
        }
    }
    fclose(fp);

    students[count++] = *new_student;

    // Define weights
    float weight_cgpa = 0.5;
    float weight_projects = 0.3;
    float weight_clubs = 0.2;

    // Rank students based on weighted score
    for (i = 0; i < count - 1; i++) {
        for (int j = 0; j < count - i - 1; j++) {
            float score_j = students[j].cgpa * weight_cgpa + students[j].projects * weight_projects + students[j].clubs * weight_clubs;
            float score_j1 = students[j + 1].cgpa * weight_cgpa + students[j + 1].projects * weight_projects + students[j + 1].clubs * weight_clubs;

            if (score_j < score_j1) {
                struct student temp = students[j];
                students[j] = students[j + 1];
                students[j + 1] = temp;
            }
        }
    }

    char ranking_filename[50];
    sprintf(ranking_filename, "Ranking_%d.txt", course_id);
    fp = fopen(ranking_filename, "w");
    if (fp == NULL) {
        printf("Error: Unable to open ranking file.\n");
        return;
    }

    for (i = 0; i < count; i++) {
        fprintf(fp, "%s,%s,%f,%d,%d\n", students[i].name, students[i].userid, students[i].cgpa, students[i].projects, students[i].clubs);
    }

    fclose(fp);
}

void show_student_ranking(struct student *s, int course_id) {
    char ranking_filename[50];
    sprintf(ranking_filename, "Ranking_%d.txt", course_id);
    FILE *fp = fopen(ranking_filename, "r");
    if (fp == NULL) {
        printf("Error: Unable to open ranking file.\n");
        return;
    }

    printf("Ranking for course %d:\n", course_id);
    char name[50], userid[30];
    float cgpa;
    int projects, clubs;  // Adding clubs
    int rank = 1; // Start with rank 1

    while (fscanf(fp, "%[^,],%[^,],%f,%d,%d", name, userid, &cgpa, &projects, &clubs) == 5) {
        if (strcmp(userid, s->userid) == 0) {
            printf("Your Rank: %d, CGPA: %.2f, Projects: %d, Clubs: %d\n", rank, cgpa, projects, clubs);
            fclose(fp);
            return; // Found the student, no need to continue
        }
        rank++; // Increment rank for next student
    }
    printf("You are not ranked in this course.\n");
}


void remove_from_ranking(struct student *s, int course_id) {
    char ranking_filename[50];
    sprintf(ranking_filename, "Ranking_%d.txt", course_id);
    FILE *fp = fopen(ranking_filename, "r");
    if (fp == NULL) {
        printf("Error: Unable to open ranking file.\n");
        return;
    }

    struct student students[MAX_STUDENTS];
    int count = 0;
    char name[50], userid[30];
    float cgpa;
    int projects, clubs;

    while (fscanf(fp, "%[^,],%[^,],%f,%d,%d", name, userid, &cgpa, &projects, &clubs) == 5) {
        if (strcmp(userid, s->userid) != 0) {
            strcpy(students[count].name, name);
            strcpy(students[count].userid, userid);
            students[count].cgpa = cgpa;
            students[count].projects = projects;
            students[count].clubs = clubs;
            count++;
        }
    }
    fclose(fp);

    fp = fopen(ranking_filename, "w");
    if (fp == NULL) {
        printf("Error: Unable to open ranking file.\n");
        return;
    }

    for (int i = 0; i < count; i++) {
        fprintf(fp, "%s,%s,%f,%d,%d\n", students[i].name, students[i].userid, students[i].cgpa, students[i].projects, students[i].clubs);
    }

    fclose(fp);
}



void handle_vacancies() {
    struct course *semesters[] = {sem2, sem3, sem4};
    int semester_sizes[] = {6, 6, 7};
    int semester_count = sizeof(semesters) / sizeof(semesters[0]);

    for (int i = 0; i < semester_count; i++) {
        for (int j = 0; j < semester_sizes[i]; j++) {
            struct course *c = &semesters[i][j];

            while (c->slots > 0 && c->waitlist_count > 0) {
                // Remove the first student from the waitlist and allocate the course to them
                char userid[30];
                strcpy(userid, c->waitlist[0]);

                // Shift the waitlist
                for (int k = 1; k < c->waitlist_count; k++) {
                    strcpy(c->waitlist[k - 1], c->waitlist[k]);
                }
                c->waitlist_count--;

                // Allocate the course to the student
                struct student s;
                if (get_student_by_userid(userid, &s)) {
                    s.courses[s.course_count++] = c->id;
                    c->slots--;
                    printf("Course %s allocated to waitlisted student %s.\n", c->name, s.name);
                    rank_students_by_cgpa_and_projects(c->id, &s);
                    update_student_courses(&s);
                }
            }
        }
    }
} 

int main() {
    read_courses_from_file();
    read_waitlist_file();

    int choice;
    struct student logged_in_student;

    do {
        printf("\nWelcome to the Student Management System\n");
        printf("1. Register\n");
        printf("2. Login\n");
        printf("3. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                register_student();
                break;
            case 2:
                if (login_student(&logged_in_student) == 0) {
                    int student_choice;
                    do {
                        printf("\nWelcome, %s!\n", logged_in_student.name);
                        printf("1. Select Courses\n");
                        printf("2. Drop Course\n");
                        printf("3. Show Selected Courses\n");
                        printf("4. Show Ranking\n");
                        printf("5. Logout\n");
                        printf("Enter your choice: ");
                        scanf("%d", &student_choice);

                        switch (student_choice) {
                            case 1:
                            {
                                select_courses(&logged_in_student);
                                handle_vacancies(); // Check for vacancies after selecting courses
                            }
                                break;
                            case 2:
                            {
                                int course_id_to_drop;
                                printf("Enter course ID to drop: ");
                                scanf("%d", &course_id_to_drop);
                                drop_course(&logged_in_student, course_id_to_drop);
                                handle_vacancies(); // Check for vacancies after dropping a course
                            }
                                break;
                            case 3:
                            {
                                display_selected_courses(&logged_in_student);
                            }
                                break;
                            case 4:
                            {
                                int course_id_to_rank;
                                printf("Enter course ID to show ranking: ");
                                scanf("%d", &course_id_to_rank);
                                show_student_ranking(&logged_in_student, course_id_to_rank);
                            }
                                break;
                            case 5:
                                printf("Logging out...\n");
                                break;
                            default:
                                printf("Invalid choice. Please try again.\n");
                        }
                    } while (student_choice != 5);

                    update_course_file();
                    update_waitlist_file();
                    handle_vacancies();
                } else {
                    printf("Invalid credentials. Please try again.\n");
                }
                break;
            case 3:
                printf("Exiting...\n");
                break;
            default:
                printf("Invalid choice. Please try again.\n");
        }
    } while (choice != 3);

    return 0;
}