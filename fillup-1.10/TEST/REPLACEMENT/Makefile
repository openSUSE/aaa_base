
TESTOBJ=../fillup
REF=./REFERENCE
OUT=./OUT
DIF=./DIFFERENCE


all:	Test1 Test2 Test3 Test4 Test5
	@echo ""
	@echo "If subdirectory $(DIF) is empty the test has been successful"
	@echo ""
	ls -al $(DIF)
	@echo ""


Test1:	Test100 Test101 Test102 Test103 Test104
Test2:  Test200 Test201
Test3:  Test300 Test301
Test4:  Test400 Test401
Test5:  Test500 Test501


Test100: fillup
	@echo "---------------------------"
	rm -f authors.*
	./CreateAuthors 1 4 7 10 13 trailer
	mv authors.text ./authors.base
	./CreateAuthors 2 14
	mv authors.text ./authors.add
	$(TESTOBJ) -t authors.base authors.add authors.outfile
	-diff $(REF)/Test100.txt authors.outfile > TestDifference.txt
	Evaluate 100
	@echo "---------------------------"

Test101: fillup
	@echo "---------------------------"
	rm -f authors.*
	./CreateAuthors 1 4 7 10 13 trailer
	cp authors.text ./authors.outfile
	mv authors.text ./authors.base
	./CreateAuthors 2 14
	mv authors.text ./authors.add
	$(TESTOBJ) -t authors.base authors.add 
	-diff $(REF)/Test101.txt authors.base > TestDifference.txt
	Evaluate 101
	-diff $(REF)/Test101.txt authors.outfile > TestDifference.txt
	Evaluate 101
	@echo "---------------------------"

Test102: fillup
	@echo "---------------------------"
	rm -f authors.*
	./CreateAuthors 4 7 10 trailer
	mv authors.text ./authors.base
	./CreateAuthors 2 8 14
	mv authors.text ./authors.add
	./CreateAuthors 4 7 10 trailer 2 14 
	$(TESTOBJ) -t authors.base authors.add authors.outfile
	cat authors.outfile empty > authors.tmp
	-diff authors.text authors.tmp > TestDifference.txt
	Evaluate 102
	-diff $(REF)/Test102.txt authors.outfile > TestDifference.txt
	Evaluate 102
	@echo "---------------------------"

Test103: fillup
	@echo "---------------------------"
	rm -f authors.*
	./CreateAuthors 4 7 10 trailer
	mv authors.text ./authors.base
	./CreateAuthors 2 8 14
	mv authors.text ./authors.add
	./CreateAuthors 4 7 10 trailer 2 14 
	$(TESTOBJ) -t authors.base authors.add
	cat authors.base empty > authors.tmp
	-diff authors.text authors.tmp > TestDifference.txt
	Evaluate 103
	-diff $(REF)/Test103.txt authors.base > TestDifference.txt
	Evaluate 103
	@echo "---------------------------"

Test104: fillup
	@echo "---------------------------"
	rm -f authors.*
	./CreateAuthors 4 7 10 trailer
	mv authors.text ./authors.base
	./CreateAuthors 2 8 14
	mv authors.text ./authors.add
	./CreateAuthors 4 7 10 trailer 2 14 
	$(TESTOBJ) -t --maintain --quiet authors.base authors.add
	cat authors.base empty > authors.tmp
	-diff authors.text authors.tmp > TestDifference.txt
	Evaluate 104
	-diff $(REF)/Test104.txt authors.base > TestDifference.txt
	Evaluate 104
	@echo "---------------------------"

Test200: fillup
	@echo "---------------------------"
	rm -f authors.*
	./CreateAuthors 1 4 7 trailer
	mv authors.text ./authors.base
	./CreateAuthors 5 14
	mv authors.text ./authors.add
	$(TESTOBJ) -t -x authors.base authors.add authors.outfile
	-diff $(REF)/Test200.txt authors.outfile > TestDifference.txt
	Evaluate 200
	@echo "---------------------------"

Test201: fillup
	@echo "---------------------------"
	rm -f authors.*
	./CreateAuthors 1-HEADER 4 7 trailer
	mv authors.text ./authors.base
	./CreateAuthors 5 14
	mv authors.text ./authors.add
	$(TESTOBJ) -t -x authors.base authors.add authors.outfile
	-diff $(REF)/Test201.txt authors.outfile > TestDifference.txt
	Evaluate 201
	@echo "---------------------------"

Test300: fillup
	@echo "---------------------------"
	rm -f authors.*
	./CreateAuthors 1 4 7 trailer
	mv authors.text ./authors.base
	./CreateAuthors 5 14
	mv authors.text ./authors.add
	$(TESTOBJ) -t -i authors.base authors.add authors.outfile
	-diff $(REF)/Test300.txt authors.outfile > TestDifference.txt
	Evaluate 300
	@echo "---------------------------"

Test301: fillup
	@echo "---------------------------"
	rm -f authors.*
	./CreateAuthors 1-HEADER 4 7 trailer
	mv authors.text ./authors.base
	./CreateAuthors 6 14
	mv authors.text ./authors.add
	$(TESTOBJ) -t -i authors.base authors.add authors.outfile
	-diff $(REF)/Test301.txt authors.outfile > TestDifference.txt
	Evaluate 301
	@echo "---------------------------"

Test400: fillup
	@echo "---------------------------"
	rm -f authors.*
	./CreateAuthors 1 4 7 10 13 trailer
	mv authors.text ./authors.base
	./CreateAuthors 2 14
	mv authors.text ./authors.add
	$(TESTOBJ) -t -r authors.base authors.add authors.outfile
	-diff $(REF)/Test400.txt authors.outfile > TestDifference.txt
	Evaluate 400
	-diff $(REF)/Test400.newtxt authors.base.new > TestDifference.txt
	Evaluate 400
	@echo "---------------------------"

Test401: fillup
	@echo "---------------------------"
	rm -f authors.*
	./CreateAuthors 1-HEADER 4 7 10 13 trailer
	mv authors.text ./authors.base
	./CreateAuthors 2 14
	mv authors.text ./authors.add
	$(TESTOBJ) -t -r authors.base authors.add authors.outfile
	-diff $(REF)/Test401.txt authors.outfile > TestDifference.txt
	Evaluate 401
	-diff $(REF)/Test401.newtxt authors.base.new > TestDifference.txt
	Evaluate 401
	@echo "---------------------------"

Test500: fillup
	@echo "---------------------------"
	rm -f authors.*
	./CreateAuthors 1 4 7 10 13 trailer
	mv authors.text ./authors.base
	./CreateAuthors 2 14
	mv authors.text ./authors.add
	$(TESTOBJ) -t -r -x authors.base authors.add authors.outfile
	-diff $(REF)/Test500.txt authors.outfile > TestDifference.txt
	Evaluate 500
	-diff $(REF)/Test500.newtxt authors.base.new > TestDifference.txt
	Evaluate 500
	@echo "---------------------------"

Test501: fillup
	@echo "---------------------------"
	rm -f authors.*
	./CreateAuthors 1-HEADER 4 7 10 13 trailer
	mv authors.text ./authors.base
	./CreateAuthors 2 14
	mv authors.text ./authors.add
	$(TESTOBJ) -t -r -x authors.base authors.add authors.outfile
	-diff $(REF)/Test501.txt authors.outfile > TestDifference.txt
	Evaluate 501
	-diff $(REF)/Test501.newtxt authors.base.new > TestDifference.txt
	Evaluate 501
	@echo "---------------------------"


fillup: $(TESTOBJ)
	make -C ../../SRC clean
	make -C ../../SRC all
	mv ../../BIN/fillup $(TESTOBJ)

clean:
	rm -rf authors.*
	rm -rf DIFFERENCE/*

