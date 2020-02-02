//lang::Cpp


#pragma once


#include <cstring>
#include <cstdlib>
#include <iostream>


#include "object.h"
#include "types.h"


/**
 * Field: represents the superclass of field objects.
 * Each field stores the region on the file where it is located.
 * This is achieved by storing the starting and ending byte
 * corresponding to the position of '<' and '>'.
 */
class Field : public Object {

public:
    size_t start; // start byte
    size_t end; // end byte
    Types type;

    /**
     * Constructs a Field object given the starting
     * and ending positions (in bytes) in the file.
     * By default a field is initialized with type: FIELD.
     */
    Field(long long int start, long long int end) {
        assert(end >= start);
        this->start = start;
        this->end = end;
        this->type = Types::FIELD;
    }

    /**
     * Constructs a new Field object from a given field 'f'
     * and a type 't'.
     */
    Field(Field* f, Types t) {
        this->start = f->get_start();
        this->end = f->get_end();
        this->type = t;
    }

    /**
     * Gets the start position of this field.
     */
    virtual size_t get_start() {
        return this->start;
    }

    /**
     * Gets the end position of this field.
     */
    virtual size_t get_end() {
        return this->end;
    }

    /**
     * Sets the start position of this field
     * only if the given start is bigger than the
     * one in this field.
     */
    virtual void set_start(size_t start) {
        if (start > this->start) {
            this->start = start;
        }
    }

    /**
     * Sets the end position of this field
     * only if it is smaller than the end position
     * in this field.
     */
    virtual void set_end(size_t end) {
        if (end < this->end) {
            this->end = end;
        }
    }

    /**
     * Returns the type associated with this field.
     */
    virtual Types get_type() {
        return this->type;
    }

    /**
     * Returns a copy of this field.
     */
    virtual Field* clone () {
        return new Field(this, this->type);
    }
};
