test-hb-coretext: test-hb-coretext.cc Makefile
	$(CXX) `pkg-config --cflags --libs harfbuzz` -framework ApplicationServices -g -Wall -Wextra $< -o $@
