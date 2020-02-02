DOCKER := docker run -ti -v `pwd`:/test w2-gtest:0.1 bash -c

all:
#	- $(DOCKER) "cd /test; ./sorer -f 0.sor -print_col_type 0"
#	- $(DOCKER) "cd /test; ./sorer -f 0.sor -is_missing_idx 0 0"
#	- $(DOCKER) "cd /test; ./sorer -f 1.sor -print_col_type 0"
#	- $(DOCKER) "cd /test; ./sorer -f 1.sor -is_missing_idx 0 1"
#	- $(DOCKER) "cd /test; ./sorer -f 1.sor -print_col_idx 0 3"
#	- $(DOCKER) "cd /test; ./sorer -f 2.sor -print_col_type 0"
#	- $(DOCKER) "cd /test; ./sorer -f 2.sor -print_col_type 1"
#	- $(DOCKER) "cd /test; ./sorer -f 2.sor -print_col_type 2"
#	- $(DOCKER) "cd /test; ./sorer -f 2.sor -print_col_type 3"
#	- $(DOCKER) "cd /test; ./sorer -f 2.sor -is_missing_idx 1 0"
#	- $(DOCKER) "cd /test; ./sorer -f 2.sor -is_missing_idx 1 1"
#	- $(DOCKER) "cd /test; ./sorer -f 2.sor -print_col_idx 3 0"
#	- $(DOCKER) "cd /test; ./sorer -f 2.sor -print_col_idx 3 1"
#	- $(DOCKER) "cd /test; ./sorer -f 2.sor -print_col_idx 2 0"
#	- $(DOCKER) "cd /test; ./sorer -f 2.sor -print_col_idx 2 1"
#	- $(DOCKER) "cd /test; ./sorer -f 3.sor -print_col_type 4"
#	- $(DOCKER) "cd /test; ./sorer -f 3.sor -print_col_idx 2 10"
	- $(DOCKER) "cd /test; time ./sorer -f 3.sor -print_col_idx 2 384200"
#	- $(DOCKER) "cd /test; ./sorer -f 4.sor -print_col_idx 0 1"
#	- $(DOCKER) "cd /test; ./sorer -f 4.sor -print_col_idx 0 2"
#	- $(DOCKER) "cd /test; ./sorer -f 4.sor -print_col_idx 1 1"
#	- $(DOCKER) "cd /test; ./sorer -f 4.sor -print_col_idx 1 2"
#	- $(DOCKER) "cd /test; ./sorer -f 1.sor -from 1 -len 74 -print_col_type 0"
#	- $(DOCKER) "cd /test; ./sorer -f 1.sor -from 1 -len 74 -is_missing_idx 0 0"
#	- $(DOCKER) "cd /test; ./sorer -f 1.sor -from 1 -len 74 -print_col_idx 0 6"
	- $(DOCKER) "cd /test; time ./sorer -f 5.sor -print_col_idx 2 384200"

clean:
	- rm sorer
