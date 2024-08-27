# Reusable makefile for building out-of-tree extension with the DuckDB extension template
#
# Inputs
#   EXT_NAME          : Upper case string describing the name of the out-of-tree extension
#   EXT_CONFIG        : Path to the extension config file specifying how to build the extension
#   EXT_FLAGS         : Extra CMake flags to pass to the build
#   EXT_RELEASE_FLAGS : Extra CMake flags to pass to the release build
#   EXT_DEBUG_FLAGS   : Extra CMake flags to pass to the debug build

.PHONY: all clean format debug release pull update wasm_mvp wasm_eh wasm_threads

all: release

TEST_PATH="/test/unittest"
DUCKDB_PATH="/duckdb"

DUCKDB_SRCDIR ?= "./duckdb/"

# For non-MinGW windows the path is slightly different
ifeq ($(OS),Windows_NT)
ifneq ($(CXX),g++)
	TEST_PATH="/test/Release/unittest.exe"
	DUCKDB_PATH="/Release/duckdb.exe"
endif
endif

#### OSX config
OSX_BUILD_FLAG=
ifneq (${OSX_BUILD_ARCH}, "")
	OSX_BUILD_FLAG=-DOSX_BUILD_ARCH=${OSX_BUILD_ARCH}
endif

#### VCPKG config
VCPKG_TOOLCHAIN_PATH?=
ifneq ("${VCPKG_TOOLCHAIN_PATH}", "")
	TOOLCHAIN_FLAGS:=${TOOLCHAIN_FLAGS} -DVCPKG_MANIFEST_DIR='${PROJ_DIR}' -DVCPKG_BUILD=1 -DCMAKE_TOOLCHAIN_FILE='${VCPKG_TOOLCHAIN_PATH}'
endif
ifneq ("${VCPKG_TARGET_TRIPLET}", "")
	TOOLCHAIN_FLAGS:=${TOOLCHAIN_FLAGS} -DVCPKG_TARGET_TRIPLET='${VCPKG_TARGET_TRIPLET}'
endif

#### Enable Ninja as generator
ifeq ($(GEN),ninja)
	GENERATOR=-G "Ninja" -DFORCE_COLORED_OUTPUT=1
endif

#### Configuration for this extension
EXTENSION_FLAGS=-DDUCKDB_EXTENSION_CONFIGS='${EXT_CONFIG}'

BUILD_FLAGS=-DEXTENSION_STATIC_BUILD=1 $(EXTENSION_FLAGS) ${EXT_FLAGS} $(OSX_BUILD_FLAG) $(TOOLCHAIN_FLAGS) -DDUCKDB_EXPLICIT_PLATFORM='${DUCKDB_PLATFORM}'

debug:
	mkdir -p  build/debug && \
	cmake $(GENERATOR) $(BUILD_FLAGS) $(EXT_DEBUG_FLAGS) -DCMAKE_BUILD_TYPE=Debug -S $(DUCKDB_SRCDIR) -B build/debug && \
	cmake --build build/debug --config Debug

release:
	mkdir -p build/release && \
	cmake $(GENERATOR) $(BUILD_FLAGS) $(EXT_RELEASE_FLAGS) -DCMAKE_BUILD_TYPE=Release -S $(DUCKDB_SRCDIR) -B build/release && \
	cmake --build build/release --config Release

# Main tests
test: test_release

test_release: release
	./build/release/$(TEST_PATH) "$(PROJ_DIR)test/*"

test_debug: debug
	./build/debug/$(TEST_PATH) "$(PROJ_DIR)test/*"

# WASM config
VCPKG_EMSDK_FLAGS=-DVCPKG_CHAINLOAD_TOOLCHAIN_FILE=$(EMSDK)/upstream/emscripten/cmake/Modules/Platform/Emscripten.cmake
WASM_COMPILE_TIME_COMMON_FLAGS=-DWASM_LOADABLE_EXTENSIONS=1 -DBUILD_EXTENSIONS_ONLY=1 -DSKIP_EXTENSIONS="parquet;json" $(TOOLCHAIN_FLAGS) $(VCPKG_EMSDK_FLAGS)
WASM_CXX_MVP_FLAGS=
WASM_CXX_EH_FLAGS=$(WASM_CXX_MVP_FLAGS) -fwasm-exceptions -DWEBDB_FAST_EXCEPTIONS=1
WASM_CXX_THREADS_FLAGS=$(WASM_COMPILE_TIME_EH_FLAGS) -DWITH_WASM_THREADS=1 -DWITH_WASM_SIMD=1 -DWITH_WASM_BULK_MEMORY=1 -pthread
WASM_LINK_TIME_FLAGS=-O3 -sSIDE_MODULE=2 -sEXPORTED_FUNCTIONS="_${EXT_NAME}_version,_${EXT_NAME}_init"

# WASM targets
wasm_mvp:
	mkdir -p build/wasm_mvp
	emcmake cmake $(GENERATOR) $(EXTENSION_FLAGS) $(WASM_COMPILE_TIME_COMMON_FLAGS) -Bbuild/wasm_mvp -DCMAKE_CXX_FLAGS="$(WASM_CXX_MVP_FLAGS)" -S $(DUCKDB_SRCDIR) -DDUCKDB_EXPLICIT_PLATFORM=wasm_mvp -DDUCKDB_CUSTOM_PLATFORM=wasm_mvp
	emmake make -j8 -Cbuild/wasm_mvp

wasm_eh:
	mkdir -p build/wasm_eh
	emcmake cmake $(GENERATOR) $(EXTENSION_FLAGS) $(WASM_COMPILE_TIME_COMMON_FLAGS) -Bbuild/wasm_eh -DCMAKE_CXX_FLAGS="$(WASM_CXX_EH_FLAGS)" -S $(DUCKDB_SRCDIR) -DDUCKDB_EXPLICIT_PLATFORM=wasm_eh -DDUCKDB_CUSTOM_PLATFORM=wasm_eh
	emmake make -j8 -Cbuild/wasm_eh

wasm_threads:
	mkdir -p ./build/wasm_threads
	emcmake cmake $(GENERATOR) $(EXTENSION_FLAGS) $(WASM_COMPILE_TIME_COMMON_FLAGS) -Bbuild/wasm_threads -DCMAKE_CXX_FLAGS="$(WASM_CXX_THREADS_FLAGS)" -S $(DUCKDB_SRCDIR) -DDUCKDB_EXPLICIT_PLATFORM=wasm_threads -DDUCKDB_CUSTOM_PLATFORM=wasm_threads
	emmake make -j8 -Cbuild/wasm_threads

#### Misc
format:
	find src/ -iname *.hpp -o -iname *.cpp | xargs clang-format --sort-includes=0 -style=file -i
	cmake-format -i CMakeLists.txt

update:
	git submodule update --remote --merge

pull:
	git submodule init
	git submodule update --recursive --remote

clean:
	rm -rf build
	rm -rf testext
	make $@ -C $(DUCKDB_SRCDIR)

clean-python:
	make $@ -C $(DUCKDB_SRCDIR)
