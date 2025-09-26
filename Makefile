tests:
	./build/bin/srcattributor -i testing/test1.json -o testing/test1.cpp.xml
	./build/bin/srcattributor -i testing/test2.json -o testing/test2.cpp.xml
	./build/bin/srcattributor -i testing/test3.json -o testing/test3.cpp.xml
	./build/bin/srcattributor -i testing/test4.json -o testing/test4.cpp.xml
	./build/bin/srcattributor -i testing/test5.json -o testing/test5.cpp.xml

clean:
	srcml testing/test1.cpp -o testing/test1.cpp.xml
	srcml testing/test2.cpp -o testing/test2.cpp.xml
	srcml testing/test3.cpp -o testing/test3.cpp.xml
	srcml testing/test4.cpp -o testing/test4.cpp.xml
	srcml testing/test5.cpp -o testing/test5.cpp.xml