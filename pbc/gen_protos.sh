rm -rf src/generated-sources
mkdir src/generated-sources
cd protos/ && protoc-c --c_out=../src/generated-sources/ *.proto
