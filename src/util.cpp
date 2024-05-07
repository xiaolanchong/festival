// Festival Sokoban Solver
// Copyright 2018-2020 Yaron Shoham

#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <sys/types.h>
#include <sys/timeb.h>
#include <fstream>
#include <string>
#include <sstream>
#ifndef LINUX
#include <windows.h>
#endif

#include "util.h"
#include "board.h"

#ifndef LINUX
#include <conio.h>
#else
#include <termios.h>
#include <unistd.h>
#endif


void exit_with_error(const char *message)
{
	printf("%s", message);
	my_getch();
	exit(0);
}

int is_same_seq(UINT_8 *a, UINT_8 *b, int len)
{
	int i;

	for (i = 0; i < len; i++)
	if (a[i] != b[i])
		return 0;

	return 1;
}


void print_2d_array(int a[MAX_SIZE][MAX_SIZE])
{
	int i, j;

	for (i = 0; i < height; i++)
	{
		for (j = 0; j < width; j++)
		{
			if (a[i][j] == 1000000)
				printf(".");
			else
				printf("%d", a[i][j] % 10);
		}
		printf("\n");
	}
}

int in_group(int val, int *group, int group_size)
{
	int i;

	for (i = 0; i < group_size; i++)
		if (group[i] == val)
			return 1;
	return 0;
}


void remove_newline(char *s)
{
	while (*s)
	{
		if (((*s) == '\r') || ((*s) == '\n'))
		{
			*s = 0;
			return;
		}
		s++;
	}
}

void remove_preceeding_spaces(char *s)
{
	int i, d, n;
	n = (int)strlen(s);

	for (i = 0; i < n; i++)
		if (s[i] != ' ')
			break;
	d = i;

	for (i = 0; i < (n - d); i++)
		s[i] = s[i + d];
	s[i] = 0;
}





int get_number_at_end_of_string(char *s)
{
	int n = (int)strlen(s);
	int i;

	for (i = n - 1; i >= 0; i--)
	{
		if ((s[i] < '0') || (s[i] > '9'))
			break;
	}

	if (i == (n - 1)) return -1;
	
	sscanf(s + i + 1, "%d", &n);
	return n;
}

void show_ints_on_initial_board(int dist[MAX_SIZE][MAX_SIZE])
{
	int i, j;

	for (i = 0; i < height; i++)
	{
		for (j = 0; j < width; j++)
		{
			if (initial_board[i][j] == WALL)
			{
				printf("#");
				continue;
			}

			if (dist[i][j] == 1000000)
			{
				printf(".");
				continue;
			}

			printf("%d", dist[i][j] % 10);
		}
		printf("\n");
	}
}

#ifdef LINUX
char linux_getch(void)
{
  // code from stackoverflow by mf_
    char buf = 0;
    struct termios old = {0};
    fflush(stdout);
    if(tcgetattr(0, &old) < 0)
        perror("tcsetattr()");
    old.c_lflag &= ~ICANON;
    old.c_lflag &= ~ECHO;
    old.c_cc[VMIN] = 1;
    old.c_cc[VTIME] = 0;
    if(tcsetattr(0, TCSANOW, &old) < 0)
        perror("tcsetattr ICANON");
    if(read(0, &buf, 1) < 0)
        perror("read()");
    old.c_lflag |= ICANON;
    old.c_lflag |= ECHO;
    if(tcsetattr(0, TCSADRAIN, &old) < 0)
        perror("tcsetattr ~ICANON");
    printf("%c\n", buf);
    return buf;
}
#endif

void my_getch()
{
#ifndef LINUX
  _getch();
#else
  linux_getch();
#endif
}





void get_date_as_string(char *time_out, char *date)
{
	time_t rawtime;
	struct tm * timeinfo;

	time(&rawtime);
	timeinfo = localtime(&rawtime);

	strftime(time_out, 80, "%X", timeinfo);
	strftime(date    , 80, "%Y-%m-%d", timeinfo);
}

void get_sol_time_as_hms(int sol_time, char *hms)
{
	sprintf(hms, "%02d:%02d:%02d", sol_time / 3600, (sol_time / 60) % 60, sol_time % 60);
}

int is_cyclic_level()
{
	int i, j;
	for (i = 0; i < height; i++)
		for (j = 0; j < width; j++)
		{
			if (initial_board[i][j] == BOX) return 0;
			if (initial_board[i][j] == TARGET) return 0;
		}
	return 1;
}

int time_limit_exceeded(int time_limit, int local_start_time)
{
	int running_time = (int)time(0) - local_start_time;
	if (running_time > time_limit)
	{
		if (verbose >= 4) printf("time limit exceeded\n");
		return 1;
	}
	return 0;
}

#if defined(WIN32) || defined(WINDOWS)

unsigned int get_core_number()
{
	SYSTEM_INFO sysinfo;
	GetSystemInfo(&sysinfo);
	return sysinfo.dwNumberOfProcessors;
}

unsigned long long get_memory()
{
	MEMORYSTATUSEX status;
	status.dwLength = sizeof(status);
	GlobalMemoryStatusEx(&status);
	return status.ullTotalPhys;
}

#else

unsigned int get_core_number()
{
	// looking for "cpu cores  : N"
	std::ifstream f("/proc/cpuinfo");
	assert(f.is_open());
	std::string line;
	unsigned int total_cores = 0;
	while (std::getline(f, line))
	{
		const char search_for[] = "cpu cores";
		auto pos = line.find(search_for);
		if (pos != std::string::npos)
		{
			std::istringstream is(line.substr(pos + sizeof(search_for) - 1));
			std::string colon;
			is >> colon;
			assert(colon == ":");
			unsigned int proc_cores = 0;
			is >> proc_cores;
			total_cores += proc_cores;
		}
	}
	return total_cores != 0 ? total_cores : 1;
}

unsigned long long get_memory()
{
	//looking for "MemTotal:   N kB"
	std::ifstream f("/proc/meminfo");
	assert(f.is_open());
	std::string line;
	while (std::getline(f, line))
	{
		const char search_for[] = "MemTotal";
		auto pos = line.find(search_for);
		if (pos != std::string::npos)
		{
			std::istringstream is(line.substr(pos + sizeof(search_for) - 1));
			std::string colon;
			is >> colon;
			assert(colon == ":");
			unsigned long long memKb = 0;
			is >> memKb;
			std::string units;
			is >> units;
			assert(units == "kB");
			return memKb * 1024;
		}
	}
	return 0;
}

#endif


int get_number_of_cores()
{
	unsigned int processor_count;
	unsigned long long memory;

	cores_num = 1;

#ifdef VISUAL_STUDIO
	return 1;
#endif

	processor_count = get_core_number();
	printf("detected %u cores\n", processor_count);

	memory = get_memory();
	memory = (int)((memory / 1073741824.0) + 0.5);
	printf("detected %lld GB memory\n", memory);

	if ((processor_count >= 2) && (memory >= 5))
		cores_num = 2;

	if ((processor_count >= 4) && (memory >= 9))
		cores_num = 4;

	if ((processor_count >= 8) && (memory >= 16))
		cores_num = 8;

	printf("using %d threads\n", cores_num);
	return cores_num;
}