// -*- C++ -*-
// $Id$

/**
 * Code generated by the The ACE ORB (TAO) IDL Compiler v2.2a_p19
 * TAO and the TAO IDL Compiler have been developed by:
 *       Center for Distributed Object Computing
 *       Washington University
 *       St. Louis, MO
 *       USA
 *       http://www.cs.wustl.edu/~schmidt/doc-center.html
 * and
 *       Distributed Object Computing Laboratory
 *       University of California at Irvine
 *       Irvine, CA
 *       USA
 * and
 *       Institute for Software Integrated Systems
 *       Vanderbilt University
 *       Nashville, TN
 *       USA
 *       http://www.isis.vanderbilt.edu/
 *
 * Information about TAO is available at:
 *     http://www.cs.wustl.edu/~schmidt/TAO.html
 **/

// TAO_IDL - Generated from
// be/be_codegen.cpp:376


#include "OnboardATCSC.h"
#include "tao/CDR.h"

#if !defined (__ACE_INLINE__)
#include "OnboardATCSC.inl"
#endif /* !defined INLINE */

// TAO_IDL - Generated from
// be/be_visitor_structure/cdr_op_cs.cpp:52

TAO_BEGIN_VERSIONED_NAMESPACE_DECL

::CORBA::Boolean operator<< (
    TAO_OutputCDR &strm,
    const OnboardATCS::TestMessage &_tao_aggregate)
{
  return
    (strm << _tao_aggregate.train_id) &&
    (strm << _tao_aggregate.subject.in ()) &&
    (strm << _tao_aggregate.text.in ()) &&
    (strm << _tao_aggregate.count) &&
    (strm << _tao_aggregate.timestamp);
}

::CORBA::Boolean operator>> (
    TAO_InputCDR &strm,
    OnboardATCS::TestMessage &_tao_aggregate)
{
  return
    (strm >> _tao_aggregate.train_id) &&
    (strm >> _tao_aggregate.subject.out ()) &&
    (strm >> _tao_aggregate.text.out ()) &&
    (strm >> _tao_aggregate.count) &&
    (strm >> _tao_aggregate.timestamp);
}

TAO_END_VERSIONED_NAMESPACE_DECL



