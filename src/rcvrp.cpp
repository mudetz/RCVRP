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
#include "eval.h"
#include "node.h"
#include "rcvrp.h"
#include "sa.h"
#include <algorithm>
#include <future>
#include <iostream>
#include <vector>

using std::async;
using std::cin;
using std::cout;
using std::future;
using std::vector;

int main(int const argc, char const **argv)
{
	/* Parse arguments */
	parse_cfg(argc, argv);

	/* Read instance from stdin */
	unsigned int nodes;
	fscanf(stdin, "%u", &nodes);

	unsigned int threshold;
	fscanf(stdin, "%u", &threshold);

	vector<unsigned> demand;
	demand.reserve(nodes);
	for (unsigned int i = 0; i < nodes; i++) {
		unsigned int d;
		cin >> d;
		demand.push_back(d);
	}

	vector<Node> coords;
	coords.reserve(nodes);
	for (unsigned int i = 0; i < nodes; i++) {
		float x;
		float y;

		cin >> x;
		cin >> y;
		coords.push_back(Node{x, y});
	}
	/* Start solving using many threads */
	vector< future< vector<unsigned int> > > threads(ctx.threads);
	for (unsigned int i = 0; i < ctx.threads; i++)
		threads.at(i) = async(sa, coords);

	vector< vector<unsigned int> > results(ctx.threads);
	for (unsigned int i = 0; i < ctx.threads; i++)
		results.at(i) = threads.at(i).get();

	/* Select best solution */
	vector<unsigned int> best = results.at(0);
	for (unsigned int i = 0; i < results.size(); i++)
		if (eval(coords, results.at(i)) > eval(coords, best))
			best = results.at(i);

	/* Output best solution cost and nodes */
	cout << eval(coords, best) << '\n';
	for (unsigned int i = 0; i < best.size(); i++)
		cout << coords.at( best.at(i) ).x
			  << ' '
			  << coords.at( best.at(i) ).y
			  << '\n';

	return 0;
}
