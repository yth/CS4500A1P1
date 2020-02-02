//lang::C++


/**
 * sorer: This program reads a file passed in with -f option.
 *
 * Optionally, -from tells you the location where you start the read in the file
 *             -len tells you how many bytes to read from the file
 *             -print_col_type asks for the type of the col indicated
 *             -print_col_idx asks for the value of the field at the col, idx
 *             -is_missing_idx asks whether the field at col, idx is an empty/missing value
 *
 * Unless, -print_col_type, print_col_idx, is_missing_idx options are used, nothing will happen.
 *
 * If those options are used, a -f must be included.
 *
 * If no -len or no -from options are given, we assume the whole file must be read.
 */


#include <iostream>
#include <cassert>
#include <cstdio>
#include <cstring>
#include <string.h>


#include <sys/stat.h>
#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/mman.h>


#include "helper.h"
#include "types.h"
#include "col.h"
#include "field.h"
#include "row.h"


const char* USAGE = "Usage: ./sorer [-f] [-from] [-len] [-print_col_type] " \
		     "[-print_col_idx] [-is_missing_idx]\n" \
		     "\n" \
		     "\t-f [filename] must be the first argument\n" \
		     "\t-from [uint] must come after -f option, if used\n" \
		     "\t-len [uint] must come after -f option, and if -from is used, after -from\n" \
		     "\t only one of -print_col_type [uint] / -print_col_idx [uint] [uint] / -is_missing_idx [uint] [uint] can be used\n" \
		     "\n" \
		     "Only one option of each kind can be used.\n";


int main(int argc, char** argv) {
    // Assert valid arguments given
    if (argc < 5 || argc > 10) { std::cout << USAGE; return 0; }

    char* filename = nullptr;
    char* len_arg = nullptr;
    char* from_arg = nullptr;
    char* output_arg = nullptr;
    char* uint1_arg = nullptr;
    char* uint2_arg = nullptr;

    // parse command line arguments
    int i = 1;
    while (i < argc) {
        if (strcmp(argv[i], "-f") == 0 && !filename && argc > i + 1) {
            filename = argv[i + 1];
            i += 2;
        } else if (strcmp(argv[i], "-from") == 0 && !from_arg && argc > i + 1) {
            from_arg = argv[i + 1];
            i += 2;
        } else if (strcmp(argv[i], "-len") == 0 && !len_arg && argc > i + 1) {
            len_arg = argv[i + 1];
            i += 2;
        } else if (strcmp(argv[i], "-print_col_type") == 0 && !output_arg && argc > i + 1) {
            output_arg = argv[i];
            uint1_arg = argv[i+1];
            i += 2;
        } else if (strcmp(argv[i], "-print_col_idx") == 0 && !output_arg && argc > i + 2) {
            output_arg = argv[i];
            uint1_arg = argv[i+1];
            uint2_arg = argv[i+2];
            i += 3;
        } else if (strcmp(argv[i], "-is_missing_idx") == 0 && !output_arg && argc > i + 2) {
            output_arg = argv[i];
            uint1_arg = argv[i+1];
            uint2_arg = argv[i+2];
            i += 3;
        } else {
            std::cout << USAGE;
            return -1;
        }
    }

    // If none of the three flags that require an output
    // is given, return directly.
    if (!output_arg) {
        return 0;
    }

    // Validate arguments
    // TODO: instead of making parse_uint throw an error,
    // 			 we could change the return type to signed integer
    //       and return a negative value.
    size_t len = SIZE_MAX;
    if (len_arg) {
        len = parse_uint(len_arg);
    }
    size_t from = 0;
    if (from_arg) {
        from = parse_uint(from_arg);
    }
    size_t uint1;
    if (uint1_arg) {
        uint1 = parse_uint(uint1_arg);
    }
    size_t uint2;
    if (uint2_arg) {
        uint2 = parse_uint(uint2_arg);
    }

    // Make sure the file exists/can be opened
    int fd = open(filename, O_RDONLY);
    assert(fd != -1);

    struct stat st;
    stat(filename, &st);
    size_t file_size = st.st_size;

    assert(from < file_size);

    // Get the number of page sizes we need
    int pg_size = getpagesize();
    int pages = (file_size / pg_size) + 1;
    size_t ask = pages * pg_size;

    char* file = (char*)mmap(nullptr, ask, PROT_READ, MAP_FILE|MAP_PRIVATE, fd, 0);

    // Convert the data into Rows and determine longest row
    size_t rows_to_parse = 500;
    Row** rows = new Row*[rows_to_parse];
    Row* r;
    size_t sofar = 0;
    size_t occupancy = 0;
    size_t max_fields = 0;
    for (size_t i = 0; i < rows_to_parse; ++i) {
        if (file[sofar] == EOF) {
            break;
        }

        r = parse_row(file, sofar);
        if (r->get_occupancy() > max_fields) {
            max_fields = r->get_occupancy();
        }

        if (r->get_occupancy()) {
            rows[occupancy] = r;
            occupancy += 1;
            sofar = r->get_end() + 1;
        } else {
            sofar = r->get_end() + 1;
            delete r;
        }
    }

    // Parse the schema
    Types* schema = parse_schema(rows, file, max_fields, occupancy);
    for (size_t i = 0; i < max_fields; ++i) {
        assert(schema[i] != Types::EMPTY);
    }

    // Determine what the user asked and do it
    if (strcmp(output_arg, "-print_col_type") == 0) {
        print_type(schema[uint1]);
    } else {
        if (from != 0 && len != file_size) {
            while(file[from] != '\n') {
                assert(file[from] != EOF);
                ++from;
            }
            ++from; // from is now the start position of where we want to read
        }

        // Get the data requested by -from and -len and
        // put them into columnar form
        Col** columnar = new Col*[max_fields];
        for (size_t i = 0; i < max_fields; ++i) {
            columnar[i] = new Col(schema[i]);
        }
        sofar = 0;
        while(sofar <= len) {
            if (file[from + sofar] == EOF || file[from + sofar] == '\0') {
                break;
            }
            Row* r = parse_row(file, from + sofar);
            size_t width = r->get_occupancy();
            if (width > max_fields) {
                sofar = r->get_end() - from + 1;
                delete r;
                continue;
            }
            Types* t = parse_schema(&r, file, width, 1);
            size_t i;
            for (i = 0; i < width; ++i) {
                if (t[i] > schema[i]) { // && t[i] != Types::EMPTY) {
                    break;
                }
            }

            if (i == width) {
                for (i = 0; i < width; ++i) {
                    Field* f = new Field(r->get_field(i), schema[i]);
                    columnar[i]->add(f);
                    delete f;
                }
            }

            sofar = r->get_end() - from + 1;
            delete[] t;
            delete r;
        }

        Field* f = columnar[uint1]->get_field(uint2);
        if (strcmp(output_arg, "-is_missing_idx") == 0) {
            if (!f || (file[f->get_start()] == '<' && file[f->get_end()] == '>')) {
                std::cout << 1 << '\n';
            } else {
                std::cout << 0 << '\n';
            }
        } else {
            assert(f);
            print_field(file, f);
        }
        for (size_t i = 0; i < max_fields; ++i) {
            delete columnar[i];
        }
        delete[] columnar;
    }

    // Exit checks
    // make sure to delete everything
    for (size_t i = 0; i < occupancy; ++i) {
        delete rows[i];
    }
    delete[] rows;
    delete schema;
    munmap(file, ask);
    close(fd);

    return 0;
}
