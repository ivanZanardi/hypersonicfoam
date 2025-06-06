/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | Copyright held by original author
     \\/     M anipulation  |
-------------------------------------------------------------------------------
License
    This file is part of OpenFOAM.

    OpenFOAM is free software; you can redistribute it and/or modify it
    under the terms of the GNU General Public License as published by the
    Free Software Foundation; either version 2 of the License, or (at your
    option) any later version.

    OpenFOAM is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
    for more details.

    You should have received a copy of the GNU General Public License
    along with OpenFOAM; if not, write to the Free Software Foundation,
    Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA

\*---------------------------------------------------------------------------*/

#include "relaxationTimeModelHE.H"
#include "dimensionedConstants.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{
  
// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //  
  
    defineTypeNameAndDebug(relaxationTimeModelHE, 0);
    defineRunTimeSelectionTable(relaxationTimeModelHE, fvMesh);
}


// * * * * * * * * * * * * * Private Member Functions  * * * * * * * * * * * //  


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

Foam::relaxationTimeModelHE::relaxationTimeModelHE
(
    rho2ReactionThermo& thermo,
    const compressibleTurbulenceModel& turbulence
)
:
    IOdictionary
    (
        thermo.twoTemperatureDictionary()
    ),
    
    mesh_(thermo.Tt().mesh()), 
    thermo_(thermo),
    turbulence_(turbulence),
    
    QHE_
    (
        IOobject
        (
            "heRelaxationSource",
            mesh_.time().timeName(),
            mesh_,
            IOobject::NO_READ,
            IOobject::NO_WRITE
        ),
        mesh_,
        dimensionedScalar("Qhe", dimensionSet(1, -1, -3, 0, 0), 0.0)
    )
    
{
    if(isDict("AppletonBrayCoefficients")) // NEW ZANARDI
    {
	crossSection_ = subDict("AppletonBrayCoefficients").lookupOrDefault<word>("crossSecNeut", word::null);
            
	if (crossSection_ != "constant" and crossSection_ != "lineFitted")
	{
	    FatalErrorIn("Foam::relaxationTimeModelHE::relaxationTimeModelHE(rho2ReactionThermo& thermo, const compressibleTurbulenceModel& turbulence)")
		<< "The electron-neutral energy exchange cross section ('crossSecNeut') can either be defined as 'constant' or 'lineFitted'."
		<< exit(FatalError);   
        } 
    }
}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

/*Foam::tmp<Foam::volScalarField>
Foam::relaxationTimeModelHE::HERelaxationSource()
{
    tmp<volScalarField> tQHE
    (
        new volScalarField
        (
            IOobject
            (
                "heRelaxationSource",
                mesh_.time().timeName(),
                mesh_,
                IOobject::NO_READ,
                IOobject::NO_WRITE
            ),
            mesh_,
            dimensionedScalar("Qhe", dimensionSet(1, -1, -3, 0, 0), 0.0)
        )
    );
    
    return tQHE;
}*/


bool Foam::relaxationTimeModelHE::read()
{
    return regIOobject::read();
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //
