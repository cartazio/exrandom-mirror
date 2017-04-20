EXAMPLES = \
	chisq_test \
	compare_times \
	count_bits \
	discrete_count_bits \
	exrandom_test \
	hist_data \
	sample_discrete_normal \
	sample_distributions \
	sample_exponential \
	sample_normal \
	sample_uniform \
	simple_discrete_normal \
	simple_exponential \
	simple_normal \
	simple_uniform \
	tabulate_normals

CC = c++
CXXFLAGS = -std=c++0x -g -O3 -Wall -Wextra
CPPFLAGS = -I../include
all: $(EXAMPLES)

clean:
	rm -f *.o $(EXAMPLES)
