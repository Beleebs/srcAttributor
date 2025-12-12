tests:
	./build/bin/srcattributor -i testing/test1.json -o testing/test1.cpp.xml
	./build/bin/srcattributor -i testing/test2.json -o testing/test2.cpp.xml
	./build/bin/srcattributor -i testing/test3.json -o testing/test3.cpp.xml
	./build/bin/srcattributor -i testing/test4.json -o testing/test4.cpp.xml
	./build/bin/srcattributor -i testing/test5.json -o testing/test5.cpp.xml

testsNew:
	./build/bin/srcattributor -i testing/test1.1.json -o testing/test1.cpp.xml
	./build/bin/srcattributor -i testing/test2.1.json -o testing/test2.cpp.xml
	./build/bin/srcattributor -i testing/test3.1.json -o testing/test3.cpp.xml
	./build/bin/srcattributor -i testing/test4.1.json -o testing/test4.cpp.xml
	./build/bin/srcattributor -i testing/test5.1.json -o testing/test5.cpp.xml

slice:
	srcml testing/test1.cpp -o testing/test1.cpp.xml --position --hash
	srcml testing/test2.cpp -o testing/test2.cpp.xml --position --hash
	srcml testing/test3.cpp -o testing/test3.cpp.xml --position --hash
	srcml testing/test4.cpp -o testing/test4.cpp.xml --position --hash
	srcml testing/test5.cpp -o testing/test5.cpp.xml --position --hash

	./sliceBuild/bin/srcslice -i testing/test1.cpp.xml -o testing/test1.1.json
	./sliceBuild/bin/srcslice -i testing/test2.cpp.xml -o testing/test2.1.json
	./sliceBuild/bin/srcslice -i testing/test3.cpp.xml -o testing/test3.1.json
	./sliceBuild/bin/srcslice -i testing/test4.cpp.xml -o testing/test4.1.json
	./sliceBuild/bin/srcslice -i testing/test5.cpp.xml -o testing/test5.1.json

clean:
	srcml testing/test1.cpp -o testing/test1.cpp.xml --position --hash
	srcml testing/test2.cpp -o testing/test2.cpp.xml --position --hash
	srcml testing/test3.cpp -o testing/test3.cpp.xml --position --hash
	srcml testing/test4.cpp -o testing/test4.cpp.xml --position --hash
	srcml testing/test5.cpp -o testing/test5.cpp.xml --position --hash