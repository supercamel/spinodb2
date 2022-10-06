//  Copyright 2022 Sam Cowen <samuel.cowen@camelsoftware.com>
//
//  Permission is hereby granted, free of charge, to any person obtaining a
//  copy of this software and associated documentation files (the "Software"),
//  to deal in the Software without restriction, including without limitation
//  the rights to use, copy, modify, merge, publish, distribute, sublicense,
//  and/or sell copies of the Software, and to permit persons to whom the
//  Software is furnished to do so, subject to the following conditions:
//
//  The above copyright notice and this permission notice shall be included in
//  all copies or substantial portions of the Software.
//
//  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
//  OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
//  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
//  DEALINGS IN THE SOFTWARE.

#include "query_executor.h"
#include "query_parser.h"

#include <regex>

#include <iostream>
using namespace std;

namespace Spino
{
    QueryExecutor::QueryExecutor(DomAllocator& alloc) : alloc(alloc)
    {
        stack.reserve(10);
        tokens = nullptr;
    }

    void QueryExecutor::set_instructions(const std::vector<Token>* i)
    {
        tokens = i;
    }

    bool QueryExecutor::execute_query(DomNode *document)
    {
        if (tokens->size() == 0)
        {
            return true;
        }


/*
        while(stack.size())
        {
            DomNode n = stack.back();
            stack.pop_back();
            n.destroy();
        }
        */

        for (auto &tok : *tokens)
        {
            switch (tok.token)
            {
            case TOK_STRING_LITERAL:
            {
                DomNode d(alloc);
                d.set_string(tok.raw, tok.len);
                stack.push_back(d);
            }
            break;
            case TOK_NUMERIC_LITERAL:
            {
                DomNode d(alloc);
                d.set_double(tok.value);
                stack.push_back(d);
            }
            break;
            case TOK_BOOL_LITERAL:
            {
                DomNode d(alloc);
                d.set_bool(tok.raw[0] == 't');
                stack.push_back(d);
            }
            break;
            case TOK_FIELD_NAME:
            {
                DomNode* tmp = document->get_member(tok.raw, tok.len);
                if (tmp)
                {
                    DomNode d(alloc);
                    d.copy(tmp);
                    stack.push_back(d);
                }
                else
                {
                    DomNode d(alloc);
                    d.set_invalid();
                    stack.push_back(d);
                }
            }
            break;
            case TOK_EQUAL:
            {
                DomNode b = stack.back();
                stack.pop_back();
                DomNode a = stack.back();
                stack.pop_back();

                DomNode result(alloc);
                if (a == b)
                {
                    result.set_bool(true);
                }
                else
                {
                    result.set_bool(false);
                }
                a.destroy();
                b.destroy();
                stack.push_back(result);
            }
            break;
            case TOK_NE:
            {
                DomNode b = stack.back();
                stack.pop_back();
                DomNode a = stack.back();
                stack.pop_back();

                DomNode result(alloc);
                if (a == b)
                {
                    result.set_bool(false);
                }
                else
                {
                    result.set_bool(true);
                }
                a.destroy();
                b.destroy();
                stack.push_back(result);
            }
            break;
            case TOK_RANGE:
            {
                DomNode max = stack.back();
                stack.pop_back();
                DomNode min = stack.back();
                stack.pop_back();

                DomNode doc = stack.back();
                stack.pop_back();

                DomNode result(alloc);
                if (!doc.is_numeric())
                {
                    result.set_bool(false);
                }
                else
                {

                    if ((doc >= min) && (doc <= max))
                    {
                        result.set_bool(true);
                    }
                    else
                    {
                        result.set_bool(false);
                    }
                }
                max.destroy();
                min.destroy();
                doc.destroy();
                stack.push_back(result);
            }
            break;
            case TOK_GREATER_THAN:
            {
                DomNode val = stack.back();
                stack.pop_back();

                DomNode doc = stack.back();
                stack.pop_back();

                DomNode result(alloc);
                if (!doc.is_numeric())
                {
                    result.set_bool(false);
                }
                else
                {
                    if (doc > val)
                    {
                        result.set_bool(true);
                    }
                    else
                    {
                        result.set_bool(false);
                    }
                }
                val.destroy();
                doc.destroy();
                stack.push_back(result);
            }
            break;
            case TOK_LESS_THAN:
            {
                DomNode val = stack.back();
                stack.pop_back();

                DomNode doc = stack.back();
                stack.pop_back();

                DomNode result(alloc);
                if (!doc.is_numeric())
                {
                    result.set_bool(false);
                }
                else
                {

                    if (doc < val)
                    {
                        result.set_bool(true);
                    }
                    else
                    {
                        result.set_bool(false);
                    }
                }
                val.destroy();
                doc.destroy();
                stack.push_back(result);
            }
            break;
            case TOK_EXISTS:
            {
                DomNode exists = stack.back();
                stack.pop_back();
                DomNode val = stack.back();
                stack.pop_back();

                DomNode result(alloc);
                if (val.get_type() == DOM_NODE_TYPE_INVALID)
                {
                    result.set_bool(!exists.get_bool());
                }
                else
                {
                    result.set_bool(exists.get_bool());
                }

                val.destroy();
                stack.push_back(result);
            }
            break;
            case TOK_AND:
            {
                int count = stack.back().get_double();
                stack.pop_back();

                DomNode result(alloc);
                result.set_bool(true);
                while (--count)
                {
                    auto v = stack.back();
                    stack.pop_back();
                    if (v.get_bool() == false)
                    {
                        result.set_bool(false);
                    }
                    v.destroy();
                }
                stack.push_back(result);
            }
            break;
            case TOK_OR:
            {
                int count = stack.back().get_double();
                stack.pop_back();

                DomNode result(alloc);
                result.set_bool(false);
                while (--count)
                {
                    auto v = stack.back();
                    stack.pop_back();
                    if (v.get_bool() == true)
                    {
                        result.set_bool(true);
                    }
                    v.destroy();
                }
                stack.push_back(result);
            }
            break;
            case TOK_TYPE:
            {
                DomNode type_str = stack.back();
                stack.pop_back();

                DomNode field = stack.back();
                stack.pop_back();

                DomNode result(alloc);
                result.set_bool(false);
                if (strcmp(type_str.get_string(), "bool") == 0)
                {
                    if (field.get_type() == DOM_NODE_TYPE_BOOL)
                    {
                        result.set_bool(true);
                    }
                }
                else if (strcmp(type_str.get_string(), "number") == 0)
                {
                    if (field.is_numeric() && (field.get_type() != DOM_NODE_TYPE_BOOL))
                    {
                        result.set_bool(true);
                    }
                }
                else if (strcmp(type_str.get_string(), "string") == 0)
                {
                    if (field.get_type() == DOM_NODE_TYPE_STRING)
                    {
                        result.set_bool(true);
                    }
                }
                else if (strcmp(type_str.get_string(), "array") == 0)
                {
                    if (field.get_type() == DOM_NODE_TYPE_ARRAY)
                    {
                        result.set_bool(true);
                    }
                }
                else if (strcmp(type_str.get_string(), "object") == 0)
                {
                    if (field.get_type() == DOM_NODE_TYPE_OBJECT)
                    {
                        result.set_bool(true);
                    }
                }
                else if (strcmp(type_str.get_string(), "null") == 0)
                {
                    if (field.get_type() == DOM_NODE_TYPE_NULL)
                    {
                        result.set_bool(true);
                    }
                }
                field.destroy();
                stack.push_back(result);
            }
            break;
            case TOK_STARTS_WITH:
            {
                DomNode val = stack.back();
                stack.pop_back();

                DomNode field = stack.back();
                stack.pop_back();

                DomNode result(alloc);
                if (field.get_type() == DOM_NODE_TYPE_STRING)
                {
                    if (strncmp(field.get_string(), val.get_string(), strlen(val.get_string())) == 0)
                    {
                        result.set_bool(true);
                    }
                    else
                    {
                        result.set_bool(false);
                    }
                }
                else
                {
                    result.set_bool(false);
                }

                field.destroy();
                val.destroy();
                stack.push_back(result);
            }
            break;
            case TOK_IN:
            {
                size_t length = stack.back().get_double();
                stack.pop_back();

                DomNode field = stack[stack.size() - (length + 1)];

                DomNode result(alloc);
                result.set_bool(false);
                while (--length)
                {
                    DomNode val = stack.back();
                    stack.pop_back();

                    if (field == val)
                    {
                        result.set_bool(true);
                    }
                    val.destroy();
                }

                stack.pop_back();
                field.destroy();

                stack.push_back(result);
            }
            break;
            case TOK_NIN:
            {
                size_t length = stack.back().get_double();
                stack.pop_back();

                DomNode field = stack[stack.size() - (length + 1)];

                DomNode result(alloc);
                result.set_bool(true);
                while (--length)
                {
                    DomNode val = stack.back();
                    stack.pop_back();

                    if (field == val)
                    {
                        result.set_bool(false);
                    }
                    val.destroy();
                }

                stack.pop_back();
                field.destroy();

                stack.push_back(result);
            }
            break;
            case TOK_NOT:
            {
                DomNode val = stack.back();
                stack.pop_back();

                DomNode result(alloc);
                result.set_bool(!val.get_bool());
                val.destroy();
                stack.push_back(result);
            }
            break;
            case TOK_REGEX:
            {
                DomNode field = stack.back();
                stack.pop_back();
                DomNode result(alloc);
                if (field.get_type() == DOM_NODE_TYPE_STRING)
                {
                    std::smatch base_match;
                    const std::regex reg = *tok.regex;
                    std::string str = field.get_string();
                    result.set_bool(std::regex_match(str, base_match, reg));
                }
                else
                {
                    result.set_bool(false);
                }

                field.destroy();
                stack.push_back(result);
            }
            break;
            case TOK_RH_BRACE:
            case TOK_LH_BRACE:
            case TOK_LH_BRACKET:
            case TOK_RH_BRACKET:
            case TOK_COLON:
            case TOK_PERIOD:
            case TOK_COMMA:
            case TOK_EOF:
            break;
            }
        }

        return stack.back().get_bool();
    }

}
