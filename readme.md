# Google Hash Code 2019

Experimentation with Google Hash Code 2019


#### Iterative, hill climb approach

Script `run_iterative.py` contains a stochastic (discrete-valued, iterative) hill climb approach to finding a solution. It starts by building slides (horizontal and vertical, the latter just by pairing next vertical photos) and concatenating them into a slideshow. Then, random adjacent permutations of the slideshow are generated: by swapping two slides or by swapping two photos between different slides. Each permutation is then evaluated for increasing the overall slideshow score, but only neighbouring slide-transitions need to be re-evaluated. If the score is increased, the change to the solution is accepted and further permutations are generated from the new solution. By doing small changes to the slideshow and checking them locally, the algorithm can improve the overall solution. It can terminate by converging to an optimum (possibly local) or by exhausting bounded resources. The script sets the limit of permutations that will be generated at each step.

There are some problems related to this approach. One of them is the problem of finding local maxima vs global maximum (or global maxima, if there is more than one permutation that gives the maximum score). If it happens that no other adjacent solution that has higher value is reachable with just one step from the current solution, then this approach will converge to that one local optimum. Different methods can be applied to increase the probability of finding the global maximum, one of them is simulated annealing. In simplification, this approach allows changes to the solution that decrease the score, but it varies the probability with which it will accept negative solutions as the time goes. In the end, it should accept only positive changes, with the hope that accepting temporarily worse solutions allowed it to find a globally better solution in the end.

The other change that could be done to this approach is to search for permutations in a more organized manner (than randomly generating them), so that after exhausting all possible neighbouring permutations, the method can guarantee finding a local optimum.

Also, heuristics used for approximating solutions to TSP can be used. This approach is similar to a pairwise exchange method and any other variations, like k-opt, could be used.

The (sample) scores obtained with this approach (max changes tested per step: 10^6):

| Test | Score  |
|------|--------|
| A    | 2      |
| B    | 111642 |
| C    | 1058   |
| D    | 351927 |
| E    | 112468 |
| Sum: | 577097 |


#### Greedy approach

Files `run_greedy_next_slide.py` and `run_greedy_next_slide.cpp` (rewritten from Python into C++ for performance) contain a greedy approach. It starts by building a list of slides (similarly as with previous approach, by generating slides just from next photos given) and building a separate slideshow from them. It takes just the first photo built as a starting point and then greedily builds the slideshow. It checks every slide that was not included in the slideshow so far and picks the one that gives the highest transition score. Again, subject to improvements (one of them described below) and addition of some additional heuristics.

Use `g++ code.cpp -o code -O3` for compiler optimizations, use `tail -f logfile` for reading the logfile as data is being appened to it.

The scores obtained with this approach:

| Test | Score     |
|------|-----------|
| A    | 2         |
| B    | 202938    |
| C    | 1426      |
| D    | 410752    |
| E    | 397965    |
| Sum: | 1,013,083 |


#### Greedy approach (building slides by photos)

File `run_greedy_next_slide_by_photos.cpp` contains a greedy approach that is an improvement to the previous method. Instead of generating slides first and then building a path of them, it searches for the next photo that gives the highest score at each step and only then builds slides of them accordingly.

The scores obtained with this approach:

| Test | Score     |
|------|-----------|
| A    | 2         |
| B    | 202938    |
| C    | 1759      |
| D    | 386956    |
| E    | 549197    |
| Sum: | 1,140,852 |

Apparently, for test case D, previous method gave higher score. The sum of highest scores obtained with these methods is 1,164,648. After generating the slideshow, in this case with greedy approaches, the solution could be subjected to any iterative optimization algorithm like the one from the first method presented here. However, as the former methods gave higher scores just by themselves, it is unlikely that the previous method would optimize the greedy solution by much (at least with this exact implementation and with that amount of resources allocated to it).

Please note that most of this code was written during Hash Code itself or during the weekend just after the event, so it could still be quite easily improved for quality or for the performance. In this repository, I wanted to share some, mostly basic, notes on the problem and share the code and solutions that I have tested during that time. Interesting things to explore, regarding iterative optimization methods in general, would be, for instance, to investigate how many neighbouring permutations are checked on average before finding a change that increases the score, and how that number changes as the solution is getting closer and closer to the optimum. Also, an average score increase per number of neighbouring solutions tested and how that changes would be potentially interesting to explore as well.