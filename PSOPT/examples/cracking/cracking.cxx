//////////////////////////////////////////////////////////////////////////
//////////////////         cracking.cxx       ////////////////////////////
//////////////////////////////////////////////////////////////////////////
////////////////           PSOPT  Example             ////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////// Title:  Catalytic Cracking of Gas Oil            ////////////////
//////// Last modified: 15 January 2009                   ////////////////
//////// Reference:     User’s guide for DIRCOL  	  ////////////////
//////// (See PSOPT handbook for full reference)           ///////////////
//////////////////////////////////////////////////////////////////////////
////////     Copyright (c) Victor M. Becerra, 2009        ////////////////
//////////////////////////////////////////////////////////////////////////
//////// This is part of the PSOPT software library, which ///////////////
//////// is distributed under the terms of the GNU Lesser ////////////////
//////// General Public License (LGPL)                    ////////////////
//////////////////////////////////////////////////////////////////////////

#include "psopt.h"

//////////////////////////////////////////////////////////////////////////
///////////////////  Define the observation function //////////
//////////////////////////////////////////////////////////////////////////



void  observation_function( adouble* observations,
                            adouble* states, adouble* controls,
                            adouble* parameters, adouble& time, int k,
                            adouble* xad, int iphase, Workspace* workspace)
{

      observations[ CINDEX(1) ] = states[ CINDEX(1) ];
      observations[ CINDEX(2) ] = states[ CINDEX(2) ];
}



//////////////////////////////////////////////////////////////////////////
///////////////////  Define the DAE's ////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

void dae(adouble* derivatives, adouble* path, adouble* states,
         adouble* controls, adouble* parameters, adouble& time,
         adouble* xad, int iphase, Workspace* workspace)
{

   adouble y1 = states[CINDEX(1)];
   adouble y2 = states[CINDEX(2)];

   adouble theta1 = parameters[ CINDEX(1) ];
   adouble theta2 = parameters[ CINDEX(2) ];
   adouble theta3 = parameters[ CINDEX(3) ];

   derivatives[CINDEX(1)] = -(theta1 + theta3)*y1*y1;
   derivatives[CINDEX(2)] =  theta1*y1*y1 - theta2*y2;

}

////////////////////////////////////////////////////////////////////////////
///////////////////  Define the events function ////////////////////////////
////////////////////////////////////////////////////////////////////////////

void events(adouble* e, adouble* initial_states, adouble* final_states,
            adouble* parameters,adouble& t0, adouble& tf, adouble* xad,
            int iphase, Workspace* workspace)
{
   // No events

}


///////////////////////////////////////////////////////////////////////////
///////////////////  Define the phase linkages function ///////////////////
///////////////////////////////////////////////////////////////////////////

void linkages( adouble* linkages, adouble* xad, Workspace* workspace)
{
  // No linkages as this is a single phase problem
}


////////////////////////////////////////////////////////////////////////////
///////////////////  Define the main routine ///////////////////////////////
////////////////////////////////////////////////////////////////////////////

int main(void)
{

   DMatrix y1meas, y2meas, tmeas;
   // Measured values of y1
   y1meas = 	"[1.0,0.8105,0.6208,0.5258,0.4345,0.3903,0.3342,0.3034, \
                  0.2735,0.2405,0.2283,0.2071,0.1669,0.153,0.1339,0.1265, \
                  0.12,0.099,0.087,0.077,0.069]";
   // Measured values of y2
   y2meas = 	"[0.0,0.2,0.2886,0.301,0.3215,0.3123,0.2716,0.2551,0.2258, \
                 0.1959,0.1789,0.1457,0.1198,0.0909,0.0719,0.0561,0.046, \
                0.028,0.019,0.014,0.01]";
   // Sampling instants
   tmeas =    "[0.0,0.025,0.05,0.075,0.1,0.125,0.15,0.175,0.2,0.225,0.25, \
            0.3,0.35,0.4,0.45,0.5,0.55,0.65,0.75,0.85,0.95]";



////////////////////////////////////////////////////////////////////////////
///////////////////  Declare key structures ////////////////////////////////
////////////////////////////////////////////////////////////////////////////

    Alg  algorithm;
    Sol  solution;
    Prob problem;

////////////////////////////////////////////////////////////////////////////
///////////////////  Register problem name  ////////////////////////////////
////////////////////////////////////////////////////////////////////////////

    problem.name          		= "Catalytic cracking of gas oil";
    problem.outfilename                 = "cracking.txt";

////////////////////////////////////////////////////////////////////////////
////////////  Define problem level constants & do level 1 setup ////////////
////////////////////////////////////////////////////////////////////////////

    problem.nphases   			= 1;
    problem.nlinkages                   = 0;

    psopt_level1_setup(problem);


/////////////////////////////////////////////////////////////////////////////
/////////   Define phase related information & do level 2 setup /////////////
/////////////////////////////////////////////////////////////////////////////

    problem.phases(1).nstates   		= 2;
    problem.phases(1).ncontrols 		= 0;
    problem.phases(1).nevents   		= 0;
    problem.phases(1).npath     		= 0;
    problem.phases(1).nparameters        	= 3;
    problem.phases(1).nodes    		    	= "[80]";
    problem.phases(1).nobserved   = 2;
    problem.phases(1).nsamples    = 21;

    psopt_level2_setup(problem, algorithm);

////////////////////////////////////////////////////////////////////////////
////////////  Enter estimation information                      ////////////
////////////////////////////////////////////////////////////////////////////


    problem.phases(1).observation_nodes      = tmeas;
    problem.phases(1).observations           = (y1meas && y2meas);
    problem.phases(1).residual_weights       = ones(2,21);


////////////////////////////////////////////////////////////////////////////
///////////////////  Declare DMatrix objects to store results //////////////
////////////////////////////////////////////////////////////////////////////

    DMatrix x, p, t;

////////////////////////////////////////////////////////////////////////////
///////////////////  Enter problem bounds information //////////////////////
////////////////////////////////////////////////////////////////////////////


    problem.phases(1).bounds.lower.states(1) =  0.0;
    problem.phases(1).bounds.lower.states(2) =  0.0;


    problem.phases(1).bounds.upper.states(1) =  2.0;
    problem.phases(1).bounds.upper.states(2) =  2.0;


    problem.phases(1).bounds.lower.parameters(1) = 0.0;
    problem.phases(1).bounds.lower.parameters(2) = 0.0;
    problem.phases(1).bounds.lower.parameters(3) = 0.0;
    problem.phases(1).bounds.upper.parameters(1) = 20.0;
    problem.phases(1).bounds.upper.parameters(2) = 20.0;
    problem.phases(1).bounds.upper.parameters(3) = 20.0;


    problem.phases(1).bounds.lower.StartTime    = 0.0;
    problem.phases(1).bounds.upper.StartTime    = 0.0;

    problem.phases(1).bounds.lower.EndTime      = 0.95;
    problem.phases(1).bounds.upper.EndTime      = 0.95;

////////////////////////////////////////////////////////////////////////////
///////////////////  Register problem functions  ///////////////////////////
////////////////////////////////////////////////////////////////////////////

    problem.dae 		= &dae;
    problem.events 		= &events;
    problem.linkages		= &linkages;
    problem.observation_function = & observation_function;

////////////////////////////////////////////////////////////////////////////
///////////////////  Define & register initial guess ///////////////////////
////////////////////////////////////////////////////////////////////////////

    DMatrix state_guess(2, 40);

    state_guess(1,colon()) = linspace(1.0,0.069, 40);
    state_guess(2,colon()) = linspace(0.30,0.01,  40);


    problem.phases(1).guess.states         = state_guess;
    problem.phases(1).guess.time           = linspace(0.0, 0.95, 40);
    problem.phases(1).guess.parameters     = zeros(3,1);


////////////////////////////////////////////////////////////////////////////
///////////////////  Enter algorithm options  //////////////////////////////
////////////////////////////////////////////////////////////////////////////

    algorithm.nlp_method                  = "IPOPT";
    algorithm.scaling                     = "automatic";
    algorithm.derivatives                 = "automatic";
//    algorithm.collocation_method          = "Hermite-Simpson";
    algorithm.nlp_iter_max                = 1000;
    algorithm.nlp_tolerance               = 1.e-4;
    algorithm.jac_sparsity_ratio          = 0.52;

////////////////////////////////////////////////////////////////////////////
///////////////////  Now call PSOPT to solve the problem   //////////////////
////////////////////////////////////////////////////////////////////////////

    psopt(solution, problem, algorithm);

////////////////////////////////////////////////////////////////////////////
///////////  Extract relevant variables from solution structure   //////////
////////////////////////////////////////////////////////////////////////////

    x = solution.get_states_in_phase(1);
    t = solution.get_time_in_phase(1);
    p = solution.get_parameters_in_phase(1);


////////////////////////////////////////////////////////////////////////////
///////////  Save solution data to files if desired ////////////////////////
////////////////////////////////////////////////////////////////////////////

    x.Save("x.dat");
    t.Save("t.dat");
    p.Print("Estimated parameters");


////////////////////////////////////////////////////////////////////////////
///////////  Plot some results if desired (requires gnuplot) ///////////////
////////////////////////////////////////////////////////////////////////////

    plot(t,x,problem.name, "time (s)", "states", "y1 y2");
    plot(t,x,problem.name, "time (s)", "states", "y1 y2",
                           "pdf", "cracking_states.pdf");

}

////////////////////////////////////////////////////////////////////////////
///////////////////////      END OF FILE     ///////////////////////////////
////////////////////////////////////////////////////////////////////////////