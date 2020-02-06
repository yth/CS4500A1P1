//lang::Cpp


#pragma once


#include <assert.h>
#include <math.h>


#include "object.h"
#include "string.h"
#include "types.h"


/**
 * TypesArray: represents a resizable array of Types
 */
class TypesArray : public Object {
public:
    size_t size_; // num of elements
    size_t capacity_; // capacity of array
    Types* vals_; // the values (owned)

    /**
     * Default constructor of this array.
     */
    TypesArray() : Object() {
        this->size_ = 0;
        this->capacity_ = 4;
        this->vals_ = new Types[this->capacity_];
    }

    /**
     * The destructor of this array.
     */
    virtual ~TypesArray() {
        delete[] this->vals_;
    }

    /**
     * Returns the number of elements in this array.
     *
     * @return the number of elements of this array
    */
    virtual size_t len() {
        return this->size_;
    }


    /**
     * Grows the array if it has reached it's capacity limit.
     */
    virtual void resize_() {
        this->capacity_ *= 2;
        Types* new_vals = new Types[this->capacity_];
        for (size_t i = 0; i < this->size_; ++i) {
            new_vals[i] = this->vals_[i];
        }
        delete[] this->vals_;
        this->vals_ = new_vals;
    }


    /**
     * Set the element of the array at the given index
     * with the given element.
     * Throws an error if index is out of bounds.
     *
     * @param i insertion index
     * @param element the element to be inserted at the given index
     */
    virtual void set(size_t i, Types element) {
        assert(i >= 0 && i < this->size_);
        this->vals_[i] = element;
    }


    /**
     * Returns an element at the given index.
     * Throws an error if index is out of bounds.
     *
     * @param index the index of the element
     * @return the element of the array at the given index
     */
    virtual Types get(size_t index) {
        assert(index >= 0 && index < this->size_);
        return this->vals_[index];
    }

    /**
     * Pushes the given item to the end of this array.
     *
     * @param item the given item to be added to the end of this array
     */
    virtual void pushBack(Types item) {
        if (this->size_ == this->capacity_) {
            this->resize_();
        }
        this->vals_[this->size_] = item;
        this->size_ += 1;
    }

    /**
     * Empties this array of all its values.
     */
    virtual void clear() {
        this->size_ = 0;
    }
};
