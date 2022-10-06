#ifndef SPINO_INDEX_H
#define SPINO_INDEX_H

#include <ctime>
#include "dom_node.h"

namespace Spino
{

    class Index
    {
    public:
        static constexpr size_t MAX_LEVELS = 20;

        class SkipListNode
        {
        public:
            SkipListNode(const DomView& dom_view, const DomView& index_node, int level) :
                dom_view(dom_view), index_node(index_node)
            {
                for (int i = 0; i < level+1; i++)
                {
                    next.push_back(nullptr);
                }
            }

            bool is_valid_dom() {
                dom_view.get_type() != DOM_NODE_TYPE_NULL;
            }

            const DomView& get_view() {
                return dom_view;
            }

            const DomView& get_index_view() {
                return index_node;
            }

            SkipListNode* get_next(int level = 0)
            {
                return next[level];
            }

            void set_next(int level, SkipListNode *node)
            {
                next[level] = node;
            }

        private:
            const DomView& dom_view;
            DomView index_node;
            std::vector<SkipListNode*> next;
        };

        struct Range
        {
            SkipListNode *first;
            SkipListNode *last;
        };

        Index(const std::string& keycstr)
        {
            head = new SkipListNode(DomView(), DomView(), MAX_LEVELS + 1);

            length = 0;
            key = keycstr;
            g_seed = time(0);
        }

        ~Index() {}

        void insert(const DomView& t)
        {
            if(t.has_member(key)) {
                const DomView& index_node = t.get_member(key);
                int rlevel = random_level();
                SkipListNode *node = new SkipListNode(t, index_node, rlevel);

                SkipListNode *iter = head;
                SkipListNode *update[MAX_LEVELS + 1];

                for (int level = MAX_LEVELS; level >= 0; level--)
                {
                    while (iter->get_next(level) != nullptr && compare(index_node, iter->get_next(level)->get_index_view()) < 0)
                    {
                        // skip along
                        iter = (SkipListNode *)iter->get_next(level);
                    }
                    update[level] = iter;
                }

                for (int i = 0; i <= rlevel; i++)
                {
                    node->set_next(i, update[i]->get_next(i));
                    update[i]->set_next(i, node);
                }

                length++;
            }
        }

        void prepend(const DomView& t)
        {
            if(t.has_member(key)) {
                const DomView& index_node = t.get_member(key);
                int rlevel = random_level();
                SkipListNode *node = new SkipListNode(t, index_node, rlevel);

                for (int i = 0; i <= rlevel; i++)
                {
                    node->set_next(i, head->get_next(i));
                    head->set_next(i, node);
                }

                length++;
            }
        }

        SkipListNode *find_first(const DomView& t)
        {
            SkipListNode *iter = less_than(t);
            if (iter)
            {
                iter = (SkipListNode *)iter->get_next(0);
            }

            if (iter && compare(t, iter->get_index_view()) == 0)
            {
                return iter;
            }
            else
            {
                return nullptr;
            }
        }

        SkipListNode *find_last(const DomView& t)
        {
            SkipListNode *iter = head;
            for (int level = MAX_LEVELS; level >= 0; level--)
            {
                while (iter->get_next(level) && compare(t, iter->get_next(level)->get_index_view()) <= 0)
                {
                    iter = (SkipListNode *)iter->get_next(level);
                }
            }

            if (iter->is_valid_dom() && compare(t, iter->get_index_view()) == 0)
            {
                return iter;
            }
            else
            {
                return nullptr;
            }
        }

        SkipListNode *greater_than(const DomView& t)
        {
            SkipListNode *iter = find_last(t);
            if (iter)
            {
                iter = (SkipListNode *)iter->get_next(0);
            }
            return iter;
        }

        SkipListNode *less_than(const DomView& t)
        {
            SkipListNode *iter = head;
            for (int level = MAX_LEVELS; level >= 0; level--)
            {
                while (iter->get_next(level) && compare(t, iter->get_next(level)->get_index_view()) < 0)
                {
                    iter = iter->get_next(level);
                }
            }

            return iter;
        }

        Range equal_range(const DomView& t)
        {
            Range range;
            range.first = find_first(t);
            range.last = find_last(t);

            return range;
        }

        SkipListNode *begin()
        {
            return head->get_next(0);
        }

        const DomView* delete_first(const DomView& t)
        {
            SkipListNode *iter = head;

            const DomView *ret = nullptr;
            for (int level = MAX_LEVELS; level >= 0; level--)
            {
                int cmp = 0;
                while (iter->get_next(level) && (cmp = compare(t, iter->get_next(level)->get_index_view())) < 0)
                {
                    // skip along
                    iter = (SkipListNode *)iter->get_next(level);
                }
                if (cmp == 0)
                {
                    if (iter->get_next(level))
                    {
                        ret = &iter->get_next(level)->get_view();
                        SkipListNode *nextnext = iter->get_next(level)->get_next(level);
                        iter->set_next(level, nextnext);
                    }
                }
            }
            return ret;
        }

        const DomView* delete_node(const DomView& doc)
        {
            SkipListNode *iter = head;

            const DomView* ret = nullptr;
            for (int level = MAX_LEVELS; level >= 0; level--)
            {
                int cmp = 0;
                while (iter->get_next(level) && (cmp = compare_with_id(doc, iter->get_next(level)->get_view())) < 0)
                {
                    iter = (SkipListNode *)iter->get_next(level);
                }
                if (cmp == 0)
                {
                    if (iter->get_next(level))
                    {
                        ret = &iter->get_next(level)->get_view();
                        SkipListNode *nextnext = iter->get_next(level)->get_next(level);
                        iter->set_next(level, nextnext);
                    }
                }
            }
            return ret;
        }

        void print()
        {
            SkipListNode *iter = (SkipListNode *)head->get_next(0);
            while (iter)
            {
                cout << iter->get_view().stringify() << endl;
                iter = (SkipListNode *)iter->get_next(0);
            }
        }

        const char *get_key()
        {
            return key.c_str();
        }

        size_t size()
        {
            return length;
        }

    protected:
        virtual int compare(const DomView& a, const DomView& b) = 0;

    private:
        int compare_with_id(const DomView& lh, const DomView& rh)
        {
            int key_result = compare(lh.get_member(key), rh);
            if (key_result == 0)
            {
                uint64_t lhid = lh.get_member("_id").get_uint();
                uint64_t rhid = rh.get_member("_id").get_uint();

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

        SkipListNode *head;

        constexpr static double SLIST_P = 0.2;
        constexpr static int FASTRAND_MAX = 32767;
        constexpr static double P_RAND = FASTRAND_MAX * SLIST_P;

        int g_seed;

        size_t length;
        std::string key;
    };

    class GenericIndex : public Index
    {
    public:
        GenericIndex(const std::string& key) : Index(key)
        {
        }

    protected:
        int compare(const DomView& b, const DomView& a)
        {
            if (b.get_type() == DOM_NODE_TYPE_NULL || a.get_type() == DOM_NODE_TYPE_NULL)
            {
                return -1;
            }

            DOM_NODE_TYPE a_type = a.get_type();
            DOM_NODE_TYPE b_type = b.get_type();

            if (a_type != b_type)
            {
                if (b_type == DOM_NODE_TYPE_DOUBLE)
                {
                    if (a_type == DOM_NODE_TYPE_INT)
                    {
                        return (double)a.get_int() - b.get_double();
                    }
                    else if (a_type == DOM_NODE_TYPE_UINT)
                    {
                        return (double)a.get_uint() - b.get_double();
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
                    return a.get_int() - b.get_int();
                }
                break;
                case DOM_NODE_TYPE_UINT:
                {
                    return (double)a.get_uint() - (double)b.get_uint();
                }
                case DOM_NODE_TYPE_DOUBLE:
                {
                    return a.get_double() - b.get_double();
                }
                case DOM_NODE_TYPE_STRING:
                {
                    int result = b.get_string().compare(a.get_string());
                    return result;
                }
                break;
                default:
                    return 0;
                }
            }
        }
    };

    class UintIndex : public Index
    {
    public:
        UintIndex(const std::string& key) : Index(key)
        {
        }

    protected:
        int compare(const DomView& b, const DomView& a)
        {
            return a.get_uint() - b.get_uint();
        }
    };
}
#endif
