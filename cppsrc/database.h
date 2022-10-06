#ifndef SPINO_DATABASE_H
#define SPINO_DATABASE_H

#include <fstream>
#include "collection.h"

namespace Spino
{

    enum FILE_ERROR
    {
        FILE_ERROR_NONE,
        FILE_ERROR_PARSE_ERROR,
        FILE_ERROR_DOES_NOT_EXIST,
        FILE_ERROR_WRITE_ERROR,
        FILE_ERROR_INVALID
    };

    class Database
    {
        public:
            Database();
            Collection &get_collection(const char *name);
            bool has_collection(const char *name);
            void drop_collection(const char *name);
            void set_bool_value(const char *key, bool value);
            bool get_bool_value(const char *key);
            void set_int_value(const char *key, int value);
            int get_int_value(const char *key);
            void set_uint_value(const char *key, unsigned int value);
            unsigned int get_uint_value(const char *key);
            void set_double_value(const char *key, double value);
            double get_double_value(const char *key);
            void set_string_value(const char *key, const char *value);
            const char *get_string_value(const char *key);
            bool has_key(const char* key);
            FILE_ERROR save(std::string path);
            FILE_ERROR load(std::string path);
            void save_not_bson_collection(std::string col_name, std::string path);
            void save_not_bson_keystore(std::string path);
            void load_not_bson_collection(std::string col_name, std::string path);
            void save_json(const char *path);
            void enable_journal(std::string jpath);
            void disable_journal();
            void consolidate(std::string path);
            std::string execute(std::string json);

            void print_keystore()
            {
                kvstore->print();
            }

        private:
            void save_json_collection(ofstream &ofile, unique_ptr<Collection> &col);


            unique_ptr<Collection> kvstore;
            std::vector<unique_ptr<Collection>> collections;
            JournalWriter jw;
    };
}

#endif
