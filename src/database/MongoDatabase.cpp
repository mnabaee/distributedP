/*
 * MongoDatabase.cpp
 *
 *  Created on: Jun 27, 2016
 *      Author: mahdy
 */

#include "MongoDatabase.h"

MongoDatabase::MongoDatabase() {


}

void MongoDatabase::init(string db, string dbName, string collectionName){
   mongoc_init ();
   client = mongoc_client_new (db.c_str());

   database = mongoc_client_get_database (client, dbName.c_str());
   collection = mongoc_client_get_collection (client, dbName.c_str(), collectionName.c_str() );
}

MongoDatabase::~MongoDatabase() {
    mongoc_collection_destroy (collection);
    mongoc_client_destroy (client);
    mongoc_cleanup ();
}

void MongoDatabase::insert(JsonObject& json){
	bson_error_t error;
	bson_t *doc = bson_new ();
	bson_init_from_json(doc, json.ToString().c_str(), json.ToString().length(), &error);
	bson_oid_t oid;
	bson_oid_init (&oid, NULL);
	BSON_APPEND_OID (doc, "_id", &oid);

    if (!mongoc_collection_insert (collection, MONGOC_INSERT_NONE, doc, NULL, &error)) {
        LogError("Failed to Insert!");
    }
    bson_destroy (doc);
}

void MongoDatabase::query(JsonObject& json, vector<JsonObject> & res, unsigned int max_results){
	bson_error_t error;
	bson_t *query_ = bson_new ();
	const bson_t *doc;

	bson_init_from_json(query_, json.ToString().c_str(), json.ToString().length(), &error);
	mongoc_cursor_t *cursor = mongoc_collection_find (collection, MONGOC_QUERY_NONE, 0, max_results, 0, query_, NULL, NULL);

	while (mongoc_cursor_next (cursor, &doc)) {
	     char* str = bson_as_json (doc, NULL);
	     string str2(str);
	     JsonObject thisJ;
	     if( JsonObject::ParseFromString(str2, thisJ) ){
	    	 res.push_back( thisJ );
	     }else{
	    	 LogError("could not parse bson to json!");
	     }
	     bson_free (str);
	}
	mongoc_cursor_destroy (cursor);
	//bson_destroy (doc);
	bson_destroy (query_);

}

unsigned int MongoDatabase::upsertMany(vector<JsonObject>& queries, vector<JsonObject>& values){
	unsigned int cnt = 0;

	if( queries.size() != values.size() ){
		LogError("Size of queries and values should be the same!");
		return cnt;
	}

	mongoc_bulk_operation_t *bulk;
	bson_error_t error;
	bson_t reply;
	char *str;
	bool ret;

	bulk = mongoc_collection_create_bulk_operation (collection, true, NULL);

	for(unsigned int i = 0; i < queries.size(); i++){
		bson_t *doc_ = bson_new ();
		bson_t *query_ = bson_new ();

		bson_init_from_json(query_, queries[i].ToString().c_str(), queries[i].ToString().length(), &error);
		JsonObject newVal;
		newVal.PutJson("$set", values[i]);
		bson_init_from_json(doc_, newVal.ToString().c_str(), newVal.ToString().length(), &error);

		mongoc_bulk_operation_update(bulk, query_, doc_, true);

		bson_destroy(query_);
		bson_destroy(doc_);
	}
	ret = mongoc_bulk_operation_execute (bulk, &reply, &error);
	str = bson_as_json (&reply, NULL);

    JsonObject thisJ;
    if( ret && JsonObject::ParseFromString(str, thisJ) &&
    	thisJ.hasValue("nUpserted")	){
    	cnt = thisJ.GetUInt("nUpserted");
    	//LogInfo("success str = %s", str);
    }else{
    	LogError("error!\n%s", error.message);
    }

	bson_free (str);
	bson_destroy (&reply);
	mongoc_bulk_operation_destroy (bulk);

	return cnt;
}

void MongoDatabase::update(JsonObject& jsonQuery, JsonObject& json){
	bson_error_t error;
	bson_t *query_ = bson_new ();
	bson_t *update = bson_new ();
	bson_init_from_json(query_, jsonQuery.ToString().c_str(), jsonQuery.ToString().length(), &error);
	bson_init_from_json(update, json.ToString().c_str(), json.ToString().length(), &error);

	mongoc_collection_update (collection, MONGOC_UPDATE_NONE, query_, update, NULL, &error);

	bson_destroy(query_);
	bson_destroy(update);
}

void MongoDatabase::remove(JsonObject& jsonQuery){
	bson_error_t error;
	bson_t *query_ = bson_new ();
	bson_init_from_json(query_, jsonQuery.ToString().c_str(), jsonQuery.ToString().length(), &error);

	mongoc_collection_remove (collection, MONGOC_REMOVE_SINGLE_REMOVE, query_, NULL, &error);
	bson_destroy(query_);
}

bool MongoDatabase::insertIfNotExist(JsonObject& query_, JsonObject& json){
	vector<JsonObject> entries;
	query(query_, entries);
	if(entries.size() == 0){
		insert(json);
		return true;
	}else return false;
}

bool MongoDatabase::updateOrInsert(JsonObject& json, JsonObject& key){
	vector<JsonObject> entries;
	query(key, entries);
	if(entries.size() == 0){
		insert(json);
		return false;
	}else{
		update(key, json);
		return true;
	}

}
