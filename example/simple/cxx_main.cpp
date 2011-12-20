#include <iostream>

#include <Wave.hpp>
#include <Damper.hpp>

#include <Coupler_Data_Source.hpp>
#include <Coupler_Data_Target.hpp>
#include <Coupler_Data_Field.hpp>

#include "Teuchos_RCP.hpp"
#include "Teuchos_CommHelpers.hpp"
#include "Teuchos_DefaultComm.hpp"

//---------------------------------------------------------------------------//
// Get the current default communicator.
template<class Ordinal>
Teuchos::RCP<const Teuchos::Comm<Ordinal> > getDefaultComm()
{
#ifdef HAVE_MPI
    return Teuchos::DefaultComm<Ordinal>::getComm();
#else
    return Teuchos::rcp(new Teuchos::SerialComm<Ordinal>() );
#endif
}

//---------------------------------------------------------------------------//
// Main function driver for the coupled Wave/Damper problem.
int main(int argc, char* argv[])
{
    // Setup the parallel domain.
    double global_min = 0.0;
    double global_max = 1.0;
    int myRank = getDefaultComm<int>()->getRank();
    int mySize = getDefaultComm<int>()->getSize();
    double myMin = mySize*myRank;
    double myMax;

    // Setup a Wave.
    Teuchos::RCP<Wave> wave = Teuchos::rcp( new Wave(myMin, myMax, 10) );

    // Setup a Damper.
    Teuchos::RCP<Damper> damper = Teuchos::rcp( new Damper(myMin, myMax, 10) );

    // Setup a Wave Data Source for the wave field.
    Teuchos::RCP<Coupler::Data_Source<double, int, double> > wave_source = 
	Teuchos::rcp( new Wave_Data_Source<double,int,double>(wave) );

    // Setup a Damper Data Target for the wave field.
    Teuchos::RCP<Coupler::Data_Target<double, int, double> > damper_target = 
	Teuchos::rcp( new Damper_Data_Target<double,int,double>(damper) );

    // Setup a Data Field for the wave field.
    Coupler::Data_Field<double,int,double> wave_field(getDefaultComm<int>(),
						      wave_source,
						      damper_target);

    // Setup a Damper Data Source for the damper field.
    Teuchos::RCP<Coupler::Data_Source<double, int, double> > damper_source = 
	Teuchos::rcp( new Damper_Data_Source<double,int,double>(damper) );

    // Setup a Wave Data Target for the damper field.
    Teuchos::RCP<Coupler::Data_Target<double, int, double> > wave_target = 
	Teuchos::rcp( new Wave_Data_Target<double,int,double>(wave) );

    // Setup a Data Field for the damper field.
    Coupler::Data_Field<double,int,double> damper_field(getDefaultComm<int>,
							damper_source,
							wave_target);

    // Iterate between the damper and wave until convergence.
    double local_norm = 0.0;
    double global_norm = 1.0;
    int num_iter = 0;
    int max_iter = 100;
    while( global_norm < 1.0e-6 && num_iter < max_iter )
    {
	// Transfer the wave field.
	wave_field.transfer();

	// Damper solve.
	damper.solve();

	// Transfer the damper field.
	damper_field.transfer();

	// Wave solve.
	local_norm = wave.solve();

	// Collect the l2 norm values to ensure convergence.
	Teuchos::reduceAll<OrdinalType,int>(*getDefaultComm<int>,
					    Teuchos::REDUCE_MAX, 
					    int(1), 
					    &local_norm, 
					    &global_norm);


	// Update the iteration count.
	++num_iter;

	// Barrier before proceeding.
	Teuchos::barrier<int>( *getDefaultComm<int>() );
    }

    std::cout << "Iterations to converge: " << num_iter << std::endl;

    return 0;
}
