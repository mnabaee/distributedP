/*
 * DatabaseAccess.h
 *
 *  Created on: Jun 27, 2016
 *      Author: mahdy
 */

#ifndef DATABASE_DATABASEACCESS_H_
#define DATABASE_DATABASEACCESS_H_

#include <types/JsonObject.h>

class DatabaseAccess {
public:

	typedef enum DatabaseType{
		MONGO,
		MYSQL,
		FILE
	} DatabaseType;

	DatabaseAccess();
	virtual ~DatabaseAccess();

};

#endif /* DATABASE_DATABASEACCESS_H_ */
