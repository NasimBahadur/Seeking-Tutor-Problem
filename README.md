# Seeking-Tutor-Problem
Using POSIX threads, mutex locks, and semaphores, I implemented a solution that synchronizes the activities of the coordinator, tutors, and the students.

# Project Description
The computer science department of EWU runs a programming club to help undergraduate students with their programming assignments. The club has a coordinator and several tutors to assist the students. The waiting area of the center has several chairs. Initially, all the chairs are empty. The coordinator is waiting for the students to arrive. The tutors are either waiting for the coordinator to notify that there are students waiting or they are busy tutoring. The tutoring area is different from the waiting area. A student, while programming for his project, decides to go to a club to get help from a tutor. After arriving at the club, the student sits in an empty chair in the waiting area and waits to be called for tutoring. If no chairs are available, the student will go back to programming and come back to the club later. Once a student arrives, the coordinator queues the student based on the student’s priority, and then the coordinator notifies an idle tutor. A tutor, once woken up, finds the student with the highest priority and begins tutoring. A tutor after helping a student, waits for the next student. A student after receiving help from a tutor goes back to programming. The priority of a student is based on the number of times the student has visited the club. A student visiting the club for the first time gets the highest priority. In general, a student visiting for the ith time has a priority higher than the priority of the student visiting for the kth time for any k > i. If two students have the same priority, then the student who came first has a higher priority.
Using POSIX threads, mutex locks, and semaphores implement a solution that synchronizes the activities of the coordinator, tutors, and the students. The total number of students, the number of tutors, the number of chairs, and the number of times a student seeks a tutor’s help are passed as command line arguments. Once a student thread takes the required number of help from the tutors, it should terminate. Once all the student threads are terminated, the tutor threads, the coordinator thread, and the main program should be terminated. Your program should work for any number of students, tutors, chairs and help sought. Allocate memory for data structures dynamically based on the input parameter(s).

# Overview
EWU runs a programming club where undergraduate students get help. Club has a coordinator and several tutors to assist the students. Students come to waiting area. If there is no available seat they will go back to programming. Otherwise, they will wait in the waiting area. Coordinator prioritize students in a queue and assign them to the tutor. If more than one student has the same priority the tutor will choose whoever came first. After getting the maximum amount of help student terminate. When all students will get help, tutor will terminate and then coordinator will be terminated. Methods are:
1. Pthread_create()
2. Pthread_join()
3. Sem_init()
4. Sem_wait()
5. Sem_post()
6. Sem_mutex()

**Operating System:** Linux

**To run this 'projectCode.c' file, we used these command line arguments in terminal:**

 gcc projectCode.c -o thread -lpthread

 ./thread

# Flowchart
![Whole diagram of the system](https://user-images.githubusercontent.com/43060004/137521011-59df24d9-bb8c-4b21-9095-858b1bf314c4.png)

**Student has 3 parts:**
1. Student has to start programming and seek help from a tutor, student needs to get a seat.  After coming to get seat, if student does not find any seat, then has to go back to programming and try again later.
2. If a student gets a seat then coordinator gets a notice and then wait for tutor to be assigned
3. After getting maximum amount of help, the student will stop seeking help .

**Coordinator has 3 parts:**
1. The coordinator waits for students to come to seek help.
2. Prioritize students according to who has come first.
3. If all students leave then coordinator notify the tutor and leaves .

**Tutor has 3 parts:**
1. Tutor waits for the coordinator to be notified if a student has come to seek help.
2. Tutor selects to student to help according to the priority of the student
3. If all students have done seeking help from the tutor, the tutor waits for coordinators notification to finish work.
