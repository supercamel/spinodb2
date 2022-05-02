#ifndef SPINO_INDEX_H
#define SPINO_INDEX_H

#include <ctime>
#include "dom_node.h"

namespace Spino
{

    class Index
    {
    public:
        constexpr static int MAX_LEVELS = 20;
        class SkipListNode
        {
        public:
            SkipListNode(DomNode *p, int level)
            {
                dom_node = p;
                for (int i = 0; i < level; i++)
                {
                    next[i] = 0;
                }
            }

            SkipListNode* get_next() {
                return (SkipListNode*)next[0];
            }

            DomNode *dom_node;
            void *next[MAX_LEVELS + 1];
        };

        struct Range
        {
            SkipListNode *first;
            SkipListNode *last;
        };

        Index(const char *keycstr)
        {
            head = (SkipListNode *)big_alloc.alloc();
            head = new (head) SkipListNode(nullptr, MAX_LEVELS + 1);

            length = 0;
            key = keycstr;
            g_seed = time(0);
        }

        ~Index()
        {
        }

        void insert(DomNode *t)
        {
            if (t->has_member(key.c_str(), key.length()))
            {
                DomNode* index_node = t->get_member(key.c_str(), key.length());
                int rlevel = random_level();
                SkipListNode *node;
                if (rlevel > 2)
                {
                    node = (SkipListNode *)big_alloc.alloc();
                }
                else
                {
                    node = (SkipListNode *)small_alloc.alloc();
                }

                node = new (node) SkipListNode(t, rlevel);
                SkipListNode *iter = head;
                SkipListNode *update[MAX_LEVELS + 1];

                for (int level = MAX_LEVELS; level >= 0; level--)
                {
                    while (iter->next[level] != nullptr && compare(index_node, ((SkipListNode *)(iter->next[level]))->dom_node) < 0)
                    {
                        // skip along
                        iter = (SkipListNode *)iter->next[level];
                    }
                    update[level] = iter;
                }

                for (int i = 0; i <= rlevel; i++)
                {
                    node->next[i] = update[i]->next[i];
                    update[i]->next[i] = (void *)node;
                }

                length++;
            }
        }

        void prepend(DomNode *t)
        {
            int rlevel = random_level();
            SkipListNode *node;

            if (rlevel > 2)
            {
                node = (SkipListNode *)big_alloc.alloc();
            }
            else
            {
                node = (SkipListNode *)small_alloc.alloc();
            }

            node = new (node) SkipListNode(t, rlevel);
            for (int i = 0; i <= rlevel; i++)
            {
                node->next[i] = head->next[i];
                head->next[i] = (void *)node;
            }

            length++;
        }

        SkipListNode *find_first(DomNode *t)
        {
            SkipListNode* iter = less_than(t);
            if(iter)
            {
                iter = (SkipListNode*)iter->next[0];
            } 
            return iter;
        }

        SkipListNode *find_last(DomNode *t)
        {
            SkipListNode *iter = head;
            for (int level = MAX_LEVELS; level >= 0; level--)
            {
                while (iter->next[level] && compare(t, ((SkipListNode *)(iter->next[level]))->dom_node) <= 0)
                {
                    iter = (SkipListNode *)iter->next[level];
                }
            }
            return (SkipListNode *)iter;
        }

        SkipListNode* greater_than(DomNode *t)
        {
            SkipListNode* iter = find_last(t);
            if(iter)
            {
                iter = (SkipListNode*)iter->next[0];
            }
            return iter;
        }

        SkipListNode* less_than(DomNode* t)
        {
            SkipListNode *iter = head;
            for (int level = MAX_LEVELS; level >= 0; level--)
            {
                while (iter->next[level] && compare(t, ((SkipListNode *)(iter->next[level]))->dom_node) < 0)
                {
                    iter = (SkipListNode *)iter->next[level];
                }
            }
            return (SkipListNode *)iter;
        }

        Range equal_range(DomNode *t)
        {
            Range range;
            range.first = find_first(t);
            range.last = find_last(t);
            return range;
        }

        SkipListNode *begin()
        {
            return (SkipListNode *)head->next[0];
        }

        DomNode *delete_first(DomNode *t)
        {
            SkipListNode *iter = head;

            DomNode *ret = nullptr;
            for (int level = MAX_LEVELS; level >= 0; level--)
            {
                int cmp = 0;
                while (iter->next[level] && (cmp = compare(t, ((SkipListNode *)(iter->next[level]))->dom_node)) < 0)
                {
                    // skip along
                    iter = (SkipListNode *)iter->next[level];
                }
                if (cmp == 0)
                {
                    if (iter->next[level])
                    {
                        ret = ((SkipListNode *)(iter->next[level]))->dom_node;
                        SkipListNode *nextnext = (SkipListNode *)((SkipListNode *)(iter->next[level]))->next[level];
                        iter->next[level] = nextnext;
                    }
                }
            }
            return ret;
        }

        DomNode *delete_node(DomNode* doc)
        {
            SkipListNode *iter = head;

            DomNode *ret = nullptr;
            for (int level = MAX_LEVELS; level >= 0; level--)
            {
                int cmp = 0;
                while (iter->next[level] && (cmp = compare_with_id(doc, ((SkipListNode *)(iter->next[level]))->dom_node)) < 0)
                {
                    iter = (SkipListNode *)iter->next[level];
                }
                if (cmp == 0)
                {
                    if (iter->next[level])
                    {
                        ret = ((SkipListNode *)(iter->next[level]))->dom_node;
                        SkipListNode *nextnext = (SkipListNode *)((SkipListNode *)(iter->next[level]))->next[level];
                        iter->next[level] = nextnext;
                    }
                }
            }
            return ret;
        }

        void print()
        {
            SkipListNode *iter = (SkipListNode *)head->next[0];
            while (iter)
            {
                cout << iter->dom_node->stringify() << endl;
                iter = (SkipListNode *)iter->next[0];
            }
        }

        const char *get_key()
        {
            return key.c_str();
        }

    private:
        int compare(DomNode *b, DomNode *rh)
        {
            DomNode *a = rh->get_member(key.c_str(), key.length());

            DOM_NODE_TYPE a_type = a->get_type();
            DOM_NODE_TYPE b_type = b->get_type();

            if (a_type != b_type)
            {
                if(b_type == DOM_NODE_TYPE_DOUBLE) {
                    if(a_type == DOM_NODE_TYPE_INT) {
                        return (double)a->get_int() - b->get_double();
                    }
                    else if(a_type == DOM_NODE_TYPE_UINT) {
                        return (double)a->get_uint() - b->get_double();
                    }
                }

                if (a_type > b_type)
                {
                    return -1;
                }
                else
                {
                    return 1;
                }
            }
            else
            {
                switch (b_type)
                {
                case DOM_NODE_TYPE_INT:
                {
                    return a->get_int() - b->get_int();
                }
                break;
                case DOM_NODE_TYPE_UINT:
                {
                    return (double)a->get_uint() - (double)b->get_uint();
                }
                case DOM_NODE_TYPE_DOUBLE:
                {
                    return a->get_double() - b->get_double();
                }
                case DOM_NODE_TYPE_STRING:
                {
                    cout << a->get_string() << " " << b->get_string() << endl;
                    return strcmp(b->get_string(), a->get_string());
                }
                break;
                default:
                    return 0;
                }
            }
        }

        int compare_with_id(DomNode *lh, DomNode *rh)
        {
            int key_result = compare(lh->get_member(key.c_str(), key.length()), rh);
            if (key_result == 0)
            {
                uint64_t lhid = lh->get_member("_id", 4)->get_uint();
                uint64_t rhid = rh->get_member("_id", 4)->get_uint();

                if (lhid > rhid)
                {
                    return 1;
                }
                else if (lhid < rhid)
                {
                    return -1;
                }
                else
                {
                    return 0;
                }
            }
            else
            {
                return key_result;
            }
        }

        int fast_rand()
        {
            g_seed = (214013 * g_seed + 2531011);
            return (g_seed >> 16) & 0x7FFF;
        }

        int random_level()
        {
            int r = fast_rand();
            int lvl = 0;
            while (r < P_RAND && lvl < MAX_LEVELS)
            {
                lvl++;
                r = fast_rand();
            }
            return lvl;
        }

        Allocator<sizeof(SkipListNode)> big_alloc;
        Allocator<32> small_alloc;

        SkipListNode *head;

        constexpr static double SLIST_P = 0.2;
        constexpr static int FASTRAND_MAX = 32767;
        constexpr static double P_RAND = FASTRAND_MAX * SLIST_P;

        int g_seed;

        size_t length;
        std::string key;
    };

}
#endif
