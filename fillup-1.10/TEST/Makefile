
testsequence: parameter replacement
	@echo ""
	@echo "If diff subdirectores are empty the test has been successful"
	@echo ""
	ls -al */DIFF*
	@echo ""
	-cat */DIFF*/*.txt


parameter:
	make -C PARAMETERS

replacement:
	make -C REPLACEMENT

clean:
	rm -rf ./fillup
	make -C PARAMETERS clean
	make -C REPLACEMENT clean

