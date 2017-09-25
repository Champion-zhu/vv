/*=========================================================================
  Program:   vv                     http://www.creatis.insa-lyon.fr/rio/vv

  Authors belong to: 
  - University of LYON              http://www.universite-lyon.fr/
  - Léon Bérard cancer center       http://www.centreleonberard.fr
  - CREATIS CNRS laboratory         http://www.creatis.insa-lyon.fr

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the copyright notices for more information.

  It is distributed under dual licence

  - BSD        See included LICENSE.txt file
  - CeCILL-B   http://www.cecill.info/licences/Licence_CeCILL-B_V1-en.html
===========================================================================**/
#ifndef __clitkLBFGSBOptimizer_h
#define __clitkLBFGSBOptimizer_h
#include "itkSingleValuedNonLinearVnlOptimizer.h"

#include "clitkCommon.h"

namespace clitk
{

/** \class LBFGSBOptimizerHelper
 * \brief Wrapper helper around vnl_lbfgsb.
 *
 * This class is used to translate iteration events, etc, from
 * vnl_lbfgsb into iteration events in ITK.
 */
class ITK_EXPORT LBFGSBOptimizerHelper;


/** \class LBFGSBOptimizer
 * \brief Limited memory Broyden Fletcher Goldfarb Shannon minimization with simple bounds.
 *
 * This class is a wrapper for converted fortan code for performing limited
 * memory Broyden Fletcher Goldfarb Shannon minimization with simple bounds.
 * The algorithm miminizes a nonlinear function f(x) of n variables subject to
 * simple bound constraints of l <= x <= u.
 *
 * See also the documentation in Numerics/lbfgsb.c
 *
 * References:
 *
 * [1] R. H. Byrd, P. Lu and J. Nocedal. 
 * A Limited Memory Algorithm for Bound Constrained Optimization, (1995), 
 * SIAM Journal on Scientific and Statistical Computing , 
 * 16, 5, pp. 1190-1208. 
 *
 * [2] C. Zhu, R. H. Byrd and J. Nocedal. 
 * L-BFGS-B: Algorithm 778: L-BFGS-B, FORTRAN routines for large scale 
 * bound constrained optimization (1997), 
 * ACM Transactions on Mathematical Software, 
 * Vol 23, Num. 4, pp. 550 - 560. 
 *
 * \ingroup Numerics Optimizers
 */
class ITK_EXPORT LBFGSBOptimizer : 
    public itk::SingleValuedNonLinearVnlOptimizer
{
public:
  /** Standard "Self" typedef. */
  typedef LBFGSBOptimizer                     Self;
  typedef itk::SingleValuedNonLinearVnlOptimizer   Superclass;
  typedef itk::SmartPointer<Self>                  Pointer;
  typedef itk::SmartPointer<const Self>            ConstPointer;
  
  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro( LBFGSBOptimizer, SingleValuedNonLinearVnlOptimizer );

  /**  BoundValue type.
   *  Use for defining the lower and upper bounds on the variables. 
   */
  typedef itk::Array<double>             BoundValueType;

  /** BoundSelection type
   * Use for defining the boundary condition for each variables. 
   */
  typedef itk::Array<long>                BoundSelectionType;

  /** Internal boundary value storage type */
  typedef vnl_vector<double>    InternalBoundValueType;

  /** Internal boundary selection storage type */
  typedef vnl_vector<long>      InternalBoundSelectionType;

  /** The vnl optimizer */
  typedef LBFGSBOptimizerHelper InternalOptimizerType;


  /** Start optimization with an initial value. */
  virtual void StartOptimization( void ) ITK_OVERRIDE;

  /** Plug in a Cost Function into the optimizer  */
  virtual void SetCostFunction( itk::SingleValuedCostFunction * costFunction ) ITK_OVERRIDE;

  /** Set the lower bound value for each variable. */
  virtual void SetLowerBound( const BoundValueType & value );
  virtual const BoundValueType & GetLowerBound();

  /** Set the upper bound value for each variable. */
  virtual void SetUpperBound( const BoundValueType & value );
  virtual const BoundValueType & GetUpperBound();

  /** Set the boundary condition for each variable, where
   * select[i] = 0 if x[i] is unbounded,
   *           = 1 if x[i] has only a lower bound,
   *           = 2 if x[i] has both lower and upper bounds, and
   *           = 3 if x[1] has only an upper bound
   */
  virtual void SetBoundSelection( const BoundSelectionType & select );
  virtual const BoundSelectionType & GetBoundSelection();

  /** Set/Get the CostFunctionConvergenceFactor. Algorithm terminates
   * when the reduction in cost function is less than factor * epsmcj
   * where epsmch is the machine precision.
   * Typical values for factor: 1e+12 for low accuracy; 
   * 1e+7 for moderate accuracy and 1e+1 for extremely high accuracy.
   */
  virtual void SetCostFunctionConvergenceFactor( double );
  itkGetMacro( CostFunctionConvergenceFactor, double );

  /** Set/Get the ProjectedGradientTolerance. Algorithm terminates
   * when the project gradient is below the tolerance. Default value
   * is 1e-5.
   */
  virtual void SetProjectedGradientTolerance( double );
  itkGetMacro( ProjectedGradientTolerance, double );

  /** Set/Get the MaximumNumberOfIterations. Default is 500 */
  virtual void SetMaximumNumberOfIterations( unsigned int );
  itkGetMacro( MaximumNumberOfIterations, unsigned int );

  /** Set/Get the MaximumNumberOfEvaluations. Default is 500 */
  virtual void SetMaximumNumberOfEvaluations( unsigned int );
  itkGetMacro( MaximumNumberOfEvaluations, unsigned int );

  /** Set/Get the MaximumNumberOfCorrections. Default is 5 */
  virtual void SetMaximumNumberOfCorrections( unsigned int );
  itkGetMacro( MaximumNumberOfCorrections, unsigned int );

  /** This optimizer does not support scaling of the derivatives. */
  void SetScales( const ScalesType & )
    {
    itkExceptionMacro( << "This optimizer does not support scales." );
    }

  /** Get the current iteration number. */
  itkGetConstReferenceMacro( CurrentIteration, unsigned int );

  /** Get the current cost function value. */
  itkGetConstReferenceMacro( Value, MeasureType );

  /** Get the current infinity norm of the project gradient of the cost
   * function. */
  itkGetConstReferenceMacro( InfinityNormOfProjectedGradient, double );

  /** Get the reason for termination */
  virtual const std::string GetStopConditionDescription() const ITK_OVERRIDE;

protected:
  LBFGSBOptimizer();
  virtual ~LBFGSBOptimizer();
  void PrintSelf(std::ostream& os, itk::Indent indent) const ITK_OVERRIDE;

  typedef Superclass::CostFunctionAdaptorType   CostFunctionAdaptorType;

private:
  LBFGSBOptimizer(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

  // give the helper access to member variables, to update iteration
  // counts, etc.
  friend class LBFGSBOptimizerHelper;

  bool                     m_OptimizerInitialized;
  InternalOptimizerType  * m_VnlOptimizer;
  mutable std::ostringstream    m_StopConditionDescription;
  BoundValueType           m_LowerBound;
  BoundValueType           m_UpperBound;
  BoundSelectionType       m_BoundSelection;

  double                   m_CostFunctionConvergenceFactor;
  double                   m_ProjectedGradientTolerance;
  unsigned int             m_MaximumNumberOfIterations;
  unsigned int             m_MaximumNumberOfEvaluations;
  unsigned int             m_MaximumNumberOfCorrections;

  unsigned int             m_CurrentIteration;
  MeasureType              m_Value;
  double                   m_InfinityNormOfProjectedGradient;

};

} // end namespace clitk

#endif
