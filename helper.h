//lang::C++


#pragma once


#include <cassert>
#include <iostream>


#include "object.h"
#include "field.h"
#include "fieldlist.h"
#include "row.h"
#include "types.h"


Field* parse_field(char* start, size_t i);
void print_type(Types t);


/**
 * Attemps to parse the given value to an unsigned integer.
 */
inline size_t parse_uint(char* c) {
    char* endptr = nullptr;
    size_t length = strlen(c);
    size_t uint = strtoul(c, &endptr, 10);
    assert(length == (size_t) (endptr - c));
    return uint;
}

/**
 * Constructs a row by parsing through the 'file' from a
 * given starting point 'start'.
 */
inline Row* parse_row(char* file, size_t start) {
    Row* r = new Row(start, start);
    size_t i = start;
    // need to check for '\0' because the page we opened we mmap
    // can be bigger than the size of the file, so unless the
    // file is exactly as big as the page, we will encounter '\0'
    // instead of EOF
    while (file[i] != '\n' && file[i] != EOF && file[i] != '\0') {
        if (file[i] == '<') {
            Field* f = parse_field(file, i);
            r->add(f);
            i = f->get_end();
            delete f;
        }
        ++i;
    }
    // If this is the last row of the file, we make sure
    // to make the row point to the character before the 'EOF'
    if (file[i] == EOF) {
        --i;
    }
    r->set_end(i);
    return r;
}

/**
 * Constructs a Field object by parsing through the 'file'
 * from the given 'start'.
 * Callers to this function have to make sure that
 * the value at file[start] == '<' since this is where
 * a field should start.
 */
inline Field* parse_field(char* file, size_t start) {
    assert(file[start] == '<');
    size_t i = start + 1;
    while (file[i] != '>') {
        assert(file[i] != '\0' && file[i] != '\n');
        ++i;
    }
    return new Field(start, i);
}

/**
 * Returns the index of the first non space character from a given 'start'
 * to an given 'end' in a 'file'.
 */
inline size_t triml(size_t start, size_t end, char* file) {
    size_t i;
    for (i = start + 1; i < end; i++) {
        if (!isspace(file[i])) {
            break;
        }
    }
    return i;
}

/**
 * Returns the index of the first non space character from a given
 * 'end' to a given 'start' in a 'file'
 */
inline size_t trimr(size_t start, size_t end, char* file) {
    assert(end != 0);
    size_t i;
    for (i = end - 1; i > start; i--) {
        if (!isspace(file[i])) {
            break;
        }
    }
    return i;
}

/**
 * Determines the type of a field in a most restricted to
 * least restricted order. The 'start' and 'end' corresponds
 * to the location (in bytes) of the value of field in a
 * given 'file'.
 * If the field is invalid (eg. <hi_there>), the FIELD type is
 * returned.
 * Note: this function is always called on non empty fields.
 */
inline Types parse_field_type(size_t start, size_t end, char* file) {
    Types result = Types::FIELD;

    if (file[start] == '\"') {
        result = Types::STRING;
    } else if (end - start + 1 == 1) {
        if (file[start] == '0' || file[start] == '1') {
            result = Types::BOOL;
        } else if (isdigit(file[start])) {
            result = Types::INT;
        } else {
            result = Types::STRING;
        }
    } else {
        char* endptr;
        strtoll(&file[start], &endptr, 10);
        // Make sure we get a number back and that it consume all of the relevant field characters.
        if ((size_t)(endptr - &file[start]) == end - start + 1) {
            return Types::INT;
        }
        strtold(&file[start], &endptr);
        if ((size_t)(endptr - &file[start]) == end - start + 1) {
            return Types::FLOAT;
        }
        for (size_t i = start; i <= end; ++i) {
            if (isspace(file[i])) {
                return result;
            }
        }
        result = Types::STRING;
    }
    return result;
}

/**
 * Determines the returns the types of 'col_index' column in a given array of
 * rows of length 'length' in a 'file'.
 * It does so checking which is the least constrained type that can fit
 * all the values in the column.
 *
 */
inline Types parse_col_type(Row** rows, char* file, size_t col_index, size_t length) {
    Types final_type = Types::BOOL;
    for(size_t i = 0; i < length; ++i) {
        Types current_type;
        Field* f = rows[i]->get_field(col_index);
        // check if the field at this index exists since
        // we can have rows with less fields
        if (!f) {
            continue;
        }
        size_t start = f->get_start();
        size_t end = f->get_end();

        start = triml(start, end, file);
        // This means we have an empty field so it doesn't help
        // to determine the type and so we ignore it
        if (start >= end) {
            continue;
        }

        end = trimr(start, end, file);
        // Make sure to set the 'start' and 'end' of the field
        // to the new values
        f->set_start(start);
        f->set_end(end);

        current_type = parse_field_type(start, end, file);
        if (current_type > final_type) {
            final_type = current_type;
            // Stop early if the least constrained type is found
            if (final_type == Types::STRING) {
                break;
            }
        }
    }

    return final_type;
}

/**
 * Returns an array of types representing the schema for
 * the 'rows' of length 'length' in this .sor 'file'. The width represents
 * how many columns this schema can at most have.
 */
inline Types* parse_schema(Row** rows, char* file, size_t width, size_t length) {
    Types* column_types = new Types[width];
    for (size_t i = 0; i < width; ++i) {
        column_types[i] = parse_col_type(rows, file, i, length);
    }
    return column_types;
}

/**
 * Prints the type of this 't' to std out.
 */
inline void print_type(Types t) {
    switch(t) {
        case Types::BOOL:
            std::cout << "BOOL" << "\n";
            break;
        case Types::INT:
            std::cout << "INT" << "\n";
            break;
        case Types::FLOAT:
            std::cout << "FLOAT" << "\n";
            break;
        case Types::STRING:
            std::cout << "STRING" << "\n";
            break;
        case Types::EMPTY:
            std::cout << "EMPTY" << "\n";
            break;
        default:
            std::cout << "Bad type encountered, this value should not be stored";
            exit(1);
    }
}

inline void print_field(char* file, Field* f) {
    size_t start = f->get_start();
    size_t end = f->get_end();
    switch(f->get_type()) {
        case Types::BOOL:
            std::cout << file[f->get_start()] << "\n";
            break;
        case Types::INT:
            std::cout << strtoll(&file[f->get_start()], nullptr, 10) << "\n";
            break;
        case Types::FLOAT:
            std::cout << strtold(&file[f->get_start()], nullptr) << "\n";
            break;
        case Types::STRING:
            if (file[start] != '\"' && file[end] != '\"') {
                std::cout << '"';
            }
            for (size_t i = start; i <= end; ++i) {
                std::cout << file[i];
            }
            if (file[start] != '\"' && file[end] != '\"') {
                std::cout << '"';
            }
            std::cout << "\n";
            break;
        default:
            std::cout << "Bad type encountered, this value should not be stored";
            exit(1);
    }
}
