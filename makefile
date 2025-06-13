# METHOD?=local
# METHOD?=sa
METHOD?=ant

.PHONY: run
run:
	cd build; make && ./polyg ../data/input.json ../data_outputs/output.json

.PHONY: test1
test1:
	cd build; make && ./polyg ../data/instance_1_$(METHOD).json ../data_outputs/output_1_$(METHOD).json

.PHONY: test2
test2:
	cd build; make && ./polyg ../data/instance_2_$(METHOD).json ../data_outputs/output_2_$(METHOD).json

.PHONY: test3
test3:
	cd build; make && ./polyg ../data/instance_3_$(METHOD).json ../data_outputs/output_3_$(METHOD).json

.PHONY: test4
test4:
	cd build; make && ./polyg ../data/instance_4_$(METHOD).json ../data_outputs/output_4_$(METHOD).json

.PHONY: test5
test5:
	cd build; make && ./polyg ../data/instance_5_$(METHOD).json ../data_outputs/output_5_$(METHOD).json

.PHONY: test6
test6:
	cd build; make && ./polyg ../data/instance_6_$(METHOD).json ../data_outputs/output_6_$(METHOD).json					

.PHONY: test7
test7:
	cd build; make && ./polyg ../data/instance_7_$(METHOD).json ../data_outputs/output_7_$(METHOD).json

.PHONY: clean
clean:
	rm -rf build

.PHONY: all
all: test1 test2 test3 test4 test5 test6 test7


.PHONY: build
build: clean
	mkdir build
	cd build; cmake ..

.PHONY: dist
dist: clean
	mkdir build
	cd build; cmake -DCMAKE_BUILD_TYPE=Release ..


.PHONY: debug
debug:
	cd build; gdb --args ./polyg -i ../input/instance_7.json -o ../outputinstance_7_output.json