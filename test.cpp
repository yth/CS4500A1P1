//lang::Cpp


/**
 * Test File
 *
 * Author: cao.yuan1@husky.neu.edu & zhan.d@husky.neu.edu
 */


#include <cstdlib>
#include <cassert>
#include <iostream>


#include "types.h"
#include "field.h"
#include "fieldlist.h"
#include "row.h"
#include "col.h"
#include "helper.h"
#include "types.h"


// Forward Declaration
void dismantle();


// Helper Functions for Tests
void fail() { std::cout << "!!!!!!!!!!!!!!!!!!!" << '\n'; exit(1); }
void OK() { }
void t_true(bool p) { if (!p) fail(); }
void t_false(bool p) { if (p) fail(); }


// setUp
Field* f1 = nullptr;
Field* f2 = nullptr;
Field* f3 = nullptr;
Field* f4 = nullptr;
Row* r1 = nullptr;
Row* r2 = nullptr;
Row* r3 = nullptr;
Row* r4 = nullptr;
FieldList* fl1 = nullptr;
FieldList* fl2 = nullptr;
FieldList* fl3 = nullptr;
FieldList* fl4 = nullptr;
Types* types = nullptr;


/*
 * We can't currently test failing constructors,
 * But if we could, these should be failing
 * */
void test_failing_constructor() {
	std::cout << __func__ << " Started\n";
	f1 = new Field(0,0);
	f2 = new Field(-1,10);
	f3 = new Field(1,-1);
	f4 = new Field(3, 1);
	dismantle();
}

void test_good_constructor() {
	std::cout << __func__ << " Started\n";
	f1 = new Field(2,3);
	t_true(f1->get_start() == 2);
	t_true(f1->get_end() == 3);
	dismantle();
}

void test_parse_uint() {
	std::cout << __func__ << " Started\n";
	t_true(parse_uint( (char*)"123") == 123);
	//t_true(parse_uint("0x100") == 4);
}

void test_parse_field() {
	std::cout << __func__ << " Started\n";
	//                    012345678901234567 8
	char* file = (char*) "< <> < > < 1 > < \">";
	f1 = parse_field(file, 0);
	t_true(f1->get_start() == 0);
	t_true(f1->get_end() == 3);
	f2 = parse_field(file, 2);
	t_true(f2->get_start() == 2);
	t_true(f2->get_end() == 3);
	f3 = parse_field(file, 9);
	t_true(f3->get_start() == 9);
	t_true(f3->get_end() == 13);
	f4 = parse_field(file, 15);
	t_true(f4->get_start() == 15);
	t_true(f4->get_end() == 18);
	dismantle();
}

void test_fieldlist_constructor() {
	std::cout << __func__ << " Started\n";
	fl1 = new FieldList();
	f1 = new Field(1, 2);

	fl1->add(f1);
	f1 = nullptr;

	// t_true(fl1->get_field(0)->get_start() == 1);

	dismantle();
}

void test_fieldlist_add() {
	std::cout << __func__ << " Started\n";
	fl1 = new FieldList();
	f1 = new Field(1, 2);
	f2 = new Field(3, 4);
	fl1->add(f1);
	fl1->add(f2);
	fl1->add(f1);
	fl1->add(f1);
	fl1->add(f1);
	fl1->add(f1);
	fl1->add(f1);
	fl1->add(f2);

	f1 = nullptr;
	f2 = nullptr;

	dismantle();
}

void test_fieldlist_get() {
	std::cout << __func__ << " Started\n";
	fl1 = new FieldList();
	f1 = new Field(1, 2);
	f2 = new Field(3, 4);
	fl1->add(f1);
	fl1->add(f2);

	f1 = nullptr;
	f2 = nullptr;

	t_true(fl1->get_occupancy() == 2);
	t_true(fl1->get_field(1)->get_start() == 3);

}


void test_row_get_start() {
	std::cout << __func__ << " Started\n";
	r1 = new Row(1,1);
	t_true(r1->get_start() == 1);
}


void test_row_get_set_end() {
	std::cout << __func__ << " Started\n";
	r1 = new Row(1,2);
	t_true(r1->get_end() == 2);
}


void test_parse_row() {
	std::cout << __func__ << " Started\n";
	// char* file = (char*) "\n";
	// r1 = parse_row(file, 0); // Do not use this test case
	// t_true(r1 == nullptr);

    char* file2 = (char*) "\n";
	char* file3 = (char*) "<>\n";
	char* file4 = (char*) "   <> <1> < abc>\n";

	r2 = parse_row(file2, 0);
	t_true(r2->get_start() == 0);
	t_true(r2->get_end() == 0);
	t_true(r2->get_occupancy() == 0);
	r3 = parse_row(file3, 0);
	t_true(r3->get_start() == 0);
	t_true(r3->get_end() == 2);
	t_true(r3->get_occupancy() == 1);

	r4 = parse_row(file4, 0);
	t_true(r4->get_start() == 0);
	t_true(r4->get_end() == 16);
	t_true(r4->get_occupancy() == 3);


	char* file1 = (char*) "<>\xFF";
	r1 = parse_row(file1, 0);
	t_true(r1->get_start() == 0);
	t_true(r1->get_end() == 1);
	t_true(r1->get_occupancy() == 1);


	dismantle();
}

void test_types_creation_equality() {
	std::cout << __func__ << " Started\n";
    Types t1 = Types::STRING;
    Types t2 = Types::FLOAT;
    Types t3 = Types::INT;
    Types t4 = Types::BOOL;
    Types t5 = Types::STRING;
    assert(t1 == t1);
    assert(t1 != t2);
    assert(t1 != t3);
    assert(t1 != t4);
    assert(t1 == t5);
    assert(t5 == t1);
}

void test_trim() {
	std::cout << __func__ << " Started\n";
	t_true(triml(0, 1, (char*)"<>") == 1);
	t_true(trimr(0, 1, (char*)"<>") == 0);
	t_true(triml(0, 2, (char*)"< >") == 2);
	t_true(trimr(0, 2, (char*)"< >") == 0);
	t_true(triml(0, 3, (char*)"<1 >") == 1);
	t_true(trimr(0, 3, (char*)"<1 >") == 1);
	t_true(triml(0, 7, (char*)"< \"hi\"  >") == 2);
	t_true(trimr(0, 7, (char*)"< \"hi\"  >") == 5);
	t_true(triml(0, 9, (char*)"< hi Bob >") == 2);
	t_true(trimr(0, 9, (char*)"< hi Bob >") == 7);
}

void test_parse_field_type() {
	std::cout << __func__ << " Started\n";
	//t_true(parse_field_type(1, 1, (char*)"< >") == Types::EMPTY);
	//t_true(parse_field_type(0, 1, (char*)"<>") == Types::EMPTY);
	t_true(parse_field_type(1, 1, (char*)"<1 >") == Types::BOOL);
	t_true(parse_field_type(1, 2, (char*)"<12 >") == Types::INT);
	t_true(parse_field_type(2, 4, (char*)"< 1.2  >") == Types::FLOAT);
	t_true(parse_field_type(2, 5, (char*)"< \"hi\"  >") == Types::STRING);
	t_true(parse_field_type(2, 7, (char*)"< hi Bob >") == Types::FIELD);
	t_true(parse_field_type(2, 9, (char*)"< \"hi Bob\" >") == Types::STRING);
}

void test_parse_col_type() {
	std::cout << __func__ << " Started\n";
	char* file = (char*)\
		"<>   <>   <>     <>          <>\n" \
		"\n" \
		"<1> <12> <> <4l> <  \"   5   \">\n" \
		"<0> <12.3> <>\xFF";
	size_t sofar = 0;
	size_t num_rows = 4;
	size_t max_fields = 0;
	size_t occupancy = 0;
	Row** rows = new Row*[num_rows];
	for (size_t i = 0; i < num_rows; ++i) {
		if (file[sofar] == EOF) {
			break;
		}

		Row* r = parse_row(file, sofar);
		if (r->get_occupancy() > max_fields) {
			max_fields = r->get_occupancy();
		}

		if (r->get_occupancy()) {
			rows[occupancy] = r;
			occupancy += 1;
		}
		sofar = r->get_end() + 1;
	}

	t_true(parse_col_type(rows, file, 0, occupancy) == Types::BOOL);
	t_true(parse_col_type(rows, file, 1, occupancy) == Types::FLOAT);
	t_true(parse_col_type(rows, file, 2, occupancy) == Types::EMPTY);
	t_true(parse_col_type(rows, file, 3, occupancy) == Types::STRING);
	t_true(parse_col_type(rows, file, 4, occupancy) == Types::STRING);
	t_true(parse_col_type(rows, file, 5, occupancy) == Types::EMPTY);

	for (size_t i = 0; i < occupancy; ++i) {
		delete rows[i];
	}
	delete[] rows;

	dismantle();
}

void test_parse_schema() {
	std::cout << __func__ << " Started\n";
	char* file = (char*) \
		"<>   <>   <>     <>          <>\n" \
		"\n" \
		"<1> <12> <1> <4l> <  \"   5   \">\n" \
		"<0> <12.3> <1.2>\xFF";
	size_t sofar = 0;
	size_t num_rows = 4;
	size_t max_fields = 0;
	size_t occupancy = 0;
	Row** rows = new Row*[num_rows];
	for (size_t i = 0; i < num_rows; ++i) {
		if (file[sofar] == EOF) {
			break;
		}

		Row* r = parse_row(file, sofar);
		if (r->get_occupancy() > max_fields) {
			max_fields = r->get_occupancy();
		}

		if (r->get_occupancy()) {
			rows[occupancy] = r;
			occupancy += 1;
		}
		sofar = r->get_end() + 1;
	}

	Types column_types[5] = {Types::BOOL, Types::FLOAT, Types::FLOAT, Types::STRING, Types::STRING};
	types = parse_schema(rows, file, max_fields, occupancy);

	for (size_t i = 0; i < max_fields; i++) {
		t_true(types[i] == column_types[i]);
	}

	for (size_t i = 0; i < occupancy; ++i) {
		delete rows[i];
	}
	delete[] rows;

	dismantle();
}


void dismantle() {
	delete f1;
	delete f2;
	delete f3;
	delete f4;
	delete r1;
	delete r2;
	delete r3;
	delete r4;
	delete fl1;
	delete fl2;
	delete fl3;
	delete fl4;
	delete[] types;

	f1 = nullptr;
	f2 = nullptr;
	f3 = nullptr;
	f4 = nullptr;
	r1 = nullptr;
	r2 = nullptr;
	r3 = nullptr;
	r4 = nullptr;
	fl1 = nullptr;
	fl2 = nullptr;
	fl3 = nullptr;
	fl4 = nullptr;
	types = nullptr;
}

int main(int argc, char** argv) {
	//test_failing_constructor(); // don't use this, will always fail
	test_good_constructor();
	test_parse_uint();
	test_parse_field();

	test_fieldlist_constructor();
	test_fieldlist_add();
	test_fieldlist_get();

	test_row_get_start();
	test_row_get_set_end();

	test_parse_row();
	test_trim();
	test_parse_field_type();
	test_parse_col_type();
	test_parse_schema();

  test_types_creation_equality();

	std::cout << "All test have passed!" << "\n";
	return 0;
}
