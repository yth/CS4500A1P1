//lang::Cpp


/**
 * Col: Represent a column of data
 *
 * Author: cao.yuan1@husky.neu.edu & zhan.d@husky.neu.edu
 */


#pragma once


#include<cstdlib>
#include<cassert>


#include "types.h"
#include "field.h"

/**
 * A class to store .sor file fields in a columnar
 * representation.
 */
class Col : public FieldList {
public:
    Types type;

    /**
     * Constructs a Col with a type.
     */
    Col(Types type) {
        this->type = type;
    }

    Types get_type() {
        return this->type;
    }

};
