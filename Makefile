# Include rules
# -include ../build/rules.mk
CXXFLAGS := --std=c++11 -Wall --pedantic -O3
DOCKER := docker run -ti -v `pwd`:/test w2-gtest:0.1 bash -c


local:
	c++ $(CXXFLAGS) main.cpp -o sorer

build:
	$(DOCKER) "cd /test ; g++ $(CXXFLAGS) main.cpp -o sorer"

unittest:
	$(DOCKER) "cd /test ; g++ $(CXXFLAGS) test.cpp -o unittest"

val:
	$(DOCKER) "cd /test ; valgrind --leak-check=yes ./sorer -f test/3.sor -print_col_idx 1 1"

test: build unittest
	@ - $(DOCKER) "cd /test ; ./sorer -f "data.sor" -from 0 -len 100 -print_col_type 0"
	@ - $(DOCKER) "cd /test ; ./sorer -f "data.sor" -from 0 -len 100 -print_col_type 2"
	@ - $(DOCKER) "cd /test ; ./sorer -f "data.sor" -from 0 -len 100 -is_missing_idx 2 0"
	@ - $(DOCKER) "cd /test ; ./sorer -f "data.sor" -from 0 -len 100 -is_missing_idx 2 1"
	@ - $(DOCKER) "cd /test ; ./sorer -f "data.sor" -from 0 -len 100 -is_missing_idx 2 2"
	@ - $(DOCKER) "cd /test ; ./sorer -f "data.sor" -from 0 -len 100 -print_col_idx 2 0"
	@ - $(DOCKER) "cd /test ; ./sorer -f "data.sor" -from 5 -len 100 -print_col_idx 1 0"
	@ - $(DOCKER) "cd /test ; ./sorer -f "data.sor" && echo Passed"
	@ - $(DOCKER) "cd /test ; ./sorer -f "data.sor" -from 0 && echo Passed"
	@ - $(DOCKER) "cd /test ; ./sorer -f data.sor -from 0 -len 1024 && echo Passed"
	@ - $(DOCKER) "cd /test ; ./sorer -f data.sor -from 0 -len 1024 && echo Passed"
	@ - $(DOCKER) "cd /test ; ./sorer -f data.sor -from 0 -len 1024 -print_col_type 0 && echo Passed"
	@ - $(DOCKER) "cd /test ; ./sorer -f data.sor -from 0 -len 1024 -print_col_typ 0 || echo Passed"
	@ - $(DOCKER) "cd /test ; ./sorer -f || echo Passed"
	@ - $(DOCKER) "cd /test ; ./sorer -f data.sor -string string || echo Passed"
	@ - $(DOCKER) "cd /test ; ./sorer -f data.sor -from 0 -len 123a || echo Passed"
	@ - $(DOCKER) "cd /test ; ./sorer -f data.sor -from 0 -len -12 || echo Passed"
	@ - $(DOCKER) "cd /test ; ./sorer -f data.sor -from 0 -len 12.2 || echo Passed"
	@ - $(DOCKER) "cd /test ; ./sorer -f data.sor -from 0 -len abc || echo Passed"
	@ - $(DOCKER) "cd /test ; ./unittest"

clean:
	rm -f unittest sorer
