//lang::Cpp


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
#include <string.h>


#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>


#include "helper.h"


const char *USAGE = "Usage: ./sorer [-f] [-from] [-len] [-print_col_type] " \
             "[-print_col_idx] [-is_missing_idx]\n" \
             "\n" \
             "\t-f [filename] must be the first argument\n" \
             "\t-from [uint] must come after -f option, if used\n" \
             "\t-len [uint] must come after -f option, and if -from is used, after -from\n" \
             "\t only one of -print_col_type [uint] / -print_col_idx [uint] [uint] / -is_missing_idx [uint] [uint] can be used\n" \
             "\n" \
             "Only one option of each kind can be used.\n";


int main(int argc, char **argv) {
    //TODO: move parsing logic to its own class

    // Assert valid arguments given
    if (argc < 5 || argc > 10) {
        std::cout << USAGE;
        return 0;
    }

    char *filename = nullptr;
    char *len_arg = nullptr;
    char *from_arg = nullptr;
    char *output_arg = nullptr;
    char *uint1_arg = nullptr;
    char *uint2_arg = nullptr;

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
            uint1_arg = argv[i + 1];
            i += 2;
        } else if (strcmp(argv[i], "-print_col_idx") == 0 && !output_arg && argc > i + 2) {
            output_arg = argv[i];
            uint1_arg = argv[i + 1];
            uint2_arg = argv[i + 2];
            i += 3;
        } else if (strcmp(argv[i], "-is_missing_idx") == 0 && !output_arg && argc > i + 2) {
            output_arg = argv[i];
            uint1_arg = argv[i + 1];
            uint2_arg = argv[i + 2];
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

    // Parse the numeric arguments
    size_t len = SIZE_MAX;
    if (len_arg) {
        len = parse_uint(len_arg);
        assert(len != SIZE_MAX);
    }
    size_t from = 0;
    if (from_arg) {
        from = parse_uint(from_arg);
        assert(from != SIZE_MAX);
    }
    size_t uint1 = 0;
    if (uint1_arg) {
        uint1 = parse_uint(uint1_arg);
        assert(uint1 != SIZE_MAX);
    }
    size_t uint2 = 0;
    if (uint2_arg) {
        uint2 = parse_uint(uint2_arg);
        assert(uint2 != SIZE_MAX);
    }


    // Make sure the file exists/can be opened
    int fd = open(filename, O_RDONLY);
    assert(fd != -1);

    // Use stat the get the file size
    struct stat st;
    stat(filename, &st);
    size_t file_size = st.st_size;

    assert(from < file_size);

    // Get the number of page sizes we need
    int pg_size = getpagesize();
    size_t pages = (size_t) (file_size / pg_size) + 1;
    size_t ask = pages * pg_size;

    //Mmap for lazy read
    char *file = (char *) mmap(nullptr, ask, PROT_READ, MAP_PRIVATE, fd, 0);

    // Parse the schema
    TypesArray *schema = parse_schema(file);

    // discard the first line if given from != 0
    size_t end = from + len;
    if (from != 0 && from < file_size) {
        while (file[from] != '\n') {
            assert(file[from] != EOF);
            ++from;
        }
        ++from;
    }
    // if the file size is smaller than the bytes we are supposed to read,
    // we just read the whole file
    if (end >= file_size) {
        end = file_size;
    } else {
        // else we discard the last line
        while (file[end] != '\n') {
            assert(end > from);
            --end;
        }
    }

    // Get the data requested by -from and -len and
    // put them into columnar form
    FieldArray **columnar = make_columnar(file, from, end, schema);

    // Determine what the user asked and do it
    if (strcmp(output_arg, "-print_col_type") == 0) {
        print_type(schema->get(uint1));
    } else {
        size_t field_start = columnar[uint1]->get_start(uint2);
        size_t field_end = columnar[uint1]->get_end(uint2);
        if (strcmp(output_arg, "-is_missing_idx") == 0) {
            if (field_end - field_start < 2) {
                std::cout << 1 << '\n';
            } else {
                std::cout << 0 << '\n';
            }
        } else {
            print_field(file, field_start, field_end, schema->get(uint1));
        }
    }
    // delete everything
	size_t num_col = schema->len();
    for (size_t k = 0; k < num_col; ++k) {
        delete columnar[k];
    }
    delete[] columnar;
    delete schema;
    munmap(file, ask);
    close(fd);
    return 0;
}
