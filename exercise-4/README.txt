# Compile + Execute
Running `make` will compile the program and start it with 100000 articles passed as a command line argument.
Running the program without make: `./exercise-4 <limit-of-articles-to-parse>`, with the default being 100000.
Important: The wiki data file ("dewiki-20220201-clean.txt") must be placed in the same directory as the executable.

## Required Libraries
For using the std parallel sorting algorithms (C++17), gcc need the Threading Building Blocks (TBB) library.
For debian: ``sudo apt-get install libtbb-dev``

## Configure
For changing the amount of articles to preview for a query, update the constant at the top of the main file: `constexpr uint32_t DEFAULT_ARTICLE_DISPLAY_COUNT = 3;`.

# Benchmark
on my 16GB i7(6-gen) windows pc
-> RAM and CPU are both at 100% during the 2 minutes of construction

## Creation
Naive sorting method builds in 10 minutes without parallelization and in 2 minutes with it.
While using ~9GB, which roughly aligns with my estimate of needing 13 bytes per character in the original text + 8 bytes per article.
This would mean for the full dataset it would need around 80GB of RAM.

Iterative sorting method takes way too long, probably a logic error on my part, because I see a lot of CPU idle time. .-.

# Query
For 'Stuttgart' the suffix array nicely out-performs the naive search with 1ms against 90-100ms.
However for 'US', which has seven times as many hits, the naive approach stays roughly the same while the suffix array time climbs to 10ms.
