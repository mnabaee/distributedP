/*
 * DateTime.cpp
 *
 *  Created on: Jan 14, 2016
 *      Author: mahdy
 */

#include "DateTime.h"

unsigned long long current_date_time_usec(){
	struct timeval tp;
	gettimeofday(&tp, NULL);

	unsigned long long usec = ((unsigned long long) tp.tv_sec) * 1000000ll + (unsigned long long) tp.tv_usec;
	return usec;
}

unsigned long long current_date_time_msec(){

	return current_date_time_usec() / 1000;

	//return time(0);
}

unsigned long long get_time_difference_ms(unsigned long long from){
	return current_date_time_msec() - from;
}

unsigned int current_time_in_day_sec(){
	time_t t = time(0);
	struct tm * now = localtime( & t );
	unsigned res = now->tm_sec + now->tm_min * 60 + now->tm_hour * 3600;
	return res;
}

unsigned int current_time_in_day_hr()
{return floor(current_time_in_day_sec() / 3600 ); }

std::string current_date_time_str(){
	  time_t rawtime;
	  struct tm * timeinfo;
	  char buffer[80];

	  time (&rawtime);
	  timeinfo = localtime(&rawtime);

	  strftime(buffer,80,"%d-%m-%Y %I:%M:%S %p",timeinfo);
	  std::string str(buffer);
	  return str;
}

const char* current_date_time_cstr(){
	return current_date_time_str().c_str();
}

unsigned long current_date(DateFormat* dateFormatted){
	time_t t = time(0);
	struct tm * now = localtime( & t );
	if(dateFormatted){
		dateFormatted->year = now->tm_year + 1970;
		dateFormatted->month = now->tm_mon + 1;
		dateFormatted->day = now->tm_mday;
	}
	return floor(t / (60*60*24));
}

std::string date_to_str(unsigned long date){
	time_t rawtime = date * (60*60*24) + 1;
	  struct tm * timeinfo;
	  char buffer[80];

	  timeinfo = localtime(&rawtime);

	  strftime(buffer,80,"%d-%m-%Y",timeinfo);
	  std::string str(buffer);
	  return str;
}
