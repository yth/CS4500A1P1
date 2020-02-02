//lang::Cpp


/**
 * Row: Represents a row of data
 *
 * Author: cao.yuan1@husky.neu.edu & zhan.d@husky.neu.edu
 */


#pragma once


#include<cstdlib>
#include<cassert>

#include "fieldlist.h"

/**
 * A class to store the fields of a .sor file
 * in a row representation. Rows can contain Fields
 * that have their type unspecified (Types::Field).
 * Each row stores its starting and ending byte position
 * in the file.
 */
class Row : public FieldList {
public:
    size_t start; // starting bytes
    size_t end; // ending bytes

    /**
     * Constructs a new row with a starting and ending
     * position.
     */
    Row(size_t start, size_t end) {
        this->start = start;
        this->end = end;
    }

    /**
     * Gets the starting byte of this row.
     */
    size_t get_start() {
        return this->start;
    }

    /**
     * Gets the ending byte of this row.
     */
    size_t get_end() {
        return this->end;
    }

    /**
     * Sets the ending byte of this row.
     */
    void set_end(size_t i) {
        this->end = i;
    }
};
