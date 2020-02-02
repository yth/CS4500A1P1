//lang::Cpp


/**
 * Object: Base Object Class that everything else inherits from.
 *
 * Author: cao.yuan1@husky.neu.edu & zhan.d@husky.neu.edu
 */


#pragma once


#include<cstdlib>


// Immutable
class Object {
	public:
		size_t hash_; // hash() of the objected, used for comparison

		/**
		 * Constructor of objects
		 */
		Object() {
			this->hash_ = 0;
		}

		/**
		 * Destructor of Objects
		 *
		 * We never create objects directly, this should never run.
		 */
		virtual ~Object() {}

		/**
		 * Returns object's hash_ when asked
		 */
		virtual size_t hash() {
			if (this->hash_ == 0) {
				this->hash_ = hash_me();
			}

			return this->hash_;
		}

		/**
		 * Creates object's hash_
		 */
		virtual size_t hash_me() {
			return reinterpret_cast<size_t>(this);
		}

		/**
		 * Assess equally based on type and hash_
		 */
		virtual bool equals(Object* other) {
			if (!other) { return false; }

			return this->hash() == other->hash();
		}
};
