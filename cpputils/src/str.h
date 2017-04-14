﻿#pragma once

#include <string>
#include <sstream>
#include <functional>
#include <vector>

#include "simple_types.h"

namespace rc {

    class str {
    public:
        str();
        str(const str &str);

        /**
         * \brief Create a str object from a std::string (must in UTF-8).
         * \param str a std::string object
         */
        str(const std::string &str);

        /**
         * \brief Create a str object from a C string (must in UTF-8).
         * \param c_str a const char pointer (C string)
         */
        str(const char *c_str);

        /**
         * \brief Create a str object from a std::wstring.
         * \param wstr a std::wstring object
         */
        str(const std::wstring &wstr);

        /**
        * \brief Create a str object from a C widechar string.
        * \param c_wstr a const wchar_t pointer (C widechar string)
        */
        str(const wchar_t *c_wstr);

        /**
         * \brief Convert nullptr to str.
         * \param null nullptr
         */
        explicit str(nullptr_t null);

        /**
         * \brief Convert any variable that can be written to a std::stringstream to a str object.
         * \tparam T type of the variable to convert
         * \param t variable to convert
         */
        template <typename T>
        explicit str(T t);

        const char *data() const;
        const char *c_str() const;
        std::string to_bytes() const;
        std::wstring to_wstring() const;

        /**
         * \brief Assign operator.
         * \param other the other str
         * \return reference of this str
         */
        str &operator=(const str &other);

        /**
         * \brief Write the string to the given ostream.
         * 
         * NOTE: If it's on WIN32, this will convert the UTF-8 string to ANSI codepage first.
         * Because of the converting, be careful when using this operator with a std::stringstream.
         * 
         * \param os output stream
         * \param obj str object
         * \return the ostream passed in
         */
        friend std::ostream &operator<<(std::ostream &os, const str &obj);

        friend bool operator<(const str &lhs, const str &rhs) { return lhs.inner_str_ < rhs.inner_str_; }
        friend bool operator>(const str &lhs, const str &rhs) { return rhs < lhs; }
        friend bool operator<=(const str &lhs, const str &rhs) { return !(lhs > rhs); }
        friend bool operator>=(const str &lhs, const str &rhs) { return !(lhs < rhs); }
        friend bool operator==(const str &lhs, const str &rhs) { return lhs.inner_str_ == rhs.inner_str_; }
        friend bool operator!=(const str &lhs, const str &rhs) { return !(lhs == rhs); }

        friend str operator+(const str &lhs, const str &rhs);
        friend str &operator+=(str &lhs, const str &rhs);
        friend str operator*(const str &obj, int count);
        friend str operator*(int count, const str &obj);
        friend str &operator*=(str &obj, int count);

        explicit operator bool() const;
        explicit operator int() const;
        explicit operator long long() const;
        explicit operator float() const;
        explicit operator double() const;

        /**
         * \brief Iterator class of the str class.
         * 
         * NOTE: This iterator is a bidirectional iterator, but be careful when using the -- operator.
         * 
         *    | 0 | 1 | 2 | 3 |
         *    ↑               ↑
         * begin()          end()
         * 
         * The * operator tries to get the unicode character at the current position of the iterator.
         * It will scan from the position to the end of the character,
         * which means if the iterator object is currently point to the "end" of the str,
         * a ValueError will be thrown.
         * 
         * In addition, when the iterator reaches the beginning or the end of the str, it will stop at there,
         * which means:
         * 
         *     auto it = s.begin();
         *     --it;
         *     assert(it == s.begin());
         */
        class iterator {
        public:
            iterator() {}
            explicit iterator(std::string::const_iterator begin,
                              std::string::const_iterator end,
                              std::string::const_iterator it);

            iterator &operator++();
            iterator operator++(int);
            iterator &operator--();
            iterator operator--(int);

            bool operator==(iterator other) const { return this->it_ == other.it_; }
            bool operator!=(iterator other) const { return !(*this == other); }

            str operator*() const;

            using difference_type = size_t;
            using value_type = str;
            using pointer = str *;
            using reference = str &;
            using iterator_category = std::bidirectional_iterator_tag;

        private:
            std::string::const_iterator begin_;
            std::string::const_iterator end_;
            std::string::const_iterator it_;
        };

        iterator begin() const {
            return iterator(this->inner_str_.begin(), this->inner_str_.end(), this->inner_str_.begin());
        }

        iterator end() const {
            return iterator(this->inner_str_.begin(), this->inner_str_.end(), this->inner_str_.end());
        }

        std::string::const_iterator c_begin() const { return this->inner_str_.begin(); }
        std::string::const_iterator c_end() const { return this->inner_str_.end(); }

        explicit str(iterator begin, iterator end);

        /**
         * \brief Calculate the unicode length of the str.
         * 
         * NOTE: Complexity of this method is O(n), so use it carefully.
         * 
         * \return length of the str
         */
        size_t length() const;
        size_t size() const; // same as length()

        /**
         * \brief Return the real byte size of the inner UTF-8 string.
         * \return byte size of the str
         */
        size_t byte_size() const;

        str operator[](index_t idx) const;
        str operator[](slice slc) const;

        str upper() const;
        str lower() const;
        bool isupper() const;
        bool islower() const;

        str strip() const { return this->strip_(true, true); }
        str strip(str chars) const { return this->strip_(chars, true, true); }
        str strip(const char *c_chars) const { return this->strip_(c_chars, true, true); }
        str strip(std::function<bool(str)> predication) const { return this->strip_(predication, true, true); }
        str lstrip() const { return this->strip_(true, false); }
        str lstrip(str chars) const { return this->strip_(chars, true, false); }
        str lstrip(const char *c_chars) const { return this->strip_(c_chars, true, false); }
        str lstrip(std::function<bool(str)> predication) const { return this->strip_(predication, true, false); }
        str rstrip() const { return this->strip_(false, true); }
        str rstrip(str chars) const { return this->strip_(chars, false, true); }
        str rstrip(const char *c_chars) const { return this->strip_(c_chars, false, true); }
        str rstrip(std::function<bool(str)> predication) const { return this->strip_(predication, false, true); }

        //        std::vector<str> split(str sep = str(" \t\r\n"), int maxsplit = -1);
        std::vector<str> split(std::function<bool(str)> predication, int maxsplit = -1) const;

    private:
        std::string inner_str_;

        str strip_(std::function<bool(str)> predication, bool left, bool right) const;
        str strip_(const char *c_chars, bool left, bool right) const;
        str strip_(str chars, bool left, bool right) const;
        str strip_(bool left, bool right) const;
    };

    template <typename T>
    str::str(T t) {
        std::stringstream ss;
        ss << t;
        if (typeid(t).name() == str("bool")) {
            this->inner_str_ = ss.str() == "1" ? "true" : "false";
        } else {
            this->inner_str_ = ss.str();
        }
    }
}