//
//  maths_textbook.cpp
//
//  Modified by Patrick Anderson on 07/05/2015.
//  "maths_textbook" is a container for mathematical constants.
//

#include "maths_textbook.hpp"
#include "../IO/IO.hpp"
#include <cmath>
#include "../../Eigen/Dense"

#include <mkl.h>
#include <iostream>

using namespace Eigen;

//------------------------------------------------------------------------------------------------//
//  Class implementation
//------------------------------------------------------------------------------------------------//
/*! Constructor */
maths_textbook::maths_textbook(bool print_)
                               :
                               print(print_) {

    // Geometry
    pi = std::acos(-1.0);

}
/*! Constructor */
maths_textbook::maths_textbook(std::string path_input_j0_, bool print_)
                               :
                               path_input_j0(path_input_j0_),
                               print(print_) {

    // Geometry
    pi = std::acos(-1.0);

    // Special functions
    IO file;
    J0_zeros = file.read_double(path_input_j0, 1000, 1, false);   // From Mathematica

}

//------------------------------------------------------------------------------------------------//
/*! Trapezoidal integration, vectorized */
double maths_textbook::trapz(ArrayXd x_, ArrayXd y_){

    int N = x_.rows() - 1;
    ArrayXd x_temp = x_.tail(N) - x_.head(N);
    ArrayXd y_temp = y_.tail(N) + y_.head(N);

    return(0.5 * (x_temp * y_temp).sum());

}

//------------------------------------------------------------------------------------------------//
/*! Cumulative trapezoidal integration */
ArrayXd maths_textbook::cumtrapz(ArrayXd x_, ArrayXd y_){

    int n_int = x_.rows() - 1;  // intervals
    ArrayXd temp_1 = 0.5 * (x_.tail(n_int) - x_.head(n_int)) *
                           (y_.tail(n_int) + y_.head(n_int));

    // Cumulative sum
    ArrayXd output = ArrayXd::Zero(n_int + 1);
    output.tail(n_int) = temp_1;
    for (int ii = 1; ii < n_int + 1; ii++)
        output(ii) += output(ii - 1);

    return(output);
}

ArrayXd maths_textbook::interp1D(ArrayXd input_array, int input_length, int output_length, int spline_order) {
    /*
	A 1D interpolating function. Was used at one point but it is now redundant.
	Need to decide whether to remove it or keep it as it took a little effort
	to get it up and working...
    */

    /* input_length == NX == Size of partition, number of breakpoints */
    /* output_length == NSITE ==Number of interpolation sites */
    /* spline_order == SPLINE_ORDER == A cubic spline to construct */ 

    if (print) std::cout << "Interpolating Array: ";

    int status;          /* Status of a Data Fitting operation */
    int Nerrs = 0;       /* Numer of errors or warnings */
    DFTaskPtr task;      /* Data Fitting operations are task based */

    /* Parameters describing the partition */
    MKL_INT nx = input_length;          /* The size of partition x */
    double x[nx];         /* Partition x */
    MKL_INT xhint;       /* Additional information about the structure of breakpoints */

    /* Parameters describing the function */
    MKL_INT ny;          /* Function dimension */
    MKL_INT yhint;       /* Additional information about the function */

    /* Parameters describing the spline */
    MKL_INT  s_order;    /* Spline order */
    MKL_INT  s_type;     /* Spline type */
    MKL_INT  ic_type;    /* Type of internal conditions */
    double* ic;          /* Array of internal conditions */
    MKL_INT  bc_type;    /* Type of boundary conditions */
    double* bc;          /* Array of boundary conditions */

    double scoeff[(nx-1)* spline_order];  /* Array of spline coefficients */
    MKL_INT scoeffhint;                   /* Additional information about the coefficients */

    /* Parameters describing interpolation computations */
    MKL_INT nsite = output_length;  /* Number of interpolation sites */
    double site[nsite];             /* Array of interpolation sites */
    MKL_INT sitehint;               /* Additional information about the structure of 
                                       interpolation sites */

    MKL_INT ndorder, dorder;  /* Parameters defining the type of interpolation */

    double* datahint;  /* Additional information on partition and interpolation sites */

    ArrayXd r = ArrayXd::Zero(output_length);
    MKL_INT rhint;    /* Additional information on the structure of the results */
    MKL_INT* cell;    /* Array of cell indices */

    /* Set values of partition x */
    xhint = DF_NON_UNIFORM_PARTITION;  /* The partition is non-uniform. */

    /* Initialize the function */
    ny = 1;  /* The function is scalar. */

    /* Set function values */
    for (int i = 0; i < nx; i++)
    {
        x[i] = i;
    }
    yhint = DF_NO_HINT;  /* No additional information about the function is provided. */

    /* Create a Data Fitting task */
    status = dfdNewTask1D( &task, nx, x, xhint, ny, input_array.data(), yhint );
    if (print) std::cout << "\n   dfdNewTask1D status: " << status << std::endl;
    Nerrs += status;

    /* Initialize spline parameters */
    s_order = DF_PP_CUBIC;     /* Spline is of the fourth order (cubic spline). */
    s_type = DF_PP_NATURAL;    /* Spline is of the Natural cubic type. */

    /* Define internal conditions for cubic spline construction (none in this example) */
    ic_type = DF_NO_IC;
    ic = NULL;

    /* Use not-a-knot boundary conditions. In this case, the is first and the last
    interior breakpoints are inactive, no additional values are provided. */
    bc_type = DF_BC_FREE_END;
    bc = NULL;
    scoeffhint = DF_NO_HINT;    /* No additional information about the spline. */

    /* Set spline parameters  in the Data Fitting task */
    status = dfdEditPPSpline1D( task, s_order, s_type, bc_type, bc, ic_type,
        ic, scoeff, scoeffhint );
    if (print) std::cout << "   dfdEditPPSpline1D status: " << status << std::endl;
    Nerrs += status;

    /* Use a standard method to construct a cubic Bessel spline: */
    status = dfdConstruct1D( task, DF_PP_SPLINE, DF_METHOD_STD );
    if (print) std::cout << "   dfdConstruct1D status: " << status << std::endl;
    Nerrs += status;

    /* Set site values */
    for (int i = 0; i < nsite; i++)
    {
        site[i] = double(i) / double(nsite) * double(nx);//i;
    }

    sitehint = DF_NON_UNIFORM_PARTITION;  /* Partition of sites is non-uniform */

    /* Request to compute spline values */
    ndorder = 1;
    dorder = 1;
    datahint = DF_NO_APRIORI_INFO;  /* No additional information about breakpoints or
                                    sites is provided. */
    rhint = DF_MATRIX_STORAGE_ROWS; /* The library packs interpolation results
                                    in row-major format. */
    cell = NULL;                    /* Cell indices are not required. */

    /* Solve interpolation problem using the default method: compute the spline values
    at the points site(i), i=0,..., nsite-1 and place the results to array r */
    status = dfdInterpolate1D( task, DF_INTERP, DF_METHOD_PP, nsite, site,
        sitehint, ndorder, &dorder, datahint, r.data(), rhint, cell );
    if (print) std::cout << "   dfdInterpolate1D status: " << status << std::endl;
    Nerrs += status;

    /* De-allocate Data Fitting task resources */
    status = dfDeleteTask( &task );
    if (print) std::cout << "   dfDeleteTask status: " << status << std::endl;
    Nerrs += status;
    if (print && Nerrs == 0) {
    	std::cout << "Interpolation successful!" <<std::endl;
    } else if (print) {
    	std::cout << "Interpolation completed with errors!" << std::endl;
    }

    return(r);
}

ArrayXd maths_textbook::interp1D(ArrayXd x, int input_length, ArrayXd y, ArrayXd site, int output_length, int spline_order) {
    /*
    A 1D interpolating function. Was used at one point but it is now redundant.
    Need to decide whether to remove it or keep it as it took a little effort
    to get it up and working...
    */

    /* input_length == NX == Size of partition, number of breakpoints */
    /* output_length == NSITE ==Number of interpolation sites */
    /* spline_order == SPLINE_ORDER == A cubic spline to construct */

    if (print) std::cout << "Interpolating Array: ";

    int status;          /* Status of a Data Fitting operation */
    int Nerrs = 0;       /* Numer of errors or warnings */
    DFTaskPtr task;      /* Data Fitting operations are task based */

    /* Parameters describing the partition */
    MKL_INT nx = input_length;          /* The size of partition x */
    MKL_INT xhint;       /* Additional information about the structure of breakpoints */

    /* Parameters describing the function */
    MKL_INT ny;          /* Function dimension */
    MKL_INT yhint;       /* Additional information about the function */

    /* Parameters describing the spline */
    MKL_INT  s_order;    /* Spline order */
    MKL_INT  s_type;     /* Spline type */
    MKL_INT  ic_type;    /* Type of internal conditions */
    double* ic;          /* Array of internal conditions */
    MKL_INT  bc_type;    /* Type of boundary conditions */
    double* bc;          /* Array of boundary conditions */

    double scoeff[(nx-1)* spline_order];  /* Array of spline coefficients */
    MKL_INT scoeffhint;                   /* Additional information about the coefficients */

    /* Parameters describing interpolation computations */
    MKL_INT nsite = output_length;  /* Number of interpolation sites */
    MKL_INT sitehint;               /* Additional information about the structure of
                                       interpolation sites */

    MKL_INT ndorder, dorder;  /* Parameters defining the type of interpolation */

    double* datahint;  /* Additional information on partition and interpolation sites */

    ArrayXd r = ArrayXd::Zero(output_length);  /* Array of interpolation results */
    MKL_INT rhint;    /* Additional information on the structure of the results */
    MKL_INT* cell;    /* Array of cell indices */

    /* Set values of partition x */
    xhint = DF_NON_UNIFORM_PARTITION;  /* The partition is non-uniform. */

    /* Initialize the function */
    ny = 1;  /* The function is scalar. */
    yhint = DF_NO_HINT;  /* No additional information about the function is provided. */

    /* Create a Data Fitting task */
    status = dfdNewTask1D( &task, nx, x.data(), xhint, ny, y.data(), yhint );
    if (print) std::cout << "\n   dfdNewTask1D status: " << status << std::endl;
    Nerrs += status;

    /* Initialize spline parameters */
    s_order = DF_PP_CUBIC;    /* Spline is of the fourth order (cubic spline). */
    s_type = DF_PP_AKIMA;     /* Spline is of the Akima cubic type. */

    /* Define internal conditions for cubic spline construction (none in this example) */
    ic_type = DF_NO_IC;
    ic = NULL;

    /* Use not-a-knot boundary conditions. In this case, the is first and the last
    interior breakpoints are inactive, no additional values are provided. */
    bc_type = DF_BC_NOT_A_KNOT;
    bc = NULL;
    scoeffhint = DF_NO_HINT;    /* No additional information about the spline. */

    /* Set spline parameters  in the Data Fitting task */
    status = dfdEditPPSpline1D( task, s_order, s_type, bc_type, bc, ic_type,
        ic, scoeff, scoeffhint );
    if (print) std::cout << "   dfdEditPPSpline1D status: " << status << std::endl;
    Nerrs += status;

    /* Use a standard method to construct a cubic Bessel spline: */
    status = dfdConstruct1D( task, DF_PP_SPLINE, DF_METHOD_STD );
    if (print) std::cout << "   dfdConstruct1D status: " << status << std::endl;
    Nerrs += status;

    sitehint = DF_NON_UNIFORM_PARTITION;  /* Partition of sites is non-uniform */

    /* Request to compute spline values */
    ndorder = 1;
    dorder = 1;
    datahint = DF_NO_APRIORI_INFO;  /* No additional information about breakpoints or
                                    sites is provided. */
    rhint = DF_MATRIX_STORAGE_ROWS; /* The library packs interpolation results 
                                    in row-major format. */
    cell = NULL;                    /* Cell indices are not required. */

    /* Solve interpolation problem using the default method: compute the spline values
    at the points site(i), i=0,..., nsite-1 and place the results to array r */ 
    status = dfdInterpolate1D( task, DF_INTERP, DF_METHOD_PP, nsite, site.data(),
        sitehint, ndorder, &dorder, datahint, r.data(), rhint, cell );
    if (print) std::cout << "   dfdInterpolate1D status: " << status << std::endl;
    Nerrs += status;

    if (print) std::cout << "nsite: " << nsite << std::endl;
    if (print) std::cout << "site max: " << site.maxCoeff() << ", r max: " << r.maxCoeff() << std::endl;

    /* De-allocate Data Fitting task resources */
    status = dfDeleteTask( &task );
    if (print) std::cout << "   dfDeleteTask status: " << status << std::endl;
    Nerrs += status;
    if (print && Nerrs == 0) {
        std::cout << "Interpolation successful!" <<std::endl;
    } else if (print) {
        std::cout << "Interpolation completed with errors!" << std::endl;
    }

    return(r);
}
