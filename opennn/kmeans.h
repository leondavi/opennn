//   OpenNN: Open Neural Networks Library
//   www.opennn.net
//
//   K - M E A N S   H E A D E R
//
//   Artificial Intelligence Techniques SL
//   artelnics@artelnics.com

#ifndef KMEANS_H
#define KMEANS_H

// System includes

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <cmath>
#include <algorithm>
#include <cstdlib>
#include <stdexcept>
#include <ctime>
#include <exception>

// OpenNN includes

#include "statistics.h"
#include "tinyxml2.h"
#include "config.h"

// System includes

//#include <sstream>
//#include <cstring>
//#include <time.h>
//#include <omp.h>
//#include <Eigen/Dense>
//#include "unsupported/Eigen/CXX11/Tensor"

// OpenNN includes

//#include "../../opennn/opennn/opennn.h"

using Eigen::Tensor;
using opennn::type;
using Eigen::Index;
using namespace std;

namespace opennn
{

class KMeans
{

public:

    KMeans(Index clusters = 3, string distance_calculation_metod = "euclidean", Index iter = 100);

    Tensor<Index, 1> calculate_outputs(const Tensor<type, 2>&);
    Tensor<type, 1> elbow_method(const Tensor<type, 2>&, Index max_clusters=10);
    Index find_optimal_clusters(const Tensor<type, 1>&);

    Tensor<Index, 1> get_cluster_labels();
    Tensor<type, 2> get_cluster_centers();
    Index get_clusters_number();

    void fit(const Tensor<type, 2>&);
    void set_cluster_number(const Index&);
    void set_centers_random(const Tensor<type, 2>&);

private:

    Index clusters_number;
    Index maximum_iterations;
    string metric;

    Tensor<type, 2> cluster_centers;
    Tensor<Index, 1> rows_cluster_labels;

};

}

#endif

// OpenNN: Open Neural Networks Library.
// Copyright(C) 2005-2023 Artificial Intelligence Techniques, SL.
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.

// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
