#ifndef ENROLLMENT_RECORD
#define ENROLLMENT_RECORD

struct Enrollment
{
    int id; // 0, 1, 2 ....
    char courseid[6];
    char studentid[6];
    char status[12];
    time_t enroll_time;    
};

#endif