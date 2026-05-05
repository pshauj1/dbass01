CXX = g++
CXXFLAGS = -Wall -O3 -std=c++17

TARGET = benchmark

SRCS = test.cc struct.cc b_tree.cc b_star_tree.cc b_plus_tree.cc
OBJS = $(SRCS:.cc=.o)

HEADERS = struct.h b_tree.h b_star_tree.h b_plus_tree.h std_record.h

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS)

%.o: %.cc $(HEADERS)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

re: clean all

.PHONY: all clean re