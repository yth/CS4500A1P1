//lang::Cpp


/**
 * FieldList: Vector container of a bunch of fields
 *
 * Author: cao.yuan1@husky.neu.edu & zhan.d@husky.neu.edu
 */


#pragma once


#include<cstdlib>
#include<cassert>


#include "object.h"
#include "field.h"


/**
 * Represents a vector of fields.
 */
class FieldList : public Object {

public:
    size_t size; // the max capacity
    size_t occupancy; // the length
    Field** fields; // the fields (owned)

    /**
     * Constructs an empty FieldList.
     */
    FieldList() {
        this->size = 4;
        this->occupancy = 0;
        this->fields = new Field*[this->size];
    }

    /**
     * Frees the values owned by this FieldList.
     */
    virtual ~FieldList() {
        for (size_t i = 0; i < this->occupancy; ++i) {
            delete this->fields[i];
        }
        delete[] this->fields;
    }

    /**
     * Add: Add an exisiting field to the field list.
     */
    virtual void add(Field* f) {
        if (this->occupancy == this->size) {
            this->size *= 2;
            Field** new_fields = new Field*[this->size];
            for(size_t i = 0; i < this->occupancy; ++i) {
                new_fields[i] = this->fields[i];
            }
            delete[] this->fields;
            this->fields = new_fields;
        }
        this->fields[this->occupancy] = f->clone();
        this->occupancy += 1;
    }

    /**
     * Returns the length of this FieldList.
     */
    virtual size_t get_occupancy() {
        return this->occupancy;
    }

    /**
     * Gets the field at index 'i'.
     * If the given index is out of bound, returns a nullptr.
     */
    virtual Field* get_field(size_t i) {
        if (i >= this->occupancy) {
            return nullptr;
        }
        return this->fields[i];
    }
};
