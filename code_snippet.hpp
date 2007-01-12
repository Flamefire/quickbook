/*=============================================================================
    Copyright (c) 2006 Joel de Guzman
    http://spirit.sourceforge.net/

    Use, modification and distribution is subject to the Boost Software
    License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
    http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#if !defined(BOOST_SPIRIT_QUICKBOOK_CODE_SNIPPET_HPP)
#define BOOST_SPIRIT_QUICKBOOK_CODE_SNIPPET_HPP

#include <boost/spirit/core.hpp>
#include <boost/spirit/actor.hpp>
#include "./detail/template_stack.hpp"

namespace quickbook
{
    struct cpp_code_snippet_grammar
        : grammar<cpp_code_snippet_grammar>
    {
        cpp_code_snippet_grammar(std::vector<template_symbol>& storage)
            : storage(storage) {}

        template <typename Scanner>
        struct definition
        {
            definition(cpp_code_snippet_grammar const& self)
            {
                typedef cpp_code_snippet_grammar self_type;
                start_ = 
                    +(
                            snippet                 [boost::bind(&self_type::compile, &self, _1, _2)]
                        |   anychar_p
                    )
                    ;

                identifier =
                    (alpha_p | '_') >> *(alnum_p | '_')
                    ;

                snippet = 
                    "//[" >> *space_p
                    >> identifier                   [assign_a(self.id)]
                    >> (*(code_elements - "//]"))
                    >> "//]"
                    ;
                
                code_elements =
                        escaped_comment         
                    |   (anychar_p - "//]")         [boost::bind(&self_type::pass_thru, &self, _1, _2)]
                    ;
                
                escaped_comment =
                        *space_p >> "//`" >> *space_p
                        >> (*(anychar_p - eol_p))   [boost::bind(&self_type::escaped_comment, &self, _1, _2)]
                        >> eol_p
                    |   *space_p >> "/*`" >> *space_p
                        >> (*(anychar_p - "*/"))    [boost::bind(&self_type::escaped_comment, &self, _1, _2)]
                        >> "*/"
                    ;
            }

            rule<Scanner> start_, snippet, identifier, code_elements, escaped_comment;

            rule<Scanner> const&
            start() const { return start_; }
        };

        void pass_thru(iterator first, iterator last) const;
        void escaped_comment(iterator first, iterator last) const;
        void compile(iterator first, iterator last) const;

        mutable std::string code;
        mutable std::string snippet;
        mutable std::string id;
        std::vector<template_symbol>& storage;
    };
}

#endif // BOOST_SPIRIT_QUICKBOOK_CODE_SNIPPET_HPP
