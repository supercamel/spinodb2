#ifndef SPINO_DATABASE_H
#define SPINO_DATABASE_H

#include <fstream>
#include "collection.h"

namespace Spino
{

    class Database
    {
    public:
        Database()
        {
            kvstore = make_unique<Collection>("__SpinoKeyValueStore__");
            kvstore->create_index("k");
        }

        Collection &get_collection(const char *name)
        {
            for (auto &col : collections)
            {
                if (std::string(col->get_name()) == name)
                {
                    return *col;
                }
            }

            collections.push_back(make_unique<Collection>(name));
            return *collections.back();
        }

        bool has_collection(const char *name)
        {
            for (auto &col : collections)
            {
                if (std::string(col->get_name()) == name)
                {
                    return true;
                }
            }

            return false;
        }

        void drop_collection(const char *name)
        {
            for (auto iter = collections.begin(); iter != collections.end(); iter++)
            {
                if (std::string((*iter)->get_name()) == name)
                {
                    collections.erase(iter);
                    return;
                }
            }
        }

        void set_bool_value(const char *key, bool value)
        {
            DomNode doc(kvstore->get_allocator());
            doc.set_object();

            DomNode *keyval = doc.push_in_place();
            keyval->set_key("k", 1);
            keyval->set_string(key, strlen(key));

            DomNode *valval = doc.push_in_place();
            valval->set_key("v", 1);
            valval->set_bool(value);

            std::string query = "{k:\"";
            query += key;
            query += "\"}";
            kvstore->upsert(query.c_str(), &doc);
        }

        bool get_bool_value(const char *key)
        {
            std::string query = "{k:\"";
            query += key;
            query += "\"}";
            const DomNode *result = kvstore->find_one(query.c_str());
            if (result)
            {
                return result->get_member("v", 1)->get_bool();
            }
            return false;
        }

        void set_int_value(const char *key, int value)
        {
            DomNode doc(kvstore->get_allocator());
            doc.set_object();

            DomNode *keyval = doc.push_in_place();
            keyval->set_key("k", 1);
            keyval->set_string(key, strlen(key));

            DomNode *valval = doc.push_in_place();
            valval->set_key("v", 1);
            valval->set_int(value);

            std::string query = "{k:\"";
            query += key;
            query += "\"}";
            kvstore->upsert(query.c_str(), &doc);
        }

        int get_int_value(const char *key)
        {
            std::string query = "{k:\"";
            query += key;
            query += "\"}";
            const DomNode *result = kvstore->find_one(query.c_str());
            if (result)
            {
                return result->get_member("v", 1)->get_int();
            }
            return -1;
        }

        void set_double_value(const char *key, double value)
        {
            DomNode doc(kvstore->get_allocator());
            doc.set_object();

            DomNode *keyval = doc.push_in_place();
            keyval->set_key("k", 1);
            keyval->set_string(key, strlen(key));

            DomNode *valval = doc.push_in_place();
            valval->set_key("v", 1);
            valval->set_double(value);

            std::string query = "{k:\"";
            query += key;
            query += "\"}";
            kvstore->upsert(query.c_str(), &doc);
        }

        double get_double_value(const char *key)
        {
            std::string query = "k\"";
            query += key;
            query += "\"}";
            const DomNode *result = kvstore->find_one(query.c_str());
            if (result)
            {
                return result->get_member("v", 1)->get_double();
            }
            return 0.0;
        }

        void set_string_value(const char *key, const char *value)
        {
            DomNode doc(kvstore->get_allocator());
            doc.set_object();

            DomNode *keyval = doc.push_in_place();
            keyval->set_key("k", 1);
            keyval->set_string(key, strlen(key));

            DomNode *valval = doc.push_in_place();
            valval->set_key("v", 1);
            valval->set_string(value, strlen(value));

            std::string query = "{k:\"";
            query += key;
            query += "\"}";
            kvstore->upsert(query.c_str(), &doc);
        }

        const char *get_string_value(const char *key)
        {
            std::string query = "k\"";
            query += key;
            query += "\"}";
            const DomNode *result = kvstore->find_one(query.c_str());
            if (result)
            {
                return result->get_member("v", 1)->get_string();
            }
            return nullptr;
        }

        void save(const char *path)
        {
            ofstream ofile;
            ofile.open(path);
            ofile << "{";

            save_collection(ofile, kvstore);

            ofile << "}";
            ofile.close();
        }

    private:

        void save_collection(ofstream& ofile, unique_ptr<Collection>& col)
        {
            ofile << "\"" << col->get_name() << "\":[";
            auto cursor = col->find("{}");
            while(cursor->has_next())
            {
                ofile << cursor->next();
                if(cursor->has_next())
                {
                    ofile << ",";
                }
            }
            ofile << "]";
        }

        unique_ptr<Collection> kvstore;
        std::vector<unique_ptr<Collection>> collections;
    };

}

#endif
