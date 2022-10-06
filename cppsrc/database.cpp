#include "database.h"

namespace Spino
{
    Database::Database()
    {
        kvstore = make_unique<Collection>("__SpinoKeyValueStore__", jw);
        kvstore->create_index("k");
    }

    Collection &Database::get_collection(const char *name)
    {
        if(std::string(name) == std::string("__SpinoKeyValueStore__"))
        {
            return *kvstore;
        }

        for (auto &col : collections)
        {
            if (std::string(col->get_name()) == name)
            {
                return *col;
            }
        }

        collections.push_back(make_unique<Collection>(name, jw));
        return *collections.back();
    }

    bool Database::has_collection(const char *name)
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

    void Database::drop_collection(const char *name)
    {
        for (auto iter = collections.begin(); iter != collections.end(); iter++)
        {
            if (std::string((*iter)->get_name()) == name)
            {
                collections.erase(iter);
                return;
            }
        }

        if (jw.get_enabled())
        {
            stringstream ss;
            ss << "{\"cmd\":\"dropCollection\",\"collection\":\"";
            ss << escape(name);
            ss << "\"}";
            jw.append(ss.str());
        }
    }

    void Database::set_bool_value(const char *key, bool value)
    {
        DomNode doc(kvstore->get_allocator());
        doc.set_object();

        DomNode *keyval = doc.push_in_place();
        keyval->set_key("k", (size_t)1);
        keyval->set_string(key, strlen(key));

        DomNode *valval = doc.push_in_place();
        valval->set_key("v", (size_t)1);
        valval->set_bool(value);

        std::string query = "{k:\"";
        query += key;
        query += "\"}";
        kvstore->upsert(query.c_str(), &doc);
    }

    bool Database::get_bool_value(const char *key)
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

    void Database::set_int_value(const char *key, int value)
    {
        DomNode doc(kvstore->get_allocator());
        doc.set_object();

        DomNode *keyval = doc.push_in_place();
        keyval->set_key("k", (size_t)1);
        keyval->set_string(key, strlen(key));

        DomNode *valval = doc.push_in_place();
        valval->set_key("v", (size_t)1);
        valval->set_int(value);

        std::string query = "{k:\"";
        query += key;
        query += "\"}";
        kvstore->upsert(query.c_str(), &doc);
    }

    int Database::get_int_value(const char *key)
    {
        std::string query = "{k:\"";
        query += key;
        query += "\"}";
        const DomNode *result = kvstore->find_one(query.c_str());
        if (result)
        {
            cout << "start" << endl;
            cout << "as string:" << result->stringify() << endl;
            return result->get_member("v", 1)->get_int();
        }
        return -1;
    }

    void Database::set_uint_value(const char *key, unsigned int value)
    {
        DomNode doc(kvstore->get_allocator());
        doc.set_object();

        DomNode *keyval = doc.push_in_place();
        keyval->set_key("k", (size_t)1);
        keyval->set_string(key, strlen(key));

        DomNode *valval = doc.push_in_place();
        valval->set_key("v", (size_t)1);
        valval->set_uint(value);

        std::string query = "{k:\"";
        query += key;
        query += "\"}";
        kvstore->upsert(query.c_str(), &doc);
    }

    unsigned int Database::get_uint_value(const char *key)
    {
        std::string query = "{k:\"";
        query += key;
        query += "\"}";
        const DomNode *result = kvstore->find_one(query.c_str());
        if (result)
        {
            return result->get_member("v", 1)->get_uint();
        }
        return -1;
    }

    void Database::set_double_value(const char *key, double value)
    {
        DomNode doc(kvstore->get_allocator());
        doc.set_object();

        DomNode *keyval = doc.push_in_place();
        keyval->set_key("k", (size_t)1);
        keyval->set_string(key, strlen(key));

        DomNode *valval = doc.push_in_place();
        valval->set_key("v", (size_t)1);
        valval->set_double(value);

        std::string query = "{k:\"";
        query += key;
        query += "\"}";
        kvstore->upsert(query.c_str(), &doc);
    }

    double Database::get_double_value(const char *key)
    {
        std::string query = "{k:\"";
        query += key;
        query += "\"}";
        const DomNode *result = kvstore->find_one(query.c_str());
        if (result)
        {
            return result->get_member("v", 1)->get_double();
        }
        return 0.0;
    }

    void Database::set_string_value(const char *key, const char *value)
    {
        DomNode doc(kvstore->get_allocator());
        doc.set_object();

        DomNode *keyval = doc.push_in_place();
        keyval->set_key("k", (size_t)1);
        keyval->set_string(key, strlen(key));

        DomNode *valval = doc.push_in_place();
        valval->set_key("v", (size_t)1);
        valval->set_string(value, strlen(value));

        std::string query = "{k:\"";
        query += key;
        query += "\"}";
        kvstore->upsert(query.c_str(), &doc);
    }

    const char *Database::get_string_value(const char *key)
    {
        std::string query = "{k:\"";
        query += key;
        query += "\"}";
        const DomNode *result = kvstore->find_one(query.c_str());
        if (result)
        {
            return result->get_member("v", 1)->get_string();
        }
        return nullptr;
    }

    bool Database::has_key(const char* key)
    {
        std::string query = "{k:\"";
        query += key;
        query += "\"}";
        const DomNode *result = kvstore->find_one(query.c_str());
        if (result)
        {
            return true;
        }
        return false;
    }

    FILE_ERROR Database::save(std::string path)
    {
        std::string tmppath = path + "spinotmp";
        ofstream out(tmppath, ios::out | ios::binary);
        if (!out)
        {
            return FILE_ERROR_WRITE_ERROR;
        }

        out.put(DOM_NODE_TYPE_ARRAY);
        out.write(kvstore->get_name(), strlen(kvstore->get_name()) + 1);
        auto cursor = kvstore->find("{}");
        while (cursor->has_next())
        {
            out.put(DOM_NODE_TYPE_OBJECT);
            cursor->next_dom()->to_not_bson(out);
        }
        out.put(0x00);

        for (auto &col : collections)
        {
            out.put(DOM_NODE_TYPE_ARRAY);
            out.write(col->get_name(), strlen(col->get_name()) + 1);
            auto cursor = col->find("{}");

            while (cursor->has_next())
            {
                out.put(DOM_NODE_TYPE_OBJECT);
                cursor->next_dom()->to_not_bson(out);
            }
            out.put(0x00);
        }
        out.flush();
        out.close();

        // move the temporary file to the correct location
        std::remove(path.c_str());                  // remove original db file
        std::rename(tmppath.c_str(), path.c_str()); // move tmp file to actual location
        std::remove(tmppath.c_str());               // remove tmp file

        // clear the journal
        if (jw.get_enabled())
        {
            std::ofstream ofs;
            ofs.open(jw.get_path(), std::ofstream::out | std::ofstream::trunc);
            ofs.close();
        }

        return FILE_ERROR_NONE;
    }

    FILE_ERROR Database::load(std::string path)
    {
        ifstream ifile(path, ios::in | ios::binary);
        if (!ifile)
        {
            return FILE_ERROR_PARSE_ERROR;
        }

        kvstore = make_unique<Collection>("__SpinoKeyValueStore__", jw);
        collections.erase(collections.begin(), collections.end());

        while (ifile.get() == DOM_NODE_TYPE_ARRAY)
        {
            std::string col_name;
            while (ifile.peek() != 0x00)
            {
                col_name += ifile.get();
            }
            ifile.get();

            if (col_name == std::string("__SpinoKeyValueStore__"))
            {
                while (ifile.get() == DOM_NODE_TYPE_OBJECT)
                {
                    DomNode doc(kvstore->get_allocator());
                    doc.set_object();
                    doc.from_not_bson(ifile);
                    kvstore->append(&doc);
                }
            }
            else
            {
                auto &col = get_collection(col_name.c_str());
                while (ifile.get() == DOM_NODE_TYPE_OBJECT)
                {
                    DomNode doc(col.get_allocator());
                    doc.set_object();
                    doc.from_not_bson(ifile);
                    col.append(&doc);
                }
            }
        }
        return FILE_ERROR_NONE;
    }

    void Database::save_not_bson_collection(std::string col_name, std::string path)
    {
        for (auto &col : collections)
        {
            if (col_name == col->get_name())
            {
                ofstream out(path, ios::out | ios::binary);
                auto cursor = col->find("{}");

                while (cursor->has_next())
                {
                    out.put(DOM_NODE_TYPE_OBJECT);
                    cursor->next_dom()->to_not_bson(out);
                }
                out.put(0x00);
            }
        }
    }

    void Database::save_not_bson_keystore(std::string path)
    {
        ofstream out(path, ios::out | ios::binary);

        auto cursor = kvstore->find("{}");
        while (cursor->has_next())
        {
            out.put(DOM_NODE_TYPE_OBJECT);
            cursor->next_dom()->to_not_bson(out);
        }
        out.put(0x00);
    }

    void Database::load_not_bson_collection(std::string col_name, std::string path)
    {
        auto &col = get_collection(col_name.c_str());

        ifstream ifile(path, ios::in | ios::binary);

        while (ifile.get() == DOM_NODE_TYPE_OBJECT)
        {
            DomNode doc(col.get_allocator());
            doc.set_object();
            doc.from_not_bson(ifile);
            col.append(&doc);
        }
    }

    void Database::save_json(const char *path)
    {
        ofstream ofile;
        ofile.open(path);
        ofile << "{";

        save_json_collection(ofile, kvstore);

        if (collections.size())
        {
            ofile << ",";
            auto iter = collections.begin();
            while (iter != collections.end())
            {
                save_json_collection(ofile, *iter);
                iter++;
                if (iter != collections.end())
                {
                    ofile << ",";
                }
            }
        }

        ofile << "}";
        ofile.close();
    }

    void Database::enable_journal(std::string jpath)
    {
        jw.set_enabled(true);
        jw.set_path(jpath);
    }

    void Database::disable_journal()
    {
        jw.set_enabled(false);
    }

    void Database::consolidate(std::string path)
    {
        bool priorState = jw.get_enabled();
        jw.set_enabled(false);

        // read the journal file and execute the commands
        std::ifstream file(jw.get_path());
        if (file.is_open())
        {
            std::string line;
            while (std::getline(file, line))
            {
                execute(line);
            }
            file.close();
        }

        jw.set_enabled(priorState);

        // write the db to disk
        // noting that save will also erase the journal
        save(path);
    }

    std::string Database::execute(std::string json)
    {
        DomAllocator alloc(1, 100);

        Parser parser(alloc);
        DomNode* doc = parser.parse(json.c_str());

        if (!doc->has_member("cmd"))
        {
            return "{\"error\":\"Missing cmd field\"}";
        }

        if (doc->get_member("cmd")->get_type() != DOM_NODE_TYPE_STRING)
        {
            return "{\"error\":\"cmd field must be of type string\"}";
        }

        std::string cmd = doc->get_member("cmd")->get_string();
        if (cmd == "count")
        {
            if (doc->has_member("collection"))
            {
                std::stringstream ss;
                ss << "{\"r\":1,\"count\":" << collections.size() << "}";
                return ss.str();
            }
            else
            {
                auto &col = get_collection(doc->get_member("collection")->get_string());
                std::stringstream ss;
                ss << "{\"r\":1,\"count\":" << col.size() << "}";
                return ss.str();
            }
        }

        if (cmd == "dropCollection")
        {
            if (doc->has_member("collection"))
            {
                drop_collection(doc->get_member("collection")->get_string());
                return "{\"msg\":\"OK\"}";
            }
            else
            {
                return "{\"error\":\"dropCollection command is missing a collection name\"}";
            }
        }

        if (cmd == "createIndex")
        {
            if (doc->has_member("collection") && (doc->has_member("field")))
            {
                DomNode *colnode = doc->get_member("collection");
                DomNode *fieldnode = doc->get_member("field");
                if ((colnode->get_type() != DOM_NODE_TYPE_STRING) || (fieldnode->get_type() != DOM_NODE_TYPE_STRING))
                {
                    return "{\"error\":\"both collection and field must be of type string\"}";
                }

                auto &col = get_collection(colnode->get_string());
                col.create_index(fieldnode->get_string());
                return "{\"msg\":\"OK\"}";
            }
            else
            {
                return "{\"error\":\"Missing collection or 'field' field in createIndex command\"}";
            }
        }

        if (cmd == "dropIndex")
        {
            if (doc->has_member("collection") && (doc->has_member("field")))
            {
                DomNode *colnode = doc->get_member("collection");
                DomNode *fieldnode = doc->get_member("field");
                if ((colnode->get_type() != DOM_NODE_TYPE_STRING) || (fieldnode->get_type() != DOM_NODE_TYPE_STRING))
                {
                    return "{\"error\":\"both collection and field must be of type string\"}";
                }

                auto &col = get_collection(colnode->get_string());
                col.drop_index(fieldnode->get_string());
                return "{\"msg\":\"OK\"}";
            }
            else
            {
                return "{\"error\":\"Missing collection or 'field' field in createIndex command\"}";
            }
        }

        if (cmd == "findOne")
        {
            if (doc->has_member("collection") && doc->has_member("query"))
            {
                DomNode *colnode = doc->get_member("collection");
                DomNode *querynode = doc->get_member("query");
                if ((colnode->get_type() != DOM_NODE_TYPE_STRING) || (querynode->get_type() != DOM_NODE_TYPE_STRING))
                {
                    return "{\"error\":\"both collection and query must be of type string\"}";
                }

                auto &col = get_collection(colnode->get_string());
                const DomNode *result = col.find_one(querynode->get_string());
                if (result)
                {
                    return result->stringify();
                }
                else
                {
                    return "{}";
                }
            }
            else
            {
                return "{\"error\":\"Collection and query fields are required\"}";
            }
        }

        if (cmd == "find")
        {
            DomNode *colnode = doc->get_member("collection");
            DomNode *querynode = doc->get_member("query");
            if ((colnode->get_type() != DOM_NODE_TYPE_STRING) || (querynode->get_type() != DOM_NODE_TYPE_STRING))
            {
                return "{\"error\":\"both collection and query must be of type string\"}";
            }

            size_t limit = 20;
            DomNode *limitnode = doc->get_member("limit");
            if (limitnode)
            {
                if (limitnode->get_type() == DOM_NODE_TYPE_UINT)
                {
                    limit = limitnode->get_uint();
                }
            }

            auto &col = get_collection(colnode->get_string());
            auto cursor = col.find(querynode->get_string())->set_limit(limit);
            string result = "{";
            while (cursor->has_next())
            {
                result += cursor->next();
                if (cursor->has_next())
                {
                    result += ",";
                }
            }
            result += "}";
            return result;
        }

        if (cmd == "append")
        {
            DomNode *colnode = doc->get_member("collection");
            DomNode *docnode = doc->get_member("document");
            if (!colnode || !docnode)
            {
                return "{\"error\":\"append command requires a collection and document field of type string\"}";
            }
            if ((colnode->get_type() != DOM_NODE_TYPE_STRING) || (docnode->get_type() != DOM_NODE_TYPE_STRING))
            {
                return "{\"error\":\"both collection and document must be of type string\"}";
            }

            auto &col = get_collection(colnode->get_string());
            if (col.append(docnode->get_string()))
            {
                return "{\"msg\":\"OK\"}";
            }
            else
            {
                return "{\"error\":\"parse error\"}";
            }
        }

        if (cmd == "upsert")
        {
            DomNode *colnode = doc->get_member("collection");
            DomNode *querynode = doc->get_member("query");
            DomNode *docnode = doc->get_member("document");
            if (!colnode || !querynode || !docnode)
            {
                return "{\"error\":\"update command requires a collection, query and document field\"}";
            }
            if ((colnode->get_type() != DOM_NODE_TYPE_STRING) || (querynode->get_type() != DOM_NODE_TYPE_STRING) || (docnode->get_type() != DOM_NODE_TYPE_STRING))
            {
                return "{\"error\":\"both collection and document must be of type string\"}";
            }

            auto &col = get_collection(colnode->get_string());
            col.upsert(querynode->get_string(), docnode->get_string());
            return "{\"msg\":\"OK\"}";
        }

        if (cmd == "drop")
        {
            DomNode *colnode = doc->get_member("collection");
            DomNode *querynode = doc->get_member("query");
            DomNode *limitnode = doc->get_member("limit");
            if (!colnode || !querynode || !limitnode)
            {
                return "{\"error\":\"update command requires a collection, query and document field\"}";
            }
            if ((colnode->get_type() != DOM_NODE_TYPE_STRING) || (querynode->get_type() != DOM_NODE_TYPE_STRING) || (limitnode->get_type() != DOM_NODE_TYPE_UINT))
            {
                return "{\"error\":\"collection and document must be of type string. limit must be an unsigned integer.\"}";
            }

            auto &col = get_collection(colnode->get_string());
            col.drop(querynode->get_string(), limitnode->get_uint());
            return "{\"msg\":\"OK\"}";
        }

        if (cmd == "getValue")
        {
            DomNode *keynode = doc->get_member("key");
            if (!keynode)
            {
                return "{\"error\":\"key field is required\"}";
            }

            if (keynode->get_type() != DOM_NODE_TYPE_STRING)
            {
                return "{\"error\":\"key field must be a string\"}";
            }

            std::string query = "{k:\"";
            query += keynode->get_string();
            query += "\"}";
            const DomNode *result = kvstore->find_one(query.c_str());
            if (result)
            {
                return result->stringify();
            }
            else
            {
                return "{}";
            }
        }

        if (cmd == "setValue")
        {
            DomNode *keynode = doc->get_member("key");
            DomNode *valuenode = doc->get_member("value");
            if (!keynode || !valuenode)
            {
                return "{\"error\":\"missing key or value fields\"}";
            }

            DomNode doc(kvstore->get_allocator());
            doc.set_object();
            DomNode *key = doc.push_in_place();
            key->set_string(keynode->get_string());
            key->set_key("k");

            DomNode *value = doc.push_in_place();
            value->copy(valuenode);
            value->set_key("v");

            kvstore->append(keynode);
            return "{\"msg\":\"OK\"}";
        }

        return "{\"error\":\"Invalid command\"}";
    }

    void Database::save_json_collection(ofstream &ofile, unique_ptr<Collection> &col)
    {
        ofile << "\"" << col->get_name() << "\":[";
        auto cursor = col->find("{}");
        while (cursor->has_next())
        {
            ofile << cursor->next();
            if (cursor->has_next())
            {
                ofile << ",";
            }
        }
        ofile << "]";
    }


}

