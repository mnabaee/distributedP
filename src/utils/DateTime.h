/*
 * DateTime.h
 *
 *  Created on: Jan 14, 2016
 *      Author: mahdy
 */

#ifndef SRC_UTILS_DATETIME_H_
#define SRC_UTILS_DATETIME_H_

#include <stdlib.h>
#include <string>
#include <sys/time.h>
#include <math.h>

typedef struct DateFormat{
	unsigned int year;
	unsigned int month;
	unsigned int day;
} DateFormat;

std::string current_date_time_str();

const char* current_date_time_cstr();

unsigned int current_time_in_day_sec();
unsigned int current_time_in_day_hr();
unsigned long long current_date_time_msec();
unsigned long long current_date_time_usec();

unsigned long long get_time_difference_ms(unsigned long long from);

unsigned long current_date(DateFormat* dateFormatted = NULL);
std::string date_to_str(unsigned long date);


#endif /* SRC_UTILS_DATETIME_H_ */
