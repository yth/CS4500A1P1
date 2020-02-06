//lang::Cpp


#pragma once


#include <cassert>
#include <iostream>


#include "object.h"
#include "field_array.h"
#include "types.h"
#include "types_array.h"


/**
 * Attemps to parse the given value to an unsigned integer.
 * @param c the value to parse
 * @return the parsed unsigned integer or SIZE_MAX if it was unable to parse it.
 */
inline size_t parse_uint(char *c) {
    char *endptr = nullptr;
    size_t length = strlen(c);
    size_t uint = strtoul(c, &endptr, 10);
    if (length != (size_t) (endptr - c)) {
        uint = SIZE_MAX;
    }
    return uint;
}


/**
 * Returns the index of the first non space character from a given 'start'
 * to an given 'end' in a 'file'.
 * @param file the file on which to check for spaces.
 * @param start the starting byte.
 * @param end the ending byte.
 * @return the byte at which the first non space character is found.
 */
inline size_t triml(char *file, size_t start, size_t end) {
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
 * @param file the file on which to check for spaces.
 * @param start the byte to end to.
 * @param end the byte to start from.
 * @return the byte at which the first non space character is found.
 */
inline size_t trimr(char *file, size_t start, size_t end) {
    // make sure we don't read before the start of the file.
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
 * Parses the type of the field delimited by its delimiters '<' and '>' pointed
 * by start and end respectively in the given file.
 * @param file the file we are working on.
 * @param start the byte position of '<'
 * @param end the byte position of '>'
 * @return returns the type of this field.
 */
inline Types parse_field_type(char* file, size_t start, size_t end) {
    Types result = Types::BOOL;

    // removes empty spaces in front and back
    start = triml(file, start, end);
    end = trimr(file, start, end);

    // If start is <= end we know that it is not an empty field
    if (start <= end) {
        if (file[start] == '\"') {
            // must be a string
            result = Types::STRING;
        } else if (end == start && (file[start] == '0' || file[start] == '1')) {
            result = Types::BOOL;
        } else {
            char *endptr;
            strtoll(&file[start], &endptr, 10);
            // Make sure we get a number back and that it consume all of the relevant field characters.
            if ((size_t) (endptr - &file[start]) == end - start + 1) {
                return Types::INT;
            }
            strtold(&file[start], &endptr);
            if ((size_t) (endptr - &file[start]) == end - start + 1) {
                return Types::FLOAT;
            }
            for (size_t i = start; i <= end; ++i) {
                if (isspace(file[i])) {
                    return result;
                }
            }
            result = Types::STRING;
        }
    }
    return result;
}


/**
 * Parses the schema of a row (line) starting at start in the file.
 * Stores the parsed schema into a given array of types.
 *
 * @param file the file we are working on.
 * @param start the starting byte to start parsing from.
 * @param row_schema the array of types that we use to store the schema and we recycle afterwards.
 * MUTATION: the start argument is mutated so that it always holds the byte
 *           where this row parsing ended. The row_schema argument is mutated to store
 *           the types of this row.
 */
inline void parse_row_schema(char* file, size_t* start, TypesArray* row_schema) {
    while (file[*start] != '\n' && file[*start] != EOF && file[*start] != '\0') {
        // find the starting byte of the field
        if (file[*start] == '<') {
            size_t field_end = *start + 1;
            // find the ending byte of the field
            while (file[field_end] != '>') {
                assert(file[field_end] != '\n');
                ++field_end;
            }
            Types field_type = parse_field_type(file, *start, field_end);
            row_schema->pushBack(field_type);
            *start = field_end + 1;
        } else {
            *start += 1;
        }
    }
}


/**
 * Parses the schema of the given file.
 * @param file the file we are working on.
 * @return the schema as an array of types.
 * INVARIANT: the result TypesArray always stores the correct schema at the beginning of
 * every iteration. Thus, only in the case of finding a longer row or where
 * less restrictive fields are found, is this result array ever changed.

 * CREDIT: to SnowyJoe team for the schema parsing algorithm.
 */
inline TypesArray *parse_schema(char *file) {
    // stores the schema state so far
    TypesArray* result = new TypesArray();
    size_t start = 0;
    // types array to use to store the row schema
    // it is used for all the necessary iterations and only deleted
    // at completion
    TypesArray* curr = new TypesArray();
    for (size_t i = 0; i < 500; ++i) {
        if (file[start] == EOF || file[start] == '\0') {
            break;
        }
        // pass the curr type array down so to store the row schema
        parse_row_schema(file, &start, curr);
        size_t result_len = result->len();
		size_t curr_len = curr->len();
        for (size_t j = 0; j < curr_len; ++j) {
            Types curr_type = curr->get(j);
            // if the current line has more element than the one we accumulated so far,
            // it means this is the longest row and we add those types into our result
            if (j >= result_len) {
                result->pushBack(curr_type);
                // change the type of a column of the so far array only if the
                // curr type is less restrictive (meaning it has higher value
                // in the enum)
            } else if (result->get(j) < curr_type) {
                result->set(j, curr_type);
            }
        }
        // reset the curr array so it can be reused
        curr->clear();
        // move cursor to next line
        start += 1;
    }
    // we can finally delete curr
    delete curr;

    return result;
}


/**
 * Parses through a row (line) of a file and builds a field array
 * storing all the starting and ending bytes delimited by '<' and '>' of the field in the row.
 *
 * @param file the file we are working on.
 * @param start the starting byte to read from.
 * @param row the field array to fill with the starting and ending bytes found.
 * MUTATION: the start argument is mutated so that it always holds the byte
 *           where this row parsing ended. The row argument is mutated to store
 *           the the starting and ending bytes found.
 */
inline void parse_row_fields(char* file, size_t* start, FieldArray* row) {
    while (file[*start] != '\n' && file[*start] != EOF && file[*start] != '\0') {
        // find the starting byte of the field
        if (file[*start] == '<') {
            size_t field_end = *start + 1;
            // find the ending byte of the field
            while (file[field_end] != '>') {
                assert(file[field_end] != '\n');
                ++field_end;
            }
            row->pushBack(*start, field_end);
            *start = field_end + 1;
        } else {
            *start += 1;
        }
    }
}


/**
 * Determines if the types row are valid according to a schema passed in.
 * A row is valid if and only if both the number of fields in the row and their types
 * respectively matches those of the schema.
 *
 * @param row_types the types array of the row to check.
 * @param schema the schema of .sor file.
 * @return whether it is valid or not.
 */
inline bool is_valid_row(TypesArray* row_types, TypesArray* schema) {
    size_t max_fields = schema->len();
    if (row_types->len() == max_fields) {
        for (size_t i = 0; i < max_fields; ++i) {
            if (row_types->get(i) > schema->get(i)) {
                return false;
            }
        }
        return true;
    }
    return false;
}


/**
 * Creates a columnar representation of a portion of a file delimited by the given
 * start and end according to a schema.
 *
 * NOTE: the function assumes that start always points to the beginning of a line
 *       and the end to the end of a line.
 *
 * @param file the file we are working on.
 * @param start the starting byte to read from.
 * @param end the ending byte to read to.
 * @param schema the schema.
 * @return an array of field arrays which are the columnar representation of this portion of file
 */
inline FieldArray** make_columnar(char* file, size_t start, size_t end, TypesArray* schema) {
    size_t max_fields = schema->len();
    FieldArray** columnar = new FieldArray*[max_fields];
    for (size_t i = 0; i < max_fields; ++i) {
        columnar[i] = new FieldArray();
        columnar[i]->set_type(schema->get(i));
    }
    while (start < end) {
        // initialized the arrays that are going to be recycled every iteration
        TypesArray* row_types = new TypesArray();
        FieldArray* row_fields = new FieldArray();
        while (file[start] != '\n' && file[start] != EOF && file[start] != '\0') {
            size_t row_start = start;
            // first parse the schema and determine if the row is valid
            parse_row_schema(file, &row_start, row_types);

            if (is_valid_row(row_types, schema)) {
                // if valid we parse the row fields and add it to our columnar
                parse_row_fields(file, &start, row_fields);
                for (size_t i = 0; i < max_fields; ++i) {
                    columnar[i]->pushBack(row_fields->get_start(i), row_fields->get_end(i));
                }
                // recycle the row_fields array
                row_fields->clear();
            }
            // recycle the row_types array
            row_types->clear();
            // move cursor to point to next line
            start = row_start + 1;
        }
        // we are done so we delete them
        delete row_types;
        delete row_fields;
    }
    return columnar;
}


/**
 * Print the given type to std out.
 * @param t the type to print.
 */
inline void print_type(Types t) {
    switch (t) {
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
        default:
            std::cout << "Bad type encountered, this value should not be stored";
            exit(1);
    }
}


/**
 * Print to std out a field of type t in a given file that has its delimiters '<' and '>' pointed to
 * by start and end.
 *
 * @param file the file to read from.
 * @param s the starting byte to read from.
 * @param e the ending byte to read to.
 * @param t the type of the field
 */
inline void print_field(char *file, int start, int end, Types t) {

    // remove empty spaces in front and back
    size_t new_start = triml(file, start, end);
    size_t new_end = trimr(file, start, end);

    // check if it is an empty field
    if (new_start < new_end) {
        switch (t) {
            case Types::BOOL:
                std::cout << file[new_start] << "\n";
                break;
            case Types::INT:
                std::cout << strtoll(&file[new_start], nullptr, 10) << "\n";
                break;
            case Types::FLOAT:
                std::cout << strtold(&file[new_start], nullptr) << "\n";
                break;
            case Types::STRING:
                if (file[new_start] != '\"' && file[new_end] != '\"') {
                    std::cout << '"';
                }
                for (size_t i = new_start; i <= new_end; ++i) {
                    std::cout << file[i];
                }
                if (file[new_start] != '\"' && file[new_end] != '\"') {
                    std::cout << '"';
                }
                std::cout << "\n";
                break;
            default:
                std::cout << "Bad type encountered, this value should not be stored";
                exit(1);
        }
    } else {
        std::cout << 1 << "\n";
    }
}
