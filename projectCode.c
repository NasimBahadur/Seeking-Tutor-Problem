#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#define MAX 100000

int mark_visited[MAX],student_priority[MAX],arrival_time[MAX],total_visit_of_student[MAX],student_id[MAX],tutor_id[MAX];
int requesting_order=0,student_done=0,occupied_chair=0,total_chair,total_student,total_tutor,help_limit;

sem_t student,coordinator,tutored_student[MAX],mutexLock;

void *student_thread(void *student_id)
{
    int s_id=*(int*)student_id; //converting void to int
    while(1)
    {
        if(total_visit_of_student[s_id-1] == help_limit)
        {
            sem_wait(&mutexLock);
            student_done++; //how many students have tutored totally
            sem_post(&mutexLock);
            printf("\n\t student-%d terminates\n",s_id);
            if(student_done == total_student) printf("\n\t All Students Have Recieved Help\n");
            sem_post(&student); //notify coordinate to terminate
            pthread_exit(NULL); //thread ends
        }

        sem_wait(&mutexLock); //mutex was initially 1, when we call sem_wait(), it turns 0, it is locked right now
        if(occupied_chair == total_chair)
        {
            sem_post(&mutexLock); //mutex unlockes it, by turning the value 1
            continue;
        }
        occupied_chair++;
        requesting_order++; //request number of student, it keeps the track of when the student came
        mark_visited[s_id-1]=requesting_order;
        printf("\nStudent Thread --> Student-%d takes a Seat.\nStudent Thread --> Empty Chairs = %d\n",s_id,total_chair-occupied_chair);
        sem_post(&mutexLock); //unlocked for other students
        sem_post(&student); //notify coordinator that student seated.
        sem_wait(&tutored_student[s_id-1]); //wait to be tutored.

        printf("\nStudent Thread --> Student-%d Received Help.\n",s_id);
        sem_wait(&mutexLock);
        total_visit_of_student[s_id-1]++; //tutored times++ after tutoring.
        printf("\nStudent Thread --> Student-%d's Priority now is %d\n",s_id, total_visit_of_student[s_id-1]);
        sem_post(&mutexLock);
    }
}

void *coordinator_thread()
{
    while(1)
    {
        if(student_done==total_student) //if all students finish, tutors threads and coordinate thread terminate.
        {
            for(int i=0; i<total_tutor; i++) sem_post(&coordinator); //notify tutors to terminate
            printf("\n\t coordinator terminates\n"); //terminate coordinate itself
            pthread_exit(NULL); //thread ends
        }
        sem_wait(&student); // waiting for students signal
        sem_wait(&mutexLock);
        for(int i=0; i<total_student; i++) //find the students who just seated and push them into the priority queue
        {
            if(mark_visited[i]>-1)
            {
                student_priority[i] = total_visit_of_student[i];
                arrival_time[i] = mark_visited[i]; //to save the time when the student came
                printf("\nCoordinator Thread --> Student-%d with Priority-%d in the queue.\n",student_id[i],total_visit_of_student[i]);
                mark_visited[i]=-1;
                sem_post(&coordinator); //notify tutor that student is in the queue.
            }
        }
        sem_post(&mutexLock);
    }
}

void *tutor_thread(void *tutor_id)
{
    int t_id=*(int*)tutor_id;
    while(1)
    {
        if(student_done==total_student) //if all students finish, tutors threads terminate.
        {
            sem_wait(&mutexLock);
            printf("\n\t tutor-%d terminates\n",t_id);
            sem_post(&mutexLock);
            pthread_exit(NULL); //thread ends
        }
        int max_request=total_student*help_limit+1; //this is the maximum serial a student can get
        int max_priority = help_limit-1;
        int s_id=-1; //it is a flag.
        sem_wait(&coordinator); //wait coordinator's notification
        sem_wait(&mutexLock);
        for(int i=0; i<total_student; i++) //find student with highest priority from priority queue. If students with same priority, who come first has higher priority
        {
            if(student_priority[i]>-1 && student_priority[i]<= max_priority)
            {
                if(arrival_time[i]<max_request)
                {
                    max_priority=student_priority[i];
                    max_request=arrival_time[i]; //who comes first, here we are trying to find the minimum arrival time if the priotiy is same
                    s_id=student_id[i];
                }
            }
        }
        if(s_id==-1) //in case no student in the queue.
        {
            sem_post(&mutexLock);
            continue;
        }
        student_priority[s_id-1] = -1; //reset the priority queue
        arrival_time[s_id-1] = -1;
        occupied_chair--;
        sem_post(&mutexLock);
        sem_wait(&mutexLock); //after tutoring
        printf("\nTutor Thread --> Student-%d is tutored by Tutor-%d\n",s_id,t_id);
        sem_post(&mutexLock);
        sem_post(&tutored_student[s_id-1]); //update shared data so student can know who tutored him.
    }
}

int main()
{
    printf("Total number of students: ");
    scanf("%d", &total_student);
    printf("Total number of Tutors: ");
    scanf("%d", &total_tutor);
    printf("Total number of Chairs: ");
    scanf("%d", &total_chair);
    printf("Maximum number of help a student can get: ");
    scanf("%d", &help_limit);

    for(int i=0; i<total_student; i++)
    {
        mark_visited[i]=-1;
        student_priority[i] = -1;
        arrival_time[i] = -1;
        total_visit_of_student[i]=0;
    }

    sem_init(&student,0,0);
    sem_init(&coordinator,0,0);
    sem_init(&mutexLock,0,1);
    for(int i=0; i<total_student; i++) sem_init(&tutored_student[i],0,0);

    pthread_t students[total_student],tutors[total_tutor],coordinator; //allocate threads

    for(int i = 0; i < total_student; i++) //create threads for student
    {
        student_id[i] = i + 1; //saved in array
        pthread_create(&students[i], NULL, student_thread, (void*) &student_id[i]);
    }

    for(int i = 0; i < total_tutor; i++) //create threads for tutors
    {
        tutor_id[i] = i + 1;
        pthread_create(&tutors[i], NULL, tutor_thread, (void*) &tutor_id[i]);
    }

    pthread_create(&coordinator,NULL,coordinator_thread,NULL); //create thread for coordinator

    //join threads, to connect the threads to main program
    for(int i =0; i < total_student; i++) pthread_join(students[i],NULL);
    for(int i =0; i < total_tutor; i++) pthread_join(tutors[i],NULL);
    pthread_join(coordinator, NULL);
}
