.PHONY: all test sanitize benchmark headless package
all:
	cmake -S . -B build/release -DCMAKE_BUILD_TYPE=Release
	cmake --build build/release --parallel
	cp build/release/ocelovyduel program
test: all
	ctest --test-dir build/release --output-on-failure
sanitize:
	cmake -S . -B build/sanitize -DCMAKE_BUILD_TYPE=Debug -DDUEL_SANITIZERS=ON -DDUEL_BUILD_GUI=OFF
	cmake --build build/sanitize --parallel
	ctest --test-dir build/sanitize --output-on-failure
benchmark: all
	./build/release/terrain_bench
headless:
	cmake -S . -B build/headless -DCMAKE_BUILD_TYPE=Release -DDUEL_BUILD_GUI=OFF
	cmake --build build/headless --parallel
package: test
	cd build/release && cpack
