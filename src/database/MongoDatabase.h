/*
 * MongoDatabase.h
 *
 *  Created on: Jun 27, 2016
 *      Author: mahdy
 */

#ifndef DATABASE_MONGODATABASE_H_
#define DATABASE_MONGODATABASE_H_

#include "DatabaseAccess.h"

#include <bson.h>
#include <bcon.h>
#include <mongoc.h>

class MongoDatabase: public DatabaseAccess {
	mongoc_client_t      *client;
	mongoc_database_t    *database;
	mongoc_collection_t  *collection;

public:
	MongoDatabase();
	virtual ~MongoDatabase();

	void init(string db, string dbName, string collectionName);

	void insert(JsonObject& json);
	void query(JsonObject& jsonQuery, vector<JsonObject> & res, unsigned int max_results = 0);
	void update(JsonObject& jsonQuery, JsonObject& json);
	void remove(JsonObject& jsonQuery);

	bool insertIfNotExist(JsonObject& query, JsonObject& json);
	bool updateOrInsert(JsonObject& json, JsonObject& key);

	unsigned int upsertMany(vector<JsonObject>& queries, vector<JsonObject>& values);

};

#endif /* DATABASE_MONGODATABASE_H_ */
