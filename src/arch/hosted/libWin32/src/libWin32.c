
#include "../libWin32.h"

#include<Windows.h>
#include<stdio.h>
#include<conio.h>


void mutex_init(mutex_t * lock) {

	*lock = CreateMutex(NULL, FALSE, NULL);
}

void mutex_destroy(mutex_t * lock) {

	CloseHandle(*lock);
}

void mutex_lock(mutex_t * lock) {

	WaitForSingleObject(*lock, INFINITE);
}

int  mutex_trylock(mutex_t * lock) {

	if (WaitForSingleObject(*lock, 0) == WAIT_OBJECT_0)
		return 0;

	return -1;
}

void mutex_unlock(mutex_t * lock) {

	ReleaseMutex(*lock);
}

int kthread_create(kthread_t * thread, int gfp_flags, void * (*start_routine)(void *), void * args)
{
	HANDLE handle = CreateThread(
		NULL, 
		0, 
		(LPTHREAD_START_ROUTINE)start_routine, 
		args, 
		0, 
		0);

	if (handle) {
		*thread = handle;
		return 0;
	}

	return -1;
}

int kthread_init()
{
	return 0;
}
void kthread_yield()
{
	Sleep(0);
}

void kthread_join(kthread_t thread)
{
	WaitForSingleObject(thread, INFINITE);
}

int  host_os_kbhit() {
	if (_kbhit())
		return 1;
	return 0;
}
int  host_os_getchar() {

	return getchar();
}
void host_os_putchar(int c) {

	putchar(c);
}
