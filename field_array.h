//lang::Cpp


/**
 * FieldArray: Vector container of a bunch of fields
 *
 * Author: cao.yuan1@husky.neu.edu & zhan.d@husky.neu.edu
 */


#pragma once


#include<cstdlib>
#include<cassert>


#include "object.h"
#include "types.h"


/**
 * FieldArray: represents a vector storing the starting and ending bytes of
 * fields in a .sor file relative to the start of the file.
 * The start of each field is delimited by '<' and end point by '>'.
 * The class also stores the type of all the delimited fields.
 * INVARIANT: the size of the starts and ends arrays are always the same
 * and the items stored at the respective indexes are referring to the
 * start and end of the same field.
 */
class FieldArray : public Object {

public:
    Types type_; // the type
    size_t size_; // the size
    size_t capacity_; // the capacity
    int* starts_; // the start bytes
    int* ends_; // the end bytes

    /**
     * Default constructor.
     */
    FieldArray() {
        this->type_ = Types::FIELD;
        this->size_ = 0;
        this->capacity_ = 4;
        this->starts_ = new int[this->capacity_];
        this->ends_ = new int[this->capacity_];
    }

    /**
     * Default destructor.
     */
    virtual ~FieldArray() {
        delete[] this->starts_;
        delete[] this->ends_;
    }


    virtual void pushBack(int start, int end) {
        if (this->size_ == this->capacity_) {
            this->resize_();
        }
        this->starts_[this->size_] = start;
        this->ends_[this->size_] = end;
        this->size_ += 1;
    }

    /**
     * Grows the array if it has reached it's capacity limit.
     */
    virtual void resize_() {
        this->capacity_ *= 2;
        int* new_starts = new int[this->capacity_];
        int* new_ends = new int[this->capacity_];
        for(size_t i = 0; i < this->size_; ++i) {
            new_starts[i] = this->starts_[i];
            new_ends[i] = this->ends_[i];
        }
        delete[] this->starts_;
        delete[] this->ends_;
        this->starts_ = new_starts;
        this->ends_ = new_ends;
    }

    /**
     * Returns the number of elements in this field array.
     *
     * @return the num of elements in this field array.
     */
    virtual size_t len() {
        return this->size_;
    }

    /**
     * Returns the starting byte of the field at index i.
     * @param i the index of the field in this field array.
     * @return the starting byte, or -1 if the given index is out of bound.
     */
    virtual int get_start(size_t i) {
        if (i >= this->size_) {
            return -1;
        }
        return this->starts_[i];
    }

    /**
     * Returns the ending byte of the field at index i.
     * @param i the index of the field in this field array.
     * @return the ending byte, or -1 if the given index is out of bound.
     */
    virtual int get_end(size_t i) {
        if (i >= this->size_) {
            return -1;
        }
        return this->ends_[i];
    }

    /**
     * Empties this field array.
     */
    virtual void clear() {
        this->size_ = 0;
    }

    /**
     * Sets the type of this field array if it was not previously set.
     * @param t the type to set.
     */
    virtual void set_type(Types t) {
        if (this->type_ == Types::FIELD) {
            this->type_ = t;
        }
    }
};