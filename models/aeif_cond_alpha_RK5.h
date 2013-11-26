/*
 *  aeif_cond_alpha_RK5.h
 *
 *  This file is part of NEST.
 *
 *  Copyright (C) 2004 The NEST Initiative
 *
 *  NEST is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  NEST is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with NEST.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef AEIF_COND_ALPHA_RK5_H
#define AEIF_COND_ALPHA_RK5_H

#include "config.h"

#include "nest.h"
#include "event.h"
#include "archiving_node.h"
#include "ring_buffer.h"
#include "connection.h"
#include "universal_data_logger.h"
#include "recordables_map.h"

/* BeginDocumentation
Name: aeif_cond_alpha_RK5 -  Conductance based exponential integrate-and-fire neuron model according to Brette and Gerstner (2005).

Description:
aeif_cond_alpha_RK5 is the adaptive exponential integrate and fire neuron according to Brette and Gerstner (2005).
Synaptic conductances are modelled as alpha-functions.

This implementation uses a 5th order Runge-Kutta solver with adaptive stepsize to integrate
the differential equation (see Numerical Recipes 3rd Edition, Press et al. 2007, Ch. 17.2).

The membrane potential is given by the following differential equation:
C dV/dt= -g_L(V-E_L)+g_L*Delta_T*exp((V-V_T)/Delta_T)-g_e(t)(V-E_e) -g_i(t)(V-E_i)-w +I_e

and

tau_w * dw/dt= a(V-E_L) -w

Parameters: 
The following parameters can be set in the status dictionary.

Dynamic state variables:
  V_m        double - Membrane potential in mV
  g_ex       double - Excitatory synaptic conductance in nS.
  dg_ex      double - First derivative of g_ex in nS/ms
  g_in       double - Inhibitory synaptic conductance in nS.
  dg_in      double - First derivative of g_in in nS/ms.
  w          double - Spike-adaptation current in pA.

Membrane Parameters:
  C_m        double - Capacity of the membrane in pF
  t_ref      double - Duration of refractory period in ms. 
  V_reset    double - Reset value for V_m after a spike. In mV.
 E_L        double - Leak reversal potential in mV. 
  g_L        double - Leak conductance in nS.
  I_e        double - Constant external input current in pA.

Spike adaptation parameters:
  a          double - Subthreshold adaptation in nS.
  b          double - Spike-triggered adaptation in pA.
  Delta_T    double - Slope factor in mV
  tau_w      double - Adaptation time constant in ms
  V_th       double - Spike initiation threshold in mV
  V_peak     double - Spike detection threshold in mV.

Synaptic parameters:
  E_ex       double - Excitatory reversal potential in mV.
  tau_syn_ex double - Rise time of excitatory synaptic conductance in ms (alpha function).
  E_in       double - Inhibitory reversal potential in mV.
  tau_syn_in double - Rise time of the inhibitory synaptic conductance in ms (alpha function).

Numerical integration parameters:
  HMIN       double - Minimal stepsize for numerical integration in ms (default 0.001ms).
  MAXERR     double - Error estimate tolerance for adaptive stepsize control (steps accepted if err<=MAXERR). In mV.
                      Note that the error refers to the difference between the 4th and 5th order RK terms.
                      Default 1e-10 mV.

Authors: Stefan Bucher, Marc-Oliver Gewaltig.

Sends: SpikeEvent

Receives: SpikeEvent, CurrentEvent, DataLoggingRequest

References: Brette R and Gerstner W (2005) Adaptive Exponential Integrate-and-Fire Model as 
            an Effective Description of Neuronal Activity. J Neurophysiol 94:3637-3642

SeeAlso: iaf_cond_alpha, aeif_cond_exp, aeif_cond_alpha
*/

namespace nest
{
  
  class aeif_cond_alpha_RK5:
  public Archiving_Node
  {
    
  public:        
    aeif_cond_alpha_RK5();
    aeif_cond_alpha_RK5(const aeif_cond_alpha_RK5&);
    ~aeif_cond_alpha_RK5();

    /**
     * Import sets of overloaded virtual functions.
     * We need to explicitly include sets of overloaded
     * virtual functions into the current scope.
     * According to the SUN C++ FAQ, this is the correct
     * way of doing things, although all other compilers
     * happily live without.
     */

    using Node::connect_sender;
    using Node::handle;

    port check_connection(Connection&, port);
    
    void handle(SpikeEvent &);
    void handle(CurrentEvent &);
    void handle(DataLoggingRequest &); 
    
    port connect_sender(SpikeEvent &, port);
    port connect_sender(CurrentEvent &, port);
    port connect_sender(DataLoggingRequest &, port);

    void get_status(DictionaryDatum &) const;
    void set_status(const DictionaryDatum &);
    
  private:
    
    void init_state_(const Node& proto);
    void init_buffers_();
    void calibrate();
    void update(Time const &, const long_t, const long_t);
    
    inline
    void aeif_cond_alpha_RK5_dynamics (const double*, double*);
    
    // END Boilerplate function declarations ----------------------------

    // Friends --------------------------------------------------------

    // The next two classes need to be friends to access the State_ class/member
    friend class RecordablesMap<aeif_cond_alpha_RK5>;
    friend class UniversalDataLogger<aeif_cond_alpha_RK5>;

 
  private:  
    // ---------------------------------------------------------------- 
  
    //! Independent parameters
    struct Parameters_ {
      double_t V_peak_;     //!< Spike detection threshold in mV
      double_t V_reset_;    //!< Reset Potential in mV
      double_t t_ref_;      //!< Refractory period in ms

      double_t g_L;         //!< Leak Conductance in nS
      double_t C_m;         //!< Membrane Capacitance in pF
      double_t E_ex;        //!< Excitatory reversal Potential in mV
      double_t E_in;        //!< Inhibitory reversal Potential in mV
      double_t E_L;         //!< Leak reversal Potential (aka resting potential) in mV
      double_t Delta_T;     //!< Slope faktor in ms.
      double_t tau_w;       //!< adaptation time-constant in ms.
      double_t a;           //!< Subthreshold adaptation in nS.
      double_t b;           //!< Spike-triggered adaptation in pA
      double_t V_th;        //!< Spike threshold in mV.
      double_t t_ref;       //!< Refractory period in ms.
      double_t tau_syn_ex;  //!< Excitatory synaptic rise time.
      double_t tau_syn_in;  //!< Inhibitory synaptic rise time.
      double_t I_e;         //!< Intrinsic current in pA.
      double_t MAXERR;      //!< Maximal error for adaptive stepsize solver
      double_t HMIN;        //!< Smallest permissible stepsize in ms.
      Parameters_();  //!< Sets default parameter values

      void get(DictionaryDatum&) const;  //!< Store current values in dictionary
      void set(const DictionaryDatum&);  //!< Set values from dicitonary
    };

  public:
    // ---------------------------------------------------------------- 
    
    /**
     * State variables of the model.
     * @note Copy constructor and assignment operator required because
     *       of C-style array.
     */
    struct State_
    {
      /**
       * Enumeration identifying elements in state array State_::y_.
       * The state vector is passed as a C array. This enum
       * identifies the elements of the vector. It must be public to be
       * accessible from the iteration function.
       */  
      enum StateVecElems
      {
	V_M   = 0,
	DG_EXC   ,  // 1
	G_EXC    ,  // 2
	DG_INH   ,  // 3
	G_INH    ,  // 4
	W        ,  // 5
	STATE_VEC_SIZE
      };

      double_t y_[STATE_VEC_SIZE];  //!< neuron state
      double_t k1[STATE_VEC_SIZE];  //!< Runge-Kutta variable
      double_t k2[STATE_VEC_SIZE];  //!< Runge-Kutta variable
      double_t k3[STATE_VEC_SIZE];  //!< Runge-Kutta variable
      double_t k4[STATE_VEC_SIZE];  //!< Runge-Kutta variable
      double_t k5[STATE_VEC_SIZE];  //!< Runge-Kutta variable
      double_t k6[STATE_VEC_SIZE];  //!< Runge-Kutta variable
      double_t k7[STATE_VEC_SIZE];  //!< Runge-Kutta variable
      double_t yin[STATE_VEC_SIZE]; //!< Runge-Kutta variable
      double_t ynew[STATE_VEC_SIZE];//!< 5th order update
      double_t yref[STATE_VEC_SIZE];//!< 4th order update
      int_t    r_;                  //!< number of refractory steps remaining

      State_(const Parameters_&);  //!< Default initialization
      State_(const State_&);
      State_& operator=(const State_&);

      void get(DictionaryDatum&) const;
      void set(const DictionaryDatum&, const Parameters_&);
    };    

    // ---------------------------------------------------------------- 

    /**
     * Buffers of the model.
     */
    struct Buffers_ {
      Buffers_(aeif_cond_alpha_RK5&);                   //!<Sets buffer pointers to 0
      Buffers_(const Buffers_&, aeif_cond_alpha_RK5&);  //!<Sets buffer pointers to 0

      //! Logger for all analog data
      UniversalDataLogger<aeif_cond_alpha_RK5> logger_;

      /** buffers and sums up incoming spikes/currents */
      RingBuffer spike_exc_;
      RingBuffer spike_inh_;
      RingBuffer currents_;

      // IntergrationStep_ should be reset with the neuron on ResetNetwork,
      // but remain unchanged during calibration. Since it is initialized with
      // step_, and the resolution cannot change after nodes have been created,
      // it is safe to place both here.
      double_t step_;           //!< simulation step size in ms
      double   IntegrationStep_;//!< current integration time step, updated by solver

      /** 
       * Input current injected by CurrentEvent.
       * This variable is used to transport the current applied into the
       * _dynamics function computing the derivative of the state vector.
       * It must be a part of Buffers_, since it is initialized once before
       * the first simulation, but not modified before later Simulate calls.
       */
      double_t I_stim_;
    };

     // ---------------------------------------------------------------- 

     /**
      * Internal variables of the model.
      */
     struct Variables_ { 
      /** initial value to normalise excitatory synaptic conductance */
      double_t g0_ex_; 
   
      /** initial value to normalise inhibitory synaptic conductance */
      double_t g0_in_;    

      int_t    RefractoryCounts_;
     };

    // Access functions for UniversalDataLogger -------------------------------
    
    //! Read out state vector elements, used by UniversalDataLogger
    template <State_::StateVecElems elem>
    double_t get_y_elem_() const { return S_.y_[elem]; }

    // ---------------------------------------------------------------- 

    Parameters_ P_;
    State_      S_;
    Variables_  V_;
    Buffers_    B_;

    //! Mapping of recordables names to access functions
    static RecordablesMap<aeif_cond_alpha_RK5> recordablesMap_;
  };

  inline  
  port aeif_cond_alpha_RK5::check_connection(Connection& c, port receptor_type)
  {
    SpikeEvent e;
    e.set_sender(*this);
    c.check_event(e);
    return c.get_target()->connect_sender(e, receptor_type);
  }

  inline
  port aeif_cond_alpha_RK5::connect_sender(SpikeEvent&, port receptor_type)
  {
    if (receptor_type != 0)
      throw UnknownReceptorType(receptor_type, get_name());
    return 0;
  }
 
  inline
  port aeif_cond_alpha_RK5::connect_sender(CurrentEvent&, port receptor_type)
  {
    if (receptor_type != 0)
      throw UnknownReceptorType(receptor_type, get_name());
    return 0;
  }

  inline
  port aeif_cond_alpha_RK5::connect_sender(DataLoggingRequest& dlr, 
				      port receptor_type)
  {
    if (receptor_type != 0)
      throw UnknownReceptorType(receptor_type, get_name());
    return B_.logger_.connect_logging_device(dlr, recordablesMap_);
  }
 
  inline
  void aeif_cond_alpha_RK5::get_status(DictionaryDatum &d) const
  {
    P_.get(d);
    S_.get(d);
    Archiving_Node::get_status(d);

    (*d)[names::recordables] = recordablesMap_.get_list();
  }

  inline
  void aeif_cond_alpha_RK5::set_status(const DictionaryDatum &d)
  {
    Parameters_ ptmp = P_;  // temporary copy in case of errors
    ptmp.set(d);                       // throws if BadProperty
    State_      stmp = S_;  // temporary copy in case of errors
    stmp.set(d, ptmp);                 // throws if BadProperty

    // We now know that (ptmp, stmp) are consistent. We do not 
    // write them back to (P_, S_) before we are also sure that 
    // the properties to be set in the parent class are internally 
    // consistent.
    Archiving_Node::set_status(d);

    // if we get here, temporaries contain consistent set of properties
    P_ = ptmp;
    S_ = stmp;
  }

/** 
 * Function computing right-hand side of ODE for the ODE solver.
 * @param y State vector (input).
 * @param f Derivatives (output).
 */  
inline
void aeif_cond_alpha_RK5::aeif_cond_alpha_RK5_dynamics (const double y[], double f[])
{
  // a shorthand
  typedef aeif_cond_alpha_RK5::State_ S;

  // y[] is the current internal state of the integrator (yin), not the state vector in the node, node.S_.y[]. 
  
  // The following code is verbose for the sake of clarity. We assume that a
  // good compiler will optimize the verbosity away ...

  // This constant is used below as the largest admissible value for the exponential spike upstroke
  static const double_t largest_exp=std::exp(10.);

  // shorthand for state variables
  const double_t& V     = y[S::V_M];
  const double_t& dg_ex = y[S::DG_EXC];
  const double_t&  g_ex = y[S::G_EXC ];
  const double_t& dg_in = y[S::DG_INH];
  const double_t&  g_in = y[S::G_INH ];
  const double_t& w     = y[S::W];

  const double_t I_syn_exc = g_ex * (V - P_.E_ex);
  const double_t I_syn_inh = g_in * (V - P_.E_in);

  // We pre-compute the argument of the exponential
  const double_t exp_arg=(V - P_.V_th) / P_.Delta_T;
  // If the argument is too large, we clip it.
  const double_t I_spike = (exp_arg>10.)? largest_exp : P_.Delta_T * std::exp(exp_arg);

  // dv/dt
  f[S::V_M  ] = ( -P_.g_L *( (V-P_.E_L) - I_spike ) 
			     - I_syn_exc - I_syn_inh - w + P_.I_e + B_.I_stim_) / P_.C_m;
  f[S::DG_EXC] = -dg_ex / P_.tau_syn_ex;
  f[S::G_EXC ] =  dg_ex - g_ex / P_.tau_syn_ex; // Synaptic Conductance (nS)
  
  f[S::DG_INH] = -dg_in / P_.tau_syn_in;
  f[S::G_INH ] =  dg_in - g_in / P_.tau_syn_in; // Synaptic Conductance (nS)
  
  // Adaptation current w.
  f[S::W    ] = ( P_.a * (V - P_.E_L) - w ) / P_.tau_w;
} 

  
} // namespace

#endif //AEIF_COND_ALPHA_RK5_H