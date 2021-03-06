/*
 * RCVRP
 * A Simulated Annealing solver for the Risk-constrained Cash-in-transit VRP
 * Copyright (C) 2017  Manuel Weitzman
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */
#include "config.h"
#include "node.h"
#include "rcvrp.h"
#include "sa.h"
#include "solution.h"
#include <future>
#include <iostream>
#include <vector>

using std::async;
using std::cin;
using std::cout;
using std::fixed;
using std::future;
using std::vector;

int main(int const argc, char const **argv)
{
	/* Parse arguments */
	parse_cfg(argc, argv);

	/* Read instance from stdin */
	unsigned int nodes;
	cin >> nodes;

	/* Read risk threshold from stdin */
	double threshold;
	cin >> threshold;
	ctx.risk_threshold = threshold;

	/* Prepare the initial solution */
	Solution sol(nodes - 1);

	/* Store demands in initial solution */
	for (unsigned int i = 0; i < nodes; i++) {
		unsigned int d;
		cin >> d;

		/* Ignore first node, for our representation, it is redundant */
		if (i == 0)
			continue;
		Solution::demand.push_back(d);
	}

	for (unsigned int i = 0; i < nodes; i++) {
		double x;
		double y;

		cin >> x;
		cin >> y;

		/* Ignore first node, for our representation, it is redundant */
		if (i == 0)
			continue;
		Solution::coords.push_back(Node{x, y});
	}

	/* Start solving using many threads */
	vector< future<Solution> > threads(ctx.threads);
	for (unsigned int i = 0; i < ctx.threads; i++)
		threads.at(i) = async(sa, sol, threshold);

	/* Wait for each thread to finish */
	vector<Solution> results(ctx.threads);
	for (unsigned int i = 0; i < ctx.threads; i++)
		results.at(i) = threads.at(i).get();

	/* Select best solution among thread executions */
	Solution best = results.at(0);
	for (unsigned int i = 0; i < results.size(); i++)
		if (results.at(i).eval(threshold) < best.eval(threshold))
			best = results.at(i);

	/* Output best solution cost and nodes */
	best.print(threshold);

	/* At this point, everything is fine */
	return 0;
}
